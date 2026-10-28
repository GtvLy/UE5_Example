# SuperSideScroller 技术文档

## 1. 项目概况

| 项目 | 说明 |
|------|------|
| 引擎版本 | Unreal Engine 5.6 |
| 模板 | Third Person Game Template |
| 模块 | `SuperSideScroller`（Runtime） |
| 依赖模块 | Engine、AIModule、UMG |
| 启用插件 | ModelingToolsEditorMode、StateTree、GameplayStateTree |
| 输入系统 | Enhanced Input |
| 主地图 | `/Game/SuperSideScroller` |
| 默认 GameMode | `BP_SuperSideScroller_GameMode`（`/Game/GameMode/`） |

## 2. 项目目录结构

```
SuperSideScroller/
├── SuperSideScroller.uproject
├── Config/
├── Source/SuperSideScroller/
│   ├── SuperSideScrollerCharacter.h/.cpp   # 基础角色
│   ├── SuperSideScroller_Player.h/.cpp     # 玩家（冲刺/投掷/收集/道具）
│   ├── SuperSideScroller_Controller.h/.cpp # 玩家控制器
│   ├── EnemyBase.h/.cpp                    # 敌人基类
│   ├── PlayerProjectile.h/.cpp             # 玩家投射物
│   ├── Anim_ProjectileNotify.h/.cpp        # 投掷动画通知
│   ├── PickableActor_Base/Collectable/Powerup  # 拾取物继承体系
│   ├── SuperSideScroller_Brick.h/.cpp      # 可破坏砖块
│   └── Variant_Combat/ | Variant_Platforming/ | Variant_SideScrolling/
├── Content/
│   ├── MainCharacter/   # 玩家模型/动画/蓝图/输入
│   ├── Enemy/           # 敌人 AI（行为树）/蓝图
│   ├── PickableItems/   # 收集物/道具
│   ├── Brick/           # 砖块
│   ├── GameMode/        # BP_SuperSideScroller_GameMode
│   ├── UI/              # 金币计数 UI
│   └── ...
└── ...
```

## 3. 模块依赖（Build.cs）

`SuperSideScroller.Build.cs` 依赖：`Core`、`CoreUObject`、`Engine`、`InputCore`、`EnhancedInput`、`AIModule`、`StateTreeModule`、`GameplayStateTreeModule`、`UMG`、`Slate`。

`PublicIncludePaths` 包含项目根目录及三个 Variant 子目录。

## 4. 架构总览

### 4.1 类继承关系

```
ACharacter
├── ASuperSideScrollerCharacter          (基础第三人称角色，abstract)
│   ├── ASuperSideScroller_Player        (玩家：冲刺/投掷/收集/道具)
│   └── AEnemyBase                       (敌人基类)

AActor
├── APlayerProjectile                    (玩家投射物)
├── APickableActor_Base                  (拾取物基类)
│   ├── APickableActor_Collectable       (金币收集物)
│   └── APickableActor_Powerup           (加速道具)
└── ASuperSideScroller_Brick             (可破坏砖块)

UAnimNotify
└── UAnim_ProjectileNotify               (投掷通知 → 生成投射物)

APlayerController
├── ASuperSideScrollerPlayerController   (基础第三人称控制器)
└── ASuperSideScroller_Controller        (专用控制器)
```

### 4.2 拾取物继承体系（多态）

```
APickableActor_Base
├── 组件：SphereCollision(30) + MeshComp + RotationComp
├── BeginOverlap → PlayerPickedUp(Player)  [虚函数]
└── PlayerPickedUp → Destroy()  [基类默认]

APickableActor_Collectable : APickableActor_Base
└── PlayerPickedUp → Player->IncrementNumberOfCollectables(CollectableValue) + Super

APickableActor_Powerup : APickableActor_Base
└── PlayerPickedUp → Super + Player->IncreaseMovementPowerup()
```

## 5. 核心类详解

### 5.1 ASuperSideScrollerCharacter（`SuperSideScrollerCharacter.h/.cpp`）

继承 `ACharacter`，`UCLASS(abstract)`。标准第三人称模板角色：相机弹簧臂臂长 400、`bUsePawnControlRotation=true`；`MaxWalkSpeed=500`、`JumpZVelocity=500`。提供 `DoMove/DoLook/DoJumpStart/DoJumpEnd` 蓝图可调用接口。

### 5.2 ASuperSideScroller_Player（`SuperSideScroller_Player.h/.cpp`）

继承 `ASuperSideScrollerCharacter`。

**属性：**
- `bool bIsSprinting`、`bool bHasPowerupActive`。
- `int32 NumberOfCollectables`（收集计数）。
- 输入：`IC_Character`（`UInputMappingContext`）、`IA_Sprint`、`IA_Throw`。
- 动画：`UAnimMontage* ThrowMontage`。
- 投射物类：`TSubclassOf<APlayerProjectile> PlayerProjectile`。

**逻辑：**
- 构造函数：`MaxWalkSpeed = 300`。
- `Sprint()`/`StopSprinting()`：
  - 冲刺：无道具 500，有道具 900。
  - 停止：无道具 300，有道具 500。
- `ThrowProjectile()`：若投掷蒙太奇未在播放则 `Montage_Play(ThrowMontage)`。
- `SpawnProjectile()`：在骨骼插槽 `ProjectileSocket` 处生成投射物，朝向角色前向。
- `IncrementNumberOfCollectables(int32 Value)`：`Value > 0` 时累加。
- `IncreaseMovementPowerup()`：`MaxWalkSpeed=500`、`JumpZVelocity=1500`，启动 8 秒计时器。
- `EndPowerup()`：`MaxWalkSpeed=300`、`JumpZVelocity=1000`，清除计时器。

> 注意：基类 `ASuperSideScrollerCharacter` 默认 `JumpZVelocity=500`，但玩家实际通过 `IncreaseMovementPowerup`/`EndPowerup` 把跳跃初速设为 1000/1500，说明玩家蓝图中对跳跃初速做了覆盖配置。

### 5.3 AEnemyBase（`EnemyBase.h/.cpp`）

继承 `ASuperSideScrollerCharacter`，仅提供 `DestroyEnemy()`（`Destroy()`）。具体敌人逻辑在蓝图 `BP_Enemy01` 与行为树中实现。

### 5.4 APlayerProjectile（`PlayerProjectile.h/.cpp`）

继承 `AActor`。

**组件：**
- `USphereComponent* CollisionComp`：半径 15，碰撞配置 `BlockAll`，作为根组件。
- `UProjectileMovementComponent* ProjectileMovement`：`ProjectileGravityScale=0`、`InitialSpeed/MaxSpeed=800`。
- `UStaticMeshComponent* MeshComp`。

**逻辑：**
- 构造函数：`InitialLifeSpan = 3.0f`。
- `OnHit`：命中 `AEnemyBase` 时调用 `DestroyEnemy()`，随后 `ExplodeProjectile()`。
- `ExplodeProjectile()`：`Destroy()`。

### 5.5 UAnim_ProjectileNotify（`Anim_ProjectileNotify.h/.cpp`）

继承 `UAnimNotify`。`Notify()` 中获取拥有者 `ASuperSideScroller_Player`，调用 `SpawnProjectile()`。

### 5.6 APickableActor_Base / Collectable / Powerup

见 4.2 继承关系。

- `APickableActor_Base`：`CollisionComp`（`OverlapAllDynamic`）、`MeshComp`、`RotationComp`；`BeginOverlap` 命中玩家时调用虚函数 `PlayerPickedUp`；基类默认 `Destroy()`。
- `APickableActor_Collectable`：`CollectableValue=1`，拾取时 `Player->IncrementNumberOfCollectables(CollectableValue)`。
- `APickableActor_Powerup`：拾取时 `Player->IncreaseMovementPowerup()`。

### 5.7 ASuperSideScroller_Brick（`SuperSideScroller_Brick.h/.cpp`）

继承 `AActor`。

**组件：** `UStaticMeshComponent* BrickMesh`（`BlockAll`）、`UBoxComponent* BrickCollision`（`BlockAll`）。

**属性：** `bool bHasCollectable`、`int32 CollectableValue = 1`。

**逻辑：** `OnHit` 命中玩家且 `bHasCollectable` 时调用 `AddCollectable`（`IncrementNumberOfCollectables`），随后 `Destroy()`。

### 5.8 控制器与游戏模式

- `ASuperSideScrollerPlayerController`：标准第三人称控制器（输入映射）。
- `ASuperSideScroller_Controller`：专用控制器（当前为空，供蓝图扩展）。
- `ASuperSideScrollerGameMode`：继承 `AGameModeBase`，stub。

## 6. 输入系统

- Enhanced Input。
- 基础输入（`Content/Input/`）：`IA_Move`、`IA_Look`、`IA_MouseLook`、`IA_Jump`；`IMC_Default`、`IMC_MouseLook`。
- 角色专属输入（`Content/MainCharacter/Input/`）：
  - `IC_SideScrollerCharacter`（角色输入映射上下文，在 `SetupPlayerInputComponent` 中以优先级 1 注册）
  - `IA_Sprint`（冲刺）
  - `IA_Throw`（投掷）

## 7. 敌人 AI（行为树）

SuperSideScroller 的核心敌人使用**行为树（Behavior Tree）**，与变体 AI 使用的 StateTree 不同：

| 资源 | 作用 |
|------|------|
| `BT_EnemyAI` | 敌人行为树 |
| `BB_EnemyAI` | 敌人黑板（Blackboard） |
| `BP_AIControllerEnemy` | 敌人 AI 控制器 |
| `BTTask_FindLocation` / `BTTask_FindLocation2` | 寻找位置任务 |
| `BTTask_FindPlayer` | 寻找玩家任务 |
| `BP_AIPoints` | AI 巡逻点 |

## 8. 资源清单（SuperSideScroller 特有）

| 类别 | 资源 |
|------|------|
| 玩家 | `BP_SuperSideScroller_MainCharacter`、`BP_SuperSideScroller_PC`、`AnimBP_SuperSideScroller_MainCharacter`、`BS_SuperSideScroller_MainCharacter` |
| 玩家模型 | `MainCharacter`、`MainCharacter_Skeleton`、`MainCharacter_Anim`、`MainCharacter_PhysicsAsset`、材质 |
| 玩家动画 | `Idle`、`Walking`、`Running`、`JumpStart/JumpLoop/JumpEnd`、`Throw`、`AM_Throw`、`SideScroller_IdleRun_1D` |
| 敌人 | `BP_Enemy01`、`AnimBP_Enemy`、`MI_Enemy01`、`Explosion_Cue`、AI 资源 |
| 拾取物 | `BP_PickagleItems`、`BP_Collectable`、`Coin`、`BP_Powerup` |
| 砖块 | `BP_Brick` |
| 投射物 | `BP_PlayerProjectile` |
| UI | `BP_UI_CoinCollection` |
| 地图 | `SuperSideScroller`、`ThirdPerson/Lvl_ThirdPerson`、三个变体地图 |

## 9. 共享变体框架（Variant System）

SuperSideScroller 项目包含与另两个项目相同架构的三套变体（模块名/API 宏为 `SUPERSIDESCROLLER_API`）：

- **Variant_Combat**：近战连击/蓄力战斗、StateTree 敌人、`ICombatAttacker/ICombatDamageable/ICombatActivatable` 接口、战斗关卡物件、`UCombatLifeBar`。
- **Variant_Platforming**：`APlatformingCharacter`（二段跳/蹬墙跳/冲刺/土狼时间）、`UAnimNotify_EndDash`。
- **Variant_SideScrolling**：`ASideScrollingCharacter`、`ASideScrollingCameraManager`、`ASideScrollingNPC`、StateTree、关卡物件、`USideScrollingUI`。

## 10. 构建与运行

- 生成项目：右键 `SuperSideScroller.uproject` → Generate Visual Studio project files。
- 编译目标：`SuperSideScrollerEditor.Target.cs`（编辑器）、`SuperSideScroller.Target.cs`（游戏）。
- 主地图为 `/Game/SuperSideScroller`，直接运行即可体验横版卷轴玩法。
