// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "MounteaEquipmentSlot.generated.h"

class IMounteaEquipmentInterface;
class UMounteaInventoryItemBase;

/**
 * Mountea Equipment Slot.
 *
 * Represents a single slot in a Mountea Equipment Component.
 *
 * This object provides functionality for managing a specific equipment slot. It allows for the equipping and manipulation of items that can be added and removed.
 * Each equipment slot is compatible with certain types of items, determined by the `SlotCompatibleTag` property.
 * The currently equipped item can be accessed with the `GetSlotItem` function.
 *
 * This class is designed to be highly flexible and extensible, allowing developers to easily create a wide variety of different equipment systems to suit their specific requirements.
 * If the current implementation does not meet your needs, all key methods are exposed as UFUNCTIONs, allowing you to extend and customize the functionality in Blueprints.
 *
 * @see UObject
 * @see UMounteaInventoryItemBase
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Equipment-Slot
 */

UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Equipment Slot (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentSlot : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * Returns the unique identifier for this slot.
	 * This ID is used to distinguish this slot from others in an equipment system.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot")
	FORCEINLINE FString GetSlotID() const { return SlotID; };

	/**
	 * Returns the tag that determines which items can be equipped in this slot.
	 * Only items that contain this tag can be equipped in this slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot")
	FORCEINLINE FGameplayTag GetSlotTag() const { return SlotCompatibleTag; };

	/**
	 * Returns the item currently equipped in this slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot")
	FORCEINLINE UMounteaInventoryItemBase* GetSlotItem() const { return ItemInSlot; };

	/**
	 * Determines if the equipment slot is currently empty.
	 *
	 * This function checks whether there is currently an item equipped in this slot.
	 * It returns true if the slot is empty (i.e., there is no item equipped), and false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot")
	FORCEINLINE bool IsEmpty() const { return ItemInSlot == nullptr; };

	UFUNCTION(Category="Mountea|EquipmentSlot")
	void UpdateItem(UMounteaInventoryItemBase* NewItem);
	
	FORCEINLINE int32 GetRepKey() const { return RepKey; };
protected:

	/**
	 * The unique identifier for this slot.
	 * This ID is used to distinguish this slot from others in an equipment system.
	 * It can be customized in the editor.
	 */
	UPROPERTY(SaveGame, Category="1. Required", EditDefaultsOnly, BlueprintReadOnly, meta=(GetOptions=GetSlotIDs))
	FString SlotID;

	/**
	 * The tag that determines which items can be equipped in this slot.
	 * Only items that contain this tag can be equipped in this slot.
	 */
	UPROPERTY(SaveGame, Category="1. Required", VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag SlotCompatibleTag;

	/**
	 * The item currently equipped in this slot.
	 * This property is automatically updated when items are equipped or unequipped.
	 */
	UPROPERTY(SaveGame, Category="2. Debug", VisibleAnywhere, BlueprintReadOnly, meta=(DisplayThumbnail=false), ReplicatedUsing=OnRep_Slot)
	UMounteaInventoryItemBase* ItemInSlot = nullptr;

private:

	UPROPERTY(VisibleAnywhere, Category="2. Debug")
	int32 RepKey = 0;

	UPROPERTY(SaveGame, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false), ReplicatedUsing=OnRep_Slot)
	TScriptInterface<IMounteaEquipmentInterface> OwningEquipment = nullptr;

private:

	UFUNCTION() void OnRep_Slot();
	UFUNCTION()
	static TArray<FString> GetSlotIDs();
	
	virtual bool IsSupportedForNetworking() const override;
	void MarkDirtyForReplication();
	
#if WITH_EDITOR
protected:
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
	
};

struct FMounteaEquipmentSlotDataCompare
{
	FMounteaEquipmentSlotDataCompare(const UMounteaInventoryItemBase* Item, const FString& ID) : SlotItem(Item), SlotID(ID) {};
	
	 const UMounteaInventoryItemBase* SlotItem;
	 const FString SlotID;
};

USTRUCT(BlueprintType)
struct FMounteaEquipmentSlotData
{
	GENERATED_BODY()

public:

	/**
	 * The Equipment Slot object associated with this data structure.
	 *
	 * This property represents an instance of a `UMounteaEquipmentSlot` object that is linked to this data structure.
	 * It provides access to the equipment slot's data and functionality, such as its ID, compatible tag, and currently equipped item.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=false))
	UMounteaEquipmentSlot* Slot;

public:
	
	bool operator==(const FMounteaEquipmentSlotData& Other) const
	{
		return Slot == Other.Slot;
	}
	
	bool operator==(const FMounteaEquipmentSlotDataCompare& Other) const
	{
		if (Slot)
		{
			return Slot->GetSlotID() == Other.SlotID && Slot->GetSlotItem() == Other.SlotItem;
		}
		return false;
	}

	bool operator!=(const FMounteaEquipmentSlotData& Other) const
	{
		return !(*this == Other);
	}

	bool operator!=(const FMounteaEquipmentSlotDataCompare& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FMounteaEquipmentSlotData& Data)
	{
		uint32 Hash = 0;
		if (Data.Slot)
		{
			Hash = HashCombine(Hash, GetTypeHash(Data.Slot->GetSlotID()));
		}
				
		return Hash;
	}

};