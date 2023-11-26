// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "MounteaEquipmentHelpers.h"
#include "Definitions/MounteaInventoryInstancedItem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaEquipmentBFL.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static int32 FindEquipmentSlotID(const TArray<FEquipmentSlot>& Slots, const FMounteaEquipmentSlotCompare& Filter)
	{
		if (const FEquipmentSlot* EmptySlot = Slots.FindByPredicate(
			[Filter](const FEquipmentSlot& Slot)
			{
				return Filter.SlotItem != nullptr && Slot.SlotName.EqualTo(Filter.SlotID) && Slot.SlotTags.HasAny(Filter.SlotItem->GetItemFlags());
			}))
		{
			return Slots.Find(*EmptySlot); // Return index of to the found empty slot.
		}

		// No empty slot found, return invalid index	
		return INDEX_NONE;
	}

	static int32 FindEquipmentSlotID(const TArray<FEquipmentSlot>& Slots, const FText& ID)
	{
		if (const FEquipmentSlot* EmptySlot = Slots.FindByPredicate(
			[ID](const FEquipmentSlot& Slot)
			{
				return Slot.SlotName.EqualTo(ID);
			}))
		{
			return Slots.Find(*EmptySlot); // Return index of to the found empty slot.
		}

		// No empty slot found, return invalid index	
		return INDEX_NONE;
	}
};
