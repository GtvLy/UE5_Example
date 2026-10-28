# UE5 C++ 游戏开发教程项目合集

> 基于《UNREAL ENGINE 5 C++ 游戏开发 完全学习教程》一书复刻的三个 Unreal Engine 5.6 项目，涵盖第三人称动作 / 生存、多人联机收集、横版卷轴平台三种玩法，重点练习 UE5 的 C++ 开发、增强输入（Enhanced Input）、AI、网络复制、UI 等核心机制。

## 项目列表

本仓库包含三个独立的 UE5 项目（均使用 Unreal Engine 5.6，第三人称模板为起点）：

| 项目 | 类型 | 重点 |
| --- | --- | --- |
| **Dodgeball** | 第三人称动作 / 生存 | 生命系统、敌人 AI、视线检测、投射物、UMG 血条 |
| **Pickups** | 多人联机收集 | 网络复制（Replication）、GameMode 比赛流程、GameState / PlayerState、计分板 |
| **SuperSideScroller** | 横版卷轴平台 | 冲刺 / 投掷 / 收集 / 道具、行为树 AI、动画蒙太奇 |

> 三个项目均内置一套可复用的「游戏变体（Variant）」框架：**战斗（Combat）**、**平台跳跃（Platforming）**、**横版卷轴（SideScrolling）**，各自拥有独立地图与蓝图，可切换 GameMode 演示。

## 一、Dodgeball（躲避球）

玩家操控角色在关卡中移动、跳跃、闪避，躲避由 AI 敌人持续投掷的躲避球。角色拥有生命值，被球命中会损失生命，生命归零即失败；坚持生存并抵达胜利区域即可获胜。

### 截图

| 截图 1 | 截图 2 | 截图 3 |
| --- | --- | --- |
| ![Dodgeball_01](Output/Dodgeball_01.png) | ![Dodgeball_02](Output/Dodgeball_02.png) | ![Dodgeball_03](Output/Dodgeball_03.png) |

### 核心机制

- **生命系统**：`HealthComponent`，100 HP，被球命中扣 34，归零触发死亡与重开界面。
- **视线检测**：`LookAtActorComponent` 基于自定义碰撞通道的射线检测，看到玩家才投掷。
- **躲避球投射物**：物理模拟的弹性球体（可反弹），命中玩家扣血、播放粒子与音效。
- **胜利与重开**：进入 `VictoryBox` 获胜；死亡弹出 `RestartWidget` 重开界面。

## 二、Pickups（拾取收集）

玩家在地图中移动、跳跃，拾取散布的收集物获得分数；收集物耗尽后比赛结束，按分数排名展示计分板。重点演示 UE 的**网络复制**与**多人比赛流程**。

### 截图

| 截图 1 | 截图 2 | 截图 3 |
| --- | --- | --- |
| ![Pickups_01](Output/Pickups_01.png) | ![Pickups_02](Output/Pickups_02.png) | ![Pickups_03](Output/Pickups_03.png) |

### 核心机制

- **收集物拾取**：`APickup`（`bReplicates`），重叠时在服务器端结算：剩余 -1、+10 分、销毁。
- **分数系统**：分数存 `PlayerState`（`SetScore`），计分板按分数降序。
- **比赛流程**：`AGameMode` 完整状态机，剩余收集物归零即结束，5 秒后 `ServerTravel` 重载地图。
- **掉出世界惩罚**：扣 10 分并在 Player Start 重生。

## 三、SuperSideScroller（超级横版卷轴）

横版卷轴平台动作游戏：玩家奔跑、跳跃，收集金币与道具，投掷投射物击败由行为树 AI 驱动的敌人。

### 截图

| 截图 1 | 截图 2 | 截图 3 |
| --- | --- | --- |
| ![SuperSideScroller_01](Output/SuperSideScroller_01.png) | ![SuperSideScroller_02](Output/SuperSideScroller_02.png) | ![SuperSideScroller_03](Output/SuperSideScroller_03.png) |

### 核心机制

- **冲刺（Sprint）**：长按冲刺键速度 300 → 500cm/s。
- **投掷投射物**：动画蒙太奇 `AM_Throw` + AnimNotify 在骨骼插槽生成投射物，命中敌人摧毁之。
- **拾取物继承体系**：`APickableActor_Base` → `Collectable`（金币）/ `Powerup`（加速道具，持续 8 秒）。
- **可破坏砖块**：撞击含收集物的砖块获得收集物并销毁。
- **行为树 AI**：敌人由 `BT_EnemyAI` + 黑板 + AI 控制器驱动巡逻与追击。

## 目录结构

```
Example/
├── Dodgeball/                 # 躲避球项目
│   ├── Dodgeball.uproject
│   ├── Source/Dodgeball/      # C++ 源码（含三个 Variant 子目录）
│   └── Content/               # 蓝图 / 资源
├── Pickups/                   # 拾取收集项目
├── SuperSideScroller/         # 横版卷轴项目
├── Output/                    # 运行截图
├── .gitignore                 # 忽略 UE 生成的 Binaries / Intermediate / Saved 等
└── .gitattributes             # Git LFS 规则（.uasset / .umap 等）
```

## 构建与运行

每个项目独立构建：

1. 右键对应 `.uproject` → **Generate Visual Studio project files**。
2. 编译目标：`<Project>Editor.Target.cs`（编辑器）/ `<Project>.Target.cs`（游戏）。
3. 在 UE 编辑器中打开主地图直接运行。

> 引擎版本：**Unreal Engine 5.6**；输入系统：**Enhanced Input**；Pickups 可通过「播放器数量 > 1」测试多人联机。

## 参考

- 书籍：《UNREAL ENGINE 5 C++ 游戏开发 完全学习教程（游戏开发者之书）》
