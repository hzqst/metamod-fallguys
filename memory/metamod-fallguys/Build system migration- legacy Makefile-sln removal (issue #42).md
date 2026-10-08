---
title: 'Build system migration: legacy Makefile/sln removal (issue #42)'
type: note
permalink: metamod-fallguys/build-system-migration-legacy-makefile-sln-removal-issue-42
tags:
- build
- cmake
- makefile
- placeholder
- glibc
- issue-42
---

# Build system migration (issue #42)

## Root cause: the `.placeholder` files

Two tracked zero-byte files existed:

- `metamod/opt.linux_i386/CDetour/asm/.placeholder`
- `build/addons/metamod/dlls/.placeholder`

They were **not** functional. The GNU Make build flattens a source path into the
object path (`metamod/Makefile`):

```
OBJ_LINUX := $(SRCFILES:%.cpp=$(OBJDIR_LINUX)/%.o)
# CDetour/asm/asm.cpp -> opt.linux_i386/CDetour/asm/asm.o
```

but the Makefile only ever created the **top-level** `opt.linux_i386` directory.
It never created the nested `CDetour/` and `CDetour/asm/` directories, so gcc
failed on a fresh checkout with:

```
Fatal error: can't create opt.linux_i386/CDetour/asm/asm.o: No such file or directory
```

Because git cannot track empty directories, committing a 0-byte file was the
hack that forced git to materialise the parent directory chain on checkout.

## Fix

CMake does not have this problem: it derives object paths as
`CMakeFiles/<target>.dir/<source>/x.o` and **creates all parent directories
itself**, and there is no "tracked empty directory" concept at all. So the whole
class of bug disappears once the legacy build systems are gone.

Removed:

- `Makefile`, `Config.mak` (root + every module), all `*/Makefile`, `*/Config.mak`
- `metamod.sln`, all `*.vcxproj` / `*.vcxproj.filters` / `*.vcxproj.user`
- `metamod/i386pe.merge` (Make-only linker script), `metamod/build_all.sh`
- `scripts/build-metamod-make-*.sh`, `scripts/build-metamod-msvc-*.bat`
- both `.placeholder` files
- tracked build output (`*/opt.linux_i386/*`, `*/msgs/*`)

## CMake flag parity (must NOT be lost)

CMake now mirrors the old Make `OPT=opt` path, otherwise a "Release" .so would
silently differ:

- `-fno-exceptions -fno-rtti -fvisibility=hidden`
- `-march=i686 -mtune=generic -msse -msse2`
- (the glibc pin `-include thirdparty/glibc_224/...` was later REMOVED entirely, see below)
- metamod + i386 + non-Debug: `-D__INTERNALS_USE_REGPARAMS__`

`-fvisibility=hidden` is safe because exported entry points use
`__attribute__((visibility("default")))` via `DLLEXPORT` (`metamod/osdep.h`).


## glibc pinning removed (follow-up)

`force_link_glibc_2.24.h` never worked: both the old Make build and the CMake
build still emitted symbols above 2.24 (`arc4random@2.36`, `__isoc23_sscanf@2.38`,
`__libc_single_threaded@2.32`) coming from the statically linked
capstone/bullet3/procmap archives built on a newer glibc. Rather than fixing it,
the whole mechanism was deleted:

- `thirdparty/glibc_224/force_link_glibc_2.24.h`
- the `-include` option in the root CMakeLists
- `-DLINK_AGAINST_OLDER_GLIBC=TRUE -DOLDER_GLIBC_PATH=...` from all
  `scripts/build-*.sh`
- the README bullet advertising 2.24 portability

Linux builds now follow the build host glibc.

Also removed in the same cleanup: the MSVC debug helper chain
(`scripts/debug-helper-AIO.bat`, `scripts/debug-SvenCoop.bat`) and its orphaned
tools (`tools/vswhere.exe`, `tools/SteamAppsLocation.exe`,
`tools/global_template.props`, `tools/global_common.props`, `tools/steam_api.dll`),
all of which existed only to configure MSVC properties / launch the deleted
`metamod.sln`. The whole `tools/` directory was then removed too:
`tools/stlfilter` was only ever invoked by the deleted `metamod/Makefile`
(`$(STLOBJ): FILTER= 2>&1 | ../tools/stlfilter`), and `tools/getents.sh` had no
remaining user.

## Repository hygiene added

- `.gitattributes`: `*.sh text eol=lf` (CRLF broke `sh script.sh` under WSL with
  a corrupt trailing `\r`), `*.bat/*.cmd eol=crlf`, binaries marked `binary`.
- `.gitignore`: build output, `opt.*/`, `debug.*/`, `msgs/`, `*.o/*.so/*.a/*.lib/*.pdb/*.exp`.
- `metamod/.gitignore` deleted (rules moved to root).

## Verification

`scripts/build-all-opt.linux_i386.sh` under WSL2: exit 0, 0 errors, produces all
six 32-bit ELF `.so` (metamod, asext, ascurl, asqcvar, asusermsg, fallguys).
