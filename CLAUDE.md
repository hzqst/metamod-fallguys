# CLAUDE.md

This file guides how to perform Agent Coding in this repository using progressive disclosure.

## Serena memories (keep context lean)

1. Use `list_memories` first to check existing memories in the current project (do not read all by default).
2. Only when needed, use `read_memory` to precisely read a specific memory file (on-demand loading).
3. If memory information is insufficient or outdated, then fall back to targeted repository file reads or ContextEngine/symbol/search-based lookup, and maintain memory content with `write_memory`, `edit_memory`, or `delete_memory`.

## High-level repository information (prefer corresponding memories first)

The following content was already organized into Serena memories during onboarding, so it is not duplicated here:

- Project purpose/background: `project_overview`
- Tech stack: `tech_stack`
- Directory structure and module breakdown: `project_structure`
- Common development commands: `suggested_commands`
- Code style and conventions: `code_style_conventions`
- Development guidelines and caveats: `development_guidelines`
- Post-task checklist: `task_completion_checklist`
- Module topic: `asext`

## "Source file" entry points when memories are insufficient (query/read on demand)

- Project docs/plugin docs: `README.md`, `README.txt`, `README_FALLGUYS.md`, `README_ASEXT.md`, `README_ASCURL.md`, `README_ASQCVAR.md`, `README_ASUSERMSG.md`, `README_CMAKE.md`
- Build and configuration entry points: `CMakeLists.txt`, `Makefile`, `Config.mak`, `metamod.sln`, `scripts/`
- Main module source code: `metamod/`, `fallguys/`, `asext/`, `ascurl/`, `asqcvar/`, `asusermsg/`
- Tools and CI: `tools/`, `.github/workflows/`
- Large directories (avoid full reads): `thirdparty/`, `build/`, `build-cmake/`, `intermediate/`, `output/`, `install/`, `Release/`, `Debug/`, `.vs/`

## Progressive disclosure key points

- Read memories first, then locate single files/symbols; avoid reading the whole repository at once.
- For symbol/binary-related directories, prioritize on-demand targeted lookup and avoid full scans.

## Misc rules

- Always `activate_project` on agent startup.
