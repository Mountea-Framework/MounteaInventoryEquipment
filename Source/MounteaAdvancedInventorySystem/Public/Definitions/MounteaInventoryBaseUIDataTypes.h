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
	FIntPoint SlotPosition = FIntPoint::ZeroValue;

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

	/**
	 * Specifies the associated user interface widget for the inventory slot.
	 *
	 * This property holds a reference to a UUserWidget, which can be used to visually represent
	 * the slot within the inventory system. Allows customization and configuration of slot appearance
	 * and behavior through the editor and Blueprints.
	 *
	 * Editable in the editor and accessible in Blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	TObjectPtr<UUserWidget> SlotWidget;

	/**
	 * Represents the quantity of items in the slot.
	 *
	 * This property holds an integer value indicating the number of items
	 * currently stored in the inventory slot. It assists in managing stackable
	 * items.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	int32 SlotQuantity = INDEX_NONE;

public:

	bool IsValid() const { return SlotWidget != nullptr; };
		
	void ResetSlot()	{	OccupiedItemId = FGuid(); SlotQuantity = INDEX_NONE;	};
	
	bool IsEmpty() const	{	return OccupiedItemId.IsValid() == false; };

	bool operator==(const FMounteaInventoryGridSlot& Other) const
	{
		return SlotPosition == Other.SlotPosition;
	}

	bool operator!=(const FMounteaInventoryGridSlot& Other) const
	{
		return !(*this == Other);
	}

	FString ToString() const;
};

FORCEINLINE uint32 GetTypeHash(const FMounteaInventoryGridSlot& Slot)
{
	return GetTypeHash(Slot.SlotPosition);
}
