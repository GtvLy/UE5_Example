// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SuperSideScrollerCharacter.h"
#include "SuperSideScroller_Player.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSIDESCROLLER_API ASuperSideScroller_Player : public ASuperSideScrollerCharacter
{
	GENERATED_BODY()
	
public:
	ASuperSideScroller_Player();
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	bool bIsSprinting;
	
protected:
	void Sprint();
	void StopSprinting();
	void ThrowProjectile();
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* IC_Character;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Sprint;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Throw;
	
private:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ThrowMontage;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APlayerProjectile> PlayerProjectile;
	
public:
	void SpawnProjectile();
	
private:
	int32 NumberOfCollectables;
	
public:
	UFUNCTION(BlueprintPure)
	int32 GetCurrentNumberOfCollectables()
	{
		return NumberOfCollectables;
	}

public:
	void IncrementNumberOfCollectables(int32 Value);
	
	
private:
	FTimerHandle PowerupHandle;
	bool bHasPowerupActive;
	
public:
	void IncreaseMovementPowerup();
	
protected:
	void EndPowerup();
};
