
// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperSideScroller_Player.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "PlayerProjectile.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"

ASuperSideScroller_Player::ASuperSideScroller_Player()
{
	bIsSprinting = false;
	GetCharacterMovement() -> MaxWalkSpeed = 300.0f;
}

void ASuperSideScroller_Player::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedPlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (EnhancedPlayerInput)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	
		if (EnhancedSubsystem)
		{
			EnhancedSubsystem -> AddMappingContext(IC_Character, 1);
		}
		
		EnhancedPlayerInput->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &ASuperSideScroller_Player::Sprint);
		EnhancedPlayerInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASuperSideScroller_Player::StopSprinting);
		EnhancedPlayerInput->BindAction(IA_Throw, ETriggerEvent::Triggered, this, &ASuperSideScroller_Player::ThrowProjectile);
	}
}

void ASuperSideScroller_Player::Sprint()
{
	if (!bIsSprinting)
	{
		bIsSprinting = true;
		if (bHasPowerupActive)
		{
			GetCharacterMovement() -> MaxWalkSpeed = 900.0f;
		}
		else
		{
			GetCharacterMovement() -> MaxWalkSpeed = 500.0f;
		}
	}
}

void ASuperSideScroller_Player::StopSprinting()
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
		if (bHasPowerupActive)
		{
			GetCharacterMovement() -> MaxWalkSpeed = 500.0f;
		}
		else
		{
			GetCharacterMovement() -> MaxWalkSpeed = 300.0f;
		}
	}
}

void ASuperSideScroller_Player::ThrowProjectile()
{
	if (ThrowMontage)
	{
		const bool bISmontagePlaying = GetMesh()->GetAnimInstance()->Montage_IsPlaying(ThrowMontage);
		
		if (!bISmontagePlaying)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(ThrowMontage, 1.0f);
		}
	}
}

void ASuperSideScroller_Player::SpawnProjectile()
{
	if (PlayerProjectile)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			const FVector SpawnLocation = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket"));
			const FRotator Rotation = GetActorForwardVector().Rotation();
			APlayerProjectile* Projectile = World->SpawnActor<APlayerProjectile>(PlayerProjectile, SpawnLocation, Rotation, SpawnParams);
		}
	}
}

void ASuperSideScroller_Player::IncrementNumberOfCollectables(int32 Value)
{
	if (Value <= 0)
	{
		return;
	}
	else
	{
		NumberOfCollectables += Value;
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of Coins: %d"), NumberOfCollectables)
}

void ASuperSideScroller_Player::IncreaseMovementPowerup()
{
	bHasPowerupActive = true;
	GetCharacterMovement() -> MaxWalkSpeed = 500.0f;
	GetCharacterMovement() -> JumpZVelocity = 1500.0f;
	
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(PowerupHandle, this, &ASuperSideScroller_Player::EndPowerup, 8.0f, false);
	}
}

void ASuperSideScroller_Player::EndPowerup()
{
	bHasPowerupActive = false;
	GetCharacterMovement() -> MaxWalkSpeed = 300.0f;
	GetCharacterMovement() -> JumpZVelocity = 1000.0f;
	
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(PowerupHandle);
	}
}