// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PaperFlipbookComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.generated.h"


UCLASS()
class AEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();
	void MoveRight(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* Collider;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UPaperFlipbookComponent* Flipbook;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
