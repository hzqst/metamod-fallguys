# Project Overview

## Purpose
This is a modified version of metamod (based on Bots-United's metamod-p) along with a group of metamod plugins, mainly designed to provide server-side feature expansions for Sven Co-op game.

The project includes:
- **metamod**: Core metamod library with enhanced APIs and compatibility fixes
- **fallguys**: Plugin required by "Fall Guys in Sven Co-op" map
- **asext**: AngelScript extension plugin for registering third-party hooks/methods
- **ascurl**: libcurl HTTP request support for AngelScript
- **asqcvar**: Client cvar retrieval plugin
- **asusermsg**: UserMsg hooking plugin

## Key Features
- Compatible with third-party plugins like amxmodx
- Designed specifically for Sven Co-op (compatibility with other games not guaranteed)
- Portable Linux binaries using older glibc version
- Fallback solution for GiveFnptrsToDll issues with newer Visual Studio versions
- Enhanced mutil API set

## Related Projects
- [Fall Guys in Sven Co-op](https://github.com/hzqst/sven-fallguys)
- Based on [Bots-United's metamod-p](https://github.com/Bots-United/metamod-p)
