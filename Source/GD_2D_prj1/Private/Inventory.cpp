// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
bool UInventory::AddItem(FString ItemName, int Quantity)
{
	if (Quantity <= 0)
	{
		return false;
	}

	if (Inventory.Contains(ItemName))
	{
		Inventory[ItemName] += Quantity;
		return true;
	}
	else
	{
		Inventory.Add(ItemName, Quantity);
		return true;
	}

	return false;
}
bool UInventory::RemoveItem(FString ItemName, int Quantity)
{
	if (Quantity <= 0)
	{
		return false;
	}

	if (Inventory.Contains(ItemName))
	{
		if (Inventory[ItemName] < Quantity)
		{
			return false;
		}

		Inventory[ItemName] -= Quantity;

		if (Inventory[ItemName] <= 0)
		{
			Inventory.Remove(ItemName);
		}

		return true;
	}

	return false;
}
bool UInventory::CheckHasItem(FString ItemName)
{
	return Inventory.Contains(ItemName);
}
bool UInventory::CheckHasItem(FString ItemName, int Quantity)
{
	if (Inventory.Contains(ItemName))
	{
		return Inventory[ItemName] >= Quantity;
	}

	return false;
}
bool UInventory::AddItem(FString ItemName)
{
	return AddItem(ItemName, 1);
}

bool UInventory::RemoveItem(FString ItemName)
{
	return RemoveItem(ItemName, 1);
}
int UInventory::GetQuantityOfItem(FString ItemName)
{
	if (Inventory.Contains(ItemName))
	{
		return Inventory[ItemName];
	}

	return 0;
}

