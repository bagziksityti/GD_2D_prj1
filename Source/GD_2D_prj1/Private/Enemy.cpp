// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create collider
    Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
    RootComponent = Collider;

    // Create flipbook animation
    Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
    Flipbook->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void AEnemy::MoveRight(float Value)
{
	float Speed = 200.0f;

	SetActorLocation(
		GetActorLocation() +
		FVector(Speed * Value * GetWorld()->GetDeltaSeconds(), 0, 0)
	);
}

