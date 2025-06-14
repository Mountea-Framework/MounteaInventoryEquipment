﻿// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventoryBaseUIDataTypes.generated.h"

/**
 * A generic inventory slot structure that encapsulates the core properties
 * shared by all types of inventory slots (e.g., item ID, quantity, associated widget).
 *
 * This struct serves as a parent/base for more specialized slot types (like grid slots,
 * list slots, etc.), allowing you to keep common logic and data in one place.
 */
USTRUCT(BlueprintType,
	meta=(HasNativeMake="/Script/MounteaAdvancedInventorySystem.MounteaInventoryUIStatics.MakeInventorySlot"))
struct FInventorySlot
{
	GENERATED_BODY()

public:
	virtual ~FInventorySlot() = default;

	/**
	 * Represents the unique identifier for the item occupying this slot.
	 *
	 * This GUID is used to track which item (if any) is stored here.
	 * If it’s invalid (IsValid() == false), the slot is considered empty.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	FGuid OccupiedItemId;

	/**
	 * The number of items stacked in this slot.
	 *
	 * If the slot is empty, this will typically be INDEX_NONE or 0,
	 * depending on your stack‐logic. You can use INDEX_NONE to signify “no valid quantity.”
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	int32 SlotQuantity = INDEX_NONE;

	/**
	 * Optional reference to a UUserWidget used to visually represent this slot.
	 *
	 * You can assign a widget (e.g., a UUserWidget subclass) here so that
	 * when the slot is rendered in the UI, it knows which widget to spawn or update.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	TObjectPtr<UUserWidget> SlotWidget;

public:

	FInventorySlot()
		: OccupiedItemId(FGuid())
		, SlotQuantity(INDEX_NONE)
		, SlotWidget(nullptr)
	{}
	
	FInventorySlot(const struct FMounteaInventoryGridSlot& GridSlot);
	
	/**
	 * Returns true if this slot currently holds no valid item (i.e., OccupiedItemId is invalid).
	 */
	virtual bool IsEmpty() const
	{
		return !OccupiedItemId.IsValid();
	}

	/**
	 * Returns true if the slot has a valid widget assigned.
	 * You can use this to check if the slot has been initialized/assigned a UI element.
	 */
	virtual bool IsValid() const
	{
		return SlotWidget != nullptr;
	}

	/**
	 * Clears this slot’s data, marking it as empty.
	 * Resets OccupiedItemId to a fresh GUID (invalid), and SlotQuantity to INDEX_NONE.
	 */
	virtual void ResetSlot()
	{
		OccupiedItemId = FGuid();
		SlotQuantity = INDEX_NONE;
		SlotWidget = nullptr;
	}

	/**
	 * Converts this slot’s state into a human‐readable string.
	 * Example format: “Item: 1234ABCD-... | Qty: 5”
	 */
	virtual FString ToString() const
	{
		const FString ItemIDString = OccupiedItemId.IsValid()
			? OccupiedItemId.ToString()
			: TEXT("None");
		const FString QtyString = (SlotQuantity >= 0)
			? FString::FromInt(SlotQuantity)
			: TEXT("None");

		return FString::Printf(TEXT("Quantity: %d | Qty: %s"), SlotQuantity, *QtyString);
	}

	bool operator==(const FInventorySlot& Other) const
	{
		return OccupiedItemId == Other.OccupiedItemId;
	}

	bool operator!=(const FInventorySlot& Other) const
	{
		return !(*this == Other);
	}
};

/**
 * Defines the position of the slot in a grid.
 *
 * This property specifies the 2D position of the inventory slot, represented as an integer point
 * with X and Y coordinates. Typically used for inventory or grid-based UI layouts.
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryGridSlot : public FInventorySlot
{
	GENERATED_BODY()

public:
	FMounteaInventoryGridSlot();
	FMounteaInventoryGridSlot(const FInventorySlot& SourceSlot);

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

public:

	bool operator==(const FMounteaInventoryGridSlot& Other) const
	{
		return SlotPosition == Other.SlotPosition;
	}

	bool operator!=(const FMounteaInventoryGridSlot& Other) const
	{
		return !(*this == Other);
	}

	virtual FString ToString() const override;
};

FORCEINLINE uint32 GetTypeHash(const FInventorySlot& Slot)
{
	return GetTypeHash(Slot.OccupiedItemId);
}

FORCEINLINE uint32 GetTypeHash(const FMounteaInventoryGridSlot& Slot)
{
	return GetTypeHash(Slot.SlotPosition);
}

FORCEINLINE bool operator==(const FInventorySlot& LHS, const FMounteaInventoryGridSlot& RHS)
{
	return LHS.OccupiedItemId == RHS.OccupiedItemId;
}
