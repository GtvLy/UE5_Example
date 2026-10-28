# Pickups 游戏设计文档

## 1. 项目概述

**Pickups（拾取收集）** 是一款基于 Unreal Engine 5 的第三人称**多人联机收集类**游戏。玩家操控角色在地图中移动、跳跃，拾取散布在场景中的可收集物（Pickup）以获取分数。当场景内所有收集物被拾取完毕后，比赛结束，系统按分数进行排名并展示计分板。

本项目为教程书籍配套练习项目，重点在于演示 UE 的**网络复制（Replication）**、**GameMode 比赛流程**、**GameState / PlayerState** 与**计分板 UI** 等多人游戏核心机制。相比 Dodgeball 项目，本项目更侧重于多人联机与状态同步。

## 2. 游戏目标与核心玩法循环

**核心玩法循环：**

```
探索地图 → 发现/拾取收集物 → 得分 → 收集物耗尽 → 比赛结束 → 查看排名
```

- **目标**：尽可能多地拾取收集物以获得更高分数。
- **结束条件**：`GameState` 中记录的剩余收集物归零（`ReadyToEndMatch`），比赛结束。
- **失败惩罚**：角色掉出世界（`FellOutOfWorld`）会扣除 10 分并重生。
- **排名**：比赛结束后，计分板按玩家分数降序排列。

## 3. 玩家体验与操控

| 输入 | 动作 |
|------|------|
| 移动（WASD / 手柄左摇杆） | 第三人称移动 |
| 视角（鼠标 / 手柄右摇杆） | 环绕视角控制 |
| 跳跃 | 跳跃 |

- 第三人称跟随相机（弹簧臂 400cm，使用 Pawn 控制旋转）。
- 移动速度 500cm/s，跳跃初速 500cm/s。
- 收集物带有旋转动画（绕 Y 轴 90°/秒），便于在场景中识别。

## 4. 核心机制

### 4.1 收集物拾取（Pickup）

- 收集物为旋转的静态网格体，使用 `OverlapAll` 碰撞配置。
- 玩家角色与收集物重叠时触发拾取（仅在服务器/权威端 `HasAuthority()` 生效）。
- 拾取效果：
  - `GameState` 剩余收集物计数 -1。
  - 玩家获得 10 分（`AddScore(10)`）。
  - 玩家个人拾取计数 +1（`AddPickups()`）。
  - 播放拾取音效（通过 `ClientPlaySound2D` RPC 在客户端播放）。
  - 收集物销毁。

### 4.2 分数系统

- 分数存储在 `PlayerState`（`SetScore`）中，由服务器权威管理。
- 拾取收集物 +10 分；掉出世界 -10 分。
- 计分板通过 `GameState::GetPlayerStatesOrderedByScore()` 按分数降序排序。

### 4.3 比赛流程（Match Flow）

- 使用 `AGameMode`（而非 `GameModeBase`），支持完整比赛状态机。
- `ReadyToStartMatch` 始终返回 `true`，比赛立即开始。
- 比赛开始时在屏幕显示绿色提示"The game has started!"。
- `ReadyToEndMatch` 在剩余收集物归零时返回 `true`，触发比赛结束。
- 比赛结束时销毁所有玩家角色，5 秒后通过 `ServerTravel` 重载地图（重新开始）。

### 4.4 掉出世界惩罚

- 角色掉出世界时：扣 10 分、销毁自身，并调用 `RestartPlayer` 在 Player Start 重生。

## 5. 角色与多人架构

- **玩家角色（PickupsCharacter）**：第三人称角色，携带落地/坠落音效，负责分数与拾取的本地封装。
- **玩家状态（PlayerState）**：记录个人拾取数量（`Pickups`，已复制）。
- **游戏状态（GameState）**：记录剩余收集物数量（`PickupsRemaining`，已复制），并提供按分数排序的玩家列表。
- **游戏模式（GameMode）**：驱动比赛生命周期与地图重载。
- **玩家控制器（PlayerController）**：管理输入映射与计分板 UI。

## 6. 关卡设计

- 以第三人称模板地图 `Lvl_ThirdPerson` 为基础。
- 场景中散布多个 `BP_Pickup` 收集物。
- 地图使用第三人称模板几何体（平台、斜坡等）与 Player Start 出生点。

## 7. UI / HUD

- **计分板（WBP_Scoreboard）**：显示所有玩家的排名、名称与分数。
  - `WBP_Score_Entry`：单行分数条目。
  - `WBP_Scoreboard_Header`：表头。
- 计分板由玩家控制器在本地创建并添加到视口，实时反映分数排名。

## 8. 音频

- 收集物拾取音效：`Pickup`。
- 角色音效：`Fall`（坠落）、`Land`（落地）、`Jump`（跳跃）、`Footstep`（脚步）。

## 9. 游戏变体（Variant）系统

与 Dodgeball 项目一致，本项目同样内置三套可复用游戏变体（战斗、平台跳跃、横版卷轴），各自拥有独立地图与蓝图。详见技术文档"共享变体框架"章节。

## 10. 技术选型概览

- **引擎**：Unreal Engine 5.6
- **多人联机**：UE 复制（`bReplicates`、`DOREPLIFETIME`、Client RPC）
- **输入**：Enhanced Input
- **比赛流程**：`AGameMode` 状态机
- **UI**：UMG
