---
title: PrivateSymbols
type: note
permalink: metamod-fallguys/private-symbols
tags:
- asext
- fallguys
- private-symbols
- engine
- reverse
---

# PrivateSymbols — asext / fallguys 用到的引擎私有符号清单

## 概述
本笔记汇总 `asext` 与 `fallguys` 两个插件实际引入的全部**游戏引擎私有符号**（`server.dll/server.so` 与 `hw.dll/hw.so` 内的私有函数与私有全局变量），给出每个符号的名字、目标模块、各平台定位方式（Windows 签名 / Linux 5.16 签名 / Linux 5.15 符号）以及是否安装 inline hook。

> 本笔记是**清单**；引入机制（`PRIVATE_FUNCTION_EXTERN/DEFINE`、`FILL_FROM_SIGNATURE*`、`VAR_FROM_SIGNATURE*` 的宏语义与命名耦合）见 [game-private-funcs-vars](memory://metamod-fallguys/game-private-funcs-vars)。

统计（私有，不含 FMOD 等外部 DLL）：函数 **51** 个（asext 40 / fallguys server 3 / fallguys engine 8），变量 **7** 个（asext 1 / fallguys engine 6），合计 **58**。

## 涉及文件
- `asext/src/serverdef.h`、`asext/src/signatures.h`、`asext/src/meta_api.cpp`、`asext/src/server_hook.cpp`
- `fallguys/src/serverdef.h`、`fallguys/src/enginedef.h`、`fallguys/src/signatures.h`、`fallguys/src/meta_api.cpp`、`fallguys/src/server_hook.cpp`、`fallguys/src/engine_hook.cpp`
- 宏定义：`metamod/include/signatures_template.h`

## 定位方式代号
| 代号 | 含义 |
| --- | --- |
| `Sig` | `FILL_FROM_SIGNATURE(server/engine, X)`：在代码段直接扫描 `X_Signature`，命中即函数首地址。 |
| `CallEnd(N)` | `FILL_FROM_SIGNATURED_CALLER_FROM_END`：扫描 caller 匹配点后定位 `call`，解析 callee 真地址。 |
| `CallStart(N)` | `FILL_FROM_SIGNATURED_CALLER_FROM_START`：同上，但相对签名**起始**偏移。 |
| `TyCallEnd/Start(N)` | Linux 专用变体，使用 `X_Signature_i686`（`engine` 类型 `i686`/`hw.so`）。 |
| `FromFunc` | `FILL_FROM_SIGNATURE_FROM_FUNCTION(dll, X, fromfunc, size)`：把扫描范围限制在已定位函数附近。 |
| `Sym` | `FILL_FROM_SYMBOL(server/engine, X)`：按 `X_Symbol`（mangled 名）`dlsym/GetProcAddress`。 |
| `VarStart/VarEnd(N)` | `VAR_FROM_SIGNATURE_FROM_START/END`：定位签名后 ±offset 处**解引用**取得变量地址。 |
| `VarSym` | `VAR_FROM_SYMBOL`：按符号直接取变量地址。 |

---

## 一、asext 私有符号（目标模块：server.dll / server.so）

### 1.1 AngelScript 注册与文档类（CASDocumentation / CASDirectoryList / CASBaseManager）
| 符号 | 归属（demangle 摘要） | Windows | Linux 5.16 | Linux 5.15 Symbol | Hook |
| --- | --- | --- | --- | --- | --- |
| `CASDocumentation_RegisterObjectType` | `CASDocumentation::RegisterObjectType(const char*,const char*,int,unsigned int)` | `CallEnd(-1)` | `CallEnd(-1)` | `_ZN16CASDocumentation18RegisterObjectTypeEPKcS1_im` | ✅ |
| `CASDocumentation_RegisterObjectProperty` | `CASDocumentation::RegisterObjectProperty(const char*,const char*,const char*,int)` | `CallEnd(-7)` | `CallStart(0)` | `_ZN16CASDocumentation22RegisterObjectPropertyEPKcS1_S1_i` | |
| `CASDocumentation_RegisterGlobalProperty` | `CASDocumentation::RegisterGlobalProperty(const char*,const char*,void*)` | `CallEnd(-15)` | `CallStart(9)` | `_ZN16CASDocumentation22RegisterGlobalPropertyEPKcS1_Pv` | |
| `CASDocumentation_RegisterGlobalFunction` | `CASDocumentation::RegisterGlobalFunction(const char*,const char*,const asSFuncPtr&,unsigned long,void*)` | `Sig` | `Sig` | `_ZN16CASDocumentation22RegisterGlobalFunctionEPKcS1_RK10asSFuncPtrmPv` | |
| `CASDocumentation_RegisterObjectMethod` | `CASDocumentation::RegisterObjectMethod(const char*,const char*,const char*,const asSFuncPtr&,unsigned long)` | `CallEnd(-7)` | `CallStart(0)` | `_ZN16CASDocumentation20RegisterObjectMethodEPKcS1_S1_RK10asSFuncPtrm` | |
| `CASDocumentation_RegisterObjectBehaviour` | `CASDocumentation::RegisterObjectBehaviour(const char*,const char*,asEBehaviours,const char*,const asSFuncPtr&,unsigned long,void*)` | `CallEnd(-8)` | `CallEnd(-1)` | `_ZN16CASDocumentation23RegisterObjectBehaviourEPKcS1_13asEBehavioursS1_RK10asSFuncPtrmPv` | |
| `CASDocumentation_RegisterFuncDef` | `CASDocumentation::RegisterFuncDef(const char*,const char*)` | `CallStart(0)` | `CallStart(0)` | `_ZN16CASDocumentation15RegisterFuncDefEPKcS1_` | |
| `CASDocumentation_RegisterEnum` | `CASDocumentation::RegisterEnum(const char*,const char*,CASDocumentation::ENUM_TYPE)` | `CallEnd(-7)` | `CallEnd(-13)` | `_ZN16CASDocumentation12RegisterEnumEPKcS1_NS_9ENUM_TYPEE` | |
| `CASDocumentation_RegisterEnumValue` | `CASDocumentation::RegisterEnumValue(const char*,const char*,const char*,int)` | `CallEnd(-7)` | `CallStart(7)` | `_ZN16CASDocumentation17RegisterEnumValueEPKcS1_S1_i` | |
| `CASDocumentation_SetDefaultNamespace` | `CASDocumentation::SetDefaultNamespace(const char*)` | `Sig` | `Sig` | `_ZN16CASDocumentation19SetDefaultNamespaceEPKc` | |
| `CASBaseManager_GetTypeInfoByName` | `CASBaseManager::GetTypeInfoByName(const std::string&)` | `Sig` | `Sig` | `_ZN14CASBaseManager17GetTypeInfoByNameERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE` | |
| `CASDirectoryList_CreateDirectory` | `CASDirectoryList::CreateDirectory(const char*,unsigned char,unsigned char,unsigned char,unsigned char)` | `CallEnd(-1)` | `CallStart(0)` | `_ZN16CASDirectoryList15CreateDirectoryEPKchhhh` | ✅ |

### 1.2 CASHook
| 符号 | 归属 | Windows | Linux 5.16 | Linux 5.15 Symbol | Hook |
| --- | --- | --- | --- | --- | --- |
| `CASHook_CASHook` | `CASHook::CASHook(unsigned char,unsigned char,const char*,const char*,const char*,const CASHookArguments&)` | `Sig` | `CallEnd(-1)` | `_ZN7CASHookC2EhhPKcS1_S1_RK16CASHookArguments` | |
| `CASHook_Call` | `CASHook::Call(int,...)` | `Sig` | `Sig` | `_ZN7CASHook4CallEiz` | |

### 1.3 引用计数 / 可调用对象
| 符号 | 归属 | Windows | Linux 5.16 | Linux 5.15 Symbol |
| --- | --- | --- | --- | --- |
| `CASRefCountedBaseClass_InternalRelease` | `CASRefCountedBaseClass::InternalRelease() const` | `CallEnd(-7)` | `CallStart(3)` | `_ZNK22CASRefCountedBaseClass15InternalReleaseEv` |
| `CScriptAny_Release` | `CScriptAny::Release() const` | `CallStart(7)` | `CallStart(0)` | `_ZNK10CScriptAny7ReleaseEv` |
| `CScriptArray_Release` | `CScriptArray::Release() const` | `CallStart(0)` | `CallEnd(-8)` | `_ZNK12CScriptArray7ReleaseEv` |
| `CASBaseCallable_Call` | `CASBaseCallable::Call(int,...)` | `CallEnd(-1)` | `CallEnd(-8)` | `_ZN15CASBaseCallable4CallEiz` |
| `CASFunction_Create` | `CASFunction::Create(asIScriptFunction*,CASModule*,bool)` | `CallEnd(-1)` | `CallStart(0)` | `_ZN11CASFunction6CreateEP17asIScriptFunctionP9CASModuleb` |

> `CASFunction_Create` 为**非成员**函数（无 `SC_SERVER_DUMMYARG`），其余方法为 `SC_SERVER_DECL`（Win `__fastcall`）。
> `CASRefCountedBaseClass_InternalRelease` 的 pthis 通过 `asGetActiveContext()` 的返回值取得，因此 `def` 为 `(void* ref)`。

### 1.4 CString / CScriptBuilder
| 符号 | 归属 | Windows | Linux 5.16 | Linux 5.15 Symbol | Hook |
| --- | --- | --- | --- | --- | --- |
| `CString_Assign` | `CString::Assign(const char*,unsigned int)` | `Sig` | `CallEnd(-1)` | `_ZN7CString6AssignEPKcj` | |
| `CString_dtor` | `CString::~CString()` | `Sig` | `Sig` | `_ZN7CStringD2Ev` | |
| `CScriptBuilder_DefineWord` | `CScriptBuilder::DefineWord(const char*)` | `CallStart(8)` | `CallEnd(-1)` | `_ZN14CScriptBuilder10DefineWordEPKc` | ✅ |

### 1.5 AngelScript 运行时
| 符号 | 归属 | Windows | Linux 5.16 | Linux 5.15 | 说明 |
| --- | --- | --- | --- | --- | --- |
| `asGetActiveContext` | `asGetActiveContext()` | `Sig` | `Sig` | 仅 `Sig`（无 `_Symbol`） | 声明为 `void* (*)()` |

### 1.6 CScriptDictionary
| 符号 | 归属 | Windows | Linux 5.16 | Linux 5.15 Symbol |
| --- | --- | --- | --- | --- |
| `CScriptDictionary_Create` | `CScriptDictionary::Create(asIScriptEngine*)` | `CallStart(3)` | `CallStart(8)` | `_ZN17CScriptDictionary6CreateEP15asIScriptEngine` |
| `CScriptDictionary_AddRef` | `CScriptDictionary::AddRef() const` | `CallStart(6)` | `CallStart(13)` | `_ZNK17CScriptDictionary6AddRefEv` |
| `CScriptDictionary_Release` | `CScriptDictionary::Release() const` | `CallStart(10)` | `Sig` | `_ZNK17CScriptDictionary7ReleaseEv` |
| `CScriptDictionary_Set` | `CScriptDictionary::Set(const CString&,void*,int)` | `Sig` | `CallEnd(-1)` | `_ZN17CScriptDictionary3SetERK7CStringPvi` |
| `CScriptDictionary_Get` | `CScriptDictionary::Get(const CString&,void*,int) const` | `Sig` | `Sig` | `_ZNK17CScriptDictionary3GetERK7CStringPvi` |
| `CScriptDictionary_Exists` | `CScriptDictionary::Exists(const CString&) const` | `Sig` | `Sig` | `_ZNK17CScriptDictionary6ExistsERK7CString` |
| `CScriptDictionary_IsEmpty` | `CScriptDictionary::IsEmpty() const` | `FromFunc`* | `Sig` | `_ZNK17CScriptDictionary7IsEmptyEv` |
| `CScriptDictionary_GetSize` | `CScriptDictionary::GetSize() const` | `FromFunc`* | `Sig` | `_ZNK17CScriptDictionary7GetSizeEv` |
| `CScriptDictionary_Delete` | `CScriptDictionary::Delete(const CString&)` | `Sig` | `Sig` | `_ZN17CScriptDictionary6DeleteERK7CString` |
| `CScriptDictionary_DeleteAll` | `CScriptDictionary::DeleteAll()` | `Sig` | `Sig` | `_ZN17CScriptDictionary9DeleteAllEv` |

\* Windows 下这两个函数不在 `CScriptDictionary` 本体附近，故用 `FILL_FROM_SIGNATURE_FROM_FUNCTION(..., g_pfn_CScriptDictionary_Exists, 0x100)` 限定在 `CScriptDictionary::Exists` 起 0x100 字节内扫描。

### 1.7 CScriptDictionary::CIterator（迭代器）
| 符号 | 归属 | Windows | Linux 5.16 | Linux 5.15 Symbol |
| --- | --- | --- | --- | --- |
| `CScriptDictionary_begin` | `CScriptDictionary::begin() const` | `CallEnd(-1)` | `CallEnd(-1)` | `_ZNK17CScriptDictionary5beginEv` |
| `CScriptDictionary_end` | `CScriptDictionary::end() const` | `CallStart(6)` | `CallEnd(-1)` | `_ZNK17CScriptDictionary3endEv` |
| `CScriptDictionary_CIterator_GetKey` | `CScriptDictionary::CIterator::GetKey() const` | `CallStart(7)` | `CallEnd(-1)` | `_ZNK17CScriptDictionary9CIterator6GetKeyEv` |
| `CScriptDictionary_CIterator_operator_NE` | `CScriptDictionary::CIterator::operator!=(const CIterator&) const` | `CallStart(4)` | `CallEnd(-1)` | `_ZNK17CScriptDictionary9CIteratorneERKS0_` |
| `CScriptDictionary_CIterator_GetValue` | `CScriptDictionary::CIterator::GetValue(void*,int) const` | `CallStart(10)` | `CallEnd(-1)` | `_ZNK17CScriptDictionary9CIterator8GetValueEPvi` |
| `CScriptDictionary_CIterator_operator_PP` | `CScriptDictionary::CIterator::operator++()` | `CallEnd(-1)` | `CallEnd(-1)` | `_ZN17CScriptDictionary9CIteratorppEv` |

> **定位宿主与 ABI 差异**：这 6 个函数的签名均落在 `CASEntityFuncs::InitializeEntity` 内部，Windows/5.16 通过 `FILL_FROM_SIGNATURED_CALLER_*` 从该函数的 `call` 反解得到真实地址。
> `begin`/`end` 的**参数顺序因 ABI 不同**：Windows（MSVC）为 `(pthis, …, CIterator* 隐藏返回)`；Linux（Itanium ABI）为 `(CIterator* 隐藏返回, pthis)`——`asext/src/serverdef.h` 用 `#ifdef _WIN32` 分别 typedef。`CIterator` 为 `{CString key; void* value; int typeId}`。

### 1.8 asext 私有变量
| 变量 | 类型 | 含义 | Windows | Linux 5.16 | Linux 5.15 Symbol |
| --- | --- | --- | --- | --- | --- |
| `g_pServerManager` | `CASServerManager**` | `CASServerManager` 单例指针 | `VarStart(5)` | 手工解析（`CASHook::VCall` 的 GOT/PLT） | `_ZZN16CASServerManager11GetInstanceEvE9pInstance` |

> 5.16 路径先用 `pattern_CASHook_VCall` 定位 `CASHook::VCall`，再读取其 PIC/GOT 引用解析出单例地址；5.15 路径直接走符号。

### 1.9 asext 中声明但未启用（占位）
| 符号 | 现状 |
| --- | --- |
| `CASBLOB_ReadData` / `CASBLOB_WriteData` | 仅在 `serverdef.h` 声明（`PRIVATE_FUNCTION_EXTERN`），**无 Signature/Symbol，也未 `DEFINE`/填充**，当前不可用。 |

---

## 二、fallguys 私有符号

### 2.1 server（server.dll / server.so）
| 符号 | 归属 | Windows | Linux 5.16 | Linux 5.15 Symbol | Hook |
| --- | --- | --- | --- | --- | --- |
| `CPlayerMove_PlayStepSound` | `CPlayerMove::PlayStepSound(int,float,bool)` | `CallEnd(-1)` | `CallEnd(-1)` | `_ZN11CPlayerMove13PlayStepSoundEifb` | ✅ |
| `PM_PlaySoundFX_SERVER` | `PM_PlaySoundFX_SERVER`（`__cdecl`） | `CallEnd(-1)` | `CallEnd(-1)` | `PM_PlaySoundFX_SERVER` | ✅ |
| `RegisterSCScriptColor24` | `RegisterSCScriptColor24(CASDocumentation*)`（`__cdecl`） | `Sig` | `Sig` | `_Z23RegisterSCScriptColor24P16CASDocumentation` | |

> `RegisterSCScriptColor24` 在 `fallguys/src/server_hook.cpp` 的 post-hook 中手工调用（非 inline hook），用于补注册颜色常量。

### 2.2 engine — 私有函数（hw.dll / hw.so）
| 符号 | 归属 | Windows | Linux 5.16 (i686) | Linux 5.15 Symbol | Hook |
| --- | --- | --- | --- | --- | --- |
| `build_number` | `build_number()` | `CallStart(0)` | `TyCallStart(0)` | `_Z12build_numberv`（5.15 先 `FILL_FROM_SYMBOL_NO_CHECK`） | |
| `SV_Physics` | `SV_Physics()` | `Sig` | `TyCallEnd(-1)` | `_Z10SV_Physicsv` | ✅ |
| `SV_PushEntity` | `SV_PushEntity(edict_t*,float[3])` | `Sig` | `TyCallStart(0)` | `_Z13SV_PushEntityP7edict_sPf` | ✅ |
| `SV_PushMove` | `SV_PushMove(edict_t*,float)` | `Sig` | `TyCallEnd(-1)` | `_Z11SV_PushMoveP7edict_sf` | ✅ |
| `SV_PushRotate` | `SV_PushRotate(edict_t*,float)` | `Sig` | `TyCallEnd(-9)` | `_Z13SV_PushRotateP7edict_sf` | ✅ |
| `SV_WriteMovevarsToClient` | `SV_WriteMovevarsToClient(sizebuf_t*)` | `Sig` | `TyCallStart(3)` | `_Z24SV_WriteMovevarsToClientP9sizebuf_s` | ✅ |
| `SV_SingleClipMoveToEntity` | `SV_SingleClipMoveToEntity(edict_t*,const float*,const float*,const float*,const float*,trace_t*)` | `CallEnd(-1)` | `TyCallEnd(-1)` | `_Z25SV_SingleClipMoveToEntityP7edict_sPKfS2_S2_S2_P7trace_t` | |
| `SV_SingleClipMoveToEntity_10152` | 同上，`build_number >= 10152` 时使用（签名与上者相同，多一个 `passedict` 参数） | `CallEnd(-1)` | `TyCallEnd(-1)` | 同上 | |

### 2.3 engine — 私有全局变量
| 变量 | 类型 | 含义 | Windows | Linux 5.16 | Linux 5.15 Symbol |
| --- | --- | --- | --- | --- | --- |
| `sv_models` | `model_t* (*)[8192]` | 服务端模型表 | `VarStart(13)` | 手工：`sv` + `offset_sv_models(0x276148)` | `sv` + `offset_sv_models` |
| `host_frametime` | `double*` | 帧时间 | `VarStart(7)` | 手工：GOT/PLT 解析（`host_frametime_Signature`） | `host_frametime` |
| `pmovevars` | `movevars_t*` | 物理参数 | `VarEnd(0)` | 手工：反汇编 `SV_WriteMovevarsToClient` 内的 GOT 引用 | `movevars` |
| `sv_areanodes` | `areanode_t (*)[32]` | 服务端碰撞区节点 | `VarStart(9)` | 手工：GOT/PLT 解析（`sv_areanodes_Signature`） | `sv_areanodes` |
| `pg_groupop` | `int*` | 物理组运算 | `VarEnd(-8)` | 手工：反汇编 `PF_SetGroupMask` 内的 GOT 引用 | `g_groupop` |
| `pg_groupmask` | `int*` | 物理组掩码 | `VarEnd(-2)` | 同上 | `g_groupmask` |

> 5.16 的 engine 变量在 `i686`/`hw.so` engine 类型下走手工解析（`gotplt_prolog` → GOT/PLT → 反汇编单指令 → `pfnDisasmRanges`），而非直接 `VAR_FROM_SIGNATURE_*`；只有 `build_number`/`SV_*` 用 `Ty` 系列签名。

### 2.4 fallguys 中**非**引擎私有符号（经引擎公共接口取得，列出以免混淆）
| 变量 | 来源 | 备注 |
| --- | --- | --- |
| `sv_gravity` | `CVAR_GET_POINTER("sv_gravity")` | 普通 cvar，非私有符号 |
| `mp_footsteps` | `CVAR_GET_POINTER("mp_footsteps")` | 普通 cvar，非私有符号 |
| `r_worldentity` / `r_worldmodel` | 本地全局（`engine_hook.cpp` 定义） | 非引擎私有 |

### 2.5 soundengine 中使用的 **FMOD 外部 DLL** 符号（非引擎私有，另注）
`FMOD_System_Create`、`FMOD_System_Init`、`FMOD_System_SetOutput`、`FMOD_System_SetCallback`、`FMOD_System_Close`、`FMOD_System_Release`、`FMOD_System_CreateSound`、`FMOD_Sound_GetLength`、`FMOD_Sound_GetFormat`、`FMOD_Sound_Release` —— 由 `fmodex.dll/fmodex.so`（`FMOD_DLL_NAME/PATH`）导出，通过 `pfnGetProcAddress` 取得，**不属于 server/hw 私有符号**。

---

## 三、引擎侧辅助签名常量（非符号本身，仅用于定位）
| 常量 | 模块 | 用途 |
| --- | --- | --- |
| `gotplt_prolog_Signature` | engine (Linux) | 定位 `__x86_get_pc_thunk_`，推导 GOT/PLT 基址 |
| `sv_model_Signature` | engine (Linux) | 定位 `sv_models` 引用指令 |
| `offset_sv_models` (=0x276148) | engine (Linux) | 由 `sv` 基址到 `sv_models` 的偏移 |
| `PF_SetGroupMask_Signature` | engine (Linux) | 定位 `PF_SetGroupMask`，进而取 `g_groupmask`/`g_groupop` |
| `pattern_CASHook_VCall` | server (Linux, asext 内联字符串) | 定位 `CASHook::VCall`，解析 `g_pServerManager` |

## 四、未启用 / 已注释的候选私有符号
| 符号 | 位置 | 现状 |
| --- | --- | --- |
| `SV_TestEntityPosition` | `fallguys/src/enginedef.h` + `signatures.h` | typedef/`PRIVATE_FUNCTION_EXTERN` 保留，但 `_Signature`/`_Symbol` 均被注释、`serverdef` 未填充；实际用的是本地 `SV_TestEntityPositionEx`（物理侧自行实现）。 |
| `SV_LinkEdict` | `fallguys/src/enginedef.h` + `signatures.h` | 全部注释。 |
| `CASBLOB_ReadData` / `CASBLOB_WriteData` | `asext/src/serverdef.h` | 仅在 asext 声明，见 §1.9。 |

## 五、注意事项
- **Windows 与 Linux 的 ABI 差异**：`SC_SERVER_DECL`（Win=`__fastcall`，Linux=空）与 `SC_SERVER_DUMMYARG` 系列宏用于适配；`begin`/`end` 等带隐藏结构体返回值的成员函数在两端参数顺序不同，改动签名必须同步两端。
- **版本分流**：Linux 通过 `CreateInterface("SCServerDLL003")` 区分 5.16（签名）与 5.15（符号）；engine 私有函数按 `pfnGetEngineType()`（`i686`/`hw.so`）走 `Ty` 系列签名。
- **caller 偏移极度敏感**：`CallStart/CallEnd(N)` 的 `N` 与游戏版本强绑定，维护 `signatures.h` 时需逐版本/平台核对。
- **变量获取是“定位 + 解引用”**：`VarStart/VarEnd(N)` 的 `N` 错位会直接读到错误地址导致崩溃。
- **失败即拒绝加载**：`FILL_FROM_SIGNATURE`/`FILL_FROM_SYMBOL`/`VAR_FROM_*` 均带判空，任一符号定位失败 `Meta_Attach` 返回 `FALSE`（`build_number` 为可空例外，用 `FILL_FROM_SYMBOL_NO_CHECK`）。
- **hook 语义**：未 hook 时 `g_call_original_X == g_pfn_X`；`INSTALL_INLINEHOOK` 后 `g_call_original_X` 变为 trampoline，调用原函数应走 `g_call_original_X`。

## 关联文档
- [game-private-funcs-vars](memory://metamod-fallguys/game-private-funcs-vars) — 引入机制与宏语义（配套阅读）
- [asext](memory://metamod-fallguys/asext)、[fallguys](memory://metamod-fallguys/fallguys) — 两个插件的模块职责与架构
