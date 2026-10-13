---
title: gamedata-signature-parity
type: note
permalink: metamod-fallguys/gamedata-signature-parity
tags:
- gamedata
- signature
- issue-55
- parity
- asext
- fallguys
---

# gamedata-signature-parity — issue #55 stage 1: replay of hand-written signatures vs GoldSrc_VibeSignatures gamedata

## Trigger signal
Before migrating asext/fallguys private-symbol lookup to GoldSrc_VibeSignatures gamedata (issue #55), we must confirm that the existing hand-written signatures / caller offsets / Linux symbols / GOT parsing resolve to the same addresses as the published gamedata; only then is migration safe.

## Method (2026-10-13)
- Temporary script `%TEMP%/issue55/verify_sigs_vs_gamedata.py` (not committed). It replays the exact `signatures_template.h` + `mutil.cpp` semantics: `SearchPattern` (first match, `0x2A` wildcard, code range = PE RX sections / ELF executable PT_LOAD), `GetNextCallAddr(addr,1)` (`E8`/`FF 15` only at addr itself), `CallStart/CallEnd`, `FromFunc`, `VarStart/VarEnd`, Linux `dlsym` (`.dynsym`), plus the Linux hand-written GOT paths (`gotplt_prolog`, `sv_model`, `host_frametime`, `DisasmRanges` for `pmovevars`, `PF_SetGroupMask`, `CASHook_VCall`), using capstone.
- Binaries: `D:/GoldSrc_VibeSignatures/bin/svencoop-{8948,10257}/{server,engine}`. Expected values: the published `https://hlnd2t.github.io/GoldSrc_VibeSignatures/gamesymbols/` datasets (2026-10-12T14:53:54Z), SHA-256 verified; matched by `(module, artifact, platform)`. Local `bin_artifacts` agree with the published data (0 differences).
- CRC-64/XZ of all 8 binaries matches `binaries.<module>.<platform>.crc64`.
- Branch reachability: Linux server takes the 5.16 signature branch if it contains `SCServerDLL003` (10257), otherwise 5.15 symbols (8948). Linux engine takes the symbol path if it exports `_Z12build_numberv` (8948), otherwise i686 signatures (10257 hw.so).
- Results for reachable branches: 224 rows → 206 MATCH, 10 MISMATCH, 8 NOT_FOUND. Every non-MATCH was adjudicated in IDA (ida-pro-mcp headless); **every disagreement is a fault on our side; the gamedata had no errors.**

## Disagreements and conclusions
| # | Version/platform | Symbol | Ours | gamedata | Conclusion |
|---|---|---|---|---|---|
| 1 | 8948+10257 Windows | `CScriptAny_Release` | same address as `CScriptArray_Release` (10257: 0x349570) | 0x3482c0 | **Ours wrong**: the signature lands on `CScriptArray::Release` (refCount at +4, gcFlag at +8, userFree). The gamedata address is the real `CScriptAny::Release` (refCount at +8, gcFlag at +12, delete this). Affects the public export `ASEXT_CScriptAny_Release`. |
| 2 | 8948+10257 Windows | `pg_groupop` / `pg_groupmask` | opcode bytes (`8B 0D..`/`8B 15..`) | 0x685b6bc / 0x685b6b8 (10257) | **Ours wrong**: commit `0e98002` (2024-10) lengthened the signature to 21 bytes but set the offsets to END-8/-2; the correct values are END-6/+0. `PF_SetGroupMask`=`sub_1D80020` confirms mask=+0x6b8, op=+0x6bc. `SV_ClipToLinksEx` (`fallguys/src/engine_hook.cpp:177-185`) dereferences `*pg_groupop` when groupinfo is non-zero, reading an illegal address. |
| 3 | 10257 Linux server | `CString_dtor` | 0x1787a2 | 0x177780 | **Ours wrong**: 0x177780 has 691 callers (including `DestructASString`) and is the real `~CString`; 0x1787a2 has 0 direct callers and is a free+reset helper (also frees the heap buffer, so runtime impact is small). |
| 4 | 10257 Linux server | `CScriptDictionary_GetSize` | 0xd4ecc (8 matches, first taken) | 0xf11aa | **Ours wrong identity**: the bytes are identical (`8B 44 24 04 8B 40 1C C3`), so behavior is the same; the gamedata address is the one `RegisterScriptDictionary_Native` registers. |
| 5 | 10257 Linux hw.so | `SV_SingleClipMoveToEntity_10152` | 0x182cd0 (8 matches, first taken) | 0xdf1f0 | **Ours wrong**: 0x182cd0 is a video-mode helper (`strncpy(dest,"gl")`); 0xdf1f0 is the real 7-argument `SV_SingleClipMoveToEntity`. |
| 6 | 10257 Linux hw.so | `pmovevars` | 0x2ee020 (`.got.plt`) | 0x464ba0 (`.bss`) | **Ours wrong**: `DisasmCallback_FindGotPltTarget` treats the first `mov esi,[esp+0x20]` as a GOT offset; the real reference is `lea edi,[ebx+0x176ba0]` (GOT 0x2ee000 + 0x176ba0 = 0x464ba0). |
| 7 | 8948 Windows | `asGetActiveContext`, `CASBaseManager_GetTypeInfoByName`, `CScriptBuilder_DefineWord`, `CScriptDictionary_Set/DeleteAll/begin/CIterator_GetValue` | not found | present | **Our Windows signatures do not support 8948**: the plugin cannot load on Windows 8948; gamedata covers it. |
| 8 | 8948 Linux (5.15 symbol branch) | `asGetActiveContext` | **never filled** in the code | present | **Our code gap**: the 5.15 branch has no `asGetActiveContext`, so `ASEXT_GetCurrentContext()` calls NULL. |

## Constraints found for stage 2
- **symbolName is not a stable key**: for C++ server functions the published `symbolName` is the `func_name` demangled signature, and it differs across platforms (Windows `unsigned int` / `std::string` vs Linux `unsigned long` / `std::__cxx11::basic_string`; Linux `CPlayerMove_PlayStepSound` is even the raw mangled name). `g_pServerManager` → `CASServerManager::GetInstance()::pInstance`. Only `artifact` (= the config name) is stable across platforms. MetaHookSv `GameData.cpp` keys only by `symbolName` (and ignores `aliases`), so porting it unchanged would require per-platform names.
- Upstream supports a per-symbol `alias` in config (`gamesymbols_json.py:_attach_aliases` → record `aliases`).
- **Dedicated-server engine coverage**: upstream svencoop only has client app 225840 (`hw.dll`/`hw.so`). On Windows, Sven Co-op Dedicated Server also loads `hw.dll`, with the same CRC64 as the client (verified locally: `f469129b520b2e16`). Whether the Linux dedicated server (app 276060) uses `hw.so` or `engine_i686.so` is not verifiable locally (no binaries); if it is `engine_i686.so`, fallguys' engine symbols need upstream to add that module.
- Issues #5/#6 suggest the Linux `_i686` signatures may have been written against `engine_i686.so`; their correctness on `hw.so` was never verified.

## User decisions (2026-10-13)
- **The runtime looks up symbols primarily by `symbolName`** (the user's reason: C++ may later have same-name overloads, and the full signature disambiguates them). Plugins supply platform-specific `symbolName`s. Whether to fall back to `artifact` when `symbolName` misses is pending confirmation; default is no fallback (refuse to load on a miss).
- The Linux dedicated server engine is `hw.so` (confirmed by the user), so the existing gamedata covers it; upstream does not need to add `engine_i686.so`.
- The existing bugs (#1–#8) get no separate hotfix; they disappear when stage 2 migrates.
- The stage 1 summary has been posted on issue #55: https://github.com/hzqst/metamod-fallguys/issues/55#issuecomment-6136255047

## Scope
Sven Co-op svencoop-8948 (5.25) / svencoop-10257 (5.26), all 58 private symbols in asext/fallguys, Windows + Linux (hw.so). engine_i686.so not verified.

## Relations
- relates_to [[PrivateSymbols]]
- relates_to [[game_private_funcs_vars]]
