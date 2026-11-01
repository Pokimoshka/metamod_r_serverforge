#pragma once

class static_allocator
{
public:
	enum memory_protection : uint8
	{
#ifdef _WIN32
		mp_readwrite = PAGE_READWRITE,
		mp_rwx = PAGE_EXECUTE_READWRITE
#else
		mp_readwrite = PROT_READ | PROT_WRITE,
		mp_rwx = PROT_READ | PROT_WRITE | PROT_EXEC
#endif
	};

	static_allocator(memory_protection protection);
	char *allocate(const size_t n);
	char *strdup(const char *string);
	void deallocate_all();
	size_t memory_used() const;
	bool contain(uint32 addr) const;
	char *find_pattern(char *pattern, size_t len);

	template<typename T>
	T *allocate()
	{
		return (T *)allocate(sizeof(T));
	}

private:
	static size_t align(size_t v, size_t a)
	{
		return (v + a - 1) & ~(a - 1);
	}

	bool allocate_page();

	enum
	{
		Pagesize = 4096
	};

	size_t m_used = 0;
	std::vector<void *> m_pages;
	std::vector<void *> m_large_pages;			// contiguous blocks bigger than one page
	std::vector<size_t> m_large_pages_sizes;	// their sizes, for freeing
	memory_protection m_protection;

	friend class CJit;
};

bool  mem_compare(const char *addr, const char *pattern, size_t len);
char *mem_find_pattern(char *pos, int range, const char *pattern, size_t len);
char *mem_find_ref(char *pos, char *end, char opcode, uint32 ref, bool relative);
char *mem_find_string_push(char *addr, const char *string, size_t len);
