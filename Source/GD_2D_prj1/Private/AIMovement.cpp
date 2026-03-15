// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMovement.h"

// Sets default values for this component's properties
UAIMovement::UAIMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CurrentPatrolNodeIndex = 0;

	// ...
}


// Called when the game starts
void UAIMovement::BeginPlay()
{
	Super::BeginPlay();
	OwnerPawn = Cast<AEnemy>(GetOwner());

	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("AIMovementComponent can only be attached to a Pawn."));
		DestroyComponent();
	}

	// ...
	
}


// Called every frame
void UAIMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MoveToNextPatrolNode();

	// ...
}


void UAIMovement::MoveToNextPatrolNode()
{
	if (PatrolNodes.Num() == 0)
	{
		return;
	}
	else if (PatrolNodes.Num() == 1)
	{
		OwnerPawn->SetActorLocation(PatrolNodes[0]->GetActorLocation());
		return;
	}

	AActor* TargetNode = PatrolNodes[CurrentPatrolNodeIndex];

	float Distance = FMath::Abs(
		OwnerPawn->GetActorLocation().X - TargetNode->GetActorLocation().X
	);

	// Move left or right
	if (OwnerPawn->GetActorLocation().X > TargetNode->GetActorLocation().X)
	{
		OwnerPawn->MoveRight(-1.0f);

		OwnerPawn->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
	}
	else
	{
		OwnerPawn->MoveRight(1.0f);

		OwnerPawn->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// If we reached the node
	if (Distance < 50.0f)
	{
		CurrentPatrolNodeIndex =
			(CurrentPatrolNodeIndex + 1) % PatrolNodes.Num();
	}
}

