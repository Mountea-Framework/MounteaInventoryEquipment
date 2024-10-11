// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Helpers/MounteaEquipmentDataTypes.h"
#include "MounteaEquipmentSlot.generated.h"

class IMounteaEquipmentInterface;
class UMounteaInventoryItem;

#define LOCTEXT_NAMESPACE "EquipmentItem"

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

/**
 * Mountea Equipment Slot.
 *
 * Represents an equipment slot within the Mountea Inventory & Equipment system.
 * Equipment slots define where items can be equipped, including compatibility tags, default state, and attachment data.
 * This class allows for flexible equipment management, enabling features like item equipping, slot state management, and networking replication.
 *
 * @see UDataAsset
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea,Inventory"), DisplayName="Equipment Slot")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentSlot : public UDataAsset
{
	GENERATED_BODY()

public:

	/**
	 * Returns the display name of this slot.
	 *
	 * @return The localized display name of the slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FText GetSlotName() const { return SlotName; };

	/**
	 * Returns tags that define which items are compatible with this slot.
	 * Only items that match these tags can be equipped.
	 *
	 * @return The gameplay tag container with the slot's compatible tags.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FGameplayTagContainer GetSlotTags() const { return SlotCompatibleTags; };

	/**
	 * Returns the item currently equipped in this slot.
	 *
	 * @return The inventory item currently in the slot, or nullptr if empty.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE UMounteaInventoryItem* GetSlotItem() const { return ItemInSlot; };

	/**
	 * Returns the current state of the equipment slot.
	 *
	 * @return The equipment slot state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE EEquipmentSlotState GetSlotState() const { return SlotState; };

	/**
	 * Returns the unique identifier for this equipment slot.
	 *
	 * @return The GUID of the slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FGuid GetSlotGuid() const { return SlotGuid; };
	
	/**
	 * Determines if the equipment slot is currently empty.
	 *
	 * Checks whether there is an item equipped in this slot.
	 * Returns true if the slot is empty (i.e., there is no item equipped), and false otherwise.
	 *
	 * @return True if the slot is empty; false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot", meta=(CustomTag="MounteaK2Validate"))
	FORCEINLINE bool IsEmpty() const { return ItemInSlot == nullptr; };
	
protected:

#pragma region REQUIRED

	/**
	 * The unique name for this slot.
	 * This is the display name of the slot.
	 *
	 * 📖 LOCALIZABLE
	 */
	UPROPERTY(SaveGame, Category="Required", EditAnywhere, BlueprintReadOnly, meta=(GetOptions=GetSlotNames))
	FText SlotName;

	/**
	 * Tags that define which items are compatible with this slot.
	 * Only items that match these tags can be equipped.
	 */
	UPROPERTY(SaveGame, Category="Required", EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer SlotCompatibleTags;

	/**
	 * The default state of this equipment slot.
	 *
	 * ❗ Occupied is not allowed.
	 */
	UPROPERTY(SaveGame, Category="Required", EditAnywhere, BlueprintReadOnly)
	EEquipmentSlotState DefaultSlotState;

	/**
	 * Data that defines how the equipment item is attached to this slot.
	 */
	UPROPERTY(SaveGame, Category="Required", EditAnywhere, BlueprintReadOnly)
	FEquipmentAttachmentData EquipmentAttachmentData;

#pragma endregion

#pragma region READONLY

	/** The current state of the equipment slot. */
	UPROPERTY(SaveGame, Category="2. Read Only", VisibleAnywhere, BlueprintReadOnly)
	EEquipmentSlotState SlotState;

	/**
	 * The item currently equipped in this slot.
	 * This property is automatically updated when items are equipped or unequipped.
	 */
	UPROPERTY(SaveGame, Category="2. Read Only", VisibleAnywhere, BlueprintReadOnly, meta=(DisplayThumbnail=false))
	TObjectPtr<UMounteaInventoryItem> ItemInSlot = nullptr;

	/**
	 * The unique identifier for this equipment slot.
	 * This ID is used to distinguish this slot from others in an equipment system.
	 * It can be customized in the editor.
	 */
	UPROPERTY(SaveGame, Category="2. Read Only", VisibleAnywhere, BlueprintReadOnly)
	FGuid SlotGuid;

private:

	/** The owning equipment interface for this slot. */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="2. Read Only", meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaEquipmentInterface> OwningEquipment = nullptr;

#pragma endregion

private:
	
	UFUNCTION()
	static TArray<FString> GetSlotNames();
	
	virtual bool IsSupportedForNetworking() const override { return true; };
	
#if WITH_EDITOR
protected:
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) override;
	
#endif
	
};

#undef LOCTEXT_NAMESPACE