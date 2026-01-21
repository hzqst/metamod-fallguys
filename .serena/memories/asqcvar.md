# asqcvar

## 概述
`asqcvar` 是一个依赖 `asext` 的 Metamod 插件，用于把客户端 `querycvar/querycvar2` 的响应（对应 HLSDK `NEW_DLL_FUNCTIONS::pfnCvarValue/pfnCvarValue2` 回调）转发到 AngelScript：既支持全局 Hook（QueryCvar/QueryCvar2），也支持按 `request_id` 注册的一次性异步回调（QueryCvar2）。

## 职责
- 通过 Metamod 导出 `GetNewDLLFunctions`，在 `NEW_DLL_FUNCTIONS` 中注册 `pfnCvarValue/pfnCvarValue2` 的实现（`NewCvarValue/NewCvarValue2`），以拦截客户端 CVar 查询响应。
- 将 `pfnCvarValue` 响应派发到 AngelScript Hook：`Hooks::Player::QueryCvar`。
- 将 `pfnCvarValue2` 响应先派发到“按 request_id 的一次性回调”，再派发到 AngelScript Hook：`Hooks::Player::QueryCvar2`。
- 向 AngelScript 注册 `CEngineFuncs::SetQueryCvar2Callback`（参数包含 `request_id` 与回调函数），用于为某个 `request_id` 绑定一次性回调。
- 为脚本编译期提供宏 `METAMOD_PLUGIN_ASQCVAR`，用于判断插件可用性并做条件编译。
- 在 `NewGameShutdown` 阶段清理仍未触发的回调引用（避免跨局/关服遗留）。

## 涉及文件 (不要带行号)
- README_ASQCVAR.md
- asqcvar/asqcvar.h
- asqcvar/asqcvar.cpp
- asqcvar/server_hook.cpp
- asqcvar/dllapi.cpp
- asqcvar/meta_api.cpp
- asqcvar/engine_api.cpp
- asqcvar/CMakeLists.txt
- asqcvar/Config.mak
- asqcvar/Makefile

## 架构
核心链路分两条：

1) 全局 Hook（QueryCvar / QueryCvar2）
- 引擎收到客户端的 querycvar 响应后，会调用 GameDLL 的 `NEW_DLL_FUNCTIONS::pfnCvarValue/pfnCvarValue2`。
- Metamod 将调用转发到本插件在 `gNewDllFunctionTable` 里注册的 `NewCvarValue/NewCvarValue2`。
- `NewCvarValue`：构造 `CString value`，调用 `ASEXT_CallHook(g_QueryCvarHook, ..., pPlayer, value)`。
- `NewCvarValue2`：构造 `CString cvarName/value`，调用 `ASEXT_CallHook(g_QueryCvar2Hook, ..., pPlayer, requestId, cvarName, value)`。

2) 按 request_id 的一次性回调（仅 QueryCvar2）
- AngelScript 侧调用 `CEngineFuncs.SetQueryCvar2Callback(request_id, callback)`。
- C++ 侧把 `aslScriptFunction*` 包装成 `CASFunction` 并存入 `g_QueryCvar2Callbacks[request_id]`。
- 当 `NewCvarValue2` 收到回包时：
  - 先执行 `ASQCvar_CallQueryCvar2Callback(pPlayer, request_id, cvarName, value)`：调用回调（如 `ASEXT_CallCASBaseCallable` 可用），随后释放引用并从 map 删除。
  - 再触发全局 Hook `QueryCvar2`。

初始化与绑定：
- `Meta_Attach` 中通过 `LOAD_PLUGIN` 显式加载 `asext`（`addons/metamod/dlls/asext.dll/.so`），并 `IMPORT_ASEXT_API` 获取 `ASEXT_*` API。
- `RegisterAngelScriptMethods`：注册脚本宏与文档/方法（`SetQueryCvar2Callback`）。
- `RegisterAngelScriptHooks`：注册两类 Hook（QueryCvar/QueryCvar2）。

简化时序图：

```
(AS) SetQueryCvar2Callback(request_id, cb)
        |
        v
g_QueryCvar2Callbacks[request_id] = CASFunction(cb)
        |
        | (client replies)
        v
(New DLL funcs) pfnCvarValue2 -> NewCvarValue2
        |
        +-> ASQCvar_CallQueryCvar2Callback(...)  // 一次性回调
        |
        +-> ASEXT_CallHook(g_QueryCvar2Hook, ...) // 全局 Hook
```

## 依赖
- 内部依赖：`asext` 插件及其导出的 API（例如 `ASEXT_RegisterHook`、`ASEXT_CallHook`、`ASEXT_CreateCASFunction` 等）。
- 引擎/SDK 依赖：HLSDK 的 `NEW_DLL_FUNCTIONS` 与 `edict_t` 回调约定（`pfnCvarValue/pfnCvarValue2`）。
- Metamod 依赖：`Meta_Query/Meta_Attach` 流程与 `META_FUNCTIONS`（本插件仅导出 `GetNewDLLFunctions`）。

## 注意事项
- `request_id` 必须唯一：`ASQCvar_SetQueryCvar2Callback` 对重复 `request_id` 直接返回 `false`，调用方需自行分配/管理 request id。
- 回调是“一次性”的：`ASQCvar_CallQueryCvar2Callback` 在触发后会释放引用并从 `g_QueryCvar2Callbacks` 删除。
- 若引擎侧未回包：回调会一直保留到 `NewGameShutdown` 调用 `ASQCvar_Shutdown` 才清理；如果脚本频繁注册但对端不响应，可能造成回调积累。
- `ASEXT_CallCASBaseCallable` 为空时：回调不会被执行，但仍会走释放/删除逻辑（避免泄漏，但会“丢回调”）。
- `Meta_Detach` 不做资源回收：当前实现主要依赖 `NewGameShutdown` 的清理路径。

## 调用方（可选）
- Metamod：调用 `Meta_Query/Meta_Attach`，并通过 `META_FUNCTIONS::pfnGetNewDLLFunctions` 取得 `NEW_DLL_FUNCTIONS` 回调表。
- HL 引擎：在收到 querycvar 响应时调用 `NEW_DLL_FUNCTIONS::pfnCvarValue/pfnCvarValue2`。
- AngelScript：
  - `g_EngineFuncs.SetQueryCvar2Callback(...)` 注册异步回调。
  - `g_Hooks.RegisterHook(Hooks::Player::QueryCvar/QueryCvar2, ...)` 注册全局 Hook。