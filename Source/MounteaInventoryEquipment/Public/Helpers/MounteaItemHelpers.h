﻿// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once
#include "GameplayTagContainer.h"

#include "MounteaItemHelpers.generated.h"

class UMounteaInventoryItemAction;
class UMounteaInventoryItemConfig;
class UMounteaInventoryItemCategory;
class UMounteaInventoryItemRarity;
struct FMounteaItemAction;
class UMounteaInstancedItem;

UENUM(BlueprintType)
enum class EItemDataSource : uint8
{
	EIDS_SourceTable				UMETA(DisplayName="Source Table"),
	EIDS_SourceItem				UMETA(DisplayName="Source Item"),

	Default									UMETA(hidden)
};

#pragma region ItemSlot

/**
 * FItemSlotStack
 *
 * This structure represents a stack of items within a single inventory slot. It holds the quantity of items in the stack
 * and unique identifiers for both the stack itself and the slot it resides in. The structure is designed to manage items
 * that are stackable in the inventory, such as ammunition, potions, or crafting materials.
 *
 * Properties:
 * - StackSize: The number of items within this particular stack. It is a crucial attribute for gameplay mechanics
 *   involving the consumption or collection of items in bulk.
 * 
 * - StackGuid: A unique identifier for this specific stack. This GUID is essential for tracking the stack across
 *   various inventory operations, such as merging, splitting, or transferring stacks between slots or inventories.
 * 
 * - SlotGuid: The unique identifier of the inventory slot that this stack belongs to. It links the stack to its
 *   location within the inventory system, allowing for organized management and retrieval of items.
 *   
 * Usage:
 * The FItemSlotStack is used within inventory systems that require detailed management of item stacks. It can be utilized
 * by systems responsible for crafting, looting, or any other in-game mechanics where items need to be counted and managed
 * in grouped quantities.
 * 
 * Constructors:
 * - Default constructor initializes a stack with zero size and new GUIDs, representing an empty stack.
 * - Parameterized constructor allows for initializing the stack with a specific size and GUIDs.
 * - Convenience constructor provides a way to create a stack with a given size and slot identifier while automatically
 *   generating a new stack GUID.
 * 
 * Methods:
 * - IsValidStackSize: Verifies that the stack size is greater than zero, meaning the stack is not empty.
 * - HasValidGuids: Checks that the stack and slot identifiers are valid (i.e., not default GUIDs).
 * - IsValid: Performs a comprehensive validity check, confirming both the stack size and GUIDs are valid.
 * 
 * Operators:
 * - Equality operator: Compares stacks based on their GUIDs to determine if they are the same.
 * 
 * Serialization:
 * - Serialize: Handles the serialization process for the stack, allowing it to be saved to or loaded from persistent storage.
 */
USTRUCT(BlueprintType, Blueprintable, DisplayName="Item Slot Stack")
struct FItemSlotStack
{
	GENERATED_BODY()

	// Default constructor with no parameters.
	FItemSlotStack()
		: StackSize(0), StackGuid(FGuid::NewGuid()), SlotGuid(FGuid())
	{};

	// Constructor with parameters to initialize the stack.
	FItemSlotStack(const int32 InStackSize, const FGuid&InStackGuid, const FGuid&InSlotGuid)
		: StackSize(InStackSize), StackGuid(InStackGuid), SlotGuid(InSlotGuid)
	{};

	// Convenience constructor for creating a stack with a size and parent Slot but generating new GUID.
	explicit FItemSlotStack(const int32 InStackSize, const FGuid& InSlotGuid)
		: StackSize(InStackSize), SlotGuid(InSlotGuid)
	{};

public:

	/**
	 * The quantity of items within this stack. This is used to determine how many items are present in the stack,
	 * affecting gameplay elements like using, dropping, or transferring items.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 StackSize = 0;

	/**
	* A unique identifier for this stack, used to distinguish it from other stacks of the same item.
	* This GUID is critical for tracking individual stacks during inventory transactions.
	* This GUID is critical as UI elements only know this GUID and identify Stacks and its owning Slots by this very value.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGuid StackGuid = FGuid::NewGuid();

	/**
	 * The identifier for the slot in which this stack resides. It ensures that the stack is associated with the correct
	 * location in the inventory, facilitating organized management of inventory items.
	 * Slot Guid represents the Item's Guid which resides in the Slot. Item Guid and Slot Guid are always the same.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGuid SlotGuid;

public:

	// Checks if the stack size is valid (greater than zero).
	bool IsValidStackSize() const
	{
		return StackSize > 0;
	};

	// Checks if the stack has valid GUIDs (not zero GUIDs).
	bool HasValidGuids() const
	{
		return StackGuid.IsValid() && SlotGuid.IsValid();
	};

	// Full validation check for the stack.
	bool IsValid() const
	{
		return IsValidStackSize() && HasValidGuids();
	};

	friend FArchive& operator<<(FArchive& Ar, FItemSlotStack& Element);
	
	bool Serialize(class FArchive& Ar);
	
	bool operator==(const FItemSlotStack& Other) const;

	bool operator==(const FGuid& Other) const;
};

/**
 * FItemSlot
 *
 * This structure represents a single slot within an inventory system in a game. It can hold a specific item and manage 
 * multiple stacks of that item if it is stackable. The primary purpose of this structure is to keep track of the items and 
 * their quantities in a way that is easy to manage and understand.
 *
 * Properties:
 * - Item: A reference to the actual item instance this slot is holding. This can be any object derived from 
 *   'UMounteaInstancedItem', which contains the data and functionality of the item in question.
 * 
 * - Stacks: A collection of 'FItemSlotStack' structures, each representing a stack of items within the slot. This is 
 *   particularly useful for managing items that can be stacked, such as resources, consumables, or ammunition.
 * 
 * Usage:
 * This struct is used wherever there is a need to keep track of items within an inventory. It provides a clear and 
 * concise way to access and manipulate the items and their quantities, whether for displaying in a UI, saving/loading 
 * game state, or any other inventory-related functionality.
 * 
 * Constructors:
 * - Default constructor creates an empty slot with no item.
 * - Parameterized constructor initializes the slot with a given item and a set of stacks.
 * - Explicit constructor creates a slot with a specified item but without predefined stacks.
 * 
 * Methods:
 * - IsValid: Checks if the slot is properly initialized with an item and has at least one stack.
 * - AreStacksValid: Ensures that all stacks within the slot have GUIDs that match the item's GUID, maintaining integrity.
 * - IsStackSizeValid: Verifies that the total quantity across all stacks does not exceed the maximum allowed for the item.
 * 
 * Operators:
 * - Equality operators: Allow comparison of slots based on the item they hold or the GUIDs associated with the item.
 * 
 * Serialization:
 * - Serialize: Manages the process of saving the slot's state or loading it from storage, ensuring that the item and
 *   its stacks are properly stored and retrieved.
 */
USTRUCT(BlueprintType, Blueprintable, DisplayName="Item Slot")
struct FItemSlot
{
	GENERATED_BODY()

	FItemSlot() : Item(nullptr)
	{};
	
	FItemSlot(const FItemSlot& Other) :
		Item(Other.Item), SlotGuid(Other.SlotGuid), Stacks(Other.Stacks)
	{};
	
	FItemSlot(UMounteaInstancedItem* NewItem, const TArray<FItemSlotStack>& NewStacks) :
		Item(NewItem), Stacks(NewStacks)
	{};

	FItemSlot(UMounteaInstancedItem* NewItem);

public:

	/**
	 * A reference to the actual unique item that this slot contains. It points to an instance of an item, defining what item
	 * the slot is holding and providing access to its data and behaviors.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(DisplayThumbnail=false))
	UMounteaInstancedItem* Item = nullptr;

	/**
	 * The identifier for the slot. 
	 * Slot Guid represents the Item's Guid which resides in the Slot. Item Guid and Slot Guid are always the same.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid SlotGuid;
	
	/**
	 * A set of item stacks, each representing a group of item quantities within this slot. It allows for the management of 
	 * multiple quantities of an item, such as stacking identical items to save space and organize the inventory.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(NoElementDuplicate))
	TArray<FItemSlotStack> Stacks;

public:

	/** Check if the slot is empty, meaning it has no Item. */
	bool IsEmpty() const
	{ return Item == nullptr; };

	/** Checks if the slot is valid, meaning it has an item and at least one stack. */
	bool IsValid() const
	{
		return IsEmpty() == false && SlotGuid.IsValid() && Stacks.Num() > 0;
	}

	/** Validates the consistency of stack GUIDs, ensuring they match with the item's GUID. */
	bool AreStacksValid() const;

	/** Validates the total stack size, ensuring it does not exceed the item's maximum stack size. */
	bool IsStackSizeValid() const;

	bool operator==(const FItemSlot& Other) const;

	bool operator==(const FItemSlot* Other) const;

	bool operator==(const FGuid& Other) const;

	bool operator==(const UMounteaInstancedItem* Other) const;

	friend FArchive& operator<<(FArchive& Ar, FItemSlot& Element);
	
	bool Serialize(class FArchive& Ar);
};

template<>
struct TStructOpsTypeTraits< FItemSlotStack > : public TStructOpsTypeTraitsBase2< FItemSlotStack >
{
	enum
	{
		WithSerializer = true,
		WithAtomic = true,
		WithIdenticalViaEquality = true
	};
};

template<>
struct TStructOpsTypeTraits< FItemSlot > : public TStructOpsTypeTraitsBase2< FItemSlot >
{
	enum
	{
		WithSerializer = true,
		WithAtomic = true,
		WithIdenticalViaEquality = true
	};
};

FORCEINLINE  uint32 GetTypeHash(const FItemSlotStack& Data)
{
	return GetTypeHash(Data.StackGuid);
};

FORCEINLINE  uint32 GetTypeHash(const FItemSlot& Data)
{
	return PointerHash(Data.Item);
};

#pragma endregion

#pragma region ItemData

#define LOCTEXT_NAMESPACE "MounteaInventoryItem"

USTRUCT(BlueprintType)
struct FMounteaItemQuantityData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=1, ClampMin=1, EditCondition="bIsStackable"))
	int32 MaxStackSize = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=1, ClampMin=1, EditCondition="bIsStackable"))
	int32 MaxQuantity = 999;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsStackable : 1;

	FMounteaItemQuantityData()
	{
		bIsStackable = true;
	}
};

/**
 * Basic structure which contains required Inventory Item data.
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemRequiredData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault), DisplayName="Item Flags (Gameplay Tags)")
	FGameplayTagContainer ItemFlags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault))
	FText ItemName = LOCTEXT("MounteaInventoryItem_ItemName", "New Item");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	UMounteaInventoryItemCategory* ItemCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	UMounteaInventoryItemRarity* ItemRarity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault, AllowedClasses="StaticMesh, SkeletalMesh"))
	UStreamableRenderAsset* ItemMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ShowOnlyInnerProperties))
	FMounteaItemQuantityData ItemQuantity;

public:

	bool IsValid() const
	{
		return
		ItemName.IsEmpty() != true &&
		ItemCategory != nullptr &&
		ItemRarity != nullptr &&
		ItemQuantity.MaxQuantity > 0;
	}

public:

	bool operator==(const FMounteaInventoryItemRequiredData& Other) const
	{
		if (ItemFlags.IsEmpty())
		{
			return ItemName.EqualTo(Other.ItemName);
		}

		return ItemName.EqualTo(Other.ItemName) && ItemFlags == Other.ItemFlags;
	}

	bool operator!=(const FMounteaInventoryItemRequiredData& Other) const
	{
		return !(*this == Other);
	}

	/*
	FMounteaInventoryItemRequiredData& operator=(const FMounteaInventoryItemRequiredData& Other)
	{
		return *this;
	}
	*/

	friend uint32 GetTypeHash(const FMounteaInventoryItemRequiredData& Data)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(Data.ItemName.ToString()));
				
		return Hash;
	}
};

/**
 * Basic structure which contains additional Inventory Item data.
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemOptionalData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasWeight : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=0.f, ClampMin=0.f, Units=kg, EditCondition="bHasWeight"))
	float BaseWeight = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasValue : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasValue"))
	float ItemBaseValue = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasDurability : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasDurability"))
	float ItemBaseDurability = 10.f;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> SpawnActor;
};

/**
 * Container for all Inventory Item Data
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemData : public FTableRowBase
{
	GENERATED_BODY()

	FMounteaInventoryItemData(): RequiredData(), OptionalData(), ItemConfig(nullptr)
	{};

	explicit FMounteaInventoryItemData(const FMounteaInventoryItemRequiredData& InRequiredData, const FMounteaInventoryItemOptionalData InOptionalData, const TSet<TSubclassOf<UMounteaInventoryItemAction>> InItemActions, UMounteaInventoryItemConfig* InItemConfig = nullptr)
		: RequiredData(InRequiredData), OptionalData(InOptionalData), ItemActions(InItemActions), ItemConfig(InItemConfig)
	{};

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMounteaInventoryItemRequiredData RequiredData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMounteaInventoryItemOptionalData OptionalData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<UMounteaInventoryItemAction>> ItemActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMounteaInventoryItemConfig* ItemConfig;

public:

	bool IsValid() const
	{
		return RequiredData.IsValid();
	}
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 