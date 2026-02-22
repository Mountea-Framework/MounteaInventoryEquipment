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


#include "Components/MounteaEquipmentComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "GameFramework/Actor.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaAttachmentsStatics.h"
#include "Statics/MounteaEquipmentStatics.h"

UMounteaEquipmentComponent::UMounteaEquipmentComponent()
{
	ComponentTags.Append( { TEXT("Equipment") } );
}

void UMounteaEquipmentComponent::Server_EquipItem_Implementation(const FInventoryItem& ItemDefinition)
{
	Execute_EquipItem(this, ItemDefinition);
}

void UMounteaEquipmentComponent::Server_EquipItemToSlot_Implementation(const FInventoryItem& ItemDefinition, const FName& SlotId)
{
	Execute_EquipItemToSlot(this, SlotId, ItemDefinition);
}

void UMounteaEquipmentComponent::Server_UnequipItem_Implementation(const FInventoryItem& ItemDefinition, const bool bUseFallbackSlot)
{
	Execute_UnequipItem(this, ItemDefinition, bUseFallbackSlot);
}

AActor* UMounteaEquipmentComponent::EquipItem_Implementation(const FInventoryItem& ItemDefinition)
{
	if (!ItemDefinition.IsItemValid())
	{
		LOG_WARNING(TEXT("[Equip Item]: Item Data not valid. No equipment will happen."))
		return nullptr;
	}

	const UMounteaInventoryItemTemplate* templateAsset = ItemDefinition.GetTemplate();
	if (!IsValid(templateAsset))
	{
		LOG_WARNING(TEXT("[Equip Item]: Item Template not valid. No equipment will happen."))
		return nullptr;
	}
	
	const auto spawnActorClass = templateAsset->SpawnActor;
	if (spawnActorClass.IsNull())
	{
		LOG_WARNING(TEXT("[Equip Item]: Spawn Actor not valid. No equipment will happen."))
		return nullptr;
	}

	UClass* spawnClass = spawnActorClass.LoadSynchronous();
	if (!IsValid(spawnClass))
	{
		LOG_WARNING(TEXT("[Equip Item]: Spawn Actor class failed to load. No equipment will happen."))
		return nullptr;
	}

	if (!UMounteaAttachmentsStatics::IsTargetClassValid(spawnClass))
	{
		LOG_WARNING(TEXT("[Equip Item]: Spawn Actor '%s' does not implement IMounteaAdvancedAttachmentAttachableInterface and has no component that does."),
			*spawnClass->GetName());
		return nullptr;
	}
	
	if (!IsAuthority())
	{
		Server_EquipItem(ItemDefinition);
		return nullptr;
	}
	
	AActor* spawnedActor = nullptr;

	// Try override binding first
	if (OverrideEquipItemFunction.GetMemberName() != NAME_None)
	{
		if (UClass* scopeClass = OverrideEquipItemFunction.GetMemberParentClass())
		{
			if (UFunction* targetFunction = scopeClass->FindFunctionByName(OverrideEquipItemFunction.GetMemberName()))
			{
				struct FParams
				{
					FInventoryItem ItemDefinition;
					AActor* OutSpawnedActor;
					bool ReturnValue;
				};

				FParams params;
				params.ItemDefinition = ItemDefinition;
				params.OutSpawnedActor = nullptr;
				params.ReturnValue = false;

				UObject* targetObject = nullptr;

				if (targetFunction->HasAnyFunctionFlags(FUNC_Static))
					targetObject = scopeClass->GetDefaultObject();
				else
				{
					// Member function: call on something that is-a scopeClass.
					// In our case, we want the component itself or its owner.
					if (const_cast<UMounteaEquipmentComponent*>(this)->IsA(scopeClass))
						targetObject = const_cast<UMounteaEquipmentComponent*>(this);
					else if (AActor* owner = GetOwner(); owner && owner->IsA(scopeClass))
						targetObject = owner;
				}

				if (targetObject)
				{
					targetObject->ProcessEvent(targetFunction, &params);
					if (params.ReturnValue)
						return params.OutSpawnedActor;
				}
			}
		}
	}

	// Fallback
	const bool bResult = UMounteaEquipmentStatics::EquipItemGeneral(this, ItemDefinition, spawnedActor);
	return bResult ? spawnedActor : nullptr;
}

AActor* UMounteaEquipmentComponent::EquipItemToSlot_Implementation(const FName& SlotId, const FInventoryItem& ItemDefinition)
{
	if (SlotId.IsNone())
		return nullptr;
	
	if (!Execute_IsValidSlot(this, SlotId))
		return nullptr;

	if (!IsAuthority())
	{
		Server_EquipItemToSlot(ItemDefinition, SlotId);
		return nullptr;
	}

	UMounteaAdvancedAttachmentSlot* targetSlot = Execute_GetSlot(this, SlotId);
	if (!IsValid(targetSlot))
		return nullptr;

	AActor* spawnedActor = nullptr;
	const bool bResult = UMounteaEquipmentStatics::EquipItemToSlot(this, ItemDefinition, targetSlot, spawnedActor);
	return bResult ? spawnedActor : nullptr;
}

bool UMounteaEquipmentComponent::UnequipItem_Implementation(const FInventoryItem& ItemDefinition, const bool bUseFallbackSlot)
{
	if (!ItemDefinition.IsItemValid())
		return false;

	if (!IsAuthority())
	{
		Server_UnequipItem(ItemDefinition, bUseFallbackSlot);
		return true;
	}

	const TArray<UMounteaAdvancedAttachmentSlot*> attachmentSlots = Execute_GetAttachmentSlots(this);
	for (const UMounteaAdvancedAttachmentSlot* slot : attachmentSlots)
	{
		if (!IsValid(slot) || !slot->IsOccupied() || !IsValid(slot->Attachment))
			continue;

		const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipmentItemInterface = UMounteaEquipmentStatics::FindEquipmentItemInterface(slot->Attachment);
		if (!equipmentItemInterface.GetObject())
			continue;

		if (IMounteaAdvancedEquipmentItemInterface::Execute_GetEquippedItemId(equipmentItemInterface.GetObject()) != ItemDefinition.GetGuid())
		continue;

		return Execute_UnequipItemFromSlot(this, slot->SlotName, bUseFallbackSlot);
	}

	return false;
}

bool UMounteaEquipmentComponent::UnequipItemFromSlot_Implementation(const FName& SlotId, const bool bUseFallbackSlot)
{
	if (SlotId.IsNone())
		return false;
	if (!Execute_IsValidSlot(this, SlotId))
		return false;

	UMounteaAdvancedAttachmentSlot* targetSlot = Execute_GetSlot(this, SlotId);
	if (!IsValid(targetSlot) || !targetSlot->IsOccupied() || !IsValid(targetSlot->Attachment))
		return false;

	UObject* attachmentObject = targetSlot->Attachment;

	if (bUseFallbackSlot)
	{
		const FName fallbackSlotId = UMounteaEquipmentStatics::ResolveFallbackSlotId(SlotId);
		const bool bHasValidFallback = !fallbackSlotId.IsNone() &&
			fallbackSlotId != SlotId &&
			Execute_IsValidSlot(this, fallbackSlotId);

		if (bHasValidFallback)
		{
			UMounteaAdvancedAttachmentSlot* fallbackSlot = Execute_GetSlot(this, fallbackSlotId);
			if (!IsValid(fallbackSlot))
				return false;

			if (UMounteaEquipmentStatics::SwitchEquippedItemSlot(this, targetSlot, fallbackSlot))
			{
				const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface = UMounteaEquipmentStatics::FindEquipmentItemInterface(fallbackSlot->Attachment);
				if (equipItemInterface.GetObject())
					IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), EEquipmentItemState::EES_Equipped);

				return true;
			}

			LOG_WARNING(TEXT("[Unequip Item From Slot]: Failed to switch slots. Falling back to detaching from current slot."));
			return false;
		}
	}

	if (!Execute_TryDetach(this, SlotId))
		return false;

	AActor* detachedAttachmentActor = UMounteaEquipmentStatics::ResolveAttachmentActor(attachmentObject);
	if (IsValid(detachedAttachmentActor) && detachedAttachmentActor != GetOwner())
		detachedAttachmentActor->Destroy();

	return true;
}

bool UMounteaEquipmentComponent::IsEquipmentItemEquipped_Implementation(const FInventoryItem& ItemDefinition) const
{
	return UMounteaEquipmentStatics::ValidateItemEquipped(this, ItemDefinition);
}

bool UMounteaEquipmentComponent::IsEquipmentItemEquippedInSlot_Implementation(const FInventoryItem& ItemDefinition, const FName& SlotName) const
{
	return UMounteaEquipmentStatics::ValidateItemEquipped(this, ItemDefinition, SlotName);
}

bool UMounteaEquipmentComponent::ExecuteEquipmentStateTransition(const FGuid& ItemGuid, const FName& TargetSlotId,
	const EEquipmentItemState ExpectedState, const EEquipmentItemState NewState, const bool bResolveAsActivation,
	const bool bLocalOnly)
{
	UMounteaAdvancedAttachmentSlot* currentSlot = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface;
	if (!UMounteaEquipmentStatics::FindEquippedItemSlotAndInterface(this, ItemGuid, currentSlot, equipItemInterface))
		return false;

	const EEquipmentItemState currentState = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(equipItemInterface.GetObject());
	if (currentState != ExpectedState)
		return false;

	FName resolvedTargetSlotId = TargetSlotId;
	if (resolvedTargetSlotId.IsNone())
	{
		resolvedTargetSlotId = bResolveAsActivation
			? UMounteaEquipmentStatics::ResolveActiveSlotId(currentSlot->SlotName)
			: UMounteaEquipmentStatics::ResolveFallbackSlotId(currentSlot->SlotName);
	}

	if (const bool bNeedsSlotSwitch = !resolvedTargetSlotId.IsNone() && resolvedTargetSlotId != currentSlot->SlotName)
	{
		UMounteaAdvancedAttachmentSlot* targetSlot = Execute_GetSlot(this, resolvedTargetSlotId);
		if (!IsValid(targetSlot))
			return false;

		bool bSwitchSuccess = false;
		if (bLocalOnly)
		{
			if (targetSlot->IsOccupied())
				return false;

			UObject* attachmentObject = currentSlot->Attachment;
			if (!currentSlot->Detach())
				return false;

			if (!targetSlot->Attach(attachmentObject))
			{
				currentSlot->Attach(attachmentObject);
				return false;
			}

			bSwitchSuccess = true;
		}
		else
			bSwitchSuccess = UMounteaEquipmentStatics::SwitchEquippedItemSlot(this, currentSlot, targetSlot);

		if (!bSwitchSuccess)
			return false;

		equipItemInterface = UMounteaEquipmentStatics::FindEquipmentItemInterface(targetSlot->Attachment);
	}

	if (!equipItemInterface.GetObject())
		return false;

	IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), NewState);
	return true;
}

bool UMounteaEquipmentComponent::ResolveEquipmentTransitionContext(const FGuid& ItemGuid, const FName& TargetSlotId,
	const EEquipmentItemState ExpectedState, const bool bResolveAsActivation, UMounteaAdvancedAttachmentSlot*& OutCurrentSlot,
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface>& OutEquipItemInterface, FName& OutResolvedTargetSlotId)
{
	OutCurrentSlot = nullptr;
	OutEquipItemInterface = TScriptInterface<IMounteaAdvancedEquipmentItemInterface>();
	OutResolvedTargetSlotId = TargetSlotId;

	if (!UMounteaEquipmentStatics::FindEquippedItemSlotAndInterface(this, ItemGuid, OutCurrentSlot, OutEquipItemInterface))
		return false;

	const EEquipmentItemState currentState = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(OutEquipItemInterface.GetObject());
	if (currentState != ExpectedState)
		return false;

	if (OutResolvedTargetSlotId.IsNone())
	{
		OutResolvedTargetSlotId = bResolveAsActivation
			? UMounteaEquipmentStatics::ResolveActiveSlotId(OutCurrentSlot->SlotName)
			: UMounteaEquipmentStatics::ResolveFallbackSlotId(OutCurrentSlot->SlotName);
	}

	return true;
}

UAnimInstance* UMounteaEquipmentComponent::ResolveOwnerAnimInstance() const
{
	const AActor* owner = GetOwner();
	if (!IsValid(owner))
		return nullptr;

	const USkeletalMeshComponent* mesh = owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(mesh))
		return nullptr;

	return mesh->GetAnimInstance();
}

bool UMounteaEquipmentComponent::TryStartTransitionMontage(const FInventoryItem& ItemDefinition, UMounteaAdvancedAttachmentSlot* CurrentSlot, 
	const FName& ResolvedTargetSlotId, const TScriptInterface<IMounteaAdvancedEquipmentItemInterface>& EquipItemInterface, const bool bIsActivating)
{
	if (!bIsActivating || !EquipItemInterface.GetObject())
		return false;

	const bool bItemAutoActivates = IMounteaAdvancedEquipmentItemInterface::Execute_DoesAutoActive(EquipItemInterface.GetObject());
	const bool bRequiresEvent = IMounteaAdvancedEquipmentItemInterface::Execute_DoesRequireActivationEvent(EquipItemInterface.GetObject());
	if (bItemAutoActivates || !bRequiresEvent)
		return false;

	UAnimMontage* montage = IMounteaAdvancedEquipmentItemInterface::Execute_GetActivationAnimation(EquipItemInterface.GetObject());
	if (!IsValid(montage))
		return false;

	UAnimInstance* animInstance = ResolveOwnerAnimInstance();
	if (!IsValid(animInstance))
		return false;

	if (const float duration = animInstance->Montage_Play(montage); duration <= 0.f)
		return false;

	PendingActivation.ItemGuid = ItemDefinition.GetGuid();
	PendingActivation.SourceSlotId = IsValid(CurrentSlot) ? CurrentSlot->SlotName : NAME_None;
	PendingActivation.TargetSlotId = ResolvedTargetSlotId;
	PendingActivation.Montage = montage;
	PendingActivation.bIsActivating = bIsActivating;

	FOnMontageEnded endDelegate;
	endDelegate.BindUObject(this, &UMounteaEquipmentComponent::OnActivationMontageEnded);
	animInstance->Montage_SetEndDelegate(endDelegate, montage);
	return true;
}

bool UMounteaEquipmentComponent::ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!ItemDefinition.IsItemValid())
		return false;

	if (PendingActivation.IsValid())
		return false;

	UMounteaAdvancedAttachmentSlot* currentSlot = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface;
	FName resolvedTargetSlotId = TargetSlotId;
	if (!ResolveEquipmentTransitionContext(
		ItemDefinition.GetGuid(),
		TargetSlotId,
		EEquipmentItemState::EES_Equipped,
		true,
		currentSlot,
		equipItemInterface,
		resolvedTargetSlotId))
	{
		return false;
	}

	// Notify-driven flow: slot switch/state change happens in AnimAttachItem.
	if (TryStartTransitionMontage(ItemDefinition, currentSlot, resolvedTargetSlotId, equipItemInterface, true))
		return true;

	if (!IsAuthority())
	{
		Server_ActivateEquipmentItem(ItemDefinition, resolvedTargetSlotId);
		return true;
	}

	return ExecuteEquipmentStateTransition(
		ItemDefinition.GetGuid(),
		resolvedTargetSlotId,
		EEquipmentItemState::EES_Equipped,
		EEquipmentItemState::EES_Active,
		true
	);
}

bool UMounteaEquipmentComponent::DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!ItemDefinition.IsItemValid())
		return false;

	if (PendingActivation.IsValid())
		return false;

	UMounteaAdvancedAttachmentSlot* currentSlot = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface;
	FName resolvedTargetSlotId = TargetSlotId;
	if (!ResolveEquipmentTransitionContext(
		ItemDefinition.GetGuid(),
		TargetSlotId,
		EEquipmentItemState::EES_Active,
		false,
		currentSlot,
		equipItemInterface,
		resolvedTargetSlotId))
	{
		return false;
	}

	if (TryStartTransitionMontage(ItemDefinition, currentSlot, resolvedTargetSlotId, equipItemInterface, false))
		return true;

	if (!IsAuthority())
	{
		Server_DeactivateEquipmentItem(ItemDefinition, resolvedTargetSlotId);
		return true;
	}

	return ExecuteEquipmentStateTransition(
		ItemDefinition.GetGuid(),
		resolvedTargetSlotId,
		EEquipmentItemState::EES_Active,
		EEquipmentItemState::EES_Equipped,
		false
	);
}

void UMounteaEquipmentComponent::Server_ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	Execute_ActivateEquipmentItem(this, ItemDefinition, TargetSlotId);
}

void UMounteaEquipmentComponent::Server_DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	Execute_DeactivateEquipmentItem(this, ItemDefinition, TargetSlotId);
}

void UMounteaEquipmentComponent::Server_AnimAttachItem_Implementation(const FGuid& ItemGuid, const FName& TargetSlotId, const bool bIsActivating)
{
	ExecuteEquipmentStateTransition(
		ItemGuid,
		TargetSlotId,
		bIsActivating ? EEquipmentItemState::EES_Equipped : EEquipmentItemState::EES_Active,
		bIsActivating ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped,
		bIsActivating
	);
}

bool UMounteaEquipmentComponent::AnimAttachItem_Implementation()
{
	if (!PendingActivation.IsValid())
		return false;

	const FPendingEquipmentActivation pendingData = PendingActivation;
	PendingActivation.Reset();

	if (!IsAuthority())
	{
		// Client-side prediction prevents temporary world-space floating before server replication arrives.
		ExecuteEquipmentStateTransition(
			pendingData.ItemGuid,
			pendingData.TargetSlotId,
			pendingData.bIsActivating ? EEquipmentItemState::EES_Equipped : EEquipmentItemState::EES_Active,
			pendingData.bIsActivating ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped,
			pendingData.bIsActivating,
			true
		);

		Server_AnimAttachItem(pendingData.ItemGuid, pendingData.TargetSlotId, pendingData.bIsActivating);
		return true;
	}

	return ExecuteEquipmentStateTransition(
		pendingData.ItemGuid,
		pendingData.TargetSlotId,
		pendingData.bIsActivating ? EEquipmentItemState::EES_Equipped : EEquipmentItemState::EES_Active,
		pendingData.bIsActivating ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped,
		pendingData.bIsActivating
	);
}

void UMounteaEquipmentComponent::OnActivationMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	PendingActivation.Reset();
}

bool UMounteaEquipmentComponent::IsAuthority() const
{
	const AActor* owningActor = GetOwner();
	if (!owningActor || !owningActor->GetWorld())
		return false;

	if (const ENetMode netMode = owningActor->GetWorld()->GetNetMode(); netMode == NM_Standalone)
		return true;
	
	if (owningActor->HasAuthority())
		return true;
		
	return false;
}
