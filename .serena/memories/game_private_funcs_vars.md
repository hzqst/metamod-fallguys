# 游戏 DLL 私有函数/私有变量引入机制（fallguys / asext）

## 概述
本仓库通过一套“声明/定义函数指针 + 运行时签名扫描/符号查找填充”的宏体系，把游戏 `server.dll/server.so` 与 `hw.dll/hw.so` 内的私有函数、私有全局变量地址引入到插件中，并在需要时进一步做 inline hook、或者直接调用。

## 职责
- 用 `signatures.h` 为不同平台/版本准备 `*_Signature`（以及 Linux 下可选的 `*_Symbol`）。
- 用 `serverdef.h`（以及 fallguys 的 `enginedef.h`）声明私有函数指针类型 `fnXxx`，并用 `PRIVATE_FUNCTION_EXTERN(Xxx)` 暴露全局指针。
- 在某个 `.cpp`（通常是 `server_hook.cpp`）里用 `PRIVATE_FUNCTION_DEFINE(Xxx)` 落地定义 `g_pfn_Xxx / g_call_original_Xxx`。
- 在 `meta_api.cpp` 的 `Meta_Attach` 中通过 `FILL_FROM_SIGNATURE* / FILL_FROM_SYMBOL* / VAR_FROM_SIGNATURE*` 在运行时填充函数指针与变量地址；失败则拒绝加载插件。
- （可选）通过 `INSTALL_INLINEHOOK(Xxx)` 把 `g_pfn_Xxx` 替换为 `NewXxx`，并把 trampoline 写回到 `g_call_original_Xxx` 供后续调用原函数。

## 涉及文件 (不要带行号)
- metamod/signatures_template.h
- fallguys/signatures.h
- fallguys/serverdef.h
- fallguys/enginedef.h
- fallguys/meta_api.cpp
- fallguys/server_hook.cpp
- asext/signatures.h
- asext/serverdef.h
- asext/meta_api.cpp
- asext/server_hook.cpp

## 架构
1) “函数指针”这条线：`PRIVATE_FUNCTION_EXTERN/DEFINE`
- 宏定义在 `metamod/signatures_template.h`：
  - `PRIVATE_FUNCTION_EXTERN(name)` 展开为 `extern fn##name g_pfn_##name; extern fn##name g_call_original_##name;`
  - `PRIVATE_FUNCTION_DEFINE(name)` 展开为 `fn##name g_pfn_##name; fn##name g_call_original_##name;`
- 约束（命名耦合）：
  - 必须先有 `typedef ... (*fnName)(...)`，宏通过 `fn##name` 拼出类型名。
  - 变量名固定为 `g_pfn_Name`（当前地址）与 `g_call_original_Name`（未 hook 时与 `g_pfn_` 相同；hook 后变为 trampoline）。

2) “签名/符号来源”这条线：`signatures.h`
- 插件各自的 `signatures.h` 都 `#include <signatures_template.h>`（实际文件为 `metamod/signatures_template.h`），并根据平台提供：
  - Windows：大量 `#define Xxx_Signature "..."`。
  - Linux：既可能提供 `Xxx_Signature`，也可能提供 `Xxx_Symbol`（用于 `dlsym/GetProcAddress` 风格的符号定位）。
- 这些宏名（`Xxx_Signature` / `Xxx_Symbol`）是 `FILL_* / VAR_*` 宏通过 `name##_Signature`/`name##_Symbol` 拼接引用的。

3) “运行时填充”这条线：`FILL_FROM_SIGNATURE* / VAR_FROM_SIGNATURE*`
- 这些宏同样在 `metamod/signatures_template.h`。
- 关键点：宏参数 `dll` 不是字符串，而是 token（例如 `engine`/`server`），宏内部会拼接出 `engineCodeBase/engineCodeSize/engineHandle/engineBase` 这类局部变量名。
  - 因此 `meta_api.cpp` 里必须按约定命名局部变量：`auto engineHandle/engineBase/engineCodeBase/engineCodeSize`、`auto serverHandle/...`。
- 常用宏语义：
  - `FILL_FROM_SIGNATURE(dll, name)`：在 `dllCodeBase~dllCodeBase+dllCodeSize` 内用 `pfnSearchPattern` 找到 `name##_Signature`，把结果写入 `g_pfn_name/g_call_original_name`，并做空指针检查。
  - `FILL_FROM_SYMBOL(dll, name)`：用 `pfnGetProcAddress(dllHandle, name##_Symbol)` 填充函数指针，并做空指针检查。
  - `FILL_FROM_SIGNATURED_CALLER_FROM_START/END(dll, name, offset)`：先定位“caller”处的签名，再用 `pfnGetNextCallAddr(...)` 解析 call 指令目标得到真实函数地址；并用 `pfnIsAddressInModuleRange(..., dllBase)` 校验地址归属。
  - `FILL_FROM_SIGNATURE_FROM_FUNCTION(dll, name, fromfunc, fromsize)`：把搜索范围限制在某函数附近（减少扫描范围，或避免全局歧义）。
- “变量地址”使用 `VAR_FROM_*` 宏：
  - `VAR_FROM_SIGNATURE_FROM_START/END(dll, name, offset)`：定位到 `name##_Signature` 后，从匹配地址 +/- offset 处按 `decltype(name)` 取出（解引用）真实地址。
  - `VAR_FROM_SYMBOL(dll, name)`：直接按符号拿变量地址。

4) fallguys 插件的落地方式（例）
- 声明：
  - `fallguys/serverdef.h`：声明 server 私有函数类型与 `PRIVATE_FUNCTION_EXTERN(...)`（如 `CPlayerMove_PlayStepSound`、`PM_PlaySoundFX_SERVER`、`RegisterSCScriptColor24`）。
  - `fallguys/enginedef.h`：声明 engine 私有函数（如 `build_number`、`SV_Physics`、`SV_Push*`、`SV_SingleClipMoveToEntity*` 等）以及一批 engine 私有全局变量指针（如 `sv_models`、`host_frametime`、`pmovevars`、`sv_areanodes`、`pg_groupop/pg_groupmask`）。
- 定义：`fallguys/server_hook.cpp` 用 `PRIVATE_FUNCTION_DEFINE(...)` 落地 `g_pfn_*/g_call_original_*`。
- 填充时机：`fallguys/meta_api.cpp` 的 `Meta_Attach` 中，在拿到 `engine*`/`server*` 模块信息后批量调用 `FILL_FROM_SIGNATURE*` 与 `VAR_FROM_SIGNATURE*`。
  - Windows 下 `CPlayerMove_PlayStepSound/PM_PlaySoundFX_SERVER` 使用 `FILL_FROM_SIGNATURED_CALLER_FROM_END(server, ..., -1)`（caller->callee 解析）。
  - Linux 下会根据是否能 `CreateInterface("SCServerDLL003")` 区分 5.16（用签名）与 5.15（用符号），并针对 `i686` 等 engine 类型使用 `FILL_FROM_SIGNATURED_TY_CALLER_*`。

5) asext 插件的落地方式（例）
- 声明：`asext/serverdef.h` 定义大量 AngelScript 相关的 server 私有类/函数指针 typedef，并用 `PRIVATE_FUNCTION_EXTERN(...)` 暴露（如 `CASHook_*`、`CASDocumentation_*`、`CScriptBuilder_DefineWord`、`CScriptDictionary_*` 等）；同时声明私有变量 `extern CASServerManager **g_pServerManager;`。
- 定义：`asext/server_hook.cpp` 用 `PRIVATE_FUNCTION_DEFINE(...)` 落地所有函数指针，并定义 `CASServerManager **g_pServerManager = NULL;`。
- 填充时机：`asext/meta_api.cpp` 的 `Meta_Attach`：
  - Windows 下主要走 `FILL_FROM_SIGNATURE` + 一些 `FILL_FROM_SIGNATURED_CALLER_*`。
  - Linux 下同样区分 5.16（签名）与 5.15（符号）。
  - `g_pServerManager` 通过 `VAR_FROM_SIGNATURE_FROM_START(server, g_pServerManager, 5)` 或 `VAR_FROM_SYMBOL(server, g_pServerManager)` 填充。

## 依赖
- `gpMetaUtilFuncs` 提供的能力：
  - `pfnSearchPattern`（签名扫描）
  - `pfnGetProcAddress`（符号查找）
  - `pfnGetNextCallAddr`（从 call site 解析 callee）
  - `pfnIsAddressInModuleRange`（地址归属校验）
  - `pfnGet*Handle/Base/CodeBase/CodeSize`（模块信息）
- 平台/版本差异：`signatures.h` 内的 `#ifdef _WIN32` + Linux 下按 Sven Co-op 版本/engine type 分流。

## 注意事项
- 强命名耦合：`PRIVATE_FUNCTION_*` 要求类型名是 `fn<name>`，签名宏名是 `<name>_Signature`/`<name>_Symbol`；`FILL_*` 的 `dll` token 必须对应 `meta_api.cpp` 中的 `engine*/server*` 局部变量命名。
- hook 语义：未安装 hook 时，`g_call_original_*` 通常与 `g_pfn_*` 相同；安装 hook 后 `INSTALL_INLINEHOOK` 会把 trampoline 写回 `g_call_original_*`，调用原函数应走 `g_call_original_*`。
- 指令级宏（CALLER 系列）对 `offset` 很敏感：同一函数在不同版本/平台偏移可能不同，需要维护 `signatures.h`（必要时用 `_TY_`/版本分流）。
- 变量获取是“定位 + 解引用”：`VAR_FROM_SIGNATURE_FROM_*` 最终是从匹配地址附近读出一个指针/地址，offset 错误会直接读错地址导致崩溃。

## 调用方（可选）
- `fallguys/meta_api.cpp`、`asext/meta_api.cpp`：负责在 `Meta_Attach` 阶段填充所有私有函数/变量。
- `fallguys/server_hook.cpp`、`asext/server_hook.cpp`：定义私有函数指针、安装 inline hook，并在业务逻辑中使用 `g_pfn_* / g_call_original_*`。