# SemiRenderEffects 计划

## 目标
- 在 fallguys 中新增 SemiRenderEffects 功能：允许实体对指定 player 使用独立的 rendermode、renderamt、rendercolor、renderfx 组合，仅在 AddToFullPack 打包到该 player 时生效。
- 提供 AngelScript-Level API（参数顺序与 entity_state_s 定义一致）：
  - `g_EntityFuncs.SetEntitySemiRenderEffects(edict_t@ pent, edict_t@ player, int rendermode, int renderamt, color24 rendercolor, int renderfx)`
  - `g_EntityFuncs.UnsetEntitySemiRenderEffects(edict_t@ pent, edict_t@ player)`
  - `g_EntityFuncs.UnsetEntitySemiRenderEffectsToAll(edict_t@ pent)`

## 计划步骤
1. 调研现有实现：定位 SemiVisible/SemiClip 的数据结构、CGameObject::AddToFullPack 的处理逻辑、以及 g_EntityFuncs 的脚本绑定位置，梳理需要改动的文件与函数。

### 调研结论（SemiVisible / SemiClip）
- `fallguys/physics.h`：`CGameObject` 持有 `m_semi_vis_mask`、`m_semiclip_to_entities`、`m_pm_semiclip_to_entities`；提供 `SetSemiVisibleMask/GetSemiVisibleMask`、`Set/Unset/IsSemiClipToEntity`、`SetPlayerSemiClipMask` 等接口；`CGameObject::AddToFullPack` 在此声明。
- `fallguys/physics.cpp`：`CPhysicsManager::SetEntitySemiVisible` 负责创建/获取 `CGameObject` 并写入掩码；`CGameObject::AddToFullPack` 里先做 SemiVisible 过滤（mask 不命中则直接 return false），再处理 SemiClip（命中则改 `state->solid = SOLID_NOT`）；`CPhysicsManager::SetEntitySemiClip*` / `UnsetEntitySemiClip*` 提供设置入口。
- `fallguys/dllapi.cpp`：`NewAddToFullPack_Post` 调用 `CPhysicsManager::AddToFullPack`，是 `CGameObject::AddToFullPack` 的链路入口。
- `fallguys/server_hook.cpp`：AS 绑定 `CASEntityFuncs__SetEntitySemiVisible`、`CASEntityFuncs__SetEntitySemiClip*`，并通过 `ASEXT_RegisterObjectMethod` 注册到 `CEntityFuncs`。
- `README_FALLGUYS.md`：已有 SemiVisible/SemiClip 的脚本文档与示例。

### AddToFullPack 现状（插入点确认）
- `fallguys/physics.cpp` 的 `CGameObject::AddToFullPack` 当前顺序：
  1) SemiVisible：`m_semi_vis_mask` 不命中则 `return false`（直接过滤实体）。
  2) SemiClip/PMSemiClip：命中则 `state->solid = SOLID_NOT`。
  3) LOD：`GetLevelOfDetailFlags` 生效时按距离调整 `state->body/modelindex/scale`。
  4) Follow：`m_follow_flags` 命中时调整 `state->aiment/movetype`。
  5) `return true`。
- SemiRenderEffects 的插入点应在 **SemiVisible 过滤之后**，并在 **LOD / Follow 之前** 覆盖 `state->render*`，以免被后续逻辑覆盖（当前逻辑不会修改 render*，但保持一致性）。

### 预计需改文件/函数（基于调研）
- `fallguys/physics.h`：扩展 `CGameObject` 存储与接口（新增 SemiRenderEffects 存储结构与 Set/Unset 查询），必要时新增辅助结构体。
- `fallguys/physics.cpp`：实现 `CPhysicsManager::Set/UnsetEntitySemiRenderEffects*`；在 `CGameObject::AddToFullPack` 中按 host 覆盖 `state->render*`。
- `fallguys/server_hook.cpp`：新增 AS 绑定 `g_EntityFuncs.SetEntitySemiRenderEffects` / `Unset...` / `Unset...ToAll`。
- `README_FALLGUYS.md`（或其他相关文档）：补充新 API 使用说明。
2. 设计数据结构：在 CGameObject 内新增按需创建的"稀疏"结构，按 player 存储 RenderEffects 组合，保证未设置时几乎零内存占用，并支持 per-player 不同值。
   - 存储建议：在 `CGameObject` 增加 `std::unique_ptr<CSemiRenderEffectsStore>`（或等价指针），默认 `nullptr`，首次 `SetEntitySemiRenderEffects` 时分配，未设置时仅占一个指针大小。
   - 条目结构（字段顺序与 entity_state_s 一致）：
     ```cpp
     struct SemiRenderEffectsEntry {
         EHANDLE target;      // 目标 player，用 EHANDLE 避免悬挂指针
         int rendermode;
         int renderamt;
         color24 rendercolor; // 直接使用 color24（AS 已注册该类型）
         int renderfx;
     };
     ```
   - 容器选择：`std::vector<SemiRenderEffectsEntry>` 线性查找（预期条目数很少）；无需预分配或固定大小数组，避免每实体常驻大内存。
   - 查询接口草案（CGameObject）：
     - `void SetSemiRenderEffects(edict_t* player, int rendermode, int renderamt, color24 rendercolor, int renderfx);`
     - `bool UnsetSemiRenderEffects(edict_t* player);`
     - `void UnsetSemiRenderEffectsToAll();`
     - `const SemiRenderEffectsEntry* FindSemiRenderEffects(edict_t* player) const;`（AddToFullPack 用于读取）
   - 入口接口草案（CPhysicsManager）：
     - `bool SetEntitySemiRenderEffects(edict_t* ent, edict_t* player, int rendermode, int renderamt, color24 rendercolor, int renderfx);`
     - `bool UnsetEntitySemiRenderEffects(edict_t* ent, edict_t* player);`
     - `bool UnsetEntitySemiRenderEffectsToAll(edict_t* ent);`
   - AddToFullPack 插入点：在 SemiVisible 过滤之后、LOD/Follow 之前，若 `FindSemiRenderEffects(host)` 命中则覆盖 `state->rendermode/renderamt/rendercolor/renderfx`。
3. 实现 C++ 逻辑：新增 Set/Unset/UnsetToAll 接口与存储；在 AddToFullPack 中按 "player -> watching -> pent" 分支覆盖 state->render* 字段。
4. 暴露脚本 API：在 as 探针或 g_EntityFuncs 绑定处新增函数，参数与命名对齐需求，并更新相关文档或示例（如有）。
5. 自检与验证：本地构建或最小测试脚本验证 per-player 生效、默认不影响其他 player、Unset 行为正确。
