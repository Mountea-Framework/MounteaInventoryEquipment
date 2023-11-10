// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MounteaItemHelpers.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryItemBFL.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Function to determine the max stack size of the item
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static int32 DetermineMaxStackSize(UMounteaInstancedItem* Item)
	{
		if (!Item)
		{
			return 0;
		}

		// Assuming GetMaxStackSize() is a function that exists on UMounteaInstancedItem
		return Item->GetMaxStackSize();
	}

	// Function to calculate the amount that can be added to the stack
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static int32 CalculateAddAmount(UMounteaInstancedItem* Item, int32 QuantityToAdd)
	{
		if (!Item)
		{
			return 0;
		}

		const int32 MaxStackSize = Item->GetMaxStackSize();
		// Logic to determine how much can be added, considering the max stack size
		// This could be as simple as min(QuantityToAdd, MaxStackSize) or more complex if needed
		return FMath::Min(QuantityToAdd, MaxStackSize);
	}

	// Function to add a new stack to the inventory
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static FItemSlotStack AddNewStack(UMounteaInstancedItem* Item, int32 Quantity)
	{
		FItemSlotStack NewStack;
		NewStack.StackSize = Quantity;
		NewStack.StackGuid = FGuid::NewGuid();
		// Link to the parent slot or item if needed
		// NewStack.SlotGuid = ...

		return NewStack;
	}

	// Function to update the total quantity of items
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static void UpdateTotalQuantity(UMounteaInstancedItem* Item, int32 QuantityToAdd)
	{
		if (!Item)
		{
			return;
		}

		// Assuming there's a function or property on UMounteaInstancedItem that tracks quantity
		Item->ModifyQuantity(QuantityToAdd);
	}
};
