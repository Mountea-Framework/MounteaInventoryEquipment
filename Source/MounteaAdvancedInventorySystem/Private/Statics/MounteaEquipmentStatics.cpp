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

#include "Algo/Count.h"
#include "Algo/Find.h"
#include "Algo/Sort.h"
#include "Algo/Transform.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/MounteaEquipmentComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaAttachmentsStatics.h"

const FMounteaEquipmentSlotHeaderData* UMounteaEquipmentStatics::ResolveSlotHeaderData(
	const UMounteaAdvancedAttachmentSlotBase* Slot,
	const UMounteaAdvancedEquipmentSettingsConfig* SettingsConfig)
{
	if (!IsValid(Slot) || !IsValid(SettingsConfig))
		return nullptr;

	return SettingsConfig->AllowedEquipmentSlots.Find(Slot->SlotName);
}

bool UMounteaEquipmentStatics::TryGetAttachmentTags(UObject* AttachmentObject, FGameplayTagContainer& OutTags)
{
	OutTags.Reset();
	if (!IsValid(AttachmentObject))
		return false;

	const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> attachableInterface =
		UMounteaAdvancedAttachmentSlot::FindAttachableInterface(AttachmentObject);
	if (!attachableInterface.GetObject())
		return false;

	OutTags = IMounteaAdvancedAttachmentAttachableInterface::Execute_GetTags(attachableInterface.GetObject());
	return !OutTags.IsEmpty();
}

bool UMounteaEquipmentStatics::IsAttachmentActiveEquipmentItem(UObject* AttachmentObject)
{
	const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipmentItemInterface =
		FindEquipmentItemInterface(AttachmentObject);
	if (!equipmentItemInterface.GetObject())
		return true;

	return IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(equipmentItemInterface.GetObject()) ==
		EEquipmentItemState::EES_Active;
}

bool UMounteaEquipmentStatics::ValidateSlotItemTypeCompatibility(
	const FInventoryItem& ItemDefinition,
	const UMounteaAdvancedAttachmentSlot* TargetSlot,
	const UMounteaAdvancedEquipmentSettingsConfig* SettingsConfig)
{
	if (!IsValid(TargetSlot) || !IsValid(SettingsConfig))
		return true;

	const FGameplayTagContainer* allowedItemTypesPtr = &TargetSlot->AllowedItemTypes;
	if (allowedItemTypesPtr->IsEmpty())
	{
		if (const FMounteaEquipmentSlotHeaderData* slotHeaderData = SettingsConfig->AllowedEquipmentSlots.Find(TargetSlot->SlotName))
			allowedItemTypesPtr = &slotHeaderData->AllowedItemTypes;
	}
	const FGameplayTagContainer& allowedItemTypes = *allowedItemTypesPtr;
	const UMounteaInventoryItemTemplate* itemTemplate = ItemDefinition.GetTemplate();
	const FGameplayTag equipmentItemType = IsValid(itemTemplate) ? itemTemplate->EquipmentItemType : FGameplayTag();

	if (allowedItemTypes.IsEmpty())
	{
		LOG_INFO(TEXT("[Validate Equipment Item Request]: Slot '%s' has empty AllowedItemTypes. Allowing all item types."),
			*TargetSlot->SlotName.ToString());
		return true;
	}

	if (!equipmentItemType.IsValid())
	{
		LOG_WARNING(TEXT("[Validate Equipment Item Request]: Item '%s' has empty EquipmentItemType. Allowing all slot types."),
			*ItemDefinition.GetItemName().ToString());
		return true;
	}

	if (!allowedItemTypes.HasTag(equipmentItemType))
	{
		LOG_WARNING(TEXT("[Validate Equipment Item Request]: Item '%s' (ItemType '%s') is not allowed in slot '%s'."),
			*ItemDefinition.GetItemName().ToString(),
			*equipmentItemType.ToString(),
			*TargetSlot->SlotName.ToString());
		return false;
	}

	return true;
}

AActor* UMounteaEquipmentStatics::ResolveOwningActor(UObject* Target)
{
	if (AActor* ownerActor = Cast<AActor>(Target))
		return ownerActor;

	if (const UActorComponent* ownerComponent = Cast<UActorComponent>(Target))
		return ownerComponent->GetOwner();

	return nullptr;
}

USceneComponent* UMounteaEquipmentStatics::ResolveQuickUseAttachmentTarget(UObject* Outer, const FName& VisualSlotId, FName& OutSocketName)
{
	OutSocketName = NAME_None;

	if (IsValid(Outer) && Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>() && !VisualSlotId.IsNone())
	{
		UMounteaAdvancedAttachmentSlot* visualSlot =
			IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, VisualSlotId);
		if (IsValid(visualSlot))
		{
			OutSocketName = visualSlot->GetAttachmentSocketName();

			if (IsValid(visualSlot->AttachmentTargetComponentOverride))
				return visualSlot->AttachmentTargetComponentOverride;

			if (visualSlot->ParentContainer.GetObject())
				return IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentTargetComponent(
					visualSlot->ParentContainer.GetObject());

			return IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentTargetComponent(Outer);
		}
	}

	AActor* owningActor = ResolveOwningActor(Outer);
	if (!IsValid(owningActor))
		return nullptr;

	if (USkeletalMeshComponent* ownerSkeletalMesh = owningActor->FindComponentByClass<USkeletalMeshComponent>())
		return ownerSkeletalMesh;

	return owningActor->GetRootComponent();
}

void UMounteaEquipmentStatics::ApplyQuickUsePlaceholderMesh(AActor* PlaceholderActor, const FInventoryItem& ItemDefinition, const UClass* PlaceholderClass)
{
	if (!IsValid(PlaceholderActor) || !ItemDefinition.IsItemValid())
		return;

	const UMounteaInventoryItemTemplate* itemTemplate = ItemDefinition.GetTemplate();
	if (!IsValid(itemTemplate) || !IsValid(itemTemplate->ItemMesh))
		return;

	if (UStaticMesh* staticMesh = Cast<UStaticMesh>(itemTemplate->ItemMesh))
	{
		if (!SetQuickUseItemStaticMesh(PlaceholderActor, staticMesh))
		{
			const FString className = IsValid(PlaceholderClass) ? PlaceholderClass->GetName() : TEXT("Unknown");
			LOG_WARNING(TEXT("[Spawn Quick Use Placeholder Actor]: Placeholder class '%s' does not expose quick-use mesh interface for static mesh setup."),
				*className)
		}
		return;
	}

	if (USkeletalMesh* skeletalMesh = Cast<USkeletalMesh>(itemTemplate->ItemMesh))
	{
		if (!SetQuickUseItemSkeletalMesh(PlaceholderActor, skeletalMesh))
		{
			const FString className = IsValid(PlaceholderClass) ? PlaceholderClass->GetName() : TEXT("Unknown");
			LOG_WARNING(TEXT("[Spawn Quick Use Placeholder Actor]: Placeholder class '%s' does not expose quick-use mesh interface for skeletal mesh setup."),
				*className)
		}
	}
}

UObject* UMounteaEquipmentStatics::ResolveFirstInterfaceObject(UObject* Target, TSubclassOf<UInterface> InterfaceClass,
	const bool bResolveOwnerForComponents)
{
	const UClass* interfaceClassPtr = InterfaceClass.Get();
	if (!IsValid(Target) || !IsValid(interfaceClassPtr))
		return nullptr;

	if (Target->GetClass()->ImplementsInterface(interfaceClassPtr))
		return Target;

	AActor* actor = Cast<AActor>(Target);
	if (!IsValid(actor) && bResolveOwnerForComponents)
	{
		if (const UActorComponent* component = Cast<UActorComponent>(Target))
			actor = component->GetOwner();
	}

	return ResolveFirstInterfaceObjectFromActor(actor, InterfaceClass);
}

UObject* UMounteaEquipmentStatics::ResolveFirstInterfaceObjectFromActor(AActor* Actor, TSubclassOf<UInterface> InterfaceClass)
{
	const UClass* interfaceClassPtr = InterfaceClass.Get();
	if (!IsValid(Actor) || !IsValid(interfaceClassPtr))
		return nullptr;

	const TArray<UActorComponent*> components = Actor->GetComponentsByInterface(InterfaceClass);
	if (!components.IsEmpty())
		return components[0];

	const UBlueprintGeneratedClass* blueprintClass = Cast<UBlueprintGeneratedClass>(Actor->GetClass());
	return ResolveFirstInterfaceObjectFromBlueprintClass(blueprintClass, InterfaceClass);
}

UObject* UMounteaEquipmentStatics::ResolveFirstInterfaceObjectFromBlueprintClass(
	const UBlueprintGeneratedClass* BlueprintClass,
	TSubclassOf<UInterface> InterfaceClass)
{
	const UClass* interfaceClassPtr = InterfaceClass.Get();
	if (!IsValid(BlueprintClass) || !IsValid(interfaceClassPtr) || !BlueprintClass->SimpleConstructionScript)
		return nullptr;

	for (const USCS_Node* node : BlueprintClass->SimpleConstructionScript->GetAllNodes())
	{
		if (!node)
			continue;

		const UActorComponent* componentTemplate = node->GetActualComponentTemplate(
			const_cast<UBlueprintGeneratedClass*>(BlueprintClass));
		if (IsValid(componentTemplate) && componentTemplate->GetClass()->ImplementsInterface(interfaceClassPtr))
			return const_cast<UActorComponent*>(componentTemplate);
	}

	return nullptr;
}

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

TSoftClassPtr<AActor> UMounteaEquipmentStatics::GetDefaultQuickUseItemClass()
{
	if (const auto* equipmentConfig = GetEquipmentSettingsConfig())
		return equipmentConfig->DefaultQuickUseItemClass;

	return TSoftClassPtr<AActor>();
}

TScriptInterface<IMounteaAdvancedEquipmentItemInterface> UMounteaEquipmentStatics::FindEquipmentItemInterface(UObject* Target)
{
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> result;

	UObject* interfaceObject = ResolveFirstInterfaceObject(
		Target,
		UMounteaAdvancedEquipmentItemInterface::StaticClass(),
		false);
	if (IsValid(interfaceObject))
	{
		result.SetObject(interfaceObject);
		result.SetInterface(Cast<IMounteaAdvancedEquipmentItemInterface>(interfaceObject));
	}

	return result;
}

TScriptInterface<IMounteaAdvancedEquipmentInterface> UMounteaEquipmentStatics::FindEquipmentInterface(UObject* Target)
{
	TScriptInterface<IMounteaAdvancedEquipmentInterface> result;

	UObject* interfaceObject = ResolveFirstInterfaceObject(
		Target,
		UMounteaAdvancedEquipmentInterface::StaticClass(),
		true);
	if (IsValid(interfaceObject))
	{
		result.SetObject(interfaceObject);
		result.SetInterface(Cast<IMounteaAdvancedEquipmentInterface>(interfaceObject));
	}

	return result;
}

TScriptInterface<IMounteaAdvancedQuickUseItemInterface> UMounteaEquipmentStatics::FindQuickUseItemInterface(UObject* Target)
{
	TScriptInterface<IMounteaAdvancedQuickUseItemInterface> result;

	UObject* interfaceObject = ResolveFirstInterfaceObject(
		Target,
		UMounteaAdvancedQuickUseItemInterface::StaticClass(),
		true);
	if (IsValid(interfaceObject))
	{
		result.SetObject(interfaceObject);
		result.SetInterface(Cast<IMounteaAdvancedQuickUseItemInterface>(interfaceObject));
	}

	return result;
}

bool UMounteaEquipmentStatics::SetQuickUseItemStaticMesh(UObject* Target, UStaticMesh* NewStaticMesh)
{
	const TScriptInterface<IMounteaAdvancedQuickUseItemInterface> quickUseInterface = FindQuickUseItemInterface(Target);
	if (!quickUseInterface.GetObject())
		return false;

	return IMounteaAdvancedQuickUseItemInterface::Execute_SetStaticMesh(quickUseInterface.GetObject(), NewStaticMesh);
}

bool UMounteaEquipmentStatics::SetQuickUseItemSkeletalMesh(UObject* Target, USkeletalMesh* NewSkeletalMesh)
{
	const TScriptInterface<IMounteaAdvancedQuickUseItemInterface> quickUseInterface = FindQuickUseItemInterface(Target);
	if (!quickUseInterface.GetObject())
		return false;

	return IMounteaAdvancedQuickUseItemInterface::Execute_SetSkeletalMesh(quickUseInterface.GetObject(), NewSkeletalMesh);
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

FName UMounteaEquipmentStatics::ResolveBestSlotIdFromTags(const FGameplayTagContainer& DesiredTags,
	const FGameplayTag& EquipmentItemType)
{
	const UMounteaAdvancedEquipmentSettingsConfig* settingsConfig = GetEquipmentSettingsConfig();
	if (!IsValid(settingsConfig))
		return NAME_None;
	if (DesiredTags.IsEmpty())
		return NAME_None;
	if (settingsConfig->AllowedEquipmentSlots.IsEmpty())
		return NAME_None;

	struct FSlotCandidate
	{
		FName SlotName = NAME_None;
		bool bHasAllDesiredTags = false;
		int32 OverlapCount = 0;
		int32 ExtraTags = 0;
	};

	TArray<FGameplayTag> desiredTags;
	DesiredTags.GetGameplayTagArray(desiredTags);

	const auto isSlotCompatible = [&desiredTags, &settingsConfig, &EquipmentItemType](const FName& slotName)
	{
		const FMounteaEquipmentSlotHeaderData* slotData = settingsConfig->AllowedEquipmentSlots.Find(slotName);
		if (!slotData)
			return false;

		if (!slotData->bIsEnabled)
			return false;

		if (EquipmentItemType.IsValid() &&
			!slotData->AllowedItemTypes.IsEmpty() &&
			!slotData->AllowedItemTypes.HasTag(EquipmentItemType))
		{
			return false;
		}

		const int32 overlapCount = Algo::CountIf(desiredTags, [slotData](const FGameplayTag& desiredTag)
		{
			return slotData->TagContainer.HasTag(desiredTag);
		});

		return overlapCount > 0;
	};

	TArray<FName> slotNames;
	settingsConfig->AllowedEquipmentSlots.GetKeys(slotNames);

	TArray<FSlotCandidate> candidates;
	candidates.Reserve(slotNames.Num());

	Algo::TransformIf(
		slotNames,
		candidates,
		isSlotCompatible,
		[&desiredTags, &settingsConfig, &DesiredTags](const FName& slotName)
		{
			const FMounteaEquipmentSlotHeaderData* slotData = settingsConfig->AllowedEquipmentSlots.Find(slotName);
			check(slotData);

			const int32 overlapCount = Algo::CountIf(desiredTags, [slotData](const FGameplayTag& desiredTag)
			{
				return slotData->TagContainer.HasTag(desiredTag);
			});

			FSlotCandidate candidate;
			candidate.SlotName = slotName;
			candidate.bHasAllDesiredTags = slotData->TagContainer.HasAll(DesiredTags);
			candidate.OverlapCount = overlapCount;
			candidate.ExtraTags = slotData->TagContainer.Num() - overlapCount;
			return candidate;
		});

	if (candidates.IsEmpty())
		return NAME_None;

	Algo::Sort(candidates, [](const FSlotCandidate& left, const FSlotCandidate& right)
	{
		if (left.bHasAllDesiredTags != right.bHasAllDesiredTags)
			return left.bHasAllDesiredTags;

		if (left.OverlapCount != right.OverlapCount)
			return left.OverlapCount > right.OverlapCount;

		if (left.ExtraTags != right.ExtraTags)
			return left.ExtraTags < right.ExtraTags;

		return left.SlotName.LexicalLess(right.SlotName);
	});

	return candidates[0].SlotName;
}

UMounteaAdvancedAttachmentSlot* UMounteaEquipmentStatics::FindSlotWithEquippedItem(UObject* Outer, const FGuid& ItemGuid)
{
	UMounteaAdvancedAttachmentSlot* foundSlot = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> foundInterface;
	return FindEquippedItemSlotAndInterface(Outer, ItemGuid, foundSlot, foundInterface) ? foundSlot : nullptr;
}

bool UMounteaEquipmentStatics::FindEquippedItemSlotAndInterface(UObject* Outer, const FGuid& ItemGuid,
	UMounteaAdvancedAttachmentSlot*& OutSlot, TScriptInterface<IMounteaAdvancedEquipmentItemInterface>& OutInterface)
{
	OutSlot = nullptr;
	OutInterface = TScriptInterface<IMounteaAdvancedEquipmentItemInterface>();

	if (!IsValid(Outer) || !ItemGuid.IsValid())
		return false;
	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
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

bool UMounteaEquipmentStatics::TryGetEquippedItemGuidFromSlot(UObject* Outer, const FName& SlotId, FGuid& OutItemGuid)
{
	OutItemGuid = FGuid();

	if (!IsValid(Outer) || SlotId.IsNone())
		return false;

	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;

	if (!IMounteaAdvancedAttachmentContainerInterface::Execute_IsValidSlot(Outer, SlotId))
		return false;

	const UMounteaAdvancedAttachmentSlot* slot = IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, SlotId);
	if (!IsValid(slot) || !slot->IsOccupied() || !IsValid(slot->Attachment))
		return false;

	const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> itemInterface = FindEquipmentItemInterface(slot->Attachment);
	if (!itemInterface.GetObject())
		return false;

	OutItemGuid = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquippedItemId(itemInterface.GetObject());
	return OutItemGuid.IsValid();
}

bool UMounteaEquipmentStatics::TryResolveInventoryItemByGuid(UObject* Outer, const FGuid& ItemGuid, FInventoryItem& OutItemDefinition)
{
	OutItemDefinition = FInventoryItem();

	if (!IsValid(Outer) || !ItemGuid.IsValid())
		return false;

	const auto tryResolveFromInventory = [&](const UObject* Candidate) -> bool
	{
		if (!IsValid(Candidate) || !Candidate->Implements<UMounteaAdvancedInventoryInterface>())
			return false;

		const FInventoryItem foundItem = IMounteaAdvancedInventoryInterface::Execute_FindItem(
			Candidate,
			FInventoryItemSearchParams(ItemGuid));
		if (!foundItem.IsItemValid())
			return false;

		OutItemDefinition = foundItem;
		return true;
	};

	if (tryResolveFromInventory(Outer))
		return true;

	AActor* owningActor = ResolveOwningActor(Outer);
	if (tryResolveFromInventory(owningActor))
		return true;

	if (!IsValid(owningActor))
		return false;

	const TArray<UActorComponent*> inventoryComponents = owningActor->GetComponentsByInterface(
		UMounteaAdvancedInventoryInterface::StaticClass());
	for (UActorComponent* inventoryComponent : inventoryComponents)
	{
		if (tryResolveFromInventory(inventoryComponent))
			return true;
	}

	return false;
}

AActor* UMounteaEquipmentStatics::SpawnQuickUsePlaceholderActor(UObject* Outer, const FInventoryItem& ItemDefinition,
	const FName& VisualSlotId)
{
	if (!IsValid(Outer) || !ItemDefinition.IsItemValid())
		return nullptr;

	UWorld* world = Outer->GetWorld();
	if (!IsValid(world) || world->GetNetMode() == NM_DedicatedServer)
		return nullptr;

	const TSoftClassPtr<AActor> quickUseClass = GetDefaultQuickUseItemClass();
	if (quickUseClass.IsNull())
		return nullptr;

	UClass* quickUseActorClass = quickUseClass.LoadSynchronous();
	if (!IsValid(quickUseActorClass))
	{
		LOG_WARNING(TEXT("[Spawn Quick Use Placeholder Actor]: Default quick use item class failed to load."))
		return nullptr;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = ResolveOwningActor(Outer);
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* placeholderActor = world->SpawnActor<AActor>(quickUseActorClass, FTransform::Identity, spawnParams);
	if (!IsValid(placeholderActor))
		return nullptr;

	placeholderActor->SetReplicates(false);
	placeholderActor->SetActorEnableCollision(false);
	ApplyQuickUsePlaceholderMesh(placeholderActor, ItemDefinition, quickUseActorClass);

	FName attachmentSocket = NAME_None;
	USceneComponent* attachmentTarget = ResolveQuickUseAttachmentTarget(Outer, VisualSlotId, attachmentSocket);
	if (IsValid(attachmentTarget))
	{
		placeholderActor->AttachToComponent(
			attachmentTarget,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			attachmentSocket);
	}

	return placeholderActor;
}

bool UMounteaEquipmentStatics::IsSlotBlockedByCurrentAttachments(const UObject* Outer,
	const UMounteaAdvancedAttachmentSlotBase* TargetSlot, const bool bIgnoreTargetSlotOccupant)
{
	if (!IsValid(Outer) || !IsValid(TargetSlot))
		return false;

	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;

	const UMounteaAdvancedEquipmentSettingsConfig* settingsConfig = GetEquipmentSettingsConfig();
	const FMounteaEquipmentSlotHeaderData* slotHeaderData = ResolveSlotHeaderData(TargetSlot, settingsConfig);
	if (!slotHeaderData || slotHeaderData->BlockedByTags.IsEmpty())
		return false;

	const TArray<UMounteaAdvancedAttachmentSlot*> slots =
		IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentSlots(Outer);
	for (const UMounteaAdvancedAttachmentSlot* slot : slots)
	{
		if (!IsValid(slot) || !slot->IsOccupied() || !IsValid(slot->Attachment))
			continue;

		if (bIgnoreTargetSlotOccupant && slot == TargetSlot)
			continue;

		if (!IsAttachmentActiveEquipmentItem(slot->Attachment))
			continue;

		FGameplayTagContainer attachmentTags;
		if (!TryGetAttachmentTags(slot->Attachment, attachmentTags))
			continue;

		if (slotHeaderData->BlockedByTags.HasAny(attachmentTags))
			return true;
	}

	return false;
}

bool UMounteaEquipmentStatics::ClearBlockedSlotsForAttachment(UObject* Outer, UObject* BlockingAttachment,
	const FGuid& BlockingItemGuid, const TSet<FName>& IgnoredSlots)
{
	if (!IsValid(Outer))
		return false;

	if (!IsValid(BlockingAttachment))
		return true;

	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;

	const UMounteaAdvancedEquipmentSettingsConfig* settingsConfig = GetEquipmentSettingsConfig();
	if (!IsValid(settingsConfig))
		return true;

	FGameplayTagContainer blockingTags;
	if (!TryGetAttachmentTags(BlockingAttachment, blockingTags) || blockingTags.IsEmpty())
		return true;

	const TArray<UMounteaAdvancedAttachmentSlot*> slots =
		IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentSlots(Outer);
	for (const UMounteaAdvancedAttachmentSlot* slot : slots)
	{
		if (!IsValid(slot) || IgnoredSlots.Contains(slot->SlotName))
			continue;

		const FMounteaEquipmentSlotHeaderData* slotHeaderData = ResolveSlotHeaderData(slot, settingsConfig);
		if (!slotHeaderData || slotHeaderData->BlockedByTags.IsEmpty())
			continue;

		if (!slotHeaderData->BlockedByTags.HasAny(blockingTags))
			continue;

		if (!slot->IsOccupied() || !IsValid(slot->Attachment))
			continue;

		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipmentItemInterface =
			FindEquipmentItemInterface(slot->Attachment);
		if (equipmentItemInterface.GetObject() && BlockingItemGuid.IsValid())
		{
			const FGuid occupantGuid = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquippedItemId(
				equipmentItemInterface.GetObject());
			if (occupantGuid.IsValid() && occupantGuid == BlockingItemGuid)
				continue;
		}

		if (Outer->Implements<UMounteaAdvancedEquipmentInterface>() && equipmentItemInterface.GetObject())
		{
			if (!IMounteaAdvancedEquipmentInterface::Execute_UnequipItemFromSlot(Outer, slot->SlotName, true))
				return false;
			continue;
		}

		UObject* previousAttachment = slot->Attachment;
		AActor* previousAttachmentActor = ResolveAttachmentActor(previousAttachment);
		if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryDetach(Outer, slot->SlotName))
			return false;

		if (IsValid(previousAttachmentActor))
			previousAttachmentActor->Destroy();
	}

	return true;
}

bool UMounteaEquipmentStatics::SwitchEquippedItemSlot(UObject* Outer, UMounteaAdvancedAttachmentSlot* CurrentSlot, UMounteaAdvancedAttachmentSlot* TargetSlot)
{
	if (!IsValid(Outer) || !IsValid(CurrentSlot) || !IsValid(TargetSlot))
		return false;

	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;

	if (CurrentSlot == TargetSlot)
		return false;

	if (!CurrentSlot->IsOccupied() || !IsValid(CurrentSlot->Attachment))
		return false;

	if (TargetSlot->IsOccupied() && !IsValid(TargetSlot->Attachment))
		return false;

	// Clear target slot if occupied
	if (TargetSlot->IsOccupied())
	{
		AActor* occupantActor = ResolveAttachmentActor(TargetSlot->Attachment);
		if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryDetach(Outer, TargetSlot->SlotName))
			return false;

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
	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;

	if (!TargetSlot->IsOccupied())
		return true;

	if (!IsValid(TargetSlot->Attachment))
		return false;

	AActor* occupantActor = ResolveAttachmentActor(TargetSlot->Attachment);
	if (!IMounteaAdvancedAttachmentContainerInterface::Execute_TryDetach(Outer, TargetSlot->SlotName))
		return false;

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

	const TSubclassOf<UInterface> equipmentItemInterfaceClass = UMounteaAdvancedEquipmentItemInterface::StaticClass();
	if (TargetClass->ImplementsInterface(equipmentItemInterfaceClass.Get()))
		return true;

	const AActor* targetActor = Cast<AActor>(TargetClass->GetDefaultObject());
	if (!IsValid(targetActor))
		return false;

	const TArray<UActorComponent*> components = targetActor->GetComponentsByInterface(UMounteaAdvancedEquipmentItemInterface::StaticClass());
	if (!components.IsEmpty())
		return true;

	const UBlueprintGeneratedClass* blueprintClass = Cast<UBlueprintGeneratedClass>(TargetClass);
	return IsValid(ResolveFirstInterfaceObjectFromBlueprintClass(blueprintClass, equipmentItemInterfaceClass));
}

bool UMounteaEquipmentStatics::ValidateEquipmentItemRequest(const UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot*& TargetSlot)
{
	if (!IsValid(Outer))
		return false;

	if (!IsValid(Outer->GetWorld()))
		return false;
	
	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;

	if (!ItemDefinition.IsItemValid())
		return false;

	const UMounteaInventoryItemTemplate* itemTemplate = ItemDefinition.GetTemplate();
	if (!IsValid(itemTemplate))
		return false;

	UClass* spawnClass = itemTemplate->SpawnActor.LoadSynchronous();
	if (!IsTargetClassValid(spawnClass))
		return false;

	const AActor* defaultActor = spawnClass->GetDefaultObject<AActor>();
	if (!IsValid(defaultActor))
		return false;

	if (!IsValid(TargetSlot))
	{
		const auto equipmentInterface = FindEquipmentItemInterface(const_cast<AActor*>(defaultActor));
		const FName preferredSlotName = equipmentInterface.GetObject() ? 
			IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemPreferredSlot(equipmentInterface.GetObject()) : 
			NAME_None;
	
		const auto preferredSlotTag = equipmentInterface.GetObject() ?
			IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentPreferredSlotTag(equipmentInterface.GetObject()) :
			FGameplayTag();
	
		if (preferredSlotName == NAME_None && !preferredSlotTag.IsValid())
			return false;

		if (IMounteaAdvancedAttachmentContainerInterface::Execute_IsValidSlot(Outer, preferredSlotName))
		{
			TargetSlot = IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, preferredSlotName);
		}
		else if (preferredSlotTag.IsValid())
		{
			const FName freeTaggedSlot = IMounteaAdvancedAttachmentContainerInterface::Execute_FindFirstFreeSlotWithTags(
				Outer,
				FGameplayTagContainer(preferredSlotTag));
			TargetSlot = IMounteaAdvancedAttachmentContainerInterface::Execute_GetSlot(Outer, freeTaggedSlot);
		}
	}

	if (!IsValid(TargetSlot) || !TargetSlot->CanAttach())
		return false;

	const UMounteaAdvancedEquipmentSettingsConfig* settingsConfig = GetEquipmentSettingsConfig();
	if (!ValidateSlotItemTypeCompatibility(ItemDefinition, TargetSlot, settingsConfig))
		return false;

	if (IsSlotBlockedByCurrentAttachments(Outer, TargetSlot, true))
	{
		LOG_WARNING(TEXT("[Validate Equipment Item Request]: Slot '%s' is currently blocked by active equipment tags."),
			*TargetSlot->SlotName.ToString());
		return false;
	}
	
	return true;
}

bool UMounteaEquipmentStatics::CreateEquipmentItemAndAttach(UObject* Outer, const FInventoryItem& ItemDefinition, const UMounteaAdvancedAttachmentSlot* TargetSlot, 
	AActor*& OutSpawnedActor)
{
	OutSpawnedActor = nullptr;

	const UMounteaInventoryItemTemplate* itemTemplate = ItemDefinition.GetTemplate();
	if (!IsValid(itemTemplate))
		return false;
	if (!IsValid(Outer))
		return false;
	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;
	if (!IsValid(TargetSlot))
		return false;

	UWorld* world = Outer->GetWorld();
	if (!IsValid(world))
		return false;
	
	UClass* classToSpawn = itemTemplate->SpawnActor.LoadSynchronous();
	if (!IsValid(classToSpawn))
		return false;
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	OutSpawnedActor = world->SpawnActor<AActor>(classToSpawn, FTransform(), spawnParams);
	if (!IsValid(OutSpawnedActor))
		return false;

	if (ItemDefinition.GetGuid().IsValid())
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
		if (IsValid(OutSpawnedActor))
			OutSpawnedActor->Destroy();
		OutSpawnedActor = nullptr;
		return false;
	}

	// Set initial equipment state after successful attach
	{
		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface = FindEquipmentItemInterface(OutSpawnedActor);
		if (equipItemInterface.GetObject())
		{
			const bool bAutoActivate = IMounteaAdvancedEquipmentItemInterface::Execute_DoesAutoActive(equipItemInterface.GetObject());
			const EEquipmentItemState initialState = bAutoActivate ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped;
			IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), initialState);
		}
	}

	return true;
}

bool UMounteaEquipmentStatics::EquipItemGeneral(UObject* Outer, const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor)
{
	OutSpawnedActor = nullptr;

	UMounteaAdvancedAttachmentSlot* preferredSlot = nullptr;
	if (!ValidateEquipmentItemRequest(Outer, ItemDefinition, preferredSlot))
		return false;

	return EquipItemToResolvedSlot(Outer, ItemDefinition, preferredSlot, OutSpawnedActor);
}

bool UMounteaEquipmentStatics::EquipItemToSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* TargetSlot,
	AActor*& OutSpawnedActor)
{
	if (!ValidateEquipmentItemRequest(Outer, ItemDefinition, TargetSlot))
		return false;

	return EquipItemToResolvedSlot(Outer, ItemDefinition, TargetSlot, OutSpawnedActor);
}

bool UMounteaEquipmentStatics::EquipItemToResolvedSlot(UObject* Outer, const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* ResolvedTargetSlot,
	AActor*& OutSpawnedActor)
{
	OutSpawnedActor = nullptr;
	if (!IsValid(Outer) || !IsValid(ResolvedTargetSlot))
		return false;
	if (!Outer->Implements<UMounteaAdvancedAttachmentContainerInterface>())
		return false;

	const bool bSupportsEquipmentInterface = Outer->Implements<UMounteaAdvancedEquipmentInterface>();
	if (bSupportsEquipmentInterface && IMounteaAdvancedEquipmentInterface::Execute_IsEquipmentItemEquipped(Outer, ItemDefinition))
	{
		UMounteaAdvancedAttachmentSlot* currentSlot = FindSlotWithEquippedItem(Outer, ItemDefinition.GetGuid());
		if (!IsValid(currentSlot) || currentSlot == ResolvedTargetSlot)
			return false;

		return SwitchEquippedItemSlot(Outer, currentSlot, ResolvedTargetSlot);
	}

	if (!EnsureSlotIsReadyForEquip(Outer, ResolvedTargetSlot))
		return false;

	return CreateEquipmentItemAndAttach(Outer, ItemDefinition, ResolvedTargetSlot, OutSpawnedActor);
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
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_EquipItem(Target.GetObject(), ItemDefinition) != nullptr;
}

bool UMounteaEquipmentStatics::EquipItemToSlot(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target,
	const FInventoryItem& ItemDefinition, const FName& SlotName)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_EquipItemToSlot(Target.GetObject(), SlotName, ItemDefinition) != nullptr;
}

bool UMounteaEquipmentStatics::IsItemEquipped(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target,
                                              const FInventoryItem& ItemDefinition)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_IsEquipmentItemEquipped(Target.GetObject(), ItemDefinition);
}

bool UMounteaEquipmentStatics::IsItemEquippedInSlot(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, 
	const FInventoryItem& ItemDefinition, const FName& SlotName)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_IsEquipmentItemEquippedInSlot(Target.GetObject(), ItemDefinition, SlotName);
}

bool UMounteaEquipmentStatics::UnequipItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, bool bUseFallbackSlot)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_UnequipItem(Target.GetObject(), ItemDefinition, bUseFallbackSlot);
}

bool UMounteaEquipmentStatics::ActivateEquipmentItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_ActivateEquipmentItem(Target.GetObject(), ItemDefinition, TargetSlotId);
}

bool UMounteaEquipmentStatics::ActivateQuickUseItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target,
	const FName& SlotId, const FName& TargetSlotId)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_ActivateQuickUseItem(Target.GetObject(), SlotId, TargetSlotId);
}

bool UMounteaEquipmentStatics::DeactivateEquipmentItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target, const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_DeactivateEquipmentItem(Target.GetObject(), ItemDefinition, TargetSlotId);
}

bool UMounteaEquipmentStatics::AnimAttachItem(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_AnimAttachItem(Target.GetObject());
}

bool UMounteaEquipmentStatics::AnimQuickItemUsed(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Target)
{
	if (!Target.GetObject())
		return false;
	return IMounteaAdvancedEquipmentInterface::Execute_AnimQuickItemUsed(Target.GetObject());
}
