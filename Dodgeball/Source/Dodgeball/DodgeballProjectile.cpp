// Fill out your copyright notice in the Description page of Project Settings.

#include "DodgeballProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DodgeballCharacter.h"
#include "Engine/CollisionProfile.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADodgeballProjectile::ADodgeballProjectile()
{
	PrimaryActorTick.bCanEverTick = false; // projectile usually doesn't need Tick

	// Create collision component
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->InitSphereRadius(35.0f);
	SphereCollision->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	SphereCollision->SetSimulatePhysics(true);
	SphereCollision->SetNotifyRigidBodyCollision(true);
	SphereCollision->OnComponentHit.AddDynamic(this, &ADodgeballProjectile::OnHit);
	RootComponent = SphereCollision;

	// Create movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
}

// Called when the game starts or when spawned
void ADodgeballProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(5.0f);
}

// Called every frame (disabled by default)
void ADodgeballProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADodgeballProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BounceSound != nullptr && NormalImpulse.Size() > 6000.0f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, BounceSoundAttenuation);	
	}
	
	ADodgeballCharacter* Player = Cast<ADodgeballCharacter>(OtherActor);
	if (Player != nullptr)
	{
		UHealthComponent* HealthComponent = Player->FindComponentByClass<UHealthComponent>();
		if (HealthComponent != nullptr)
		{
			HealthComponent->LoseHealth(Damage);
		}

		if (HitParticle != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),HitParticle, GetActorLocation());
		}

		if (DamageSound != nullptr)
		{
			UGameplayStatics::PlaySound2D(this, DamageSound);
			
		}
		Destroy();
	}
}
