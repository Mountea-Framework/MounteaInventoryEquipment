// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "MounteaInventoryItem.generated.h"

class IMounteaAdvancedInventoryInterface;
class UMounteaInventoryItemTemplate;

/**
* Represents a single instance of an inventory item.
* Contains all runtime data for the item including its template, quantity, durability and custom data.
*/
USTRUCT(BlueprintType)
struct FInventoryItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	
/*************************************************************/
/*********** CONSTRUCTOR & UTILS ******************/
/*************************************************************/

public:
	
	FInventoryItem();
	explicit FInventoryItem(UMounteaInventoryItemTemplate* InTemplate, const int32 InQuantity = 1,
							const float InDurability = 1.f,
							TScriptInterface<IMounteaAdvancedInventoryInterface> InOwningInventory = nullptr);
	
/*************************************************************/
/************** CORE FUNCTIONALITY *****************/
/*************************************************************/
	
	/** Check if the item instance is valid */
	bool IsItemValid() const;

	/** Check if the item instance is owned by Inventory */
	bool IsItemInInventory() const;
	
/*************************************************************/
/*********************** GETTERS ************************/
/*************************************************************/
	
public:
	
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
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory() const { return OwningInventory; };

/*************************************************************/
/************************ SETTERS ***********************/
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

/*************************************************************/
/******************* OPERATORS ************************/
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
	
protected:
	
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
	
	/** Map of affector slots to their corresponding attached items. Items are stored as guids to avoid hard references */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	TMap<FGameplayTag,FGuid> AffectorSlots;

	/** Inventory which currently owns this Item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	TScriptInterface<IMounteaAdvancedInventoryInterface> OwningInventory;


/*************************************************************/
/******************* SERIALIZATION**********************/
/*************************************************************/

public:

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Guid;
		Ar << Template;
		Ar << Quantity;
		Ar << Durability;
		CustomData.NetSerialize(Ar, Map, bOutSuccess);

		if (Ar.IsLoading())
			AffectorSlots.Empty();
		
		int32 NumPairs = AffectorSlots.Num();
		Ar << NumPairs;

		if (Ar.IsLoading())
		{
			for (int32 i = 0; i < NumPairs; ++i)
			{
				FGameplayTag Key;
				FGuid Value;
				Ar << Key;
				Ar << Value;
				AffectorSlots.Add(Key, Value);
			}
		}
		else
		{
			for (const auto& Pair : AffectorSlots)
			{
				FGameplayTag Key = Pair.Key;
				FGuid Value = Pair.Value;
				Ar << Key;
				Ar << Value;
			}
		}
    
		bOutSuccess = true;
		return true;
	}

	void PreReplicatedRemove(const struct FInventoryItemArray& InArraySerializer) {};
	void PostReplicatedAdd(const struct FInventoryItemArray& InArraySerializer) {};
	void PostReplicatedChange(const struct FInventoryItemArray& InArraySerializer) {};
	//void PostReplicatedReceive(const FInventoryItemArray::FPostReplicatedReceiveParameters& Parameters) {};
};

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
