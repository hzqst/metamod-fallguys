# fallguys: SemiRenderEffects

## 概述
- SemiRenderEffects 用于 **按玩家覆盖渲染参数**：仅在 `AddToFullPack` 为某个玩家打包实体状态时，覆盖该玩家看到的 `rendermode`、`renderamt`、`rendercolor`、`renderfx`。
- 只影响 per-player 的网络状态（`entity_state_s`），不会改变实体本身的渲染/碰撞/物理，对其他玩家不产生影响。

## 核心实现
- 数据结构（`fallguys/physics.h`）：
  - `SemiRenderEffectsEntry`：`EHANDLE target` + `rendermode/renderamt/rendercolor/renderfx`。
  - `CGameObject` 持有 `std::unique_ptr<std::vector<SemiRenderEffectsEntry>> m_semi_render_effects`，默认 `nullptr`，按需分配（稀疏存储）。
- `CGameObject` 接口（`fallguys/physics.h`）：
  - `SetSemiRenderEffects`：若存储为空则分配；按 `entry.target.Get() == player` 查找，存在则更新，否则新增条目。
  - `UnsetSemiRenderEffects`：无存储或未找到返回 `false`；命中则 `erase` 并返回 `true`。
  - `UnsetSemiRenderEffectsToAll`：清空向量（不释放 `unique_ptr`）。
  - `FindSemiRenderEffects`：返回匹配条目的指针；未命中返回 `nullptr`。
- 入口 API（`fallguys/physics.cpp`）：
  - `CPhysicsManager::SetEntitySemiRenderEffects`：校验 `ent`/`player` 非空且非 `free`；无 `CGameObject` 时创建并 `AddGameObject`；调用 `SetSemiRenderEffects`。
  - `CPhysicsManager::UnsetEntitySemiRenderEffects`：校验 `ent`/`player`；无 `CGameObject` 则返回 `false`；调用 `UnsetSemiRenderEffects`。
  - `CPhysicsManager::UnsetEntitySemiRenderEffectsToAll`：校验 `ent`；无 `CGameObject` 则返回 `false`；调用 `UnsetSemiRenderEffectsToAll`。
- 生效点（`fallguys/physics.cpp`，`CGameObject::AddToFullPack`）：
  - 先做 SemiVisible 过滤；再处理 SemiClip/PMSemiClip（`state->solid = SOLID_NOT`）。
  - 随后 `FindSemiRenderEffects(host)` 命中则覆盖 `state->rendermode/renderamt/rendercolor/renderfx`。
  - 最后再执行 LOD 与 Follow 逻辑。

## AngelScript-level API（`fallguys/server_hook.cpp`）
- `bool CEntityFuncs::SetEntitySemiRenderEffects(edict_t@ ent, edict_t@ player, int rendermode, int renderamt, color24 rendercolor, int renderfx)`
- `bool CEntityFuncs::UnsetEntitySemiRenderEffects(edict_t@ ent, edict_t@ player)`
- `bool CEntityFuncs::UnsetEntitySemiRenderEffectsToAll(edict_t@ ent)`

## 注意事项
- 目标玩家通过 `EHANDLE` 存储，避免悬挂指针；匹配条件为 `entry.target.Get() == player`。
- 仅覆盖单个玩家看到的 render 字段；不影响其他玩家、实体碰撞或可见性过滤。
- `player` 为 `nullptr` 或 `free` 时直接返回 `false`；`ent` 为 `free` 时同样返回 `false`。
- `UnsetEntitySemiRenderEffectsToAll` 仅清空条目，不销毁 `CGameObject`。