# asusermsg

## 概述
`asusermsg` 是一个依赖 `asext` 的 Metamod 插件，用于在 Sven Co-op 的 AngelScript 环境中拦截/阻止/改写 UserMsg：通过 Hook 引擎 `MessageBegin/Write*/MessageEnd`，在消息发送完成时把消息元信息与参数缓冲提供给脚本回调，并可选择阻止原消息或由脚本自行重发。

## 职责
- 通过 Metamod 导出 `GetEngineFunctions`，Hook 引擎消息发送相关的 `pfnMessageBegin/pfnWrite*/pfnMessageEnd`，对 UserMsg 进行“录制缓冲 + 结束时派发”。
- 提供 AngelScript API：
  - `CEngineFuncs.RegisterUserMsgHook(msgType, callback)` 注册某个 msgType 的回调。
  - `CEngineFuncs.EnableUserMsgHook(msgType, enabled)` 单独启用/禁用某个 msgType 的 Hook。
  - `CEngineFuncs.EnableUserMsgHookGlobal(enabled)` 全局启用/禁用（常用于避免脚本重发导致递归）。
  - `CEngineFuncs.BlockCurrentUserMsg()` 在回调内标记阻止当前消息。
  - `CEngineFuncs.GetUserMsgId(name)` 通过名字获取 msgType。
  - `CEngineFuncs.GetUserMsgArgCount/Type/Integer/Float/GetUserMsgArgString` 在回调内读取本次消息参数。
- 在 `NewGameShutdown` 阶段释放脚本回调引用并清空注册表（避免跨局残留）。
- 为脚本编译期提供宏 `METAMOD_PLUGIN_ASUSERMSG` 以便条件编译。

## 涉及文件 (不要带行号)
- README_ASUSERMSG.md
- asusermsg/asusermsg.h
- asusermsg/asusermsg.cpp
- asusermsg/server_hook.cpp
- asusermsg/engine_api.cpp
- asusermsg/dllapi.cpp
- asusermsg/meta_api.cpp
- asusermsg/CMakeLists.txt
- asusermsg/Config.mak
- asusermsg/Makefile

## 架构
核心对象：`CUserMsgHookManager`（全局实例 `g_UserMsgHookManager`）。

1) 引擎侧 Hook（拦截消息写入流程）
- `GetEngineFunctions` 返回的 `meta_engfuncs` 把以下引擎函数替换为插件实现：
  - `NewMessageBegin(msg_dest, msg_type, origin, edict)`
  - `NewWriteByte/Char/Short/Long/Angle/Coord/String/Entity(...)`
  - `NewMessageEnd()`
- 当满足 `IsHookGlobalEnabled() && IsHookEnabled(msg_type)` 时：
  - `NewMessageBegin` 进入录制模式：调用 `CUserMsgHookManager::MessageBegin(...)` 初始化本次消息上下文，并 `MRES_SUPERCEDE` 阻止原引擎写入流程。
  - 随后的 `NewWrite*` 会把每个参数追加到 `m_msgArgs[]`（带 `UserMsgArg_*` 类型标记），并 `MRES_SUPERCEDE`。
  - `NewMessageEnd` 在结束时：
    - 先调用 `CallUserMsgHook()` 派发给脚本回调。
    - 若脚本回调期间调用过 `BlockCurrentUserMsg()` 导致返回“应阻止”，则清理缓冲并 `MRES_SUPERCEDE`（不再发送原消息）。
    - 否则调用 `SendBufferedMessage()` 用 `g_engfuncs.pfnMessageBegin/pfnWrite*/pfnMessageEnd` 重放缓冲参数，把消息按原样发送出去，然后清理缓冲。
- 当不满足 Hook 条件时：各 `New*` 都直接 `MRES_IGNORED`，走原引擎发送路径。

2) 脚本侧注册与回调
- `RegisterAngelScriptMethods` 通过 `asext`：
  - 定义宏 `METAMOD_PLUGIN_ASUSERMSG`。
  - 注册 `UserMsgHookCallback` 函数签名（文档/类型）。
  - 将 `CEngineFuncs.*` 方法绑定到一组 C++ wrapper（如 `CASEngineFuncs__RegisterUserMsgHook`），最终调用 `g_UserMsgHookManager`。
- `RegisterUserMsgHook(msgType, aslfn)`：把 `aslScriptFunction*` 包装为 `CASFunction` 并存入 `m_UserMsgCallbacks[msgType]`，同时默认启用该 msgType。

简化时序图：

```
(engine) pfnMessageBegin -> NewMessageBegin
                     |      (开始缓冲)
(engine) pfnWrite*   -> NewWrite*
                     |      (追加参数)
(engine) pfnMessageEnd -> NewMessageEnd
                       |-> CallUserMsgHook()  (AS 回调可 Block)
                       |-> [not blocked] SendBufferedMessage() 重放发送
```

## 依赖
- 内部依赖：`asext` 插件及其导出的 API（例如 `ASEXT_RegisterDocInitCallback`、`ASEXT_CreateCASFunction`、`ASEXT_CallCASBaseCallable` 等）。
- 引擎/SDK 依赖：HL 引擎 `enginefuncs_t` 消息发送函数族（`MessageBegin/Write*/MessageEnd`）。
- Metamod 依赖：`Meta_Query/Meta_Attach` 与 `META_FUNCTIONS::pfnGetEngineFunctions` 挂接流程（本插件同时导出 `GetNewDLLFunctions` 以在 `NewGameShutdown` 做清理）。

## 注意事项
- `GetUserMsgId` 的时机：脚本在 `MapInit` 过早调用可能拿不到正确 ID（README 中提示“usermsg 尚未被 server dll 注册”）。
- 递归风险：脚本在回调里用 `NetworkMessage` 重发同类型消息会再次触发 Hook；推荐临时 `EnableUserMsgHookGlobal(false)` 再重发（README 示例）。
- 参数缓冲上限：`MAX_USERMSG_ARG_COUNT` 为 256；写入超过上限时会 `ClearMessage()`，导致本次消息上下文被丢弃。
- `EnableUserMsgHook` 在消息录制期间（`IsHooked()==true`）会返回 `false`，避免中途切换导致状态不一致。
- 多回调行为：`m_UserMsgCallbacks[msgType]` 支持存多个回调，但 `CallUserMsgHook` 在首次 `ASEXT_CallCASBaseCallable` 返回真时就提前返回（因此后续回调可能不会被执行，需谨慎依赖“多个回调都触发”的语义）。
- `SendBufferedMessage` 的 Long 重放：当前实现对 `UserMsgArg_Long` 使用了 `pfnWriteShort`（而不是 `pfnWriteLong`），可能导致参数截断/行为异常（若该类型被实际使用需重点验证）。

## 调用方（可选）
- Metamod：调用 `Meta_Query/Meta_Attach`，并通过 `pfnGetEngineFunctions` 安装引擎函数 Hook。
- HL 引擎：在发送 UserMsg 时调用 `MessageBegin/Write*/MessageEnd`。
- AngelScript：通过 `g_EngineFuncs.RegisterUserMsgHook(...)` 注册拦截逻辑，并通过 `g_EngineFuncs.GetUserMsgArg*` 读取参数、`BlockCurrentUserMsg()` 阻止原消息。