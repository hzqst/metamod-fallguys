---
title: task_completion_checklist
type: note
permalink: metamod-fallguys/task-completion-checklist
---

# Task Completion Checklist

## When a Task is Completed

### 1. Code Quality Checks
- [ ] Ensure code follows project style conventions (see `code_style_conventions.md`)
- [ ] Run the component or aggregate `format-check` target
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
Use shared FormatValidation and clang-format 23.1.3. Configure
`FORMAT_VALIDATION_ONLY=ON` and build `format-check`; `format` applies corrections.
Normal native builds do not run formatting. Generated `.clang-format` is ignored.

## Common Pitfalls to Avoid
- Don't change the pinned format configuration locally; use the shared tooling
- Don't forget platform-specific preprocessor directives
- Don't break compatibility with third-party plugins
- Preserve imported SDK contents under `metamod/include/HLSDK/`
- Don't commit build artifacts (they're in .gitignore)
