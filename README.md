# [Metamod-r](https://github.com/rehlds/metamod-r) [![Percentage of issues still open](http://isitmaintained.com/badge/open/rehlds/metamod-r.svg)](http://isitmaintained.com/project/rehlds/metamod-r "Percentage of issues still open") [![GitHub license](https://img.shields.io/github/license/rehlds/metamod-r.svg?longCache=true&style=flat-square)](https://github.com/rehlds/metamod-r/blob/master/LICENSE)

**Metamod-r** is based on the original version of [Metamod](http://metamod.org/) written by _Will Day_ for Half-Life 1 with some improvements from [Jussi Kivilinna](https://github.com/jkivilin) ([Metamod-p](https://github.com/jkivilin/metamod-p)). This product contains a large number of performance optimizations and more pure code. The core was written using JIT compiler.

**Metamod-r is recommended to be run with [ReHLDS](https://github.com/rehlds/ReHLDS) (`API 3.1+`).
There is compatible with original `HLDS`**

## metamod_r_serverforge fork

This repository is a maintained fork of Metamod-r for the ServerForge CS 1.6
infrastructure. It keeps the full upstream history and adds bug fixes and
extra functionality on top of the latest upstream release.

### Fixed
- Fixed stack buffer overflow in `META_CONS`, `META_CLIENT` and
  `mutil_LogConsole` when a log message is longer than the internal buffer
  (`Q_vsnprintf`/`_vsnprintf` returns the would-be length on truncation and
  was used as an index without clamping).
- Fixed missing format argument in the "forced unload overriding failed
  detach" debug message (`mplugin.cpp`).
- Fixed `meta require`: the server now actually exits when a required plugin
  is missing or failed to load (the original `do_exit(1)` behavior was lost).
- Fixed "usage: meta cvars" typo in the `meta config` usage message.
- Fixed stray `b>` prefix that corrupted buffered log messages printed
  before the engine function table is available.
- Fixed undefined behavior from passing negative `char` values to `isspace`
  and `isdigit` (`trimbuf`, config parser).
- Fixed out-of-bounds read in `is_abs_path` for empty paths.
- Fixed a null-pointer crash when `/proc/self/maps` cannot be opened on Linux
  (`CSysModule::load`).
- Fixed unchecked `VirtualAlloc`/`mmap` failures and an unsigned underflow in
  `memory_used` in the JIT static allocator; large JIT allocations now get a
  dedicated contiguous block.
- Fixed undefined behavior (signed left-shift overflow) in
  `mutil_MakeRequestId`.
- Fixed a potential out-of-bounds write in `MPlayerList` for mods with
  `maxClients` greater than `MAX_CLIENTS`.
- Added missing allocation failure checks in the Win32 linkent export table
  combiner.
- Fixed "chache" typo in the gamedll install log message.
- `MConfig` now has a destructor and no longer leaks `m_filename` when the
  config file is loaded more than once.

### Added
- `meta debug <level>` - set the metamod debug level at runtime.
- `meta config save` - write the current config options back to `config.ini`
  (written atomically via a temp file + rename).
- `meta list <plugin>` - list only the child plugins loaded by the given
  plugin (by index or non-ambiguous prefix).
- `meta pause` / `meta unpause` without arguments now pause/unpause all
  running/paused plugins at once.
- `meta` commands are now case-insensitive, matching the original Metamod
  behavior.

|HLDS | [ReHLDS](https://github.com/rehlds/ReHLDS) |  OS | Download |
|---------| -------|    --- | ---   |
| :heavy_check_mark: | :heavy_check_mark: `API 3.1+` |![](https://i.imgur.com/AzhAYR4.png)   ![](https://i.imgur.com/t23p9tU.png)   |  [![Download](https://img.shields.io/github/downloads/rehlds/metamod-r/latest/total?sort=date&style=flat-square)](https://github.com/rehlds/metamod-r/releases/latest)

[![Official Site](https://img.shields.io/badge/Link-Official%20site-3CB371.svg?longCache=true&style=flat-square)](https://metamod-r.org/)
[![Stable](https://img.shields.io/badge/status-stable-default.svg?style=flat-square)](https://github.com/rehlds/metamod-r/)
[![GitHub issues](https://img.shields.io/github/issues/rehlds/metamod-r.svg?longCache=true&style=flat-square)](https://github.com/rehlds/metamod-r/issues)
[![GitHub forks](https://img.shields.io/github/forks/rehlds/metamod-r.svg?longCache=true&style=flat-square)](https://github.com/rehlds/metamod-r/network)
[![GitHub stars](https://img.shields.io/github/stars/rehlds/metamod-r.svg?longCache=true&style=flat-square)](https://github.com/rehlds/metamod-r/stargazers)

## Documentation
* All actual documentation in ![en](https://i.imgur.com/rm67tUZ.png) **English** and ![ru](https://i.imgur.com/ItziiKg.png) **Russian** languages is placed at [this link](https://github.com/rehlds/metamod-r/wiki).

### Supported games
* ![en](https://i.imgur.com/rm67tUZ.png) Actual [list of supported games](https://github.com/rehlds/metamod-r/wiki/Supported-games).
* ![ru](https://i.imgur.com/ItziiKg.png) Актуальный [список поддерживаемых игр](https://github.com/rehlds/metamod-r/wiki/Поддерживаемые-игры).

## Build instructions
### Checking requirements
There are several software requirements for building Metamod-r:

#### Windows
<pre>
Visual Studio 2015 (C++14 standard) and later
</pre>

#### Linux
<pre>
git >= 1.8.5
cmake >= 3.10
GCC >= 4.9.2 (Optional)
ICC >= 15.0.1 20141023 (Optional)
LLVM (Clang) >= 6.0 (Optional)
</pre>

### Building

#### Windows
Use `Visual Studio` to build, open `msvc/metamod.sln` and just select from the solution configurations list `Release` or `Debug`

#### Linux

* Optional options using `build.sh --compiler=[gcc] --jobs=[N] -D[option]=[ON or OFF]` (without square brackets)

<pre>
-c=|--compiler=[icc|gcc|clang]  - Select preferred C/C++ compiler to build
-j=|--jobs=[N]                  - Specifies the number of jobs (commands) to run simultaneously (For faster building)

<sub>Definitions (-D)</sub>
DEBUG                           - Enables debugging mode
USE_STATIC_LIBSTDC              - Enables static linking library libstdc++
</pre>

* ICC          <pre>./build.sh --compiler=intel</pre>
* LLVM (Clang) <pre>./build.sh --compiler=clang</pre>
* GCC          <pre>./build.sh --compiler=gcc</pre>

##### Checking build environment (Debian / Ubuntu)

<details>
<summary>Click to expand</summary>

<ul>
<li>
Installing required packages
<pre>
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install -y gcc-multilib g++-multilib
sudo apt-get install -y build-essential
sudo apt-get install -y libc6-dev libc6-dev-i386
</pre>
</li>

<li>
Select the preferred C/C++ Compiler installation
<pre>
1) sudo apt-get install -y gcc g++
2) sudo apt-get install -y clang
</pre>
</li>
</ul>

</details>
