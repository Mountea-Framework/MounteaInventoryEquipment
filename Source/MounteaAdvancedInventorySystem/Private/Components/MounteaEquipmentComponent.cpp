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
#include "Interfaces/Equipment/MounteaAdvancedEquipmentItemInterface.h"
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

bool UMounteaEquipmentComponent::BuildEquipmentTransitionContext(const FGuid& ItemGuid, const FName& TargetSlotId,
	const EEquipmentItemState ExpectedState, const EEquipmentItemState NewState, const bool bResolveAsActivation,
	FEquipmentTransitionContext& OutContext)
{
	OutContext = FEquipmentTransitionContext();
	OutContext.ItemGuid = ItemGuid;
	OutContext.ResolvedTargetSlotId = TargetSlotId;
	OutContext.ExpectedState = ExpectedState;
	OutContext.NewState = NewState;
	OutContext.bResolveAsActivation = bResolveAsActivation;

	UMounteaAdvancedAttachmentSlot* currentSlotRaw = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface;
	if (!UMounteaEquipmentStatics::FindEquippedItemSlotAndInterface(this, ItemGuid, currentSlotRaw, equipItemInterface))
		return false;

	OutContext.CurrentSlot = currentSlotRaw;
	OutContext.EquipItemInterface = equipItemInterface;

	const EEquipmentItemState currentState = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(equipItemInterface.GetObject());
	if (currentState != ExpectedState)
		return false;

	if (OutContext.ResolvedTargetSlotId.IsNone())
	{
		OutContext.ResolvedTargetSlotId = bResolveAsActivation
			? UMounteaEquipmentStatics::ResolveActiveSlotId(currentSlotRaw->SlotName)
			: UMounteaEquipmentStatics::ResolveFallbackSlotId(currentSlotRaw->SlotName);
	}

	OutContext.bNeedsSlotSwitch = !OutContext.ResolvedTargetSlotId.IsNone() &&
		OutContext.ResolvedTargetSlotId != currentSlotRaw->SlotName;
	if (OutContext.bNeedsSlotSwitch)
	{
		OutContext.TargetSlot = Execute_GetSlot(this, OutContext.ResolvedTargetSlotId);
		if (!IsValid(OutContext.TargetSlot))
			return false;
	}

	return true;
}

bool UMounteaEquipmentComponent::ExecuteEquipmentStateTransition(const FEquipmentTransitionContext& Context, const bool bLocalOnly)
{
	if (!IsValid(Context.CurrentSlot) || !Context.EquipItemInterface.GetObject())
		return false;

	const EEquipmentItemState currentState = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(Context.EquipItemInterface.GetObject());
	if (currentState != Context.ExpectedState)
		return false;

	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface = Context.EquipItemInterface;
	if (Context.bNeedsSlotSwitch)
	{
		if (!IsValid(Context.TargetSlot))
			return false;

		bool bSwitchSuccess = false;
		if (bLocalOnly)
		{
			if (Context.TargetSlot->IsOccupied())
				return false;

			UObject* attachmentObject = Context.CurrentSlot->Attachment;
			if (!Context.CurrentSlot->Detach())
				return false;

			if (!Context.TargetSlot->Attach(attachmentObject))
			{
				Context.CurrentSlot->Attach(attachmentObject);
				return false;
			}

			bSwitchSuccess = true;
		}
		else
			bSwitchSuccess = UMounteaEquipmentStatics::SwitchEquippedItemSlot(this, Context.CurrentSlot, Context.TargetSlot);

		if (!bSwitchSuccess)
			return false;

		equipItemInterface = UMounteaEquipmentStatics::FindEquipmentItemInterface(Context.TargetSlot->Attachment);
	}

	if (!equipItemInterface.GetObject())
		return false;

	IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), Context.NewState);
	return true;
}

bool UMounteaEquipmentComponent::ShouldUseDeferredTransition(const FEquipmentTransitionContext& Context, const bool bIsActivating) const
{
	if (!bIsActivating || !Context.EquipItemInterface.GetObject())
		return false;

	const bool bItemAutoActivates = IMounteaAdvancedEquipmentItemInterface::Execute_DoesAutoActive(Context.EquipItemInterface.GetObject());
	const bool bRequiresEvent = IMounteaAdvancedEquipmentItemInterface::Execute_DoesRequireActivationEvent(Context.EquipItemInterface.GetObject());
	return !bItemAutoActivates && bRequiresEvent;
}

void UMounteaEquipmentComponent::ArmPendingActivation(const FInventoryItem& ItemDefinition, const FEquipmentTransitionContext& Context,
	const bool bIsActivating, UAnimMontage* Montage)
{
	PendingActivation.ItemGuid = ItemDefinition.GetGuid();
	PendingActivation.SourceSlotId = IsValid(Context.CurrentSlot) ? Context.CurrentSlot->SlotName : NAME_None;
	PendingActivation.TargetSlotId = Context.ResolvedTargetSlotId;
	PendingActivation.Montage = Montage;
	PendingActivation.bIsActivating = bIsActivating;
	PendingActivation.RequestedAtTimeSeconds = GetOwner() && GetOwner()->GetWorld()
		? GetOwner()->GetWorld()->GetTimeSeconds()
		: -1.0;
}

bool UMounteaEquipmentComponent::IsPendingActivationExpired() const
{
	if (!PendingActivation.IsValid())
		return false;

	if (PendingActivationTimeoutSeconds <= 0.f)
		return false;

	if (PendingActivation.RequestedAtTimeSeconds < 0.0)
		return false;

	const AActor* owningActor = GetOwner();
	if (!owningActor || !owningActor->GetWorld())
		return false;

	const double elapsed = owningActor->GetWorld()->GetTimeSeconds() - PendingActivation.RequestedAtTimeSeconds;
	return elapsed >= PendingActivationTimeoutSeconds;
}

void UMounteaEquipmentComponent::ResetPendingActivationIfExpired()
{
	if (IsPendingActivationExpired())
		PendingActivation.Reset();
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

bool UMounteaEquipmentComponent::TryStartTransitionMontage(const FInventoryItem& ItemDefinition, const FEquipmentTransitionContext& Context, const bool bIsActivating)
{
	if (!bIsActivating || !Context.EquipItemInterface.GetObject())
		return false;

	UAnimMontage* montage = IMounteaAdvancedEquipmentItemInterface::Execute_GetActivationAnimation(Context.EquipItemInterface.GetObject());
	if (!IsValid(montage))
		return false;

	UAnimInstance* animInstance = ResolveOwnerAnimInstance();
	if (!IsValid(animInstance))
		return false;

	if (const float duration = animInstance->Montage_Play(montage); duration <= 0.f)
		return false;

	ArmPendingActivation(ItemDefinition, Context, bIsActivating, montage);

	FOnMontageEnded endDelegate;
	endDelegate.BindUObject(this, &UMounteaEquipmentComponent::OnActivationMontageEnded);
	animInstance->Montage_SetEndDelegate(endDelegate, montage);
	return true;
}

bool UMounteaEquipmentComponent::ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	ResetPendingActivationIfExpired();

	if (!ItemDefinition.IsItemValid())
		return false;

	if (PendingActivation.IsValid())
		return false;

	FEquipmentTransitionContext transitionContext;
	if (!BuildEquipmentTransitionContext(
		ItemDefinition.GetGuid(),
		TargetSlotId,
		EEquipmentItemState::EES_Equipped,
		EEquipmentItemState::EES_Active,
		true,
		transitionContext))
	{
		return false;
	}

	if (ShouldUseDeferredTransition(transitionContext, true))
	{
		// Montage-driven flow: transition is committed by AnimAttachItem notify callback.
		if (TryStartTransitionMontage(ItemDefinition, transitionContext, true))
			return true;

		UAnimMontage* activationMontage = nullptr;
		if (transitionContext.EquipItemInterface.GetObject())
			activationMontage = IMounteaAdvancedEquipmentItemInterface::Execute_GetActivationAnimation(transitionContext.EquipItemInterface.GetObject());

		// Procedural flow: no montage configured, so external animation system is expected to call AnimAttachItem manually.
		if (!IsValid(activationMontage))
		{
			ArmPendingActivation(ItemDefinition, transitionContext, true);
			return true;
		}

		LOG_WARNING(TEXT("[Activate Equipment Item]: Failed to play activation montage. Falling back to immediate transition."))
	}

	if (!IsAuthority())
	{
		Server_ActivateEquipmentItem(ItemDefinition, transitionContext.ResolvedTargetSlotId);
		return true;
	}

	return ExecuteEquipmentStateTransition(transitionContext);
}

bool UMounteaEquipmentComponent::DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	ResetPendingActivationIfExpired();

	if (!ItemDefinition.IsItemValid())
		return false;

	if (PendingActivation.IsValid())
		return false;

	FEquipmentTransitionContext transitionContext;
	if (!BuildEquipmentTransitionContext(
		ItemDefinition.GetGuid(),
		TargetSlotId,
		EEquipmentItemState::EES_Active,
		EEquipmentItemState::EES_Equipped,
		false,
		transitionContext))
	{
		return false;
	}

	if (TryStartTransitionMontage(ItemDefinition, transitionContext, false))
		return true;

	if (!IsAuthority())
	{
		Server_DeactivateEquipmentItem(ItemDefinition, transitionContext.ResolvedTargetSlotId);
		return true;
	}

	return ExecuteEquipmentStateTransition(transitionContext);
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
	FEquipmentTransitionContext transitionContext;
	if (!BuildEquipmentTransitionContext(
		ItemGuid,
		TargetSlotId,
		bIsActivating ? EEquipmentItemState::EES_Equipped : EEquipmentItemState::EES_Active,
		bIsActivating ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped,
		bIsActivating,
		transitionContext))
	{
		return;
	}

	ExecuteEquipmentStateTransition(transitionContext);
}

bool UMounteaEquipmentComponent::AnimAttachItem_Implementation()
{
	ResetPendingActivationIfExpired();

	if (!PendingActivation.IsValid())
		return false;

	const FPendingEquipmentActivation pendingData = PendingActivation;
	PendingActivation.Reset();

	if (!IsAuthority())
	{
		// Client-side prediction prevents temporary world-space floating before server replication arrives.
		FEquipmentTransitionContext transitionContext;
		if (BuildEquipmentTransitionContext(
			pendingData.ItemGuid,
			pendingData.TargetSlotId,
			pendingData.bIsActivating ? EEquipmentItemState::EES_Equipped : EEquipmentItemState::EES_Active,
			pendingData.bIsActivating ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped,
			pendingData.bIsActivating,
			transitionContext))
		{
			ExecuteEquipmentStateTransition(transitionContext, true);
		}

		Server_AnimAttachItem(pendingData.ItemGuid, pendingData.TargetSlotId, pendingData.bIsActivating);
		return true;
	}

	FEquipmentTransitionContext transitionContext;
	if (!BuildEquipmentTransitionContext(
		pendingData.ItemGuid,
		pendingData.TargetSlotId,
		pendingData.bIsActivating ? EEquipmentItemState::EES_Equipped : EEquipmentItemState::EES_Active,
		pendingData.bIsActivating ? EEquipmentItemState::EES_Active : EEquipmentItemState::EES_Equipped,
		pendingData.bIsActivating,
		transitionContext))
	{
		return false;
	}

	return ExecuteEquipmentStateTransition(transitionContext);
}

bool UMounteaEquipmentComponent::TryGetPendingEquipmentActivation(FPendingEquipmentActivation& OutPendingActivation) const
{
	if (IsPendingActivationExpired())
	{
		OutPendingActivation = FPendingEquipmentActivation();
		return false;
	}

	OutPendingActivation = PendingActivation;
	return OutPendingActivation.IsValid();
}

void UMounteaEquipmentComponent::OnActivationMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	(void)Montage;
	(void)bInterrupted;
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
