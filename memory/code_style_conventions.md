---
title: code_style_conventions
type: note
permalink: metamod-fallguys/code-style-conventions
---

# Code Style and Conventions

## File Headers
- Copyright headers with GNU GPL license information
- Author attribution and modification history

## Indentation and Formatting
- Shared FormatValidation pins clang-format 23.1.3 and uses four spaces, no tabs.
- ColumnLimit is 0; include sorting is disabled to preserve existing include order.
- Run `format-check` or `format` after configuring `FORMAT_VALIDATION_ONLY=ON`.
- Generated `.clang-format` files are ignored; HLSDK and vendor sources are excluded.

## Naming Conventions
- **Types/Structs**: lowercase with underscores, suffix `_t` or `_s`
  - Examples: `gamedll_t`, `gamedll_s`, `engine_t`
- **Classes**: PascalCase with 'M' prefix for metamod classes
  - Examples: `MPluginList`, `MConfig`, `MRegCmdList`, `MPlayerList`
- **Functions**: lowercase with underscores
  - Examples: `game_autodetect`, `link_game`, `log_meta`
- **Variables**: lowercase with underscores
  - Examples: `metamod_handle`, `meta_version`, `gpGlobals`
- **Global variables**: Often prefixed with 'g_'
  - Examples: `g_engfuncs`, `g_plugin_engfuncs`
- **Constants/Macros**: UPPERCASE with underscores
  - Examples: `PLUGINS_INI`, `NAME_MAX`, `PATH_MAX`

## Header Guards
- Use `#ifndef` / `#define` / `#endif` pattern
- Format: `FILENAME_H` (uppercase)
  ```cpp
  #ifndef METAMOD_H
  #define METAMOD_H
  // ...
  #endif
  ```

## Comments
- Use both C-style (`/* */`) and C++-style (`//`) comments
- Multi-line comments typically use C-style
- Single-line comments use C++-style
- File descriptions at the top of files
- License information in block comments

## Include Order
1. Compiler/platform compatibility headers (`comp_dep.h`)
2. API headers (`meta_api.h`)
3. Internal headers (alphabetically)
4. System headers (if needed)

## Platform-Specific Code
- Use preprocessor directives for platform differences:
  ```cpp
  #ifdef WIN32
  // Windows-specific code
  #elif defined(__linux__)
  // Linux-specific code
  #endif
  ```
- Common defines: `WIN32`, `_WINDOWS`, `LINUX`, `_LINUX`, `PLATFORM_POSIX`

## Visibility Attributes
- Use `DLLHIDDEN` macro for internal symbols
- Use `DLLEXPORT` for exported symbols
- Example: `extern gamedll_t GameDLL DLLHIDDEN;`

## Type Definitions
- Use `typedef` for struct types
- Define both struct tag and typedef:
  ```cpp
  typedef struct gamedll_s {
      // members
  } gamedll_t;
  ```

## Boolean Type
- Custom boolean type: `mBOOL` (defined in `types_meta.h`)
- Not using standard C++ `bool` for compatibility

## File Extensions
- Header files: `.h`
- Source files: `.cpp`
- Resource files: `.rc` (Windows)
- Definition files: `.def` (Windows DLL exports)

## Documentation
- Inline comments for complex logic
- Function/method documentation typically in header files
- README files for each major component/plugin
