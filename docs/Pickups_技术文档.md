# Pickups 技术文档

## 1. 项目概况

| 项目 | 说明 |
|------|------|
| 引擎版本 | Unreal Engine 5.6 |
| 模板 | Third Person Game Template |
| 模块 | `Pickups`（Runtime） |
| 依赖模块 | Engine、AIModule、UMG |
| 启用插件 | ModelingToolsEditorMode、StateTree、GameplayStateTree |
| 输入系统 | Enhanced Input |
| 主地图 | `/Game/ThirdPerson/Lvl_ThirdPerson` |
| 默认 GameMode | `BP_GameMode`（`/Game/Blueprints/BP_GameMode`） |

## 2. 项目目录结构

```
Pickups/
├── Pickups.uproject
├── Config/             # 含 Layouts/ 目录（编辑器布局）
├── Source/Pickups/
│   ├── Pickup.h/.cpp               # 收集物
│   ├── PickupsCharacter.h/.cpp     # 玩家角色
│   ├── PickupsGameMode.h/.cpp      # 游戏模式（AGameMode）
│   ├── PickupsGameState.h/.cpp     # 游戏状态
│   ├── PickupsPlayerState.h/.cpp   # 玩家状态
│   ├── PickupsPlayerController.h/.cpp
│   └── Variant_Combat/ | Variant_Platforming/ | Variant_SideScrolling/
├── Content/
│   ├── Blueprints/      # BP_GameMode、BP_Pickup、BP_PlayerController
│   ├── Sounds/          # Fall、Footstep、Jump、Land、Pickup
│   ├── UI/              # WBP_Scoreboard、WBP_Score_Entry、WBP_Scoreboard_Header
│   └── ...
└── ...
```

## 3. 模块依赖（Build.cs）

`Pickups.Build.cs` 依赖：`Core`、`CoreUObject`、`Engine`、`InputCore`、`EnhancedInput`、`AIModule`、`StateTreeModule`、`GameplayStateTreeModule`、`UMG`、`Slate`。

`PublicIncludePaths` 包含项目根目录及三个 Variant 子目录。

## 4. 架构总览

### 4.1 类继承关系

```
AActor
└── APickup                    (收集物，replicated)

ACharacter
└── APickupsCharacter          (玩家角色，abstract)

AGameMode
└── APickupsGameMode           (比赛流程，abstract)

AGameState
└── APickupsGameState          (剩余收集物计数，replicated)

APlayerState
└── APickupsPlayerState        (个人拾取计数，replicated)

APlayerController
└── APickupsPlayerController   (输入 + 计分板，abstract)
```

### 4.2 网络复制设计

| 类 | 复制属性 | 方式 |
|----|----------|------|
| `APickup` | （整 actor） | `bReplicates = true` |
| `APickupsGameState` | `PickupsRemaining` | `DOREPLIFETIME` |
| `APickupsPlayerState` | `Pickups` | `DOREPLIFETIME` |

### 4.3 拾取与计分流程

```
玩家重叠 APickup
  → OnBeginOverlap (仅 HasAuthority)
    → GameState->RemovePickups()      // 剩余 -1
    → Character->ClientPlaySound2D()  // 客户端播放音效
    → Character->AddScore(10)         // PlayerState->SetScore
    → Character->AddPickups()         // PlayerState->Pickups++
    → Destroy()
```

## 5. 核心类详解

### 5.1 APickup（`Pickup.h/.cpp`）

继承 `AActor`。

**组件：**
- `UStaticMeshComponent* Mesh`：碰撞配置 `OverlapAll`，作为根组件。
- `URotatingMovementComponent* RotatingMovement`：`RotationRate = FRotator(0, 90, 0)`。

**属性：**
- `USoundBase* PickupSound`（`EditAnywhere`）。

**关键点：**
- 构造函数中 `bReplicates = true`、`PrimaryActorTick.bCanEverTick = false`。
- `OnBeginOverlap`：
  - `Cast<APickupsCharacter>`，若为空或 `!HasAuthority()` 则返回。
  - 获取 `APickupsGameState`，调用 `RemovePickups()`、`ClientPlaySound2D`、`AddScore(10)`、`AddPickups()`，最后 `Destroy()`。

### 5.2 APickupsCharacter（`PickupsCharacter.h/.cpp`）

继承 `ACharacter`，`UCLASS(abstract)`，实际使用 `BP_ThirdPersonCharacter` 派生蓝图。

**输入动作：** `JumpAction`、`MoveAction`、`LookAction`、`MouseLookAction`。

**属性：** `FallSound`、`LandSound`。

**关键方法：**
- `Landed()`：落地时播放落地音效。
- `FellOutOfWorld()`：扣 10 分、销毁自身、调用 `RestartPlayer` 重生。
- `AddScore(float)`：`GetPlayerState()->SetScore(当前分 + Score)`。
- `AddPickups()`：`GetPlayerState<APickupsPlayerState>()->AddPickup()`。
- `ClientPlaySound2D(USoundBase*)`：`UFUNCTION(Client, Unreliable)`，在客户端播放 2D 音效。
- `EndPlay`：当结束原因是被销毁时播放坠落音效。

**移动参数：** 胶囊体 42×96；相机弹簧臂臂长 400、`bUsePawnControlRotation=true`；`MaxWalkSpeed=500`、`JumpZVelocity=500`、`AirControl=0.35`。

### 5.3 APickupsGameMode（`PickupsGameMode.h/.cpp`）

继承 `AGameMode`（支持完整比赛状态机），`UCLASS(abstract)`。

- `APickupsGameState* MyGameState`：缓存游戏状态。
- `BeginPlay`：获取 `APickupsGameState`。
- `ShouldSpawnAtStartSpot()`：返回 `false`（不使用默认出生点逻辑，由蓝图控制）。
- `HandleMatchHasStarted()`：屏幕打印绿色提示。
- `HandleMatchHasEnded()`：屏幕打印红色提示；销毁所有 `APickupsCharacter`；5 秒后 `RestartMap`。
- `ReadyToStartMatch_Implementation()`：返回 `true`。
- `ReadyToEndMatch_Implementation()`：返回 `MyGameState && !MyGameState->HasPickups()`。
- `RestartMap()`：`GetWorld()->ServerTravel(GetWorld()->GetName())` 重载地图。

### 5.4 APickupsGameState（`PickupsGameState.h/.cpp`）

继承 `AGameState`。

- `int32 PickupsRemaining`（`Replicated, EditAnywhere, BlueprintReadWrite`）。
- `BeginPlay()`：`GetAllActorsOfClass(APickup::StaticClass())` 统计收集物总数。
- `RemovePickups()`：`PickupsRemaining--`。
- `HasPickups()`：`PickupsRemaining > 0`。
- `GetPlayerStatesOrderedByScore()`：复制 `PlayerArray` 并按 `GetScore()` 降序排序（`BlueprintCallable`）。
- 复制：`DOREPLIFETIME(APickupsGameState, PickupsRemaining)`。

### 5.5 APickupsPlayerState（`PickupsPlayerState.h/.cpp`）

继承 `APlayerState`。

- `int32 Pickups`（`Replicated, BlueprintReadOnly`）。
- `AddPickup()`：`Pickups++`。
- 复制：`DOREPLIFETIME(APickupsPlayerState, Pickups)`。

### 5.6 APickupsPlayerController（`PickupsPlayerController.h/.cpp`）

继承 `APlayerController`，`UCLASS(abstract)`。

- 输入映射上下文：`DefaultMappingContexts`、`MobileExcludedMappingContexts`。
- `ScoreboardMenuClass`（`EditDefaultsOnly`，`TSubclassOf<UUserWidget>`）。
- `BeginPlay`：本地控制器创建计分板（`WBP_Scoreboard`）并 `AddToViewport(0)`。
- `SetupInputComponent`：注册 Enhanced Input 映射上下文。

## 6. 输入系统

- Enhanced Input，资源位于 `Content/Input/`（`IA_Move`、`IA_Look`、`IA_MouseLook`、`IA_Jump`；`IMC_Default`、`IMC_MouseLook`；触控控件）。

## 7. 资源清单（Pickups 特有）

| 类别 | 资源 |
|------|------|
| 蓝图 | `BP_GameMode`、`BP_Pickup`、`BP_PlayerController` |
| UI | `WBP_Scoreboard`、`WBP_Score_Entry`、`WBP_Scoreboard_Header` |
| 音频 | `Fall`、`Footstep`、`Jump`、`Land`、`Pickup` |
| 地图 | `ThirdPerson/Lvl_ThirdPerson`、`Variant_Combat/Lvl_Combat`、`Variant_Platforming/Lvl_Platforming`、`Variant_SideScrolling/Lvl_SideScrolling` |

## 8. 共享变体框架（Variant System）

Pickups 项目包含与 Dodgeball 相同架构的三套变体（模块名/API 宏为 `PICKUPS_API`）：

- **Variant_Combat**：`ACombatCharacter`、`ACombatEnemy`、`ACombatAIController`、`ACombatEnemySpawner`、战斗接口、StateTree 节点、战斗关卡物件、`UCombatLifeBar`。
- **Variant_Platforming**：`APlatformingCharacter`（二段跳/蹬墙跳/冲刺/土狼时间）、`UAnimNotify_EndDash`。
- **Variant_SideScrolling**：`ASideScrollingCharacter`、`ASideScrollingCameraManager`、`ASideScrollingNPC`、StateTree、关卡物件、`USideScrollingUI`。

> 三个项目中的 Variant 代码结构一致，仅在模块导出宏与头文件包含路径上存在差异。

## 9. 构建与运行

- 生成项目：右键 `Pickups.uproject` → Generate Visual Studio project files。
- 编译目标：`PickupsEditor.Target.cs`（编辑器）、`Pickups.Target.cs`（游戏）。
- 该游戏为多人游戏，可通过"播放器数量 > 1"（PIE 多客户端）或专用服务器测试复制逻辑。
