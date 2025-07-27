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

#include "Decorations/Templates/MounteaEquipItemAction.h"

#include "Components/MounteaAttachableComponent.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

UMounteaEquipItemAction::UMounteaEquipItemAction()
{
	ItemActionData.ActionDisplayName = FText::FromString(TEXT("Equip"));
	ItemActionData.ActionDescription = FText::FromString(TEXT("Equip this item"));
	ItemActionData.ActionPriority = 200;
	ItemActionData.bRequiresConfirmation = false;
}

bool UMounteaEquipItemAction::IsActionVisible_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	const UMounteaInventoryItemTemplate* itemTemplate = TargetItem.GetTemplate();
	if (!itemTemplate)
		return false;

	return (itemTemplate->ItemFlags & static_cast<uint8>(EInventoryItemFlags::EIIF_Consumable)) == 0;
}

bool UMounteaEquipItemAction::IsAllowed_Implementation(const FInventoryItem& TargetItem) const
{
	if (!Super::IsAllowed_Implementation(TargetItem))
		return false;

	const UMounteaInventoryItemTemplate* itemTemplate = TargetItem.GetTemplate();
	if (!itemTemplate || !itemTemplate->SpawnActor.IsValid())
		return false;

	const auto equipmentInterface = GetEquipmentInterface(GetAvatarActorFromActorInfo());
	if (!equipmentInterface)
		return false;

	return true;
}

// TODO: Do not spawn actor unless needed (might already exist in different slot)
bool UMounteaEquipItemAction::ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem)
{
	return false;
	/*
	const UMounteaInventoryItemTemplate* itemTemplate = TargetItem.GetTemplate();
	if (!itemTemplate)
		return false;

	TSoftClassPtr<AActor> spawnActorClass = itemTemplate->SpawnActor;
	if (!spawnActorClass.IsValid())
	{
		LOG_WARNING(TEXT("[EquipItemAction] No spawn actor defined for item %s"), *TargetItem.GetGuid().ToString())
		return false;
	}

	const auto equipmentInterface = GetEquipmentInterface(ActionInitiator);
	if (!equipmentInterface)
	{
		LOG_ERROR(TEXT("[EquipItemAction] No equipment interface found"))
		return false;
	}

	AActor* owningActor = equipmentInterface->Execute_GetOwningActor(equipmentInterface.GetObject());
	if (!owningActor || !owningActor->GetWorld())
		return false;

	UClass* actorClass = spawnActorClass.LoadSynchronous();
	if (!actorClass)
	{
		LOG_ERROR(TEXT("[EquipItemAction] Failed to load spawn actor class"))
		return false;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = owningActor;
	spawnParams.Instigator = owningActor->GetInstigator();

	AActor* spawnedActor = owningActor->GetWorld()->SpawnActor<AActor>(actorClass, spawnParams);
	if (!spawnedActor)
	{
		LOG_ERROR(TEXT("[EquipItemAction] Failed to spawn actor"))
		return false;
	}

	UMounteaAttachableComponent* attachableComponent = spawnedActor->FindComponentByClass<UMounteaAttachableComponent>();
	if (!attachableComponent)
	{
		LOG_WARNING(TEXT("[EquipItemAction] Spawned actor has no attachable component"))
		spawnedActor->Destroy();
		return false;
	}

	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> containerInterface = equipmentInterface.GetObject();

	if (!attachableComponent->AttachTo(containerInterface))
	{
		LOG_WARNING(TEXT("[EquipItemAction] Failed to attach spawned actor"))
		spawnedActor->Destroy();
		return false;
	}

	LOG_INFO(TEXT("[EquipItemAction] Successfully equipped item %s"), *TargetItem.GetGuid().ToString())
	return true;
	*/
}

FText UMounteaEquipItemAction::GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const
{
	const UMounteaInventoryItemTemplate* itemTemplate = TargetItem.GetTemplate();
	if (!itemTemplate)
		return FText::FromString(TEXT("Invalid item itemTemplate"));

	if (!itemTemplate->SpawnActor.IsValid())
		return FText::FromString(TEXT("Item cannot be equipped"));

	if (!GetEquipmentInterface(GetAvatarActorFromActorInfo()))
		return FText::FromString(TEXT("No equipment system available"));

	return Super::GetDisallowedReason_Implementation(TargetItem);
}

TScriptInterface<IMounteaAdvancedEquipmentInterface> UMounteaEquipItemAction::GetEquipmentInterface(UObject* ActionInitiator) const
{
	if (!ActionInitiator)
		return nullptr;

	AActor* actor = Cast<AActor>(ActionInitiator);
	if (!actor)
		return nullptr;

	const auto equipmentComponents = actor->GetComponentsByInterface(UMounteaAdvancedEquipmentInterface::StaticClass());
	if (equipmentComponents.Num() == 0)
	{
		LOG_ERROR(TEXT("[EquipItemAction] No equipment interface found on actor %s"), *actor->GetName())
		return nullptr;
	}

	return equipmentComponents[0];
}