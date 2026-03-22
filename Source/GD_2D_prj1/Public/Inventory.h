// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GD_2D_PRJ1_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();
	bool AddItem(FString ItemName, int Quantity);
	bool RemoveItem(FString ItemName, int Quantity);

	bool CheckHasItem(FString ItemName);
	bool CheckHasItem(FString ItemName, int Quantity);

	int GetQuantityOfItem(FString ItemName);

	// Overloads
	bool AddItem(FString ItemName);
	bool RemoveItem(FString ItemName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	// Our inventory will store item name and quantity
	TMap<FString, int> Inventory;
		
};
