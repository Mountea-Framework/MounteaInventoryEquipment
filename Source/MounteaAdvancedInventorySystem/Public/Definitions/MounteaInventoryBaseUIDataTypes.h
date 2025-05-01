// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventoryBaseUIDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FMounteaInventoryGridSlot
{
	GENERATED_BODY()

public:
	
	/**
	 * Defines the position of the slot in a grid.
	 *
	 * This property specifies the 2D position of the inventory slot, represented as an integer point
	 * with X and Y coordinates. Typically used for inventory or grid-based UI layouts.
	 *
	 * Editable in the editor and accessible in Blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	FIntPoint SlotPosition;

	/**
	 * Represents the unique identifier for the item occupying the grid slot.
	 *
	 * This property stores a globally unique identifier (GUID) corresponding to the item
	 * currently placed in the slot. It is used to track and manage item assignments
	 * within the inventory system.
	 *
	 * Read-only and visible in the editor and accessible in Blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	FGuid OccupiedItemId;

public:
		
	void ResetSlot()	{	OccupiedItemId = FGuid();	};
	
	bool IsEmpty() const	{	return OccupiedItemId.IsValid() == false;	};

	bool operator==(const FMounteaInventoryGridSlot& Other) const
	{
		return SlotPosition == Other.SlotPosition && OccupiedItemId == Other.OccupiedItemId;
	}

	bool operator!=(const FMounteaInventoryGridSlot& Other) const
	{
		return !(*this == Other);
	}
};

FORCEINLINE uint32 GetTypeHash(const FMounteaInventoryGridSlot& Slot)
{
	return GetTypeHash(Slot.SlotPosition) ^ GetTypeHash(Slot.OccupiedItemId);
}
