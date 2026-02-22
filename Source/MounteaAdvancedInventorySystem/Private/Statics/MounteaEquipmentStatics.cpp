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


#include "Statics/MounteaEquipmentStatics.h"

#include "Algo/Find.h"
#include "Components/ActorComponent.h"
#include "Components/MounteaEquipmentComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaAttachmentsStatics.h"

UMounteaAdvancedEquipmentSettingsConfig* UMounteaEquipmentStatics::GetEquipmentSettingsConfig()
{
	const auto* inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	return inventorySettings ? inventorySettings->AdvancedEquipmentSettingsConfig.LoadSynchronous() : nullptr;
}

TSet<TSoftClassPtr<USceneComponent>> UMounteaEquipmentStatics::GetAllowedAttachmentTargets()
{
	if (const auto* equipmentConfig = GetEquipmentSettingsConfig())
		return equipmentConfig->AllowedAttachmentTargets;

	TSet<TSoftClassPtr<USceneComponent>> returnValue;

	returnValue.Add(TSoftClassPtr<USceneComponent>(USceneComponent::StaticClass()));
	returnValue.Add(TSoftClassPtr<USceneComponent>(UStaticMeshComponent::StaticClass()));
	returnValue.Add(TSoftClassPtr<USceneComponent>(USkeletalMeshComponent::StaticClass()));

	return returnValue;
}

TScriptInterface<IMounteaAdvancedEquipmentItemInterface> UMounteaEquipmentStatics::FindEquipmentItemInterface(UObject* Target)
{
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> result;

	if (!IsValid(Target))
		return result;

	if (Target->GetClass()->ImplementsInterface(UMounteaAdvancedEquipmentItemInterface::StaticClass()))
	{
		result.SetObject(Target);
		result.SetInterface(Cast<IMounteaAdvancedEquipmentItemInterface>(Target));
		return result;
	}

	AActor* actor = Cast<AActor>(Target);
	if (!IsValid(actor))
		return result;

	const TArray<UActorComponent*> components = actor->GetComponentsByInterface(
		UMounteaAdvancedEquipmentItemInterface::StaticClass());
	if (components.Num() > 0)
	{
		UActorComponent* component = components[0];
		result.SetObject(component);
		result.SetInterface(Cast<IMounteaAdvancedEquipmentItemInterface>(component));
		return result;
	}

	const UBlueprintGeneratedClass* blueprintClass = Cast<UBlueprintGeneratedClass>(actor->GetClass());
	if (!blueprintClass || !blueprintClass->SimpleConstructionScript)
		return result;

	for (const USCS_Node* node : blueprintClass->SimpleConstructionScript->GetAllNodes())
	{
		if (!node)
			continue;

		const UActorComponent* componentTemplate = node->GetActualComponentTemplate(
			const_cast<UBlueprintGeneratedClass*>(blueprintClass));
		if (IsValid(componentTemplate) && componentTemplate->GetClass()->ImplementsInterface(
			UMounteaAdvancedEquipmentItemInterface::StaticClass()))
		{
			UActorComponent* mutableTemplate = const_cast<UActorComponent*>(componentTemplate);
			result.SetObject(mutableTemplate);
			result.SetInterface(Cast<IMounteaAdvancedEquipmentItemInterface>(mutableTemplate));
			return result;
		}
	}

	return result;
}

FName UMounteaEquipmentStatics::ResolveFallbackSlotId(const FName& CurrentSlotId)
{
	const UMounteaAdvancedEquipmentSettingsConfig* settingsConfig = GetEquipmentSettingsConfig();
	if (!settingsConfig)
		return NAME_None;

	const FMounteaEquipmentSlotHeaderData* slotData = settingsConfig->AllowedEquipmentSlots.Find(CurrentSlotId);
	if (!slotData)
		return NAME_None;

	return slotData->FallbackSlot;
}

FName UMounteaEquipmentStatics::ResolveActiveSlotId(const FName& StorageSlotId)
{
	const UMounteaAdvancedEquipmentSettingsConfig* settingsConfig = GetEquipmentSettingsConfig();
	if (!settingsConfig)
		return NAME_None;

	for (const auto& [slotId, slotData] : settingsConfig->AllowedEquipmentSlots)
	{
		if (slotData.FallbackSlot == StorageSlotId)
			return slotId;
	}

	return NAME_None;
}

UMounteaAdvancedAttachmentSlot* UMounteaEquipmentStatics::FindSlotWithEquippedItem(UObject* Outer, const FGuid& ItemGuid)
{
	if (!IsValid(Outer) || !ItemGuid.IsValid())
		return nullptr;

	const TArray<UMounteaAdvancedAttachmentSlot*> slots = IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentSlots(Outer);
	for (UMounteaAdvancedAttachmentSlot* slot : slots)
	{
		if (!IsValid(slot) || !slot->IsOccupied() || !IsValid(slot->Attachment))
			continue;

		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> iface = FindEquipmentItemInterface(slot->Attachment);
		if (!iface.GetObject())
			continue;

		if (IMounteaAdvancedEquipmentItemInterface::Execute_GetEquippedItemId(iface.GetObject()) == ItemGuid)
			return slot;
	}

	return nullptr;
}

bool UMounteaEquipmentStatics::FindEquippedItemSlotAndInterface(UObject* Outer, const FGuid& ItemGuid,
	UMounteaAdvancedAttachmentSlot*& OutSlot, TScriptInterface<IMounteaAdvancedEquipmentItemInterface>& OutInterface)
{
	OutSlot = nullptr;
	OutInterface = TScriptInterface<IMounteaAdvancedEquipmentItemInterface>();

	if (!IsValid(Outer) || !ItemGuid.IsValid())
		return false;

	const TArray<UMounteaAdvancedAttachmentSlot*> slots = IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentSlots(Outer);
	for (UMounteaAdvancedAttachmentSlot* slot : slots)
	{
		if (!IsValid(slot) || !slot->IsOccupied() || !IsValid(slot->Attachment))
			continue;

		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> iface = FindEquipmentItemInterface(slot->Attachment);
		if (!iface.GetObject())
			continue;

		if (IMounteaAdvancedEquipmentItemInterface::Execute_GetEquippedItemId(iface.GetObject()) == ItemGuid)
		{
			OutSlot = slot;
			OutInterface = iface;
			return true;
		}
	}

	return false;
}

bool UMounteaEquipmentStatics::SwitchEquippedItemSlot(UObject* Outer, UMounteaAdvancedAttachmentSlot* CurrentSlot, UMounteaAdvancedAttachmentSlot* TargetSlot)
{
	if (!IsValid(Outer) || !IsValid(CurrentSlot) || !IsValid(TargetSlot))
		return false;

	if (CurrentSlot == TargetSlot)
		return false;

	if (!CurrentSlot->IsOccupied() || !IsValid(CurrentSlot->Attachment))
		return false;

	// Clear target slot if occupied
	if (TargetSlot->IsOccupied())
	{
		if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryDetach(Outer, TargetSlot->SlotName))
			return false;

		AActor* occupantActor = ResolveAttachmentActor(TargetSlot->Attachment);
		if (IsValid(occupantActor))
			occupantActor->Destroy();
	}

	UObject* attachmentObject = CurrentSlot->Attachment;
	if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryDetach(Outer, CurrentSlot->SlotName))
		return false;

	if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryAttach(Outer, TargetSlot->SlotName, attachmentObject))
	{
		// Rollback: re-attach to original slot
		IMounteaAdvancedAttachmentContainerInterface::Execute_TryAttach(Outer, CurrentSlot->SlotName, attachmentObject);
		return false;
	}

	return true;
}

bool UMounteaEquipmentStatics::EnsureSlotIsReadyForEquip(UObject* Outer, const UMounteaAdvancedAttachmentSlot* TargetSlot)
{
	if (!IsValid(Outer) || !IsValid(TargetSlot))
		return false;

	if (!TargetSlot->IsOccupied())
		return true;

	if (!IsValid(TargetSlot->Attachment))
		return false;

	if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryDetach(Outer, TargetSlot->SlotName))
		return false;

	AActor* occupantActor = ResolveAttachmentActor(TargetSlot->Attachment);
	if (IsValid(occupantActor))
		occupantActor->Destroy();

	return true;
}

AActor* UMounteaEquipmentStatics::ResolveAttachmentActor(UObject* AttachmentObject)
{
	if (AActor* attachmentActor = Cast<AActor>(AttachmentObject))
		return attachmentActor;

	if (const UActorComponent* attachmentComponent = Cast<UActorComponent>(AttachmentObject))
		return attachmentComponent->GetOwner();

	return nullptr;
}

bool UMounteaEquipmentStatics::IsTargetClassValid(const UClass* TargetClass)
{
	if (!UMounteaAttachmentsStatics::IsTargetClassValid(TargetClass))
		return false;
	
	if (!IsValid(TargetClass))
		return false;

	if (TargetClass->ImplementsInterface(UMounteaAdvancedEquipmentItemInterface::StaticClass()))
		return true;

	const AActor* targetActor = Cast<AActor>(TargetClass->GetDefaultObject());
	if (!IsValid(targetActor))
		return false;

	const TArray<UActorComponent*> components = targetActor->GetComponentsByInterface(
		UMounteaAdvancedEquipmentItemInterface::StaticClass());
	if (components.Num() > 0)
		return true;

	const UBlueprintGeneratedClass* blueprintClass = Cast<UBlueprintGeneratedClass>(TargetClass);
	if (!blueprintClass || !blueprintClass->SimpleConstructionScript)
		return false;

	for (const USCS_Node* node : blueprintClass->SimpleConstructionScript->GetAllNodes())
	{
		if (!node)
			continue;

		const UActorComponent* componentTemplate = node->GetActualComponentTemplate(
			const_cast<UBlueprintGeneratedClass*>(blueprintClass));
		if (IsValid(componentTemplate) && componentTemplate->GetClass()->ImplementsInterface(
			UMounteaAdvancedEquipmentItemInterface::StaticClass()))
			return true;
	}

	return false;
}

bool UMounteaEquipmentStatics::ValidateEquipmentItemRequest(const UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot*& TargetSlot)
{
	if (!IsValid(Outer))
		return false;

	if (!IsValid(Outer->GetWorld()))
		return false;
	
	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>() && !Outer->Implements<UMounteaAdvancedEquipmentInterface>())
		return false;

	if (!ItemDefinition.IsItemValid())
		return false;

	const auto spawnClass = ItemDefinition.GetTemplate()->SpawnActor.LoadSynchronous();
	if (!IsTargetClassValid(spawnClass))
		return false;

	const auto defaultActor = spawnClass->GetDefaultObject<AActor>();
	if (!IsValid(defaultActor))
		return false;

	if (!IsValid(TargetSlot))
	{
		const auto equipmentInterface = FindEquipmentItemInterface(defaultActor);
		const FName preferredSlotName = equipmentInterface.GetObject() ? 
			IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemPreferredSlot(equipmentInterface.GetObject()) : 
			NAME_None;
	
		const auto preferredSlotTag = equipmentInterface.GetObject() ?
			IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentPreferredSlotTag(equipmentInterface.GetObject()) :
			FGameplayTag();
	
		if (preferredSlotName == NAME_None && !preferredSlotTag.IsValid())
			return false;

		TargetSlot = IMounteaAdvancedAttachmentContainerInterface::Execute_IsValidSlot(Outer, preferredSlotName) ? 
							IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, preferredSlotName) :
							IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, 
								IMounteaAdvancedAttachmentContainerInterface::Execute_FindFirstFreeSlotWithTags(Outer, FGameplayTagContainer(preferredSlotTag)));
	}

	if (!IsValid(TargetSlot) || !TargetSlot->CanAttach())
		return false;
	
	return true;
}

bool UMounteaEquipmentStatics::CreateEquipmentItemAndAttach(UObject* Outer, const FInventoryItem& ItemDefinition, const UMounteaAdvancedAttachmentSlot* TargetSlot, 
	AActor*& OutSpawnedActor)
{
	OutSpawnedActor = nullptr;
	if (!IsValid(ItemDefinition.GetTemplate()))
		return false;
	if (!IsValid(Outer) || !IsValid(Outer->GetWorld()))
		return false;
	if (!IsValid(TargetSlot))
		return false;
	
	const auto classToSpawn = ItemDefinition.GetTemplate()->SpawnActor.LoadSynchronous();
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	OutSpawnedActor = Outer->GetWorld()->SpawnActor<AActor>(classToSpawn, FTransform(), spawnParams);

	if (IsValid(OutSpawnedActor) && ItemDefinition.GetGuid().IsValid())
	{
		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipmentItemInterface = FindEquipmentItemInterface(OutSpawnedActor);
		if (equipmentItemInterface.GetObject())
		{
			IMounteaAdvancedEquipmentItemInterface::Execute_SetEquippedItemId(
				equipmentItemInterface.GetObject(),
				ItemDefinition.GetGuid()
			);
		}
	}
	
	if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryAttach(Outer, TargetSlot->SlotName, OutSpawnedActor))
	{
		OutSpawnedActor->Destroy();
		OutSpawnedActor = nullptr;
		return false;
	}

	// Set initial equipment state after successful attach
	if (IsValid(OutSpawnedActor))
	{
		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface = FindEquipmentItemInterface(OutSpawnedActor);
		if (equipItemInterface.GetObject())
		{
			const bool bAutoActivate = IMounteaAdvancedEquipmentItemInterface::Execute_DoesAutoActive(equipItemInterface.GetObject());
			const EEquipmentItemState initialState = bAutoActivate ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped;
			IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), initialState);
		}
	}

	return IsValid(OutSpawnedActor);
}

bool UMounteaEquipmentStatics::EquipItemGeneral(UObject* Outer, const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor)
{
	OutSpawnedActor = nullptr;

	UMounteaAdvancedAttachmentSlot* preferredSlot = nullptr;
	if (!ValidateEquipmentItemRequest(Outer, ItemDefinition, preferredSlot))
		return false;

	if (IMounteaAdvancedEquipmentInterface::Execute_IsEquipmentItemEquipped(Outer, ItemDefinition))
	{
		UMounteaAdvancedAttachmentSlot* currentSlot = FindSlotWithEquippedItem(Outer, ItemDefinition.GetGuid());
		if (!IsValid(currentSlot) || currentSlot == preferredSlot)
			return false;

		return SwitchEquippedItemSlot(Outer, currentSlot, preferredSlot);
	}

	if (!EnsureSlotIsReadyForEquip(Outer, preferredSlot))
		return false;

	return CreateEquipmentItemAndAttach(Outer, ItemDefinition, preferredSlot, OutSpawnedActor);
}

bool UMounteaEquipmentStatics::EquipItemToSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* TargetSlot,
	AActor*& OutSpawnedActor)
{
	if (!ValidateEquipmentItemRequest(Outer, ItemDefinition, TargetSlot))
		return false;

	if (IMounteaAdvancedEquipmentInterface::Execute_IsEquipmentItemEquipped(Outer, ItemDefinition))
	{
		UMounteaAdvancedAttachmentSlot* currentSlot = FindSlotWithEquippedItem(Outer, ItemDefinition.GetGuid());
		if (!IsValid(currentSlot) || currentSlot == TargetSlot)
			return false;

		return SwitchEquippedItemSlot(Outer, currentSlot, TargetSlot);
	}

	if (!EnsureSlotIsReadyForEquip(Outer, TargetSlot))
		return false;

	return CreateEquipmentItemAndAttach(Outer, ItemDefinition, TargetSlot, OutSpawnedActor);
}

bool UMounteaEquipmentStatics::ValidateItemEquipped(const UMounteaEquipmentComponent* EquipmentComponent, const FInventoryItem& ItemDefinition, const FName SlotName)
{
	if (!IsValid(EquipmentComponent) || !ItemDefinition.IsItemValid())
		return false;

	FGuid expectedItemGuid = ItemDefinition.GetGuid();
	if (!expectedItemGuid.IsValid())
		return false;

	const auto resolveExpectedItemGuid = [&]() -> bool
	{
		const auto inventoryManager = ItemDefinition.GetOwningInventory();
		if (inventoryManager.GetObject() == nullptr || !inventoryManager.GetObject()->Implements<UMounteaAdvancedInventoryInterface>())
			return false;

		const auto itemData = IMounteaAdvancedInventoryInterface::Execute_FindItem(
			inventoryManager.GetObject(),
			FInventoryItemSearchParams(ItemDefinition.GetGuid())
		);
		if (!itemData.IsItemValid())
			return false;

		expectedItemGuid = itemData.GetGuid();
		return expectedItemGuid.IsValid();
	};

	if (!resolveExpectedItemGuid())
		return false;

	const auto hasMatchingEquippedGuid = [&](const UMounteaAdvancedAttachmentSlot* Slot) -> bool
	{
		if (!IsValid(Slot) || !Slot->IsOccupied() || !IsValid(Slot->Attachment))
			return false;

		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipmentItemInterface = FindEquipmentItemInterface(Slot->Attachment);
		if (!equipmentItemInterface.GetObject())
			return false;

		return IMounteaAdvancedEquipmentItemInterface::Execute_GetEquippedItemId(equipmentItemInterface.GetObject()) == expectedItemGuid;
	};

	if (SlotName != NAME_None)
	{
		const UMounteaAdvancedAttachmentSlot* foundSlot = IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(EquipmentComponent, SlotName);
		return hasMatchingEquippedGuid(foundSlot);
	}

	const TArray<UMounteaAdvancedAttachmentSlot*> attachmentSlots = IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentSlots(EquipmentComponent);
	if (attachmentSlots.IsEmpty())
		return false;

	return Algo::FindByPredicate(attachmentSlots, hasMatchingEquippedGuid) != nullptr;
}

bool UMounteaEquipmentStatics::EquipItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition)
{
	return IMounteaAdvancedEquipmentInterface::Execute_EquipItem(Target.GetObject(), ItemDefinition) != nullptr;
}

bool UMounteaEquipmentStatics::EquipItemToSlot(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target,
	const FInventoryItem& ItemDefinition, const FName& SlotName)
{
	return IMounteaAdvancedEquipmentInterface::Execute_EquipItemToSlot(Target.GetObject(), SlotName, ItemDefinition) != nullptr;
}

bool UMounteaEquipmentStatics::IsItemEquipped(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target,
                                              const FInventoryItem& ItemDefinition)
{
	return IMounteaAdvancedEquipmentInterface::Execute_IsEquipmentItemEquipped(Target.GetObject(), ItemDefinition);
}

bool UMounteaEquipmentStatics::IsItemEquippedInSlot(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, 
	const FInventoryItem& ItemDefinition, const FName& SlotName)
{
	return IMounteaAdvancedEquipmentInterface::Execute_IsEquipmentItemEquippedInSlot(Target.GetObject(), ItemDefinition, SlotName);
}

bool UMounteaEquipmentStatics::UnequipItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, bool bUseFallbackSlot)
{
	return IMounteaAdvancedEquipmentInterface::Execute_UnequipItem(Target.GetObject(), ItemDefinition, bUseFallbackSlot);
}

bool UMounteaEquipmentStatics::ActivateEquipmentItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_ActivateEquipmentItem(Target.GetObject(), ItemDefinition, TargetSlotId);
}

bool UMounteaEquipmentStatics::DeactivateEquipmentItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_DeactivateEquipmentItem(Target.GetObject(), ItemDefinition, TargetSlotId);
}
