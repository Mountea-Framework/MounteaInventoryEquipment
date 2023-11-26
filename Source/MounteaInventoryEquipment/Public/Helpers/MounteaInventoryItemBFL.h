// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventoryEquipmentConsts.h"
#include "MounteaItemHelpers.h"
#include "Definitions/MounteaInventoryInstancedItem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryItemBFL.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemBFL"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

/*===============================================================================
		INVENTORY ACTIONS
===============================================================================*/

	UFUNCTION(BlueprintCallable, Category="Inventory")
	static FInventoryUpdateResult ProcessItemAction(UMounteaInventoryItemAction* Action, UMounteaInstancedItem* Item, FMounteaDynamicDelegateContext Context)
	{
		FInventoryUpdateResult Result;
		
		// Validate the request
		if (!Action)
		{
			Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
			Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid Action.");
		
			return Result;
		}
	
		// Validate the request
		if (!Item)
		{
			Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
			Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item.");
		
			return Result;
		}

		Action->InitializeAction(Item, Context);

		if (!Item->GetWorld())
		{
			Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
			Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item, there is no world!.");
		
			return Result;
		}
		
		return Action->ProcessAction(Item);
	}

/*===============================================================================
		SLOTS MANAGEMENT
===============================================================================*/
	
	// Function to determine the max stack size of the item
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static int32 DetermineMaxStackSize(UMounteaInstancedItem* Item)
	{
		if (!Item)
		{
			return 0;
		}

		// Assuming GetMaxStackSize() is a function that exists on UMounteaInstancedItem
		return Item->GetItemData().RequiredData.ItemQuantity.MaxStackSize;
	}

	// Function to calculate the amount that can be added to the stack
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static int32 CalculateAddAmount(const UMounteaInstancedItem* Item, const int32 QuantityToAdd)
	{
		if (!Item)
		{
			return 0;
		}

		const int32 MaxStackSize =Item->GetItemData().RequiredData.ItemQuantity.MaxStackSize;
		// Logic to determine how much can be added, considering the max stack size
		// This could be as simple as min(QuantityToAdd, MaxStackSize) or more complex if needed
		return FMath::Min(QuantityToAdd, MaxStackSize);
	}

	static int32 FindEmptySlot(const TArray<FItemSlot>& Slots, UMounteaInstancedItem* ItemInstance)
	{
		if (const FItemSlot* EmptySlot = Slots.FindByPredicate(
			[](const FItemSlot& Slot)
			{
				return Slot.IsEmpty();
			}))
		{
			return Slots.Find(*EmptySlot); // Return index of to the found empty slot.
		}

		// No empty slot found, return invalid index	
		return INDEX_NONE;
	}
	
	static FItemSlot MakeNewSlot(UMounteaInstancedItem* NewItemInstance, const int32& Quantity)
	{
		// Create a new slot for the item instance
		FItemSlot NewSlot = FItemSlot(NewItemInstance);

		// Add to existing stacks or create new ones as necessary, and update the remaining quantity
		UpdateStacksInSlot(NewSlot, Quantity);

		// Return the new slot with updated stacks
		return NewSlot;
	}
	
	static void UpdateStacksInSlot(FItemSlot& Slot, const int32 Quantity)
	{
		// Assuming DetermineMaxStackSize() is a function that determines the maximum stack size for the item
		const int32 MaxStackSize = DetermineMaxStackSize(Slot.Item); 
		int32 RemainingQuantity = Quantity;

		// Update existing non-full stacks
		for (FItemSlotStack& Stack : Slot.Stacks)
		{
			if (Stack.StackSize < MaxStackSize)
			{
				const int32 AvailableSpace = MaxStackSize - Stack.StackSize;
				const int32 AmountToAdd = FMath::Min(AvailableSpace, RemainingQuantity);

				Stack.StackSize += AmountToAdd;
				RemainingQuantity -= AmountToAdd;

				if (RemainingQuantity <= 0)
				{
					return; // All the quantity has been successfully added to existing stacks.
				}
			}
		}

		// Create new stacks if any quantity is left
		while (RemainingQuantity > 0)
		{
			const int32 AmountToAdd = FMath::Min(MaxStackSize, RemainingQuantity);
			FItemSlotStack NewStack(AmountToAdd, Slot.SlotGuid); // Assuming constructor takes stack size and slot GUID
			Slot.Stacks.Add(NewStack);

			RemainingQuantity -= AmountToAdd;
		}
	}

	// Function to reduce quantity in stacks and remove empty stacks
	static void ReduceQuantityInStacks(FItemSlot& Slot, int32 QuantityToReduce)
	{
		// Sort stacks by their quantity from lowest to highest
		Slot.Stacks.Sort([](const FItemSlotStack& A, const FItemSlotStack& B) {
			return A.StackSize < B.StackSize;
		});

		TArray<FItemSlotStack> FinalStacks = Slot.Stacks;
		
		// Reduce quantity stack by stack
		for (FItemSlotStack& Stack : Slot.Stacks)
		{
			if (QuantityToReduce <= 0) break; // No more quantity to reduce

			const int32 QuantityReduced = FMath::Min(QuantityToReduce, Stack.StackSize);
			Stack.StackSize -= QuantityReduced;
			QuantityToReduce -= QuantityReduced;

			
			// If a stack is empty, mark it for removal
			if (Stack.StackSize <= 0)
			{
				Stack.StackSize = 0; // Ensure the stack size is not negative
			}
		}

		// Remove all empty stacks
		Slot.Stacks.RemoveAll([](const FItemSlotStack& Stack) {
			return Stack.StackSize <= 0;
		});
	}
	
	// Function to add a new stack to the inventory
	static int32 UpdateStack(FItemSlot& Slot, const FGuid& StackGuid, const int32 Quantity)
	{
		// If no valid stack is found, add a new one
		if (FItemSlotStack* ExistingStack = Slot.Stacks.FindByKey(StackGuid))
		{
			// Logic to update the existing stack's quantity
			const int32 AvailableSpace = DetermineMaxStackSize(Slot.Item) - ExistingStack->StackSize;
			const int32 AmountToAdd = FMath::Min(AvailableSpace, Quantity);
        
			ExistingStack->StackSize += AmountToAdd;
        
			// If the stack size drops to 0 or below, remove the stack
			if (ExistingStack->StackSize <= 0)
			{
				Slot.Stacks.RemoveSingle(*ExistingStack);
			}
        
			return Quantity - AmountToAdd;
		}
		else
		{
			// Add a new stack with the specified quantity
			const int32 AmountToAdd = FMath::Min(DetermineMaxStackSize(Slot.Item), Quantity);
			const FItemSlotStack NewStack(AmountToAdd, Slot.SlotGuid);
			Slot.Stacks.Add(NewStack);

			return Quantity - AmountToAdd;
		}
	}

/*===============================================================================
		VALIDATIONS
===============================================================================*/
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static bool IsSlotValid(const FItemSlot& Slot)
	{
		return Slot.IsValid();
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	static bool IsItemValid(const UMounteaInstancedItem* Item)
	{
		if (!Item) return false;

		if (!Item->GetItemData().IsValid()) return false;
		
		return true;
	}
};

#undef LOCTEXT_NAMESPACE