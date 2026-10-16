// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DodgeballProjectile.generated.h"

UCLASS(Blueprintable)
class DODGEBALL_API ADodgeballProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADodgeballProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Collision sphere for projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodgeball", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;

	// Handles projectile movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodgeball", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	// Called when projectile hits something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const
	{
		return ProjectileMovement;
	}

protected:
	UPROPERTY(EditAnywhere, Category = Damage)
	float Damage = 34.0f;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* BounceSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* DamageSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundAttenuation* BounceSoundAttenuation;

protected:
	UPROPERTY(EditAnywhere, Category = Particles)
	class UParticleSystem* HitParticle;
};
