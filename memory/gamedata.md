---
title: gamedata
type: note
permalink: metamod-fallguys/gamedata
tags:
- metamod
- gamedata
- asext
- fallguys
- private-symbols
---

# gamedata — metamod 托管的 GoldSrc_VibeSignatures 符号目录

## 概述
自 issue #55（2026-10）起，asext/fallguys 的所有引擎私有符号都从 [GoldSrc_VibeSignatures](https://github.com/hlnd2t/GoldSrc_VibeSignatures) 发布的 gamedata 解析，替代手写签名。实现是 MetaHookSv `MetaHook/src/GameData.cpp` 的精简移植：只保留 function/global 两种 kind，按当前平台（`windows`/`linux`）加载记录，去掉 blob/mirror/client alias。

## 涉及文件
- `metamod/src/gamedata.h`、`metamod/src/gamedata.cpp`：运行时目录（namespace `GameData`）
- `metamod/include/gamesymbol.h`：`meta_gamesymbol_kind_t`、`meta_gamesymbol_status_t`（取值与 MetaHookSv 一致）
- `metamod/include/mutil.h`、`metamod/src/mutil.cpp`：`pfnResolveGameSymbol`、`pfnGetGameSymbolStatusString`（接口 5:17，追加在 `pfnGetCodeSize` 之后）
- `metamod/include/meta_api.h`：`META_INTERFACE_VERSION "5:17"`
- `metamod/src/metamod.cpp`：`meta_init_gamedata()`，在 `meta_load_gamedll()` 之后、`Plugins->load()` 之前
- `metamod/include/signatures_template.h`：`RESOLVE_FROM_GAMEDATA` / `FILL_FROM_GAMEDATA` / `VAR_FROM_GAMEDATA`
- `metamod/scripts/sync-gamedata.py`：构建期同步/裁剪/校验脚本（MetaHookSv 同名脚本改造）
- `metamod/cmake/Build.cmake`：`mmfg_add_gamedata(target manifest)`；`metamod/cmake/Dependencies.cmake`：`mmfg_gamedata_headers()`（RapidJSON + Chocobo1Hash）
- `asext/scripts/manifests/asext.json`、`fallguys/scripts/manifests/fallguys.json`
- 测试：`metamod/tests/test_gamedata.cpp`（CTest `mmfg_test_gamedata`）、`metamod/scripts/tests/test_sync_gamedata.py`

## 数据格式
- 上游索引 `https://hlnd2t.github.io/GoldSrc_VibeSignatures/gamesymbols/index.json`：index schema 4；快照 schema 5，`source.snapshotSchemaVersion` 8，`analysisOutputContractVersion` 3。
- 记录 `{platform, module, symbolName, kind, payload}`；function payload `func_rva/func_size`，global payload `gv_rva`；`binaries.<module>.<platform>.crc64` 为模块文件 CRC-64/XZ。
- 安装布局：`addons/metamod/gamedata/<plugin>/{index.json, svencoop-8948.json, svencoop-10257.json}`。裁剪后的快照用稳定文件名 `<gameVersion>.json`，内容确定（Windows/Linux 构建逐字节相同）。

## 运行时流程
1. `meta_init_gamedata()`：`CollectIndexDirs(<gamedir>/addons/metamod/gamedata)` 递归收集（排序、跳过符号链接）所有含 `index.json` 的目录 → `Initialize()`；每个 index 可选，至少一个加载成功即可用。诊断逐条 `META_WARNING("Gamedata: ...")`，成功时 `META_LOG("Loaded gamedata catalog from ... (N index file(s))")`（写入游戏根目录 `metamod.log`）。
2. 快照校验 size + SHA-256 + 各 schema 版本；同一 (crc64, symbolName) 的内容不同的重复记录标记为 conflicted；非 function/global kind 记为 unsupported。
3. `Resolve(base, symbolName, kind)`：取模块路径（Windows `GetModuleFileNameA`；Linux `dladdr` 且要求 `dli_fbase == base`）→ 计算文件 CRC-64（按 base 缓存，带互斥）→ 查 (crc64, symbolName) → 校验 kind → Windows 用 PE `SizeOfImage` 做越界检查（Linux 不检查）→ 返回 `base + rva`。
4. Linux 基址：插件传入的 `serverBase/engineBase` 来自 `mutil_GetModuleBaseByHandle`（`dladdr(l_ld).dli_fbase`）；Sven 的 `server.so`/`hw.so` 第一个 PT_LOAD vaddr 为 0，所以 `dli_fbase == l_addr`，RVA 直接相加即可。

## 关键决策
- **按 symbolName 查找**（不是 artifact 名）：C++ 将来可能出现同名重载；symbolName 未命中**不回退**，插件拒绝加载。
- **只用 gamedata**：删除签名、Linux 5.15 符号分支、手写 GOT/PLT 解析；旧签名宏只为仓库外插件保留在公共头中。
- **metamod 托管**：目录只加载一次，插件通过 mutil 接口查询；接口版本升到 5:17，因为 `mplugin.cpp` 只拷贝 metamod 自身 `sizeof(mutil_funcs)`，旧 metamod 会给新插件未初始化的成员。
- Linux 专用服务器引擎是 `hw.so`，与客户端同一份二进制；Windows 专用服务器也用同一份 `hw.dll`。

## 构建期同步
- `mmfg_add_gamedata()` 定义 `MMFG_SYNC_GAMEDATA`（默认 ON，需要 Python 3.8+）。自定义目标 `<plugin>_gamedata` 先运行 `sync-gamedata.py --manifest ... --target-dir ${PROJECT_BINARY_DIR}/assets/addons/metamod/gamedata/<plugin> --temp-root ${PROJECT_BINARY_DIR}/gamedata-sync`，再用 `--validate-only` 做覆盖校验；`install(DIRECTORY ... OPTIONAL)`。
- 拿不到上游索引时回退到上次缓存的索引离线构建；目标路径只接受 `.../addons/metamod/gamedata[/<plugin>]`；加锁 Windows 用 msvcrt，POSIX 用 fcntl。
- manifest 字段：`gameVersions`、`platforms`（默认 windows+linux）、`symbols`（两平台共用名）、`platformSymbols`（按平台的名字，如 Windows `unsigned int` 与 Linux `unsigned long` 两种写法）。覆盖校验按 (gameVersion, platform) 逐一检查。
- 没有移植 MetaHookSv 那个 83 KB 的 `validate-gamedata.py`；`--validate-only` 加上覆盖校验就是质量门槛。

## 验证（2026-10-13）
- 阶段一：签名与 gamedata 逐项比对，所有差异都是我方签名的错误（[[gamedata-signature-parity]]）。
- Windows Sven Co-op 10257 实机（halflife-mcp）：
  - `Loaded gamedata catalog ... (2 index file(s))`。
  - 56 个符号全部解析，每个模块反推出唯一基址。
  - `meta list` 中 5 个插件均为 RUN。
  - AngelScript 冒烟（PlayerPostThinkPost、PlayerMovePlaySoundFX + GetRunPlayerMovePlayerIndex、EnablePhysicWorld、SetGroupTrace）正常。
  - 退出码 0xC0000409 是迁移前就存在的问题（旧版 DLL 同样出现）。
- CTest：Windows 8/8，WSL Ubuntu 24.04 i386（带 ASan/UBSan）15/15。
- Python：`python -B -m unittest discover -s metamod/scripts/tests` 14 个用例通过。
- CI：metamod 组件的 `build.yml` 以 `MMFG_METAMOD_BUILD_TESTS=ON` 配置，并在 Windows/Ubuntu × Debug/Release 上跑 CTest 和上面的 Python 测试（Debug 本地也验证过：Windows 8/8，Linux 15/15）。
- Linux 运行时没有真机验证：WSL 缺少 32 位依赖（libcurl/SDL2/GL/steam_api），无法 dlopen 真实的 `.so`。

## 新增符号步骤
1. 在上游快照中确认两个平台都发布了，并记下各自的 symbolName。
2. 把它加入插件 manifest：两平台同名放 `symbols`，名字不同放 `platformSymbols`。
3. 在插件 `gamesymbols.h` 中加 `X_GameSymbol`，如需区分平台用 `#ifdef _WIN32`。
4. 在 `Meta_Attach` 中加 `FILL_FROM_GAMEDATA` 或 `VAR_FROM_GAMEDATA`。
5. 用 `--validate-only` 在构建时确认覆盖校验通过。

## Relations
- relates_to [[game_private_funcs_vars]]
- relates_to [[PrivateSymbols]]
- relates_to [[gamedata-signature-parity]]
- relates_to [[asext]]
