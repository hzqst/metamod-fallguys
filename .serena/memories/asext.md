# asext 模块源码级分析

## 概述
- asext 是 `metamod-fallguys` 的 AngelScript 扩展插件（动态库）。它通过签名扫描/符号解析定位 server.dll/server.so 中的 AngelScript 私有实现，并将注册与 hook 能力导出给其他插件使用。
- 对外 API 头文件是 `asext/include/asext_api.h`（动态加载宏 + 轻量类型/枚举），内部导出声明在 `asext/asext.h`；示例用法在 `README_ASEXT.md`。

## 职责
- 在 `Meta_Attach` 中定位 AngelScript 相关私有函数/全局对象，建立调用入口。
- 安装内联 hook 捕获 AngelScript 初始化关键节点（文档对象注册、目录创建、脚本构建器定义词）。
- 向其他模块暴露注册对象方法/行为/枚举/函数定义、hook 注册与触发、以及若干核心对象的薄封装（CString、CScriptDictionary、CScriptAny/Array、CASFunction 等）。

## 架构
- 插件入口与 Metamod 集成：`asext/meta_api.cpp`（`Meta_Query`/`Meta_Attach`/`Meta_Detach`）、`asext/dllapi.cpp`（`GetEntityAPI2`/`GetEntityAPI2_Post`/`GetNewDLLFunctions`）、`asext/engine_api.cpp`、`asext/h_export.cpp`、`asext/dllmain.cpp`。
- Hook 与私有接口层：`asext/server_hook.cpp` + `asext/serverdef.h` + `asext/signatures.h`，依赖 `metamod/signatures_template.h` 中的签名扫描/内联 hook 宏。
- 对外 API 层：`asext/asext.h`（导出函数声明） + `asext/include/asext_api.h`（供外部插件动态导入）。

## 核心实现（含 workflow）
1. **插件加载**：Metamod 调用 `Meta_Query` 校验接口版本并缓存 `gpMetaUtilFuncs`；`Meta_Attach` 复制函数表并获取 server 模块句柄/基址/代码段范围。随后通过签名扫描或符号解析定位 AngelScript 私有函数指针，并输出日志。
2. **函数指针与 hook 安装**：`Meta_Attach` 设置 `ASEXT_CallHook` 与 `ASEXT_CallCASBaseCallable` 指向原始调用入口，并安装内联 hook：`CASDocumentation_RegisterObjectType`、`CASDirectoryList_CreateDirectory`、`CScriptBuilder_DefineWord`。
3. **初始化回调触发**：
   - `NewCASDocumentation_RegisterObjectType` 只在注册 `CSurvivalMode`（`"Survival Mode handler"` 且特定 flags）时触发文档初始化回调列表，随后清空并置 `g_ASDocInit=true`。
   - `NewCASDirectoryList_CreateDirectory` 在创建 `"scripts/plugins/store"` 且满足 flags/access/permanent 时触发目录初始化回调列表，随后清空并置 `g_ASDirInit=true`。
   - `NewCScriptBuilder_DefineWord` 在定义词为 `"SERVER"` 后触发脚本构建器回调列表。
4. **注册与调用 API**：
   - `ASEXT_Register*` 系列向回调列表追加；文档/目录回调在已初始化后会直接返回 false。
   - `ASEXT_RegisterObject* / RegisterEnum* / RegisterFuncDef` 通过 `g_call_original_*` 进入 AngelScript 注册流程，处理调用约定差异（`SC_SERVER_*` 宏）。
   - `ASEXT_RegisterHook` 创建 `CASHook` 并调用 `CASHook::CASHook` 完成注册，返回 hook 句柄；调用时使用 `ASEXT_CallHook`（函数指针）触发。
5. **生命周期收尾**：`NewServerActivate`（post）在 `g_ASDocInit/g_ASDirInit` 成立时卸载相关 hook；`Meta_Detach` 在卸载时解除 hook。

## 依赖
- **Metamod/HL SDK**：`meta_api.h`/`dllapi.h`/`extdll.h`、`enginefuncs_t`、`DLL_FUNCTIONS` 等。
- **签名扫描与 Hook**：`metamod/signatures_template.h` 提供 `FILL_FROM_SIGNATURE*`、`VAR_FROM_SIGNATURE*`、`INSTALL_INLINEHOOK` 等宏；`asext/signatures.h` 定义 Windows/Linux 的签名或符号名。
- **AngelScript 私有结构**：`asext/serverdef.h` 与 `asext/include/asext_api.h` 中包含大量占位/未知字段，用于偏移访问与调用约定适配。
- **外部插件接入**：`README_ASEXT.md` 示范使用 `LOAD_PLUGIN` + `IMPORT_ASEXT_API` 宏导入 asext API。

## 注意事项
- **注册时机要求**：DocInit/DirInit 回调必须在 AngelScript 初始化之前注册；`ASEXT_RegisterScriptBuilderDefineCallback` 需在模块启动前注册；否则会失败或无效。
- **版本与平台敏感**：签名扫描/符号名高度依赖游戏版本与平台（Windows/Linux），`Meta_Attach` 中对 Sven Co-op 5.15/5.16 采用不同路径。
- **调用约定差异**：`SC_SERVER_DECL/SC_SERVER_DUMMYARG` 宏用于适配平台调用约定，不可随意更改。
- **函数指针判空**：`ASEXT_CallHook` / `ASEXT_CallCASBaseCallable` 为运行时设置的函数指针，调用前需要判空（README 中示例也是如此）。
- **潜在问题**：`ASEXT_UnregisterScriptBuilderDefineCallback` 只做了 `remove_if` 没有 `erase`，实际不会从列表移除；`g_ASHooks` 使用 `new` 创建但未集中释放；结构体字段大量为未知/占位，改动需谨慎。
