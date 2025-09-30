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
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "MounteaInventoryItem.generated.h"

class IMounteaAdvancedInventoryInterface;
class UMounteaInventoryItemTemplate;
class UTexture2D;

/**
 * FInventoryItemSnapshot is a lightweight data structure for tracking replication changes.
 * Snapshots capture the state of inventory items before replication to detect and calculate
 * deltas for quantity, durability, and custom data changes.
 *
 * @see [Replication System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Replication)
 * @see FInventoryItem
 */
USTRUCT()
struct FInventoryItemSnapshot
{
	GENERATED_BODY()

	FInventoryItemSnapshot() = default;
    
	explicit FInventoryItemSnapshot(const struct FInventoryItem& Item);

	bool HasQuantityChanged(const FInventoryItem& Current) const;

	int32 GetQuantityDelta(const FInventoryItem& Current) const;

	bool HasDurabilityChanged(const FInventoryItem& Current) const;

	float GetDurabilityDelta(const FInventoryItem& Current) const;

	bool HasCustomDataChanged(const FInventoryItem& Current) const;

private:
	int32 Quantity = INDEX_NONE;
	float Durability = -1.f;
	FGameplayTagContainer CustomData;
};

/**
 * FInventoryItem represents a runtime instance of an inventory item with network replication support.
 * Item instances contain template references, current quantity, durability, custom data, and affector slots.
 * Implements FastArraySerializer for efficient delta replication in multiplayer environments.
 *
 * @see [Inventory Items](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventoryItems)
 * @see UMounteaInventoryItemTemplate
 * @see IMounteaAdvancedInventoryInterface
 */
USTRUCT(BlueprintType,meta=(HasNativeMake="/Script/MounteaAdvancedInventorySystem.MounteaInventoryStatics.NewInventoryItem"))
struct FInventoryItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	
	/*************************************************************/
	/************ CONSTRUCTOR & UTILS *******************/
	/*************************************************************/

public:
	
	FInventoryItem();
	FInventoryItem(const FGuid& ItemGuid) : Guid(ItemGuid) {};
	explicit FInventoryItem(UMounteaInventoryItemTemplate* InTemplate, const int32 InQuantity = 1,
							const float InDurability = 1.f,
							TScriptInterface<IMounteaAdvancedInventoryInterface> InOwningInventory = nullptr);
	
	/*************************************************************/
	/*************** CORE FUNCTIONALITY ******************/
	/*************************************************************/
	
	/** Check if the item instance is valid */
	bool IsItemValid() const;

	/** Check if the item instance is owned by Inventory */
	bool IsItemInInventory() const;

	/** Returns a string representation of the item */
	FString ToString() const;
	
	/*************************************************************/
	/************************ GETTERS ************************/
	/*************************************************************/
	
public:
	/**
	 * 
	 * @return Returns Item name from Template. 'Invalid' if not valid!
	 */
	FText GetItemName() const;
	
	/**
	 * @return The unique identifier of this item instance
	 */
	FGuid GetGuid() const { return Guid; }
	
	/**
	 *
	 *  @return The template defining this item's base properties
	 */
	UMounteaInventoryItemTemplate* GetTemplate() const { return Template; }
	
	/**
	 *
	 *  @return The current quantity of this item
	 */
	int32 GetQuantity() const { return Quantity; }
	
	/**
	 *
	 *  @return The current durability of this item
	 */
	float GetDurability() const { return Durability; }
	
	/**
	 *
	 *  @return The custom data tags associated with this item
	 */
	const FGameplayTagContainer& GetCustomData() const { return CustomData; }
	
	/**
	 *
	 *  @return The map of affector slots and their corresponding item GUIDs
	 */
	const TMap<FGameplayTag, FGuid>& GetAffectorSlots() const { return AffectorSlots; }

	/**
	 *
	 * @return Owning Inventory of this Item. Could be null!
	 */
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory() const { return OwningInventory; }

	/**
	 * Retrieves the short description of the inventory item.
	 * Returns a localized text based on the item's template, or a default text if the template is invalid.
	 *
	 * @return A localized text containing the item's short information if the template is valid, or a fallback invalid message otherwise.
	 */
	FText GetItemShortInfo() const;

	/**
	 * Retrieves the detailed long description of the inventory item.
	 * This information is typically used for extended item descriptions.
	 *
	 * @return A localized text containing the item's long information if the template is valid, or a fallback invalid message otherwise.
	 */
	FText GetItemLongInfo() const;

	/**
	 * Retrieves the cover image associated with the inventory item.
	 * If the item has a valid associated template, the cover image is loaded and returned.
	 * Otherwise, returns nullptr.
	 *
	 * @return A pointer to the texture representing the item's cover, or nullptr if unavailable.
	 */
	UTexture2D* GetCover() const;
	
	/*************************************************************/
	/************************* SETTERS ************************/
	/*************************************************************/
	
public:
	
	/**
	 * Sets the unique identifier of this item instance
	 * @param InGuid The new GUID to set
	 * @return True if the GUID was changed
	 */
	bool SetGuid(const FGuid& InGuid)
	{
		if (InGuid != Guid && InGuid.IsValid())
		{
			Guid = InGuid;
			return true;
		}
		return false;
	}
	
	/**
	 * Sets the template defining this item's base properties
	 * @param InTemplate The new template to set
	 * @return True if the template was changed
	 */
	bool SetTemplate(UMounteaInventoryItemTemplate* InTemplate);
	
	/**
	 * Sets the current quantity of this item
	 * @param InQuantity The new quantity to set
	 * @return True if the quantity was changed
	 */
	bool SetQuantity(const int32 InQuantity);
	
	/**
	 * Sets the current durability of this item
	 * @param InDurability The new durability to set
	 * @return True if the durability was changed
	 */
	bool SetDurability(const float InDurability);
	
	/**
	 * Sets the custom data tags associated with this item
	 * @param InCustomData The new custom data to set
	 * @return True if the custom data was changed
	 */
	bool SetCustomData(const FGameplayTagContainer& InCustomData)
	{
		if (InCustomData != CustomData)
		{
			CustomData = InCustomData;
			return true;
		}
		return false;
	}
	
	/**
	 * Sets the map of affector slots and their corresponding item GUIDs
	 * @param InAffectorSlots The new affector slots map to set
	 * @return True if the affector slots were changed
	 */
	bool SetAffectorSlots(const TMap<FGameplayTag, FGuid>& InAffectorSlots);

	/**
	 * Sets the new Onwing Inventory
	 * @param InOwningInventory The new Inventory
	 * @return True if the Inventory was changed
	 */
	bool SetOwningInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& InOwningInventory);

	/*************************************************************/
	/******************** OPERATORS *************************/
	/*************************************************************/
	
public:
	
	/**
	 * Equality operator based on GUID and Template match
	 * @param Other The other inventory item to compare with
	 * @return True if items have matching GUIDs and Templates
	 */
	bool operator==(const FInventoryItem& Other) const
	{
		return Guid == Other.Guid && Template == Other.Template;
	}

	/**
	 * Inequality operator based on GUID and Template mismatch
	 * @param Other The other inventory item to compare with
	 * @return True if either GUID or Template differs
	 */
	bool operator!=(const FInventoryItem& Other) const
	{
		return !(*this == Other);
	}
	

	/*************************************************************/
	/*********** PROTECTED PROPERTIES ***************/
	/*************************************************************/
	
public:
	
	/** A unique identifier for this specific instance of the inventory item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	FGuid Guid;

	/** A reference to the static template defining the base properties of the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	TObjectPtr<UMounteaInventoryItemTemplate> Template;

	/**
	 * The current quantity of this item (for stackable items)
	 * Items which cannot stack are set to 1
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	int32 Quantity = 1;

	/** The current durability of the item, if applicable */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	float Durability = 1.f;

	/** Custom tags for item-specific metadata or state (e.g., `ItemState.Broken`) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	FGameplayTagContainer CustomData;

	// TODO: Replace TMap with hashed array of custom struct
	/** Map of affector slots to their corresponding attached items. Items are stored as guids to avoid hard references */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data", NotReplicated)
	TMap<FGameplayTag,FGuid> AffectorSlots;

	/** Inventory which currently owns this Item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	TScriptInterface<IMounteaAdvancedInventoryInterface> OwningInventory;

private:
	/** Snapshot of the item state before replication changes */
	UPROPERTY(NotReplicated, Transient)
	mutable FInventoryItemSnapshot PreReplicationSnapshot;
    
	/** Takes a snapshot of current state before replication */
	void CapturePreReplicationSnapshot() const
	{
		PreReplicationSnapshot = FInventoryItemSnapshot(*this);
	}


	/*************************************************************/
	/******************* SERIALIZATION************************/
	/*************************************************************/

public:

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	void PreReplicatedRemove(const struct FInventoryItemArray& InArraySerializer);
	void PostReplicatedAdd(const struct FInventoryItemArray& InArraySerializer);
	void PostReplicatedChange(const struct FInventoryItemArray& InArraySerializer);
	//void PostReplicatedReceive(const FInventoryItemArray::FPostReplicatedReceiveParameters& Parameters) {};
};

/**
 * FInventoryItemArray is a replicated container for inventory item collections.
 * Provides efficient delta serialization for inventory item arrays using Unreal's FastArraySerializer
 * to minimize network bandwidth in multiplayer scenarios.
 *
 * @see [Replication System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Replication)
 * @see FInventoryItem
 */
USTRUCT(BlueprintType)
struct FInventoryItemArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FInventoryItem> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventoryItem>(Items, DeltaParams, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FInventoryItemArray > : public TStructOpsTypeTraitsBase2< FInventoryItemArray >
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};

template<>
struct TStructOpsTypeTraits<FInventoryItem> : public TStructOpsTypeTraitsBase2<FInventoryItem>
{
	enum 
	{
		WithNetSerializer = true,
	};
};
