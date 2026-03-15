// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PaperSprite.h"
#include "PickUpAsset.generated.h"

/**
 * 
 */
UCLASS()
class GD_2D_PRJ1_API UPickUpAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPickUpAsset();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	UPaperSprite* Icon;
	
};
