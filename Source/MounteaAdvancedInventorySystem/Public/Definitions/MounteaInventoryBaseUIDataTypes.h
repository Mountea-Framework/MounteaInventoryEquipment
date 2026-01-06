// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventoryItem.h"
#include "MounteaInventoryBaseUIDataTypes.generated.h"

// ============================================================================
//  Inventory Slot & Item Widget Data
//  Responsibility:
//  - FInventorySlot: purely inventory/model-side container information.
//  - FMounteaInventoryGridSlot: adds grid position on top of FInventorySlot.
//  - FItemWidgetData: all data a UI widget needs to render an item/slot.
// ============================================================================

class UUserWidget;

/**
 * FInventoryItemData represents the pure logical data of a single inventory entry.
 *
 * This structure contains no references to widgets or other UI-specific types.
 * It is suitable for use in both runtime logic and UI data binding, and can be
 * shared between inventory systems and UI presentation layers.
 *
 * In the UI chain:
 *   UISlot (slot widget)
 *     -> UISlotData (FInventorySlot)
 *         -> UIItem (child widget implementing UMounteaAdvancedInventoryItemWidgetInterface)
 *             -> UIItemData (FInventoryItemData, this struct) -> FInventoryItem
 *
 * @see FInventorySlot
 * @see FMounteaInventoryGridSlot
 */
USTRUCT(BlueprintType,
	meta = (HasNativeMake = "/Script/MounteaAdvancedInventorySystem.MounteaInventoryUIStatics.MakeInventorySlot"))
struct FInventoryItemData
{
	GENERATED_BODY()

public:

	/**
	 * Quantity of the item represented by this data.
	 *
	 * If the data represents an empty slot, this will typically be INDEX_NONE or 0,
	 * depending on your stack logic. INDEX_NONE can be used to signify “no valid quantity”.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Data")
	int32 Quantity = INDEX_NONE;

	/**
	 * Inventory Item Data which is stored in this Widget.
	 * 
	 * This data is stored to simplify the search logic.
	 * If this Item is invalid (IsItemValid() == false), the data is considered to
	 * represent an empty slot.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Data")
	FInventoryItem ContainingItem;

public:

	FInventoryItemData()
		: Quantity(INDEX_NONE)
		, ContainingItem(FInventoryItem())
	{
	}

	FInventoryItemData(const int32 InQuantity, const FInventoryItem& InContainingItem)
		: Quantity(InQuantity)
		, ContainingItem(InContainingItem)
	{
	}

	bool IsEmpty() const
	{ return !ContainingItem.IsItemValid(); }

	void Reset()
	{
		Quantity = INDEX_NONE;
		ContainingItem = FInventoryItem();
	}

	FString ToString() const
	{
		const FString Item = ContainingItem.IsItemValid()
			? ContainingItem.ToString()
			: TEXT("None");
		const FString QtyString = (Quantity >= 0)
			? FString::FromInt(Quantity)
			: TEXT("None");

		return FString::Printf(
			TEXT("ItemId: %s \n Quantity: %s"),
			*Item,
			*QtyString
		);
	}

	bool operator==(const FInventoryItemData& Other) const
	{ return ContainingItem == Other.ContainingItem; }

	bool operator!=(const FInventoryItemData& Other) const
	{ return !(*this == Other); }
};

/**
 * FInventorySlot is the base structure for all inventory slot UI representations.
 *
 * A slot:
 * - Has its own GUID (SlotGuid) to identify the slot instance.
 * - Maintains a reference to the child widget that visually represents the item,
 *   which should implement UMounteaAdvancedInventoryItemWidgetInterface.
 *
 * The slot does not own item data directly. Instead, it queries the child widget
 * for FInventoryItemData via the interface.
 *
 * In the chain:
 *   UISlot (slot widget)
 *     -> UISlotData (FInventorySlot)
 *         -> UIItem (child widget implementing UMounteaAdvancedInventoryItemWidgetInterface)
 *             -> UIItemData (FInventoryItemData)
 *
 * @see FInventoryItemData
 * @see FMounteaInventoryGridSlot
 */
USTRUCT(BlueprintType,
	meta = (HasNativeMake = "/Script/MounteaAdvancedInventorySystem.MounteaInventoryUIStatics.MakeInventorySlot"))
struct FInventorySlot
{
	GENERATED_BODY()

public:
	virtual ~FInventorySlot() = default;
	
	// Unique Slot ID.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	FGuid SlotGuid;

	/**
	 * Child widget that visually represents the item inside this slot.
	 *
	 * This widget should implement UMounteaAdvancedInventoryItemWidgetInterface.
	 * It may be null when the slot is not currently represented in the UI or
	 * before initialization.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> ItemWidget;

public:

	FInventorySlot()
		: SlotGuid(FGuid())
		, ItemWidget(nullptr)
	{
	}

	FInventorySlot(UUserWidget* InItemWidget)
		: SlotGuid(FGuid())
		, ItemWidget(InItemWidget)
	{
	}

	void OverrideGuid(const FGuid& InGuid)
	{ SlotGuid = InGuid; }
	
	bool IsEmpty() const;
	
	bool HasItemWidget() const
	{ return ItemWidget != nullptr; }
	
	virtual void ResetSlot()
	{ ItemWidget = nullptr; }
	
	virtual FString ToString() const;

	bool operator==(const FInventorySlot& Other) const
	{ return SlotGuid == Other.SlotGuid; }

	bool operator!=(const FInventorySlot& Other) const
	{ return !(*this == Other); }
};

/**
 * FMounteaInventoryGridSlot extends FInventorySlot with 2D grid positioning
 * for grid-based inventory layouts.
 *
 * Grid slots provide spatial positioning information for inventory items in
 * grid-based systems, enabling precise placement and collision checks within
 * structured inventory grids.
 *
 * Position is stored in the slot, not in item/widget data.
 *
 * @see FInventorySlot
 * @see FInventoryItemData
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryGridSlot : public FInventorySlot
{
	GENERATED_BODY()

public:
	FMounteaInventoryGridSlot()
		: SlotPosition(FIntPoint::ZeroValue)
	{
	}

	FMounteaInventoryGridSlot(const FInventorySlot& SourceSlot)
	{
		ItemWidget = SourceSlot.ItemWidget;
		SlotPosition = FIntPoint::ZeroValue;
	}

public:

	/**
	 * Defines the position of the slot in a grid.
	 *
	 * This property specifies the 2D position of the inventory slot, represented as an integer
	 * point with X and Y coordinates. Typically used for inventory or grid-based UI layouts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Primary Data")
	FIntPoint SlotPosition = FIntPoint::ZeroValue;

public:

	bool operator==(const FMounteaInventoryGridSlot& Other) const
	{
		return SlotPosition == Other.SlotPosition
			&& static_cast<const FInventorySlot&>(*this) == static_cast<const FInventorySlot&>(Other);
	}

	bool operator!=(const FMounteaInventoryGridSlot& Other) const
	{
		return !(*this == Other);
	}

	virtual FString ToString() const override
	{
		return FString::Printf(
			TEXT("%s\nGridPos: (%d, %d)"),
			*FInventorySlot::ToString(),
			SlotPosition.X,
			SlotPosition.Y
		);
	}
};

/**
 * FInventoryUICustomData stores UI-specific metadata associated with inventory elements.
 *
 * UI systems may read these values to reconstruct state, while gameplay systems should
 * generally treat this data as optional and UI-only.
 */
USTRUCT(BlueprintType)
struct FInventoryUICustomData
{
	GENERATED_BODY()
	
public:

	/**
	 * Collection of GUIDs used to store UI-specific identifiers.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TArray<FGuid> StoredIds;
};


FORCEINLINE uint32 GetTypeHash(const FInventoryItemData& Data)
{
	return HashCombine(
		GetTypeHash(Data.ContainingItem.GetGuid()),
		GetTypeHash(Data.Quantity)
	);
}

FORCEINLINE uint32 GetTypeHash(const FInventorySlot& Slot)
{
	return GetTypeHash(Slot.SlotGuid);
}

FORCEINLINE uint32 GetTypeHash(const FMounteaInventoryGridSlot& Slot)
{
	return HashCombine(
		GetTypeHash(static_cast<const FInventorySlot&>(Slot)),
		GetTypeHash(Slot.SlotPosition)
	);
}

FORCEINLINE bool operator==(const FInventorySlot& LHS, const FMounteaInventoryGridSlot& RHS)
{
	return LHS.SlotGuid == RHS.SlotGuid;
}