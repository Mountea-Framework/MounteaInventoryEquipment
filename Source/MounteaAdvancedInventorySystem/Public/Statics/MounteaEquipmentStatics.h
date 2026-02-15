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
	
	static bool ValidateEquipmentItemRequest(const UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* preferredSlot, 
		bool& bValue);

#pragma endregion

	/**
	 * Attempts to equip an item to the given Outer equipment based on the item's definition.
	 * Validates the Outer object, the item definition, and determines the appropriate equipment slot.
	 * If successful, spawns the specified actor and assigns it to the desired slot.
	 *
	 * @param Outer            The target object, which must implement either the
	 *                         MounteaAdvancedAttachmentContainerInterface or MounteaAdvancedEquipmentInterface.
	 * @param ItemDefinition   The definition of the item to be equipped. Includes data such as the item's template and spawn settings.
	 * @param OutSpawnedActor  Output reference to the spawned actor, if the equip operation is successful.
	 *
	 * @return                 True if the item was successfully equipped, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Equip Item")
	static bool EquipItem(UObject* Outer, const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor);

	/**
	 * Equips an inventory item to a specified attachment slot within the Mountea Equipment System.
	 * This function spawns an actor for the provided item definition and attaches it to the specified slot.
	 *
	 * @param Outer              The outer context for the spawned actor, typically an owning object or actor.
	 * @param ItemDefinition     The definition of the inventory item to be equipped.
	 * @param TargetSlot         The target attachment slot where the item should be equipped.
	 * @param OutSpawnedActor    Reference to the spawned actor, representing the equipped item.
	 *                           This is set to nullptr if no actor is spawned.
	 *
	 * @return Returns true if the item was successfully equipped; false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Equip Item To Slot")
	static bool EquipItemToSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* TargetSlot, 
		AActor*& OutSpawnedActor);

#if WITH_EDITOR
	UFUNCTION(BlueprintInternalUseOnly)
	bool Prototype_EquipItem(const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor) { return false; };
#endif
};
