// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DodgeballProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LookAtActorComponent.h"
#include "Slate/SGameLayerManager.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LookAtActorComponent = CreateDefaultSubobject<ULookAtActorComponent>(TEXT("Look At Actor Component"));
	LookAtActorComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	LookAtActorComponent->SetTarget(PlayerCharacter);
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	bCanSeePlayer =	LookAtActorComponent->CanSeeTarget();
	
	if (bCanSeePlayer != bPreviousCanSeePlayer)
	{
		if (bCanSeePlayer)
		{
			GetWorldTimerManager().SetTimer(ThrowTimerHandle, this, &AEnemyCharacter::ThrowDodgeball, ThrowingInterval, true, ThrowingDelay);
			
		}
		else
		{
			GetWorldTimerManager().ClearTimer(ThrowTimerHandle);
		}
	}
	bPreviousCanSeePlayer = bCanSeePlayer;
	
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::ThrowDodgeball()
{
	if (!DodgeballClass) return;

	FVector ForwardVector = GetActorForwardVector();
	float SpawnDistance = 40.0f;
	FVector SpawnLocation = GetActorLocation() + (ForwardVector * SpawnDistance);
	FRotator SpawnRotation = GetActorRotation();
	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	ADodgeballProjectile* Projectile = GetWorld()->SpawnActorDeferred<ADodgeballProjectile>(
		DodgeballClass,
		SpawnTransform,
		this,
		this,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (Projectile)
	{
		UProjectileMovementComponent* MoveComp = Projectile->GetProjectileMovement();
		if (MoveComp)
		{
			MoveComp->InitialSpeed = 2200.0f;
			MoveComp->MaxSpeed = 2200.0f;
		}

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
	}
} 