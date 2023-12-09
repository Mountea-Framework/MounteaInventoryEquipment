// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaEquipmentHelpers.h"
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
				return
				Filter.SlotItem != nullptr &&
					Slot.SlotIdentity.SlotName.EqualTo(Filter.SlotID) &&
						( Slot.SlotIdentity.SlotTag == Filter.SlotTag || Slot.SlotIdentity.SlotCompatibleTags.HasTag(Filter.SlotTag) );
			}))
		{
			return Slots.Find(*EmptySlot); // Return index of to the found empty slot.
		}

		// No empty slot found, return invalid index	
		return INDEX_NONE;
	}

	static int32 FindEquipmentSlotID(const TArray<FEquipmentSlot>& Slots, const FGameplayTag& ID)
	{
		if (const FEquipmentSlot* EmptySlot = Slots.FindByPredicate(
			[ID](const FEquipmentSlot& Slot)
			{
				return Slot.SlotIdentity.SlotTag == ID;
			}))
		{
			return Slots.Find(*EmptySlot); // Return index of to the found empty slot.
		}

		// No empty slot found, return invalid index	
		return INDEX_NONE;
	}
};
