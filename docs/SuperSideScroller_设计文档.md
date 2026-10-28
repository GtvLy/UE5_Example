# SuperSideScroller 游戏设计文档

## 1. 项目概述

**SuperSideScroller（超级横版卷轴）** 是一款基于 Unreal Engine 5 的**横版卷轴平台动作**游戏。玩家操控角色在关卡中奔跑、跳跃，收集金币（Collectable）与道具（Powerup），并投掷投射物击败敌人。敌人由行为树（Behavior Tree）AI 驱动巡逻与追击。

本项目为教程书籍配套练习项目，重点演示**横版卷轴玩法**、**自定义角色/敌人**、**行为树 AI**、**拾取物继承体系**、**能力道具**与**投射物**等机制。

## 2. 游戏目标与核心玩法循环

**核心玩法循环：**

```
移动/跳跃前进 → 收集金币 → 拾取加速道具 → 投掷投射物击败敌人 → 击碎砖块 → 收集奖励
```

- **目标**：探索关卡、收集金币、击败敌人、利用道具提升能力。
- **收集**：金币（`Collectable`）增加计数；道具（`Powerup`）临时强化移动与跳跃。
- **战斗**：投掷投射物（`PlayerProjectile`）摧毁敌人。
- **破坏**：撞击可破坏砖块（`Brick`）获得其中的收集物。

## 3. 玩家体验与操控

| 输入 | 动作 |
|------|------|
| 移动（WASD / 手柄） | 横版左右移动 |
| 跳跃 | 跳跃 |
| 冲刺（`IA_Sprint`，长按） | 加速奔跑 |
| 投掷（`IA_Throw`） | 投掷投射物 |

- 角色基础移动速度 300cm/s，冲刺时 500cm/s。
- 基础跳跃初速 1000cm/s。
- 投掷动作通过动画蒙太奇（`AM_Throw`）触发，在动画通知处生成投射物。

## 4. 核心机制

### 4.1 冲刺（Sprint）

- 长按冲刺键进入冲刺状态，移动速度从 300 提升至 500cm/s。
- 松开后恢复。

### 4.2 投掷投射物（Projectile）

- 触发投掷动作后播放 `AM_Throw` 蒙太奇。
- 动画通知 `Anim_ProjectileNotify` 在角色手部/骨骼插槽（`ProjectileSocket`）生成投射物。
- 投射物为无重力直线飞行（速度 800cm/s），命中敌人时摧毁敌人并自爆，3 秒后自动销毁。

### 4.3 收集物（Collectable）

- 金币类收集物，重叠时增加玩家收集计数（`NumberOfCollectables`），随后销毁。
- 收集物带旋转动画，便于识别。

### 4.4 道具（Powerup）

- 拾取后临时强化：移动速度提升至 500cm/s（冲刺 900cm/s），跳跃初速提升至 1500cm/s。
- 持续 8 秒后自动失效，恢复基础数值（移动 300、跳跃 1000）。

### 4.5 可破坏砖块（Brick）

- 砖块为可破坏的静态网格体（`BlockAll` 碰撞）。
- 玩家撞击砖块时：若砖块配置为包含收集物（`bHasCollectable`），则增加对应收集物；随后砖块销毁。

### 4.6 敌人（Enemy）

- 敌人由**行为树 AI** 驱动，行为包括寻找位置、寻找玩家等任务。
- 玩家投射物命中敌人时，敌人被摧毁。

## 5. 角色与敌人

### 5.1 玩家角色（SuperSideScroller_Player）

- 继承自 `ASuperSideScrollerCharacter`，附加冲刺、投掷、收集计数与道具能力。
- 拥有收集计数（`NumberOfCollectables`）与道具激活状态（`bHasPowerupActive`）。

### 5.2 敌人（EnemyBase）

- 继承自 `ASuperSideScrollerCharacter`，提供 `DestroyEnemy()` 销毁接口。
- 具体敌人为蓝图 `BP_Enemy01`，配属行为树 AI 控制器 `BP_AIControllerEnemy`。

## 6. 关卡设计

- 主地图 `SuperSideScroller.umap`，采用横版卷轴布局。
- 场景元素：
  - **砖块（BP_Brick）**：可破坏、可藏收集物。
  - **收集物（BP_Collectable）**：金币。
  - **道具（BP_Powerup）**：加速强化。
  - **敌人（BP_Enemy01）**：行为树 AI 敌人。
  - **AI 巡逻点（BP_AIPoints）**：供行为树寻路使用。

## 7. UI / HUD

- **金币计数（BP_UI_CoinCollection）**：显示玩家收集到的金币数量。

## 8. 音频与视觉

- 敌人爆炸音效（`Explosion_Cue`）。
- 使用自定义角色模型（`MainCharacter` 骨骼网格体）与专属动画（Idle、Walking、Running、Jump、Throw 等）。
- 敌人使用独立材质（`MI_Enemy01`）。

## 9. 游戏变体（Variant）系统

与 Dodgeball、Pickups 项目一致，本项目同样内置三套可复用游戏变体（战斗、平台跳跃、横版卷轴），各自拥有独立地图与蓝图。详见技术文档"共享变体框架"章节。

## 10. 技术选型概览

- **引擎**：Unreal Engine 5.6
- **AI**：行为树（Behavior Tree）+ 黑板（Blackboard）+ AI 控制器（核心敌人）；StateTree（变体 AI）
- **输入**：Enhanced Input
- **动画**：动画蒙太奇 + AnimNotify 驱动投射物生成
- **UI**：UMG
