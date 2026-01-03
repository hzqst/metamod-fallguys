# Task Completion Checklist

## When a Task is Completed

### 1. Code Quality Checks
- [ ] Ensure code follows project style conventions (see `code_style_conventions.md`)
- [ ] Check indentation (tabs, not spaces)
- [ ] Verify naming conventions are followed
- [ ] Add appropriate comments and documentation
- [ ] Include proper copyright headers if creating new files

### 2. Build Verification
**Important**: Only build if explicitly requested by the user or if the changes are critical.

#### Windows Build
```batch
cd scripts
build-all-x86-Release.bat
```

#### Linux Build
```bash
cd scripts
./build-all-opt.linux_i386.sh
```

### 3. Testing
**Note**: This project does not have automated unit tests. Testing is done manually by:
- Loading the plugins in Sven Co-op server
- Verifying plugin functionality in-game
- Checking server console for errors

### 4. Documentation
- [ ] Update relevant README files if adding new features
- [ ] Document any new APIs or hooks
- [ ] Update configuration examples if needed

### 5. Git Workflow (if applicable)
- [ ] Review changes: `git diff`
- [ ] Check status: `git status`
- [ ] Stage changes: `git add <files>`
- [ ] Commit with descriptive message: `git commit -m "message"`
- [ ] Push if needed: `git push`

## Important Notes

### DO NOT Automatically Run
- **Build commands** - Only build when explicitly requested
- **Test commands** - No automated tests exist
- **Deployment** - Installation is manual (see README.md)

### DO Verify
- Code compiles without errors (if build is requested)
- No obvious syntax errors
- Changes align with project conventions
- Documentation is updated if needed

### Platform Considerations
- Remember this is a **Windows** development environment
- Use Windows-style paths with backslashes when needed
- Use appropriate Windows commands (dir, type, findstr, etc.)
- Git commands work through Git for Windows

## Linting and Formatting
**Note**: This project does not use automated linting or formatting tools.
- Follow the style conventions manually
- Use vim/Visual Studio formatting settings
- Refer to existing code as examples

## Common Pitfalls to Avoid
- Don't use spaces for indentation (use tabs)
- Don't forget platform-specific preprocessor directives
- Don't break compatibility with third-party plugins
- Don't modify Half-Life SDK files (hlsdk/ is a submodule)
- Don't commit build artifacts (they're in .gitignore)
