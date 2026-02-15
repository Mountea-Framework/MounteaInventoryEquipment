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

#include "Components/ActorComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaAttachmentsStatics.h"

namespace EquipmentStatics
{
	static UMounteaAdvancedInventorySettings* GetInventorySettings()
	{
		return GetMutableDefault<UMounteaAdvancedInventorySettings>();
	}
}

UMounteaAdvancedEquipmentSettingsConfig* UMounteaEquipmentStatics::GetEquipmentSettingsConfig()
{
	return EquipmentStatics::GetInventorySettings() ? EquipmentStatics::GetInventorySettings()->AdvancedEquipmentSettingsConfig.LoadSynchronous() : nullptr;
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

bool UMounteaEquipmentStatics::ValidateEquipmentItemRequest(const UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* preferredSlot, bool& bValue)
{
	if (!IsValid(Outer))
	{
		bValue = false;
		return true;
	}

	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>() && !Outer->Implements<UMounteaAdvancedEquipmentInterface>())
	{
		bValue = false;
		return true;
	}

	if (!ItemDefinition.IsItemValid())
	{
		bValue = false;
		return true;
	}

	const auto spawnClass = ItemDefinition.GetTemplate()->SpawnActor.LoadSynchronous();
	if (!IsTargetClassValid(spawnClass))
	{
		bValue = false;
		return true;
	}

	const auto defaultActor = spawnClass->GetDefaultObject<AActor>();
	if (!IsValid(defaultActor))
	{
		bValue = false;
		return true;
	}

	if (!IsValid(preferredSlot))
	{
		const auto equipmentInterface = FindEquipmentItemInterface(defaultActor);
		const FName preferredSlotName = equipmentInterface.GetObject() ?
											IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemPreferredSlot(equipmentInterface.GetObject()) :
											NAME_None;
	
		const auto preferredSlotTag = equipmentInterface.GetObject() ?
										  IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentPreferredSlotTag(equipmentInterface.GetObject()) :
										  FGameplayTag();
	
		if (preferredSlotName == NAME_None && !preferredSlotTag.IsValid())
		{
			bValue = false;
			return true;
		}

		preferredSlot = IMounteaAdvancedAttachmentContainerInterface::Execute_IsValidSlot(Outer, preferredSlotName) ? 
							IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, preferredSlotName) :
							IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, 
								IMounteaAdvancedAttachmentContainerInterface::Execute_FindFirstFreeSlotWithTags(Outer, FGameplayTagContainer(preferredSlotTag)));
	}

	if (!IsValid(preferredSlot))
	{
		bValue = false;
		return true;
	}
	
	return false;
}

bool UMounteaEquipmentStatics::EquipItem(UObject* Outer, const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor)
{
	OutSpawnedActor = nullptr;

	UMounteaAdvancedAttachmentSlot* preferredSlot = nullptr;
	bool bValue;
	if (ValidateEquipmentItemRequest(Outer, ItemDefinition, preferredSlot, bValue)) 
		return bValue;

	return EquipItemToSlot(Outer, ItemDefinition, preferredSlot, OutSpawnedActor);
}

bool UMounteaEquipmentStatics::EquipItemToSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* TargetSlot, 
	AActor*& OutSpawnedActor)
{
	bool bValue;
	
	if (ValidateEquipmentItemRequest(Outer, ItemDefinition, TargetSlot, bValue)) 
		return bValue;
	
	return true;
}
