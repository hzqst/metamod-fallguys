---
title: component_repository_migration
type: note
permalink: metamod-fallguys/component-repository-migration
---

# Component repository migration (Issue #44)

## Trigger and constraints

The aggregate's global include paths and preinstalled static libraries prevented its
components from building independently. Preserve runtime API names, binary layouts,
existing dependency revisions, author history and the addons/metamod installation tree.

## Repository ownership

Six root component directories are gitlinks to their matching repositories under the
metamod-svencoop organization. Public headers are in include/, private implementation
in src/. Metamod owns include/HLSDK/ and nested thirdparty/capstone_fork/procmap_fork.
ASExt owns include/asext_api.h and include/std_string.h. ASCurl retains curl/OpenSSL
vendor trees and FallGuys retains FMOD headers and owns Bullet as a nested
thirdparty/bullet3_fork submodule. The aggregate shares the customized AngelScript
SDK and FormatValidation submodules.

Histories were filtered only in temporary clones; the original aggregate history was
not rewritten. Layout commits were checked against original Git blobs before formatting.
Keep the customized AngelScript SDK: its asSFuncPtr layout fixes are required by the host.

## Correct build practice

- Every component supports standalone CMake and add_subdirectory integration.
- Explicit *_SOURCE_PATH CMake values win over environment defaults; omitted values use
  pinned FetchContent. Invalid explicit paths fail. Metamod's initialized private vendor
  submodules are reused before fetching their same commits.
- Metamod::SDK and ASExt::SDK expose headers without creating native dependency targets.
  FallGuys consumes Capstone headers; Metamod alone links its static library.
- Source-only FetchContent uses SOURCE_SUBDIR mmfg-source-only and disables recursive
  vendor checkout. The actual vendor libraries are added separately with target guards.
- Set CMAKE_POLICY_DEFAULT_CMP0077=NEW around old vendor projects: otherwise their
  option() calls discard normal overrides and silently enable unwanted Capstone architectures.
- Linux static-runtime link options belong to each plugin target, never to a sibling
  metamod target. Preserve Metamod's internal regparm convention and x86 compiler detection.
- Commit and publish dependency repositories first, then pin their commits in consumers
  and update aggregate gitlinks. Do not use local URL rewrites in delivered configuration.

## Formatting

FormatValidation 13c9fabe058e1f887ad1b03bb6884de911192c6a pins clang-format 23.1.3.
Use FORMAT_VALIDATION_ONLY=ON with format-check/format; native compilers are unnecessary.
The aggregate covers all components even when disabled for native builds. Generated
.clang-format is ignored; HLSDK and vendor trees retain their original contents.

## Verification practice

Exercise all six components on Windows Win32 and Linux i386, Debug/Release, using both
local source overrides and default FetchContent. Check aggregate builds and installs,
invalid overrides, offline reuse of populated dependencies, compiler-free formatting,
plugin-only target trees, export sets, 32-bit binaries and Windows PDBs. Finally verify
fresh recursive clones and public-URL dependency resolution after publishing.

Migration verification scripts and raw logs are retained outside the repository in
D:/mmfg-issue44/verification. Native and formatting CI perform fresh public clones for
each component. Build evidence does not replace an in-game Sven Co-op runtime check.

## Applicability

Use this pattern when changing component boundaries, SDK include dependencies,
FetchContent pins or aggregate build orchestration. Preserve fixed third-party revisions
and vendor content unless a separate change explicitly updates them.

## Organization and aggregate repository rename (2026-10-20)

- Trigger: the organization was renamed to `metamod-svencoop`, and the aggregate moved from `hzqst/metamod-fallguys` to `metamod-svencoop/metamod-svencoop`.
- Constraint: GitHub URL redirects do not update literal `github.repository` checks in workflows; the old checks skip Windows, Ubuntu and release build jobs after transfer.
- Correct practice: use `https://github.com/metamod-svencoop/metamod-svencoop.git` for aggregate origin, update all four workflow checks, and migrate current documentation, plugin homepage URLs and component CMake dependency URLs. Keep existing pinned revisions and vendor sources. Commit component changes separately before updating aggregate gitlinks.
- Local path: `D:/metamod-fallguys` remains in use. The user deferred renaming it to `D:/metamod-svencoop` because active workspace/MCP processes hold the directory open. After closing those processes, rename the directory, update the Basic Memory project path, and configure fresh build trees rather than reuse CMake caches containing the old absolute path. Preserve the existing Basic Memory project name/permalinks unless explicitly migrating them.
- Verification: Windows Win32 Debug and Release configure/build succeeded; each CTest run passed 10 tests and skipped the optional blob catalog test. Release scripts passed 42 unit tests; actionlint passed for the three changed workflows; aggregate clang-format 23.1.3 checked 186 files with zero differences after aligning plugin-info comments. Linux and hosted GitHub Actions were not run locally.
- Applicability: repository identity and URL migration; existing package names, CMake project identifiers and runtime plugin/API names are preserved.
