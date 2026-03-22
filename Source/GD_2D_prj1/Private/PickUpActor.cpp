// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUpActor.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"


APickUpActor::APickUpActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Assign our collider to the root component
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	RootComponent = Collider;

	// Set collider to behave like a trigger
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetGenerateOverlapEvents(true);
	Collider->SetCollisionResponseToAllChannels(ECR_Overlap);
	Collider->SetCollisionProfileName(TEXT("Trigger"));

	// Bind overlap event
	Collider->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::OnBeginOverlap);

	// Create sprite component
	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
	if (PickUpAsset && PickUpAsset->Icon)
	{
		Sprite->SetSprite(PickUpAsset->Icon);
	}
	
}


// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void APickUpActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		Destroy();
	}
}


