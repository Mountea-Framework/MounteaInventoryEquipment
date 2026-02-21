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
#include "Definitions/MounteaInventoryItem.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentItemInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaEquipmentStatics.generated.h"

class UMounteaAdvancedEquipmentSettingsConfig;
class UMounteaAdvancedAttachmentSlot;
class UMounteaEquipmentComponent;
class IMounteaAdvancedEquipmentInterface;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaEquipmentStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
#pragma region Settings

	/**
	 * Retrieves the advanced configuration settings for the Mountea Equipment System.
	 * Facilitates access to equipment-specific configurations like equipment slots, allowed attachment targets etc..
	 *
	 * @return - A pointer to the Mountea Advanced Equipment Settings configuration object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Equipment Settings Config")
	static UMounteaAdvancedEquipmentSettingsConfig* GetEquipmentSettingsConfig();

	/**
	 * Retrieves the set of scene component classes that are permitted as attachment targets
	 * for the Mountea Equipment System. This information is defined in the advanced equipment
	 * settings configuration and determines which components can be valid attachment targets
	 * for equippable items.
	 *
	 * If no configuration is found, a default set of allowed attachment targets including
	 * USceneComponent, UStaticMeshComponent, and USkeletalMeshComponent will be returned.
	 *
	 * @return - A set of soft class pointers representing allowed attachment target classes for equipment.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Equipment Settings Config")
	static TSet<TSoftClassPtr<USceneComponent>> GetAllowedAttachmentTargets();
	
#pragma endregion

#pragma region Helpers
	
	/**
	 * Finds the equipment item interface on the target object or its components.
	 *
	 * @param Target  Target object to inspect (typically an Actor or Component)
	 * @return  First found equipment item interface, or empty if none found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find Equipment Item Interface")
	static TScriptInterface<IMounteaAdvancedEquipmentItemInterface> FindEquipmentItemInterface(UObject* Target);
	
	/**
	 * Checks if the specified class is valid to be equipment item.
	 * 
	 * @param TargetClass  Target class to validate for being equipment item
	 * @return  True if the class is valid, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Equipment",
		meta=(CustomTag="MounteaK2Validate"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Is Valid Equipment Item Class")
	static bool IsTargetClassValid(const UClass* TargetClass);
	
	static bool ValidateEquipmentItemRequest(const UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot*& TargetSlot);
	
	static bool CreateEquipmentItemAndAttach(UObject* Outer, const FInventoryItem& ItemDefinition, const UMounteaAdvancedAttachmentSlot* TargetSlot, 
		AActor*& OutSpawnedActor);
	
	static bool EquipItemGeneral(UObject* Outer, const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor);
	
	static bool EquipItemToSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* TargetSlot, 
		AActor*& OutSpawnedActor);
	
	static bool ValidateItemEquipped(const UMounteaEquipmentComponent* EquipmentComponent, const FInventoryItem& ItemDefinition, FName SlotName = NAME_None);

#pragma endregion
	
#pragma region Equipment

	/**
	 * Equips the specified item on the provided target that implements the Mountea Advanced Equipment Interface.
	 * This utility function enables equipping items by leveraging the interface's internal logic.
	 *
	 * @param Target  The target object that implements the Mountea Advanced Equipment Interface. Typically an Actor or Component.
	 * @param ItemDefinition  The definition of the inventory item to be equipped.
	 * @return  True if the item was successfully equipped, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Equip Item")
	static bool EquipItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition);

	/**
	 * Attempts to equip the specified inventory item to a designated equipment slot on the target object.
	 *
	 * @param Target - The target object implementing the Mountea Advanced Equipment Interface, which will receive the item.
	 * @param ItemDefinition - The definition of the inventory item to be equipped to the specified slot.
	 * @param SlotName - The name of the equipment slot where the item should be equipped.
	 * @return - True if the item was successfully equipped to the slot, otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(AutoCreateRefTerm="SlotName"),
		DisplayName="Equip Item to Slot")
	static bool EquipItemToSlot(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& SlotName);

	/**
	 * Determines if a specified inventory item is currently equipped on a target object
	 * that implements the Mountea Advanced Equipment Interface.
	 *
	 * @param Target  The target object implementing the Mountea Advanced Equipment Interface, typically an Actor or Component.
	 * @param ItemDefinition  The definition of the inventory item to check for an equipped state.
	 * @return  True if the item is equipped on the target, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Is Item Equipped")
	static bool IsItemEquipped(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition);

	/**
	 * Checks if a specified inventory item is currently equipped in a specific slot on a target object.
	 * The target object must implement the Mountea Advanced Equipment Interface.
	 *
	 * @param Target  The target object implementing the Mountea Advanced Equipment Interface; typically an Actor or Component.
	 * @param ItemDefinition  The definition of the inventory item whose equipped status is to be checked.
	 * @param SlotName  The name of the equipment slot to check for the specified item.
	 * @return  True if the item is equipped in the specified slot on the target, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Validate"),
		meta=(AutoCreateRefTerm="SlotName"),
		DisplayName="Is Item Equipped In Slot")
	static bool IsItemEquippedInSlot(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& SlotName);

	/**
	 * Unequips the specified inventory item from the provided target that implements the Mountea Advanced Equipment Interface.
	 * This function utilizes the interface's internal logic to handle unequipping, including fallback options if necessary.
	 *
	 * @param Target  The target object implementing the Mountea Advanced Equipment Interface, typically an Actor or Component.
	 * @param ItemDefinition  The definition of the inventory item to be unequipped.
	 * @param bUseFallbackSlot  Indicates whether a fallback equipment slot should be used if the item's equipped slot cannot be determined.
	 * @return  True if the item was successfully unequipped, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Unequip Item")
	static bool UnequipItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, bool bUseFallbackSlot = true);
	
#pragma endregion 

#if WITH_EDITOR
	UFUNCTION(BlueprintInternalUseOnly)
	bool Prototype_EquipItem(const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor) { return false; };
#endif
};
