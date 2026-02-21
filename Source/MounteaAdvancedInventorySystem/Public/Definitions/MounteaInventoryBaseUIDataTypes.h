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
#include "MounteaInventoryBaseUIEnums.h"
#include "MounteaInventoryItem.h"
#include "MounteaInventoryBaseUIDataTypes.generated.h"

enum class ECommonInputType : uint8;
enum class EMounteaWidgetInputMethod : uint8;
enum class EMounteaWidgetInputKey : uint8;
class UUserWidget;
class UTextureCube;
class UMounteaSelectableInventoryItemAction;

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
	 * Inventory Item Data which is stored in this Widget.
	 * 
	 * This data is stored to simplify the search logic.
	 * If this Item is invalid (IsItemValid() == false), the data is considered to
	 * represent an empty slot.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Data", 
		meta=(DisplayPriority=0))
	FInventoryItem ContainingItem;

	/**
	 * Quantity of the item represented by this data.
	 *
	 * If the data represents an empty slot, this will typically be INDEX_NONE or 0,
	 * depending on your stack logic. INDEX_NONE can be used to signify “no valid quantity”.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Data", 
		meta=(DisplayPriority=1))
	int32 Quantity = INDEX_NONE;

public:

	FInventoryItemData()
		: ContainingItem(FInventoryItem())
		, Quantity(INDEX_NONE)
	{
	}

	FInventoryItemData(const int32 InQuantity, const FInventoryItem& InContainingItem)
		: ContainingItem(InContainingItem)
		, Quantity(InQuantity)
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
	
	bool IsDirty(const FInventoryItemData& PreviousState) const
	{
		if (*this != PreviousState)
			return true;
		return Quantity != PreviousState.Quantity;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", 
		meta=(DisplayPriority=0))
	FGuid SlotGuid;

	/**
	 * Child widget that visually represents the item inside this slot.
	 *
	 * This widget should implement UMounteaAdvancedInventoryItemWidgetInterface.
	 * It may be null when the slot is not currently represented in the UI or
	 * before initialization.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", 
		meta=(DisplayPriority=1))
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Primary Data", 
		meta=(DisplayPriority=0))
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", 
		meta=(DisplayPriority=0))
	TArray<FGuid> StoredIds;
};

USTRUCT(BlueprintType)
struct FMounteaPreviewDirectionalLightSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(EditCondition="bEnabled"),
		meta=(DisplayPriority=0))
	FLinearColor LightColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(EditCondition="bEnabled"),
		meta=(DisplayPriority=1))
	FRotator Rotation = FRotator(-40.0f, -67.5f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(EditCondition="bEnabled"),
		meta=(DisplayPriority=2))
	float Intensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(DisplayPriority=3))
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(EditCondition="bEnabled"),
		meta=(DisplayPriority=4))
	bool bCastShadows = false;
};

USTRUCT(BlueprintType)
struct FMounteaPreviewSkyLightSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(EditCondition="bEnabled"),
		meta=(DisplayPriority=0))
	TSoftObjectPtr<UTextureCube> Cubemap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(EditCondition="bEnabled"),
		meta=(DisplayPriority=1))
	float Intensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(DisplayPriority=2))
	bool bEnabled = true;
};

USTRUCT(BlueprintType)
struct FMounteaPreviewPostProcessSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(EditCondition="bEnabled"),
		meta=(DisplayPriority=0))
	FPostProcessSettings PostProcessSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, 
		meta=(DisplayPriority=1))
	bool bEnabled = false;
};

/**
 * FMounteaPreviewCameraControlSettings defines configuration for camera manipulation
 * in preview environments. Controls sensitivity, limits, and enabled state for
 * rotation, zoom, and height adjustment interactions.
 */
USTRUCT(BlueprintType)
struct FMounteaPreviewCameraControlSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(EditCondition="bAllowRotation", UIMin="0.0", ClampMin="0.0", UIMax="180.0", ClampMax="180.0"),
		meta=(DisplayPriority=0))
	FVector3f RotationLimits = FVector3f(0.f, 180.f, 180.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(EditCondition="bAllowZoom", UIMin="0.01", ClampMin="0.01", UIMax="100.0", ClampMax="100.0"),
		meta=(DisplayPriority=1))
	FVector2f ZoomLimits = FVector2f(1.f, 5.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Performance", 
		meta=(UIMin="0.0", ClampMin="0.0", UIMax="60.0", ClampMax="60.0"),
		meta=(DisplayPriority=2))
	float IdleThreshold = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(EditCondition="bAllowRotation", UIMin="0.01", ClampMin="0.01", UIMax="10.0", ClampMax="10.0"),
		meta=(DisplayPriority=3))
	float CameraRotationSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(EditCondition="bAllowZoom", UIMin="0.01", ClampMin="0.01", UIMax="5.0", ClampMax="5.0"),
		meta=(DisplayPriority=4))
	float CameraZoomSensitivity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(EditCondition="bAllowHeightAdjustment", UIMin="0.01", ClampMin="0.01", UIMax="10.0", ClampMax="10.0"),
		meta=(DisplayPriority=5))
	float CameraHeightSensitivity = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(EditCondition="bAllowHeightAdjustment", UIMin="0.01", ClampMin="0.01", UIMax="100.0", ClampMax="100.0"),
		meta=(DisplayPriority=6))
	float HeightLimit = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Performance", 
		meta=(UIMin="0.0", ClampMin="0.0", UIMax="60.0", ClampMax="60.0"),
		meta=(DisplayPriority=7))
	float PreviewTickFrequency = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(DisplayPriority=8))
	bool bAllowRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(DisplayPriority=9))
	bool bAllowZoom = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Controls", 
		meta=(DisplayPriority=10))
	bool bAllowHeightAdjustment = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Performance", 
		meta=(DisplayPriority=11))
	uint8 bAutoStartTick : 1;
};

/**
 * FMounteaWidgetInputPayload represents a lightweight, UI-friendly container
 * for input values forwarded from Player Controllers or Pawns to UI widgets.
 *
 * This struct intentionally avoids Enhanced Input–specific types to ensure
 * widgets remain decoupled from input mapping contexts and gameplay systems.
 *
 * Only the relevant value for a given input should be populated.
 */
USTRUCT(BlueprintType)
struct FMounteaWidgetInputPayload
{
	GENERATED_BODY()

	/** Two-dimensional input value (e.g. mouse delta, analog stick rotation). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mountea|UI Input", 
		meta=(DisplayPriority=0))
	FVector2D Vector2DValue = FVector2D::ZeroVector;

	/** Scalar input value (e.g. mouse wheel delta, trigger axis, zoom amount). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mountea|UI Input", 
		meta=(DisplayPriority=1))
	float FloatValue = 0.f;

	/** Defines type of input to consume. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|UI Input", 
		meta=(DisplayPriority=2))
	EMounteaWidgetInputMethod InputMethod = EMounteaWidgetInputMethod::Boolean;
	
	/** Defines type of input key to consume. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|UI Input", 
		meta=(DisplayPriority=3))
	EMounteaWidgetInputKey InputKey;

	/** Boolean input value (e.g. confirm, cancel, toggle). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mountea|UI Input", 
		meta=(DisplayPriority=4))
	bool bBoolValue = false;
};

/**
 * FMounteaWidgetInputKeyChord represents a UI-friendly binding:
 * - Primary Key
 * - Optional Modifier Key
 *
 * If ModifierKey is invalid, no modifier is required.
 */
USTRUCT(BlueprintType)
struct FMounteaWidgetInputKeyChord
{
	GENERATED_BODY()

public:
	/** Primary key (e.g. I, Escape, Gamepad_FaceButton_Bottom). */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input", 
		meta=(DisplayPriority=0))
	FKey Key;

	/** Optional modifier key (e.g. LeftControl, RightShift). If invalid => no modifier required. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input", 
		meta=(DisplayPriority=1))
	FKey ModifierKey;
};

/**
 * FMounteaWidgetInputKeyTextureMapping defines the icon representation for a specific input method.
 *
 * Used by FMounteaWidgetInputActionMapping to provide per-device icon overrides (e.g. keyboard glyph
 * vs. gamepad glyph) for the same logical UI action/key binding.
 */
USTRUCT(BlueprintType)
struct FMounteaWidgetInputKeyTextureMapping
{
	GENERATED_BODY()

public:
	
	/**
	 * Icon texture used for the specified input method.
	 * Stored as a soft reference to support deferred loading and reduce memory overhead.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input", 
		meta=(DisplayPriority=1))
	TSoftObjectPtr<UTexture2D> InputIcon;

	/**
	 * Input method this mapping applies to.
	 * Determines when this icon should be used based on the currently active input type.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input", 
		meta=(DisplayPriority=0))
	ECommonInputType InputType;	
};

/**
 * FMounteaWidgetInputActionMapping defines a UI action (identified by a gameplay tag)
 * and its associated key bindings (simple keys and/or key chords).
 *
 * The tag describes WHAT action should be triggered, while the key lists describe
 * WHICH physical inputs can trigger it.
 *
 * This struct is intentionally Enhanced Input–agnostic and can be evaluated from
 * Slate key events or from gameplay-side input routing.
 */
USTRUCT(BlueprintType)
struct FMounteaWidgetInputActionMapping
{
	GENERATED_BODY()
	
	FMounteaWidgetInputActionMapping() : bConsume(true)
	{};

	/** 
	 * Gameplay tag identifying the UI action (e.g. UI.Inventory.Close, UI.Inventory.Confirm). 
	 * Limited to "Input" category only!
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input", 
		meta=(Categories="Input,Mountea_Inventory.Input"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=0))
	FGameplayTag ActionTag;

	/**
	 * Widget states in which this action must not trigger.
	 * Used to suppress actions based on current widget state (e.g. disabled, modal, editing text).
	 * Example:
	 * - Input Actions "Confirm" and "Use" both use same Gamepad Key
	 * → If Inventory has Modal window which requires confirmation, the Widget states get updated with the modal Window's state
	 * → Then "Use" can blacklist ModalWindow state
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input", 
		meta=(Categories="Mountea_Inventory.WidgetState.Modal,State"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=1))
	FGameplayTagContainer BlacklistedWidgetStates;

	/** Keys that can trigger this action (Esc, Enter, Gamepad_FaceButton_Right, etc.). */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input",
		meta=(ShowOnlyInnerProperties),
		meta=(TitleProperty="InputType"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=2))
	TMap<FKey, FMounteaWidgetInputKeyTextureMapping> Keys;

	/**
	 * Higher the value, higher is the priority of specified Input.
	 * If you have input with more Keys, the Key with higher priority wins.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Mountea|UI Input", 
		meta=(DisplayPriority=3))
	int32 InputPriority = INDEX_NONE;

	// TODO: Holding 2 buttons at the same time for example
	/** Key chords (key + optional modifiers) that can trigger this action. */
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mountea|UI Input")
	//TArray<FMounteaWidgetInputKeyChord> Chords;

	/** If true, the UI layer should consume the input when this mapping triggers. */
	UPROPERTY(BlueprintReadOnly, Category="Mountea|UI Input", AdvancedDisplay, 
		meta=(DisplayPriority=4))
	uint8 bConsume : 1;
	
	// TODO:
	// - allow remapping
};

#pragma region ItemActionsQueue

struct FActionQueueEntry
{
	TObjectPtr<UMounteaSelectableInventoryItemAction> Action = nullptr;
	TObjectPtr<UObject> Payload = nullptr;
	FDateTime CreationTime;
};

struct FActionsQueue
{
	TArray<FActionQueueEntry> Pending;
    
	bool HasPending() const { return Pending.Num() > 0; }
    
	void Enqueue(FActionQueueEntry&& Entry)
	{
		Pending.Add(MoveTemp(Entry));
	}
    
	bool Remove(UMounteaSelectableInventoryItemAction* Action)
	{
		const int32 index = Pending.IndexOfByPredicate([Action](const FActionQueueEntry& Entry)
		{
			return Entry.Action == Action;
		});
        
		if (index != INDEX_NONE)
		{
			Pending.RemoveAt(index);
			return true;
		}
		return false;
	}
    
	void Clear()
	{
		Pending.Reset();
	}
};

#pragma endregion 

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