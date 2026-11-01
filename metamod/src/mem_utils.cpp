#include "precompiled.h"

static_allocator::static_allocator(memory_protection protection) : m_protection(protection)
{
}

char *static_allocator::allocate(const size_t n)
{
	if (!n)
		return nullptr;

	// Large allocations get their own dedicated contiguous block; the
	// normal path (n <= Pagesize) keeps using the bump-pointer allocator.
	if (n > Pagesize) {
		size_t needed = static_allocator::align(n, Pagesize);
#ifdef _WIN32
		void *block = VirtualAlloc(nullptr, needed, MEM_COMMIT, m_protection);
		if (!block)
			return nullptr;
#else
		void *block = mmap(nullptr, needed, m_protection, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		if (block == MAP_FAILED)
			return nullptr;
#endif

		m_large_pages.push_back(block);
		m_large_pages_sizes.push_back(needed);
		return reinterpret_cast<char *>(block);
	}

	if (!m_pages.size() || m_used + n > Pagesize) {
		if (!allocate_page())
			return nullptr;
	}

	auto ptr = reinterpret_cast<char *>(m_pages.back()) + m_used;
	m_used += n;
	return ptr;
}

char *static_allocator::strdup(const char *string)
{
	size_t len = Q_strlen(string) + 1;
	char *dst = allocate(len);
	if (!dst)
		return nullptr;

	return (char *)Q_memcpy(dst, string, len);
}

void static_allocator::deallocate_all()
{
	for (auto page : m_pages)
#ifdef _WIN32
		VirtualFree(page, 0, MEM_RELEASE);
#else
		munmap(page, Pagesize);
#endif

	for (size_t i = 0; i < m_large_pages.size(); i++)
#ifdef _WIN32
		VirtualFree(m_large_pages[i], 0, MEM_RELEASE);
#else
		munmap(m_large_pages[i], m_large_pages_sizes[i]);
#endif

	m_pages.clear();
	m_large_pages.clear();
	m_large_pages_sizes.clear();
	m_used = 0;
}

size_t static_allocator::memory_used() const
{
	size_t total = m_pages.empty() ? 0 : (m_pages.size() - 1) * Pagesize + m_used;

	for (auto size : m_large_pages_sizes)
		total += size;

	return total;
}

bool static_allocator::contain(uint32 addr) const
{
	for (auto p : m_pages) {
		if (uint32(p) <= addr && addr < uint32(p) + Pagesize)
			return true;
	}

	for (size_t i = 0; i < m_large_pages.size(); i++) {
		if (uint32(m_large_pages[i]) <= addr && addr < uint32(m_large_pages[i]) + m_large_pages_sizes[i])
			return true;
	}

	return false;
}

char *static_allocator::find_pattern(char *pattern, size_t len)
{
	for (auto p : m_pages) {
		for (char *c = (char *)p, *e = c + Pagesize - len; c < e; c++) {
			if (mem_compare(c, pattern, len))
				return c;
		}
	}

	for (size_t i = 0; i < m_large_pages.size(); i++) {
		for (char *c = (char *)m_large_pages[i], *e = c + m_large_pages_sizes[i] - len; c < e; c++) {
			if (mem_compare(c, pattern, len))
				return c;
		}
	}

	return nullptr;
}

bool static_allocator::allocate_page()
{
#ifdef WIN32
	auto page = VirtualAlloc(nullptr, Pagesize, MEM_COMMIT, m_protection);
	if (!page)
		return false;
#else
	auto page = mmap(nullptr, Pagesize, m_protection, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (page == MAP_FAILED)
		return false;
#endif

	m_used = 0;
	m_pages.push_back(page);
	return true;
}

bool mem_compare(const char *addr, const char *pattern, size_t len)
{
	for (auto c = pattern, pattern_end = pattern + len; c < pattern_end; ++c, ++addr) {
		if (*c == *addr || *c == '\x2A')
			continue;
		return false;
	}
	return true;
}

char *mem_find_pattern(char *pos, int range, const char *pattern, size_t len)
{
	for (auto c = pos + range - len; pos < c; ++pos) {
		if (mem_compare(pos, pattern, len))
			return pos;
	}

	return nullptr;
}

char *mem_find_ref(char *pos, char *end, char opcode, uint32 ref, bool relative)
{
	for (; pos < end; ++pos)
	{
		if (*pos == opcode)
		{
			if (relative)
			{
				if ((uint32)pos + 5 + *(uint32 *)(pos + 1) == ref)
					return pos;
			}
			else
			{
				if (*(uint32 *)(pos + 1) == ref)
					return pos;
			}
		}
	}

	return nullptr;
}

char *mem_find_string_push(char *addr, const char *string, size_t len)
{
	char *ptr = mem_find_pattern(addr, len, string, Q_strlen(string) + 1);
	if (!ptr)
		return nullptr;

	return mem_find_ref(addr, addr + len - 5, '\x68', (uint32)ptr, false);
}
