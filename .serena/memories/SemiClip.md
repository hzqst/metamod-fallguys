# SemiClip（fallguys）

## 概述
- SemiClip 是按实体/玩家定向的“半碰撞”机制：让某个实体对特定玩家/实体变为非实体碰撞，同时在客户端侧把其 `solid` 设为 `SOLID_NOT`（仅对指定玩家生效）。
- 逻辑分两路：`SemiClip`（影响服务器碰撞 + PlayerMove + 客户端 `solid`）和 `PMSemiClip`（影响 PlayerMove + 客户端 `solid`，不影响服务器 `ShouldCollide`）。

## 核心实现
- 状态存储：`CGameObject` 内部维护 `m_semiclip_to_entities` 与 `m_pm_semiclip_to_entities`（`std::vector<EHANDLE>`），并提供 `Set/Unset/Is*` 方法；`SetPlayerSemiClipMask`/`SetPlayerPMSemiClipMask` 通过 `player_mask` 遍历 `gpGlobals->maxClients` 设置/清除（`fallguys/physics.h`）。
- 设置入口：`CPhysicsManager::SetEntitySemiClip*`/`SetEntityPMSemiClip*` 在 `fallguys/physics.cpp` 中实现，若 `CGameObject` 不存在会创建并 `AddGameObject`。
- 服务器碰撞过滤：`CPhysicsManager::ShouldCollide` 检查 `IsSemiClipToEntity`，在 `NewShouldCollide`（`fallguys/dllapi.cpp`）中使用，返回 false 时 `MRES_SUPERCEDE` 禁止碰撞。
- PlayerMove 过滤：`PM_StartMove` 在 `NewPM_Move` 中调用，通过 `PM_ShouldCollide` 判断：若当前玩家在 `IsSemiClipToEntity` 或 `IsPMSemiClipToEntity` 列表中，则从 `pmove->physents` 排除（`fallguys/physics.cpp`）。
- 客户端 `solid`：`CGameObject::AddToFullPack` 若 host 在半碰撞列表中，则将 `state->solid = SOLID_NOT`，让客户端认为该实体对该玩家不可碰撞（`fallguys/physics.cpp`）。
- 特殊流程（SolidOptimizer）：`CGameObject::StartFrame` 若启用 `SolidOptimizer`，会先对所有“solid player”设置半碰撞；`CSolidOptimizerGhostPhysicObject::OnTouchRigidBody` 在玩家触碰时调用 `RemovePlayerSemiClipMask` 逐一解除（`fallguys/physics.cpp`）。该特性可以大幅度减少客户端侧不必要的ClipNode碰撞检测计算。

## 注意事项
- `player_mask` 按玩家索引 1..`gpGlobals->maxClients` 对应位 `(1 << (entindex - 1))`；`SetEntitySemiClipToPlayer`/`Unset...` 仅接受玩家索引范围（`fallguys/server_hook.cpp`）。
- `SemiClip` 与 `PMSemiClip` 的差异：`PMSemiClip` 不影响 `ShouldCollide`（服务器碰撞），但会影响 PM 过滤与客户端 `solid`；`SemiClip` 影响全部路径。
- `SetEntitySemiClip*` 会在实体尚未有 `CGameObject` 时创建对象；实体释放由 `RemoveGameObject` 处理，调用端无需手动创建。
- `PM_StartMove` 使用 `std::remove_if` 过滤 `pmove->physents`，但未显式更新 `numphysent`（可作为行为注意点进行验证）。

## AngelScript-level API（`CEntityFuncs`）

### 预定义
- 预定义宏：`METAMOD_PLUGIN_FALLGUYS`（脚本可用于条件编译）。

### CEntityFuncs 方法
- `bool SetEntitySemiClip(edict_t@ ent, int player_mask)`
- `bool SetEntitySemiClipToPlayer(edict_t@ ent, int entindex)`
- `bool UnsetEntitySemiClipToPlayer(edict_t@ ent, int entindex)`
- `bool SetEntitySemiClipToEntityIndex(edict_t@ ent, int entindex)`
- `bool UnsetEntitySemiClipToEntityIndex(edict_t@ ent, int entindex)`
- `bool SetEntitySemiClipToEntity(edict_t@ ent, edict_t@ targetEntity)`
- `bool UnsetEntitySemiClipToEntity(edict_t@ ent, edict_t@ targetEntity)`
- `bool UnsetEntitySemiClipToAll(edict_t@ ent)`
- `bool SetEntityPMSemiClip(edict_t@ ent, int player_mask)`
- `bool SetEntityPMSemiClipToPlayer(edict_t@ ent, int entindex)`
- `bool UnsetEntityPMSemiClipToPlayer(edict_t@ ent, int entindex)`
- `bool SetEntityPMSemiClipToEntityIndex(edict_t@ ent, int entindex)`
- `bool UnsetEntityPMSemiClipToEntityIndex(edict_t@ ent, int entindex)`
- `bool SetEntityPMSemiClipToEntity(edict_t@ ent, edict_t@ targetEntity)`
- `bool UnsetEntityPMSemiClipToEntity(edict_t@ ent, edict_t@ targetEntity)`
- `bool UnsetEntityPMSemiClipToAll(edict_t@ ent)`