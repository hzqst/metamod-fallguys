# fallguys 模块源码级分析

## 概述
- fallguys 是一个 Metamod 插件（动态库），通过签名扫描/符号解析挂接引擎与 server 私有函数，提供“物理系统 + 特殊推挤/碰撞逻辑 + 自定义脚步声 + AngelScript 扩展接口”。
- 对外 API 主要体现在 AngelScript 侧：注册一系列类型与方法（物理对象、车辆、声音信息等）以及事件 Hook。

## 职责
- **引擎/服务器 Hook**：在引擎与 server 的关键路径上插入逻辑（SV_Physics、SV_PushEntity、PM_PlaySoundFX_SERVER 等）。
- **物理系统集成**：维护 `CPhysicsManager`，构建/管理 Bullet 物理世界，驱动每帧仿真与实体创建、碰撞过滤、可见性、触发器/水体/车辆等。
- **AngelScript 交互**：通过 `asext` 注册 AS 类型/方法与 Hook，实现脚本与引擎/物理交互。
- **自定义脚步声**：基于 engine/movevars 与 server 音效回调改写脚步声逻辑。
- **兼容与特例**：处理 Sven Co-op 5.15/5.16、Fall Guys S1/S2/S3 的兼容差异。

## 架构
- **入口与生命周期**：`fallguys/meta_api.cpp`（`Meta_Query`/`Meta_Attach`/`Meta_Detach`）。
- **Metamod DLL 接口**：`fallguys/dllapi.cpp`（`GetEntityAPI2`/`GetEntityAPI2_Post`/`GetNewDLLFunctions` 与大量 `New*` Hook 入口）。
- **引擎 Hook 层**：`fallguys/engine_hook.cpp` + `fallguys/enginedef.h`（SV_*、movevars、areanodes、groupmask 等）。
- **服务器 Hook 层**：`fallguys/server_hook.cpp` + `fallguys/serverdef.h`（CPlayerMove/PM_PlaySoundFX 等 + AS 注册）。
- **物理系统核心**：`fallguys/physics.h/.cpp`（`CPhysicsManager` 及大量结构体/参数类型）。
- **声音引擎**：`fallguys/soundengine.h/.cpp`（FMOD 动态加载与 SoundInfo 查询）。
- **配置/构建**：`fallguys/CMakeLists.txt`、`fallguys/Config.mak`、`fallguys/signatures.h`。

## 核心实现与 workflow
1. **加载与初始化**（`Meta_Query`/`Meta_Attach`）：
   - 校验 Metamod 版本（>= 5:16），保存 `gpMetaUtilFuncs`。
   - 获取 engine/server 模块基址与代码段范围，定位引擎/服务器私有函数与全局变量（签名/符号）。
   - 动态加载 `asext`（`LOAD_PLUGIN` + `IMPORT_ASEXT_API`）。
   - Windows/非 Windows 分支：根据 `build_number`、`SCServerDLL003` 等条件选择不同签名或符号路径。
   - 初始化 FMOD、安装 engine/server Hook、注册 AngelScript 方法与 Hook（`RegisterAngelScriptMethods` / `RegisterAngelScriptHooks`）。

2. **游戏/地图生命周期**（`dllapi.cpp`）：
   - `NewGameInit_Post`：缓存 `sv_gravity`/`mp_footsteps` 并初始化 `gPhysicsManager`。
   - `NewStartFrame`：`EntityStartFrame` -> 更新重力 -> `StepSimulation` -> `EntityStartFrame_Post`。
   - `NewSpawn`/`NewSpawn_Post`：对 world 实体触发 `PreSpawn`/`PostSpawn`。
   - `NewServerActivate`：根据地图名切换 Season1/物理世界启用策略。
   - `NewServerDeactivate`：重置状态、关闭自定义脚步声、禁用物理并清理实体。
   - `NewPM_Move`/`NewPM_Move_Post`：驱动物理与 PM 过程的开始/结束。
   - `NewAddToFullPack_Post`：结合 `CPhysicsManager::AddToFullPack` 与 AS Hook 决定实体可见性。
   - `NewOnFreeEntPrivateData`：释放实体私有数据。
   - `NewShouldCollide`：调用 `CPhysicsManager::ShouldCollide` 并可强制阻止碰撞。
   - `NewTouch`/`NewBlocked`/`NewSetAbsBox`：处理触发器/玩家接触/阻挡与 Season1 超推力逻辑。

3. **引擎 Hook**（`engine_hook.cpp`）：
   - `NewSV_Physics`：执行原始 SV_Physics 后调用 `EntityEndFrame`。
   - `NewSV_PushEntity`/`NewSV_PushMove`/`NewSV_PushRotate`：处理“SuperPusher”与玩家推挤队列，必要时手动触发 `pfnTouch`。
   - `NewSV_WriteMovevarsToClient`：在自定义脚步声模式下暂时修改 movevars->footsteps。

4. **服务器 Hook + AS 注册**（`server_hook.cpp`）：
   - `NewCPlayerMove_PlayStepSound`/`NewPM_PlaySoundFX_SERVER`：结合 AS Hook 与自定义脚步声开关控制行为。
   - `RegisterAngelScriptMethods`：向 AS 注册大量物理类型/方法（`Physic*`、`EnvStudioKeyframe`、`SoundEngine_SoundInfo` 等），并注册脚本宏 `METAMOD_PLUGIN_FALLGUYS`。
   - `RegisterAngelScriptHooks`（`fallguys.cpp`）：注册 `PlayerAddToFullPack`、`PlayerPostThinkPost`、`PlayerTouchTrigger`、`PlayerTouchImpact`、`PlayerTouchPlayer`、`PlayerMovePlayStepSound`、`PlayerMovePlaySoundFX` 等 Hook。

5. **物理系统核心**（`physics.cpp`）：
   - `CPhysicsManager::Init` 创建 Bullet 物理世界与回调；`StepSimulation` 驱动仿真；`Shutdown` 清理所有资源。
   - 提供创建/管理实体、车辆、触发器/水体、碰撞过滤、可见性/半透明/半碰撞、冲量/力、LOD 等大量能力。

6. **声音引擎**（`soundengine.cpp`）：
   - `LoadFMOD_Server` 动态加载 `fmodex` 并解析 API；`SoundEngine_GetSoundInfo` 读取声音文件并返回格式/长度等。

## 依赖
- **Metamod/HL SDK**：`meta_api.h`/`dllapi.h`/`extdll.h`，Hook 表与引擎/服务器 API。
- **asext**：`asext/include/asext_api.h`（AS 扩展、Hook 注册与调用）。
- **Bullet3**：物理系统核心（`CPhysicsManager`）。
- **FMOD Ex**：声音信息查询（动态加载 `fmodex`）。
- **Capstone/反汇编**：Linux 分支通过 `gpMetaUtilFuncs->pfnDisasm*` 解析 GOT/PLT 寻址。
- **签名扫描**：`metamod/signatures_template.h` + `fallguys/signatures.h`。

## 注意事项
- **版本/平台敏感**：大量签名扫描与符号依赖游戏版本、平台与引擎类型（`build_number`、`SCServerDLL003` 等）。
- **Hook 依赖 `asext`**：`Meta_Attach` 必须成功加载 `asext`，否则 AS 注册与 Hook 不可用。
- **自定义脚步声**：依赖 `mp_footsteps`/`movevars->footsteps`，需要严格在 Hook 中维护临时状态。
- **Season1 兼容逻辑**：`NewServerActivate`/`NewBlocked`/`NewSV_PushEntity` 中存在与地图/旧版本行为绑定的特殊处理。
- **资源清理**：`Meta_Detach` 中调用 `gPhysicsManager.Shutdown()` 与 `UnloadFMOD()`；若新增资源/Hook，需要同步处理释放。
