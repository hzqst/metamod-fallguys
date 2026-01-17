# fallguys: Semi-Visible

## 概述
- “Semi-Visible” 是 **按玩家可见性过滤** 的机制：对某个实体设置一个玩家位掩码，只有被允许的玩家才会在 `AddToFullPack` 阶段收到该实体的网络状态，从而“看见”该实体；未在掩码里的玩家直接被过滤（实体不会进入该玩家的 full pack）。
- 该功能只影响 **网络可见性**，不修改模型渲染参数，也不改变碰撞/物理；与 `SemiClip`（半碰撞）是独立的功能。

## 核心实现
- 入口 API：`CPhysicsManager::SetEntitySemiVisible(edict_t* ent, int player_mask)`（`fallguys/physics.cpp`）
  - 若 `ent->free` 则返回 false。
  - 通过 `GetGameObject(ent)` 获取 `CGameObject`，不存在就创建并 `AddGameObject`。
  - 调用 `CGameObject::SetSemiVisibleMask(player_mask)` 保存掩码。
- 数据存储：`CGameObject` 里维护 `m_semi_vis_mask`（`fallguys/physics.h`）。
- 生效点：`CGameObject::AddToFullPack(...)`（`fallguys/physics.cpp`）
  - 若 `GetSemiVisibleMask() != 0`：
    - 取 `hostindex = g_engfuncs.pfnIndexOfEdict(host)`。
    - 使用位判断：`mask & (1 << (hostindex - 1))`。
    - 若结果为 0，直接 `return false`，导致该实体对该玩家不可见。
  - 该逻辑通过 `CPhysicsManager::AddToFullPack` 被 `dllapi` 的 `NewAddToFullPack_Post` 调用。

## 注意事项
- **掩码位序**：`player_mask` 使用 `1 << (playerIndex - 1)`（玩家索引 1..maxClients）。
- **mask=0 的含义**：不启用 Semi-Visible 过滤（所有玩家都能看到）。
- **不影响物理**：该机制不改变实体的 `solid` 或物理碰撞；仅在 full pack 阶段决定是否发送实体状态。
- **无增量 API**：仅提供“设置掩码”的入口（`SetSemiVisibleMask`/`SetEntitySemiVisible`），没有单独的“add/remove 单个玩家”接口；需要重新传入完整 mask 或置 0 关闭。
- **无需已有物理对象**：`SetEntitySemiVisible` 会在没有 `CGameObject` 时创建，确保后续 `AddToFullPack` 生效。

## AngelScript-level API
- `CEntityFuncs::SetEntityPartialViewer(edict_t@ ent, int player_mask) -> bool`
  - 与 `SetEntitySemiVisible` 为同一实现（别名/兼容接口）。
- `CEntityFuncs::SetEntitySemiVisible(edict_t@ ent, int player_mask) -> bool`
  - 直接调用 `CPhysicsManager::SetEntitySemiVisible`。

相关注册位置：`fallguys/server_hook.cpp`（`ASEXT_RegisterObjectMethod`）。