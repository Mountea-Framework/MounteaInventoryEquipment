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
#include "Interfaces/Equipment/MounteaAdvancedQuickUseItemInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaEquipmentStatics.generated.h"

class UMounteaAdvancedEquipmentSettingsConfig;
class UMounteaAdvancedAttachmentSlot;
class UMounteaEquipmentComponent;
class IMounteaAdvancedEquipmentInterface;
class USkeletalMesh;
class UStaticMesh;

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

	/**
	 * Gets default quick-use placeholder actor class from equipment config.
	 *
	 * @return  Configured quick-use actor class (soft reference), or null soft class if not configured.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Config",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Default Quick Use Item Class")
	static TSoftClassPtr<AActor> GetDefaultQuickUseItemClass();
	
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
	 * Finds the equipment container interface on the target object or its components.
	 *
	 * @param Target  Target object to inspect (typically an Actor or Component)
	 * @return  First found equipment interface, or empty if none found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find Equipment Interface")
	static TScriptInterface<IMounteaAdvancedEquipmentInterface> FindEquipmentInterface(UObject* Target);

	/**
	 * Finds the quick-use visual interface on the target object or its components.
	 *
	 * @param Target  Target object to inspect (typically an Actor or Component)
	 * @return  First found quick-use interface, or empty if none found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Quick Use",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find Quick Use Item Interface")
	static TScriptInterface<IMounteaAdvancedQuickUseItemInterface> FindQuickUseItemInterface(UObject* Target);

	/**
	 * Assigns a static mesh to a quick-use visual instance.
	 *
	 * @param Target  Quick-use actor or component implementing quick-use interface
	 * @param NewStaticMesh  Static mesh to apply
	 * @return  True if mesh assignment succeeded
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Quick Use",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Set Quick Use Item Static Mesh")
	static bool SetQuickUseItemStaticMesh(UObject* Target, UStaticMesh* NewStaticMesh);

	/**
	 * Assigns a skeletal mesh to a quick-use visual instance.
	 *
	 * @param Target  Quick-use actor or component implementing quick-use interface
	 * @param NewSkeletalMesh  Skeletal mesh to apply
	 * @return  True if mesh assignment succeeded
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Quick Use",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Set Quick Use Item Skeletal Mesh")
	static bool SetQuickUseItemSkeletalMesh(UObject* Target, USkeletalMesh* NewSkeletalMesh);
	
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

	/**
	 * Validates whether an item can be equipped and resolves a usable target slot.
	 *
	 * If TargetSlot is null, preferred slot resolution is attempted from the item's equipment interface data.
	 *
	 * @param Outer  Object implementing attachment/equipment container behavior.
	 * @param ItemDefinition  Item to validate for equipping.
	 * @param TargetSlot  In/out resolved slot candidate.
	 * @return  True if request is valid and TargetSlot is resolved/usable.
	 */
	static bool ValidateEquipmentItemRequest(const UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot*& TargetSlot);

	/**
	 * Spawns an equipment actor from item template and attaches it into the specified slot.
	 *
	 * Also initializes equipped item GUID and initial equipment state on the spawned item interface when available.
	 *
	 * @param Outer  Attachment/equipment container context.
	 * @param ItemDefinition  Item defining actor class and payload.
	 * @param TargetSlot  Slot where actor should be attached.
	 * @param OutSpawnedActor  Spawned actor output (nullptr on failure).
	 * @return  True if spawn + attach succeeded.
	 */
	static bool CreateEquipmentItemAndAttach(UObject* Outer, const FInventoryItem& ItemDefinition, const UMounteaAdvancedAttachmentSlot* TargetSlot, 
		AActor*& OutSpawnedActor);

	/**
	 * General equip entry that validates request and uses preferred slot resolution.
	 *
	 * @param Outer  Attachment/equipment container context.
	 * @param ItemDefinition  Item to equip.
	 * @param OutSpawnedActor  Spawned actor output when a new actor was created.
	 * @return  True if equip succeeded.
	 */
	static bool EquipItemGeneral(UObject* Outer, const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor);

	/**
	 * Equip entry targeting a specific slot instance.
	 *
	 * @param Outer  Attachment/equipment container context.
	 * @param ItemDefinition  Item to equip.
	 * @param TargetSlot  Explicit slot to equip to.
	 * @param OutSpawnedActor  Spawned actor output when a new actor was created.
	 * @return  True if equip succeeded.
	 */
	static bool EquipItemToSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* TargetSlot, 
		AActor*& OutSpawnedActor);

	/**
	 * Performs final equip against an already resolved slot.
	 *
	 * Handles "already equipped" slot switching and fresh spawn/attach flow.
	 *
	 * @param Outer  Attachment/equipment container context.
	 * @param ItemDefinition  Item to equip.
	 * @param ResolvedTargetSlot  Slot previously validated/resolved.
	 * @param OutSpawnedActor  Spawned actor output when a new actor was created.
	 * @return  True if equip succeeded.
	 */
	static bool EquipItemToResolvedSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* ResolvedTargetSlot,
		AActor*& OutSpawnedActor);

	/**
	 * Validates whether an item is currently equipped, optionally in a specific slot.
	 *
	 * @param EquipmentComponent  Equipment component to query.
	 * @param ItemDefinition  Item to test.
	 * @param SlotName  Optional slot restriction (NAME_None means any slot).
	 * @return  True if equipped per provided constraints.
	 */
	static bool ValidateItemEquipped(const UMounteaEquipmentComponent* EquipmentComponent, const FInventoryItem& ItemDefinition, const FName SlotName = NAME_None);

	/**
	 * Resolves the fallback slot for the given slot from equipment settings.
	 * E.g., "RightHand" → "Back" (where item goes when deactivated).
	 *
	 * @param CurrentSlotId  The slot to look up the fallback for.
	 * @return  The fallback slot name, or NAME_None if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Resolve Fallback Slot Id")
	static FName ResolveFallbackSlotId(const FName& CurrentSlotId);

	/**
	 * Resolves the active slot for the given storage slot via reverse fallback lookup.
	 * Finds which slot uses the given slot as its fallback.
	 * E.g., "Back" → "RightHand" (if RightHand's fallback is Back).
	 *
	 * @param StorageSlotId  The storage slot to reverse-lookup.
	 * @return  The active slot name, or NAME_None if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Resolve Active Slot Id")
	static FName ResolveActiveSlotId(const FName& StorageSlotId);

	/**
	 * Finds the attachment slot where a specific equipped item lives.
	 *
	 * @param Outer  The equipment container to search.
	 * @param ItemGuid  The GUID of the item to find.
	 * @return  The slot containing the item, or nullptr if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find Slot With Equipped Item")
	static UMounteaAdvancedAttachmentSlot* FindSlotWithEquippedItem(UObject* Outer, const FGuid& ItemGuid);

	/**
	 * Switches an equipped item from one slot to another, handling occupant cleanup.
	 *
	 * @param Outer  The equipment container owning the slots.
	 * @param CurrentSlot  The slot the item is currently in.
	 * @param TargetSlot  The slot to move the item to.
	 * @return  True if the switch was successful.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Switch Equipped Item Slot")
	static bool SwitchEquippedItemSlot(UObject* Outer, UMounteaAdvancedAttachmentSlot* CurrentSlot, UMounteaAdvancedAttachmentSlot* TargetSlot);

	/**
	 * Finds slot + equipment interface pair for a specific equipped item GUID.
	 *
	 * @param Outer  Attachment/equipment container context.
	 * @param ItemGuid  GUID to find.
	 * @param OutSlot  Resolved slot output.
	 * @param OutInterface  Resolved equipment item interface output.
	 * @return  True if a matching equipped item was found.
	 */
	static bool FindEquippedItemSlotAndInterface(UObject* Outer, const FGuid& ItemGuid,
		UMounteaAdvancedAttachmentSlot*& OutSlot,
		TScriptInterface<IMounteaAdvancedEquipmentItemInterface>& OutInterface);

	/**
	 * Reads equipped item GUID from a concrete slot.
	 *
	 * @param Outer  Attachment/equipment container context.
	 * @param SlotId  Slot to inspect.
	 * @param OutItemGuid  GUID output.
	 * @return  True if slot contains valid equipment item GUID.
	 */
	static bool TryGetEquippedItemGuidFromSlot(UObject* Outer, const FName& SlotId, FGuid& OutItemGuid);

	/**
	 * Resolves inventory item definition by GUID from local inventory contexts.
	 *
	 * Search order: Outer (if inventory), owning actor (if inventory), then owning actor inventory components.
	 *
	 * @param Outer  Context object.
	 * @param ItemGuid  GUID to resolve.
	 * @param OutItemDefinition  Resolved inventory item output.
	 * @return  True if item definition was found.
	 */
	static bool TryResolveInventoryItemByGuid(UObject* Outer, const FGuid& ItemGuid, FInventoryItem& OutItemDefinition);

	/**
	 * Spawns and prepares a non-replicated quick-use placeholder actor for animation visuals.
	 *
	 * Applies item mesh via quick-use interface helpers and attaches to resolved visual slot target (or owner fallback target).
	 *
	 * @param Outer  Context object.
	 * @param ItemDefinition  Source item for mesh payload.
	 * @param VisualSlotId  Preferred visual slot for attachment.
	 * @return  Spawned placeholder actor, or nullptr if not spawned.
	 */
	static AActor* SpawnQuickUsePlaceholderActor(UObject* Outer, const FInventoryItem& ItemDefinition, const FName& VisualSlotId);

	/**
	 * Ensures target slot can accept a new equip operation.
	 *
	 * If occupied, existing attachment is detached and owning actor is destroyed.
	 *
	 * @param Outer  Attachment/equipment container context.
	 * @param TargetSlot  Slot to prepare.
	 * @return  True if slot is ready for equip.
	 */
	static bool EnsureSlotIsReadyForEquip(UObject* Outer, const UMounteaAdvancedAttachmentSlot* TargetSlot);

	/**
	 * Resolves an owning actor from an attachment object (actor or component).
	 *
	 * @param AttachmentObject  Attachment instance.
	 * @return  Owning actor or nullptr.
	 */
	static AActor* ResolveAttachmentActor(UObject* AttachmentObject);

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
		meta=(CustomTag="MounteaK2Setter,MounteaK2EquipmentSlot"),
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
		meta=(CustomTag="MounteaK2Validate,MounteaK2EquipmentSlot"),
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

	/**
	 * Activates the specified equipped item, transitioning it from Equipped to Active state.
	 * May play an activation animation if configured on the item.
	 *
	 * @param Target  The target implementing the equipment interface.
	 * @param ItemDefinition  The item to activate.
	 * @param TargetSlotId  Optional target slot to activate into. If None, resolved automatically.
	 * @return  True if activation was initiated, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter,MounteaK2EquipmentSlot"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(AutoCreateRefTerm="TargetSlotId"),
		DisplayName="Activate Equipment Item")
	static bool ActivateEquipmentItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& TargetSlotId);

	/**
	 * Activates a Quick Use item in the specified slot for a given target that implements the Mountea Advanced Equipment Interface.
	 * Ensures that the provided target is valid before delegating the activation process.
	 *
	 * @param Target - The target object implementing the IMounteaAdvancedEquipmentInterface to activate the Quick Use item on.
	 * @param SlotId - The name of the slot from which the Quick Use item will be activated.
	 * @param TargetSlotId - The identifier of the target slot that may influence the activation process.
	 * @return - Returns true if the activation succeeds, otherwise returns false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter,MounteaK2EquipmentSlot"),
		meta=(MounteaK2AllowedItemType_SlotId="Mountea_Inventory.Equipment.ItemType.QuickUse"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(AutoCreateRefTerm="SlotId,TargetSlotId"),
		DisplayName="Activate Quick Use Item")
	static bool ActivateQuickUseItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FName& SlotId, const FName& TargetSlotId);

	/**
	 * Deactivates the specified active item, transitioning it from Active to Equipped state.
	 * Moves the item back to its fallback/storage slot.
	 *
	 * @param Target  The target implementing the equipment interface.
	 * @param ItemDefinition  The item to deactivate.
	 * @param TargetSlotId  Optional target slot to deactivate into. If None, resolved automatically via fallback.
	 * @return  True if deactivation was initiated, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter,MounteaK2EquipmentSlot"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(AutoCreateRefTerm="TargetSlotId"),
		DisplayName="Deactivate Equipment Item")
	static bool DeactivateEquipmentItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& TargetSlotId);

	/**
	 * Commits pending equipment transition requested by animation notify.
	 *
	 * Typically consumed by equip/deequip anim notify to finalize deferred slot/state transition.
	 *
	 * @param Target  Target implementing equipment interface.
	 * @return  True if notify handling succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Anim Attach Item")
	static bool AnimAttachItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target);

	/**
	 * Commits quick-use animation notify workflow.
	 *
	 * Typically consumed by quick-use anim notify to finalize/destroy temporary placeholder visuals.
	 *
	 * @param Target  Target implementing equipment interface.
	 * @return  True if notify handling succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Anim Quick Item Used")
	static bool AnimQuickItemUsed(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target);

#pragma endregion 

#if WITH_EDITOR
	UFUNCTION(BlueprintInternalUseOnly)
	bool Prototype_EquipItem(const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor) { return false; };
#endif
};
