# Development Guidelines

## Design Patterns and Principles

### Plugin Architecture
- **Plugin System**: Metamod uses a plugin architecture where each plugin is a separate DLL/SO
- **Hook Chain**: Plugins can hook into game engine functions and form a chain of handlers
- **Meta Return**: Plugins use `META_RES` return values to control hook chain execution
  - `MRES_IGNORED`: Continue to next plugin
  - `MRES_HANDLED`: Mark as handled but continue
  - `MRES_OVERRIDE`: Override return value
  - `MRES_SUPERCEDE`: Stop chain and use this result

### Memory Management
- Manual memory management (no smart pointers in legacy code)
- Be careful with DLL boundaries (don't free memory across DLL boundaries)
- Use engine's memory allocation functions when appropriate

### Platform Abstraction
- Use `osdep.h` and `osdep.cpp` for platform-specific code
- Separate Windows and Linux implementations in different files:
  - `osdep_*_win32.cpp` for Windows
  - `osdep_*_linux.cpp` for Linux
- Use preprocessor directives for platform-specific code

### API Design
- **mutil API**: Utility functions provided to plugins
- **meta_api**: Core metamod API for plugin interaction
- **Engine API**: Wrapped engine functions for plugins
- Maintain backward compatibility with existing plugins

## Coding Practices

### Error Handling
- Use logging functions from `log_meta.h`
- Log levels: ERROR, WARNING, INFO, DEBUG
- Return appropriate error codes
- Don't crash the server - handle errors gracefully

### Thread Safety
- Be aware of multi-threading in server environment
- Use appropriate synchronization when needed
- Engine callbacks may be called from different threads

### Performance Considerations
- Minimize overhead in frequently called hooks
- Cache lookups when possible
- Avoid unnecessary string operations
- Be mindful of memory allocations in hot paths

## AngelScript Integration (asext plugin)

### Hook Registration
- Plugins can register custom hooks in AngelScript engine
- Use asext API to extend AngelScript functionality
- Follow AngelScript calling conventions

### Script Callbacks
- Properly handle script exceptions
- Validate script function signatures
- Clean up script contexts appropriately

## Compatibility Requirements

### Third-Party Plugin Compatibility
- Must remain compatible with amxmodx and other metamod plugins
- Don't break existing plugin APIs
- Maintain ABI compatibility

### Game Compatibility
- Primary target: Sven Co-op
- Other Half-Life mods may work but are not guaranteed
- Test with target game version

### Compiler Compatibility
- Support Visual Studio 2017/2019/2022 on Windows
- Support GCC on Linux
- Use C++17 features cautiously (check compiler support)

## Security Considerations

### Input Validation
- Validate all external input (config files, commands, etc.)
- Check buffer sizes before copying
- Sanitize file paths

### DLL Loading
- Verify DLL signatures when possible
- Check for malicious plugins
- Sandbox plugin operations when feasible

## Debugging Tips

### Windows Debugging
- Use Visual Studio debugger
- Attach to game server process
- Use debug builds for detailed information
- Check Windows Event Viewer for crashes

### Linux Debugging
- Use GDB for debugging
- Check core dumps
- Use valgrind for memory issues
- Monitor server console output

### Common Issues
- **Plugin load failures**: Check plugins.ini syntax and paths
- **Crashes on startup**: Verify DLL dependencies
- **Hook not working**: Check hook registration and return values
- **Memory leaks**: Use memory profiling tools

## Contributing Guidelines

### Before Making Changes
1. Understand the existing code structure
2. Check if similar functionality exists
3. Consider backward compatibility
4. Plan for both Windows and Linux

### Code Review Checklist
- [ ] Follows project coding style
- [ ] Works on both Windows and Linux (if applicable)
- [ ] Doesn't break existing plugins
- [ ] Includes appropriate error handling
- [ ] Has necessary documentation
- [ ] Tested in actual game environment

### Documentation Requirements
- Update README files for new features
- Document new APIs in header files
- Add examples for complex functionality
- Update installation instructions if needed

## Special Considerations

### Half-Life Engine Quirks
- Engine uses C-style APIs
- Be aware of engine version differences
- Some functions may behave differently in Sven Co-op
- Engine callbacks have specific calling conventions

### Metamod-Specific
- Plugin load order matters
- Some hooks are called very frequently
- Meta return values control execution flow
- Plugins share the same address space

### Build System Notes
- CMake is the preferred build system
- Visual Studio projects are maintained for convenience
- Makefiles exist for legacy support
- Third-party libraries must be built first
