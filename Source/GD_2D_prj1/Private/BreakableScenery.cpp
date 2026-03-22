// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableScenery.h"
#include "Components/BoxComponent.h"

// Sets default values
ABreakableScenery::ABreakableScenery()
{
	// Disable Tick since scenery does not update
	PrimaryActorTick.bCanEverTick = false;

	// Create sprite component
	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	RootComponent = Sprite;

	// Create collider
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent);

	// Basic collision setup
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}



