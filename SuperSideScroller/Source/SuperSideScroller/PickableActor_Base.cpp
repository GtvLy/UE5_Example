// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableActor_Base.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "SuperSideScroller_Player.h"

// Sets default values
APickableActor_Base::APickableActor_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	CollisionComp->InitSphereRadius(30.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	RootComponent = CollisionComp;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	
	RotationComp = CreateDefaultSubobject<URotatingMovementComponent>("RotationComp");
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APickableActor_Base::BeginOverlap);
}

// Called when the game starts or when spawned
void APickableActor_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickableActor_Base::PlayerPickedUp(class ASuperSideScroller_Player* Player)
{
	Destroy();
}

void APickableActor_Base::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASuperSideScroller_Player* Player = Cast<ASuperSideScroller_Player>(OtherActor);
	if (Player)
	{
		PlayerPickedUp(Player);
	}
}
