# Dodgeball 技术文档

## 1. 项目概况

| 项目 | 说明 |
|------|------|
| 引擎版本 | Unreal Engine 5.6 |
| 模板 | Third Person Game Template（第三人称游戏模板） |
| 模块 | `Dodgeball`（Runtime） |
| 依赖模块 | Engine、AIModule、UMG |
| 启用插件 | ModelingToolsEditorMode、StateTree、GameplayStateTree |
| 输入系统 | Enhanced Input |
| 主地图 | `/Game/ThirdPerson/Lvl_ThirdPerson` |
| 默认 GameMode | `BP_ThirdPersonGameMode` |

## 2. 项目目录结构

```
Dodgeball/
├── Dodgeball.uproject
├── Config/            # 引擎、输入、渲染配置
├── Source/Dodgeball/  # C++ 源码
│   ├── Dodgeball*.h/cpp             # 模块入口与核心类
│   ├── Variant_Combat/              # 战斗变体
│   ├── Variant_Platforming/         # 平台跳跃变体
│   └── Variant_SideScrolling/       # 横版卷轴变体
├── Content/           # 资源
│   ├── Input/         # 输入动作与映射
│   ├── ThirdPerson/   # 第三人称蓝图/音频
│   ├── Physics/       # 物理材质
│   ├── Variant_Combat/ | Variant_Platforming/ | Variant_SideScrolling/
│   └── Characters/    # Mannequin 角色与动画
└── ...
```

## 3. 模块依赖（Build.cs）

`Dodgeball.Build.cs` 声明以下模块依赖：

- **PublicDependencyModuleNames**：`Core`、`CoreUObject`、`Engine`、`InputCore`、`EnhancedInput`、`AIModule`、`StateTreeModule`、`GameplayStateTreeModule`、`UMG`、`Slate`、`SlateCore`
- **PublicIncludePaths**：包含项目根目录及三个 Variant 子目录（用于跨目录引用变体类）。

## 4. 架构总览

### 4.1 类继承关系

```
ACharacter
├── ADodgeballCharacter        (玩家角色，实现 IHealthInterface)
└── AEnemyCharacter            (敌人角色)

AActor
├── ADodgeballProjectile       (躲避球投射物)
├── AVictoryBox                (胜利触发器)
├── AWall                      (普通墙体)
└── AMusicManager              (背景音乐)

UActorComponent
└── UHealthComponent           (生命组件)

USceneComponent
└── ULookAtActorComponent      (视线检测组件)

UBlueprintFunctionLibrary
└── UDodgeballFunctionLibrary  (视线检测静态函数)

APlayerController
└── ADodgeballPlayerController (输入映射 + HUD/重开界面管理)

UUserWidget
├── UHUDWidget                 (血条)
└── URestartWidget             (重开/退出界面)

UInterface
└── IHealthInterface           (死亡/受伤事件接口)
```

### 4.2 核心交互流

```
敌人 Tick → LookAtActorComponent 视线检测 → 能看见玩家?
    ├─ 是 → 启动计时器 → ThrowDodgeball() → 生成 ADodgeballProjectile
    └─ 否 → 停止计时器

ADodgeballProjectile OnHit → 命中玩家?
    ├─ 是 → UHealthComponent::LoseHealth(34) → IHealthInterface::OnTakeDamage → 更新血条
    │        → 若生命 ≤ 0 → IHealthInterface::OnDeath → 弹出重开界面
    └─ 否 → 反弹音效
```

## 5. 核心类详解

### 5.1 ADodgeballCharacter（`DodgeballCharacter.h/.cpp`）

继承 `ACharacter` 并实现 `IHealthInterface`。标记为 `UCLASS(abstract)`，实际使用蓝图派生类 `BP_ThirdPersonCharacter`。

**组件：**
- `USpringArmComponent* CameraBoom`：相机弹簧臂，臂长 900cm，相对俯仰 -70°。
- `UCameraComponent* FollowCamera`：跟随相机。
- `UHealthComponent* HealthComponent`：生命组件（默认 100 HP）。

**输入动作（Enhanced Input）：**
- `JumpAction`、`MoveAction`、`LookAction`、`MouseLookAction`。

**关键方法：**
- `DoMove(float Right, float Forward)`：基于控制器朝向计算前向/右向并施加移动。
- `DoLook(float Yaw, float Pitch)`：追加控制器偏航/俯仰。
- `DoJumpStart()/DoJumpEnd()`：跳跃开始/结束。
- `OnDeath_Implementation()`：死亡时通知控制器弹出重开界面。
- `OnTakeDamage_Implementation()`：受伤时通知控制器刷新血条。

**移动参数：** 胶囊体 42×96；`MaxWalkSpeed=500`、`JumpZVelocity=500`、`AirControl=0.35`。

### 5.2 UHealthComponent（`HealthComponent.h/.cpp`）

- `float Health = 100.0f`（`EditDefaultsOnly`）。
- `LoseHealth(float Amount)`：扣血 → 触发 `OnTakeDamage` → 若 `Health <= 0` 则触发 `OnDeath`。
- `GetHealthPercent()`：返回 `Health / 100.0f`。

> 注意：扣血逻辑通过 `GetOwner()->Implements<UHealthInterface>()` 判断并调用接口事件。

### 5.3 IHealthInterface（`HealthInterface.h`）

- `OnDeath()`：`BlueprintNativeEvent`，纯虚 `OnDeath_Implementation()`。
- `OnTakeDamage()`：`BlueprintNativeEvent`，纯虚 `OnTakeDamage_Implementation()`。

### 5.4 AEnemyCharacter（`EnemyCharacter.h/.cpp`）

- 组件：`ULookAtActorComponent* LookAtActorComponent`。
- 状态：`bCanSeePlayer`、`bPreviousCanSeePlayer`（用于检测视线状态变化边沿）。
- 计时：`ThrowingInterval=2.0f`、`ThrowingDelay=0.5f`。
- `ThrowDodgeball()`：在自身前方 40cm 处 `SpawnActorDeferred` 生成躲避球，设置 `InitialSpeed/MaxSpeed=2200`，随后 `FinishSpawningActor`。
- `TSubclassOf<ADodgeballProjectile> DodgeballClass`：躲避球蓝图类（`BP_DodgeballProjectile`）。

### 5.5 ADodgeballProjectile（`DodgeballProjectile.h/.cpp`）

**组件：**
- `USphereComponent* SphereCollision`：半径 35，碰撞配置 `BlockAllDynamic`，开启物理模拟与刚体碰撞通知。
- `UProjectileMovementComponent* ProjectileMovement`：初速/最大速度 1500，`bShouldBounce=true`、`Bounciness=0.3`、`bRotationFollowsVelocity=true`。

**属性：**
- `Damage = 34.0f`（`EditAnywhere`）。
- `BounceSound`、`DamageSound`、`BounceSoundAttenuation`、`HitParticle`。

**逻辑：**
- `BeginPlay`：`SetLifeSpan(5.0f)`。
- `OnHit`：若冲击力 `NormalImpulse.Size() > 6000` 播放反弹音；若命中玩家，`LoseHealth(Damage)`、生成命中粒子、播放命中音、销毁自身。

### 5.6 ULookAtActorComponent（`LookAtActorComponent.h/.cpp`）

- `TickComponent` 每帧调用 `LookAtActor()`。
- `LookAtActor()`：构造忽略列表（自身 + 目标），调用 `CanSeeActor` 做射线检测；可见时用 `FindLookAtRotation` 让拥有者朝向玩家并返回 `true`。
- `CanSeeTarget()`：供外部查询可见状态。

### 5.7 UDodgeballFunctionLibrary（`DodgeballFunctionLibrary.h/.cpp`）

- 静态函数 `CanSeeActor(const UWorld*, FVector Location, const AActor* TargetActor, TArray<const AActor*> IgnoreActors)`。
- 使用碰撞通道 `ECC_GameTraceChannel2`（自定义视线通道）做 `LineTraceSingleByChannel`，返回 `!Hit.bBlockingHit`。
- 绘制红色调试射线（`DrawDebugLine`）。

### 5.8 ADodgeballPlayerController（`DodgeballPlayerController.h/.cpp`）

- 输入映射上下文：`DefaultMappingContexts`、`MobileExcludedMappingContexts`。
- 移动端触控控件：`MobileControlsWidgetClass`。
- `BP_RestartWidget`、`BP_HUDWidget`：蓝图类引用。
- `BeginPlay`：本地控制器生成 HUD（`AddToViewport`）；触控环境生成移动控件。
- `ShowRestartWidget()`：`SetPause(true)` + `UIOnly` 输入模式 + 显示鼠标，创建并显示重开界面。
- `HideRestartWidget()`：移除界面、取消暂停、恢复游戏输入模式。
- `UpdateHealthPercent(float)`：刷新 HUD 血条。

### 5.9 UHUDWidget / URestartWidget（`HUDWidget.h`、`RestartWidget.h`）

- `UHUDWidget`：`UProgressBar* HealthBar`（`BindWidget`），`UpdateHealthPercent`。
- `URestartWidget`：`RestartButton`、`ExitButton`（`BindWidget, OptionalWidget`）；`OnRestartClicked`/`OnExitClicked` 在蓝图中实现；`NativeOnInitialized` 绑定按钮事件。

### 5.10 其他 Actor

- `AVictoryBox`：`UBoxComponent`（60×60×60，偏移 Z=120），重叠玩家时 `QuitGame`。
- `AWall`：仅含 `USceneComponent` 根组件，作为关卡几何。
- `AMusicManager`：含 `UAudioComponent`，用于背景音乐。

## 6. 输入系统

- 使用 Enhanced Input（`EnhancedPlayerInput` / `EnhancedInputComponent`）。
- 输入资源位于 `Content/Input/`：
  - `IA_Move`、`IA_Look`、`IA_MouseLook`、`IA_Jump`
  - `IMC_Default`、`IMC_MouseLook`
  - 触控：`BPI_TouchInterface`、`UI_Thumbstick`、`UI_TouchSimple`
- `ADodgeballPlayerController::SetupInputComponent()` 将映射上下文注册到 `UEnhancedInputLocalPlayerSubsystem`。

## 7. 碰撞与视线

- 躲避球使用 `BlockAllDynamic` 碰撞配置并模拟物理。
- 视线检测使用**自定义碰撞通道 `ECC_GameTraceChannel2`**（在项目碰撞设置中定义），实现敌人→玩家的可见性判定。

## 8. 资源清单（Dodgeball 特有）

| 类别 | 资源 |
|------|------|
| 蓝图 | `BP_DodgeballProjectile`、`BP_EnemyCharacter`、`BP_GhostWall`、`BP_MusicManager`、`BP_VictoryBox`、`BP_Wall`、`BP_ThirdPersonCharacter`、`BP_ThirdPersonGameMode`、`BP_ThirdPersonPlayerController` |
| UI | `BP_HUDWidget`、`BP_RestartWidget` |
| 音频 | `BOUNCE`、`Dodgeball`、`PUNCH`、`VGMA` |
| 物理 | `PM_Dodgeball` |
| 地图 | `NewMap`、`ThirdPerson/Lvl_ThirdPerson`、`Variant_Combat/Lvl_Combat`、`Variant_Platforming/Lvl_Platforming`、`Variant_SideScrolling/Lvl_SideScrolling` |

## 9. 共享变体框架（Variant System）

Dodgeball 项目同样包含三套变体（与另两个项目架构一致，模块名/API 宏不同）：

### 9.1 Variant_Combat（战斗）

- **角色**：`ACombatCharacter`（连击 + 蓄力攻击、伤害/死亡/重生、生命条）。
- **敌人**：`ACombatEnemy`（StateTree AI，连击/蓄力/死亡布娃娃）。
- **AI**：`ACombatAIController`（`UStateTreeAIComponent`）、`ACombatEnemySpawner`（逐个生成敌人，耗尽后激活其它可激活物）。
- **接口**：`ICombatAttacker`、`ICombatDamageable`、`ICombatActivatable`。
- **关卡物件**：`CombatActivationVolume`、`CombatCheckpointVolume`、`CombatDamageableBox`、`CombatDummy`、`CombatLavaFloor`。
- **StateTree 节点**：Combo Attack、Charged Attack、Wait for Landing、Face Towards Actor/Location、Set Character Speed、GetPlayerInfo、条件"Character is Grounded"。
- **AnimNotify**：`CheckCombo`、`CheckChargedAttack`、`DoAttackTrace`。
- **UI**：`UCombatLifeBar`。

### 9.2 Variant_Platforming（平台跳跃）

- `APlatformingCharacter`：长按跳跃、二段跳、蹬墙跳、冲刺、土狼时间（Coyote Time，0.16s）。
- `UAnimNotify_EndDash`：冲刺动画结束恢复控制。
- `APlatformingGameMode` / `APlatformingPlayerController`：玩家死亡后在 Player Start 重生。

### 9.3 Variant_SideScrolling（横版卷轴）

- `ASideScrollingCharacter`：横版移动、二段跳、蹬墙跳、软平台下坠、交互、土狼时间。
- `ASideScrollingCameraManager`：平滑横版相机 + 水平边界。
- `ASideScrollingNPC` / `ASideScrollingAIController`：StateTree 驱动的 NPC，可被交互暂时禁用。
- 关卡物件：`JumpPad`、`MovingPlatform`、`Pickup`、`SoftPlatform`。
- 接口：`ISideScrollingInteractable`。
- UI：`USideScrollingUI`（拾取计数）。

## 10. 构建与运行

- 生成项目：右键 `Dodgeball.uproject` → Generate Visual Studio project files。
- 编译目标：`DodgeballEditor.Target.cs`（编辑器）、`Dodgeball.Target.cs`（游戏）。
- 主地图为第三人称模板地图，可在编辑器直接运行。
