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
#include "Components/SkeletalMeshComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentItemInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
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

bool UMounteaEquipmentComponent::ValidateEquipRequest(const FInventoryItem& ItemDefinition) const
{
	if (!ItemDefinition.IsItemValid())
	{
		LOG_WARNING(TEXT("[Equip Item]: Item Data not valid. No equipment will happen."))
		return false;
	}

	const UMounteaInventoryItemTemplate* templateAsset = ItemDefinition.GetTemplate();
	if (!IsValid(templateAsset))
	{
		LOG_WARNING(TEXT("[Equip Item]: Item Template not valid. No equipment will happen."))
		return false;
	}
	
	const auto spawnActorClass = templateAsset->SpawnActor;
	if (spawnActorClass.IsNull())
	{
		LOG_WARNING(TEXT("[Equip Item]: Spawn Actor not valid. No equipment will happen."))
		return false;
	}

	UClass* spawnClass = spawnActorClass.LoadSynchronous();
	if (!IsValid(spawnClass))
	{
		LOG_WARNING(TEXT("[Equip Item]: Spawn Actor class failed to load. No equipment will happen."))
		return false;
	}

	if (!UMounteaEquipmentStatics::IsTargetClassValid(spawnClass))
	{
		LOG_WARNING(TEXT("[Equip Item]: Spawn Actor '%s' is not a valid equipment item class."), *spawnClass->GetName());
		return false;
	}

	return true;
}

bool UMounteaEquipmentComponent::TryExecuteEquipOverride(const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor) const
{
	OutSpawnedActor = nullptr;
	if (OverrideEquipItemFunction.GetMemberName() == NAME_None)
		return false;

	UClass* scopeClass = OverrideEquipItemFunction.GetMemberParentClass();
	if (!IsValid(scopeClass))
		return false;

	UFunction* targetFunction = scopeClass->FindFunctionByName(OverrideEquipItemFunction.GetMemberName());
	if (!IsValid(targetFunction))
		return false;

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
	{
		targetObject = scopeClass->GetDefaultObject();
	}
	else
	{
		UMounteaEquipmentComponent* mutableThis = const_cast<UMounteaEquipmentComponent*>(this);
		if (mutableThis->IsA(scopeClass))
		{
			targetObject = mutableThis;
		}
		else if (AActor* owner = GetOwner(); IsValid(owner) && owner->IsA(scopeClass))
		{
			targetObject = owner;
		}
	}

	if (!IsValid(targetObject))
		return false;

	targetObject->ProcessEvent(targetFunction, &params);
	OutSpawnedActor = params.OutSpawnedActor;
	return params.ReturnValue;
}

AActor* UMounteaEquipmentComponent::EquipItem_Implementation(const FInventoryItem& ItemDefinition)
{
	if (!ValidateEquipRequest(ItemDefinition))
		return nullptr;
	
	if (!IsAuthority())
	{
		Server_EquipItem(ItemDefinition);
		return nullptr;
	}
	
	AActor* spawnedActor = nullptr;
	if (TryExecuteEquipOverride(ItemDefinition, spawnedActor))
		return spawnedActor;

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

bool UMounteaEquipmentComponent::ActivateQuickUseItem_Implementation(const FName& SlotId, const FName& TargetSlotId)
{
	if (SlotId.IsNone())
		return false;

	FGuid itemGuid;
	if (!UMounteaEquipmentStatics::TryGetEquippedItemGuidFromSlot(this, SlotId, itemGuid))
		return false;

	FInventoryItem quickUseItemDefinition;
	if (!UMounteaEquipmentStatics::TryResolveInventoryItemByGuid(this, itemGuid, quickUseItemDefinition))
	{
		LOG_WARNING(TEXT("[Activate Quick Use Item]: Failed to resolve inventory item definition for equipped item guid '%s'."),
			*itemGuid.ToString());
		return false;
	}

	const FName resolvedActiveSlotId = UMounteaEquipmentStatics::ResolveActiveSlotId(SlotId);
	const FName resolvedVisualSlotId = !TargetSlotId.IsNone()
		? TargetSlotId
		: (!resolvedActiveSlotId.IsNone() ? resolvedActiveSlotId : SlotId);

	AActor* quickUsePlaceholderActor = UMounteaEquipmentStatics::SpawnQuickUsePlaceholderActor(
		this,
		quickUseItemDefinition,
		resolvedVisualSlotId);
	const bool bRegisteredQuickUsePlaceholder = IsValid(quickUsePlaceholderActor)
		&& RegisterQuickUsePlaceholderActor(itemGuid, quickUsePlaceholderActor);

	if (IsValid(quickUsePlaceholderActor) && !bRegisteredQuickUsePlaceholder)
	{
		LOG_WARNING(TEXT("[Activate Quick Use Item]: Failed to register quick-use placeholder actor for '%s'. Actor will be kept alive temporarily."),
			*itemGuid.ToString());
		quickUsePlaceholderActor->SetLifeSpan(3.0f);
	}

	const bool bActivated = Execute_ActivateEquipmentItem(this, quickUseItemDefinition, resolvedVisualSlotId);
	if (!bActivated && bRegisteredQuickUsePlaceholder)
		ConsumeQuickUsePlaceholderActor(itemGuid, true);

	return bActivated;
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

bool UMounteaEquipmentComponent::BuildTransitionContextForType(const FGuid& ItemGuid, const FName& TargetSlotId,
	const EEquipmentTransitionType TransitionType, FEquipmentTransitionContext& OutContext)
{
	EEquipmentItemState expectedState = EEquipmentItemState::EES_Idle;
	EEquipmentItemState newState = EEquipmentItemState::EES_Idle;
	bool bResolveAsActivation = false;
	if (!ResolveTransitionDefinition(TransitionType, expectedState, newState, bResolveAsActivation))
		return false;

	return BuildEquipmentTransitionContext(
		ItemGuid,
		TargetSlotId,
		expectedState,
		newState,
		bResolveAsActivation,
		OutContext);
}

bool UMounteaEquipmentComponent::ResolveTransitionDefinition(const EEquipmentTransitionType TransitionType,
	EEquipmentItemState& OutExpectedState, EEquipmentItemState& OutNewState, bool& OutResolveAsActivation) const
{
	switch (TransitionType)
	{
		case EEquipmentTransitionType::EET_Activate:
			OutExpectedState = EEquipmentItemState::EES_Equipped;
			OutNewState = EEquipmentItemState::EES_Active;
			OutResolveAsActivation = true;
			return true;
		case EEquipmentTransitionType::EET_Deactivate:
			OutExpectedState = EEquipmentItemState::EES_Active;
			OutNewState = EEquipmentItemState::EES_Equipped;
			OutResolveAsActivation = false;
			return true;
		default:
			return false;
	}
}

bool UMounteaEquipmentComponent::ExecuteEquipmentStateTransition(const FEquipmentTransitionContext& Context, const bool bLocalOnly)
{
	if (!IsValid(Context.CurrentSlot) || !Context.EquipItemInterface.GetObject())
		return false;

	const EEquipmentItemState currentState = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(Context.EquipItemInterface.GetObject());
	if (currentState != Context.ExpectedState)
		return false;

	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface = Context.EquipItemInterface;
	if (!bLocalOnly && Context.NewState == EEquipmentItemState::EES_Active)
	{
		TSet<FName> ignoredSlots;
		if (IsValid(Context.CurrentSlot))
			ignoredSlots.Add(Context.CurrentSlot->SlotName);
		if (IsValid(Context.TargetSlot))
			ignoredSlots.Add(Context.TargetSlot->SlotName);

		if (!UMounteaEquipmentStatics::ClearBlockedSlotsForAttachment(
			this,
			Context.CurrentSlot->Attachment,
			Context.ItemGuid,
			ignoredSlots))
		{
			return false;
		}
	}

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

bool UMounteaEquipmentComponent::ShouldUseDeferredTransition(const FEquipmentTransitionContext& Context,
	const EEquipmentTransitionType TransitionType) const
{
	if (TransitionType != EEquipmentTransitionType::EET_Activate && TransitionType != EEquipmentTransitionType::EET_Deactivate)
		return false;

	if (!Context.EquipItemInterface.GetObject())
		return false;

	const bool bItemAutoActivates = IMounteaAdvancedEquipmentItemInterface::Execute_DoesAutoActive(Context.EquipItemInterface.GetObject());
	const bool bRequiresEvent = IMounteaAdvancedEquipmentItemInterface::Execute_DoesRequireActivationEvent(Context.EquipItemInterface.GetObject());
	return !bItemAutoActivates && bRequiresEvent;
}

void UMounteaEquipmentComponent::ArmPendingActivation(const FInventoryItem& ItemDefinition, const FEquipmentTransitionContext& Context,
	const EEquipmentTransitionType TransitionType, UAnimMontage* Montage, const float MontageDuration)
{
	PendingActivation.ItemGuid = ItemDefinition.GetGuid();
	PendingActivation.SourceSlotId = IsValid(Context.CurrentSlot) ? Context.CurrentSlot->SlotName : NAME_None;
	PendingActivation.TargetSlotId = Context.ResolvedTargetSlotId;
	PendingActivation.Montage = Montage;
	PendingActivation.TransitionType = TransitionType;
	PendingActivation.TimeoutSeconds = ResolvePendingTransitionTimeout(Montage, MontageDuration);
	PendingActivation.RequestedAtTimeSeconds = GetOwner() && GetOwner()->GetWorld()
		? GetOwner()->GetWorld()->GetTimeSeconds()
		: -1.0;
	SetCurrentTransitionType(TransitionType);
}

double UMounteaEquipmentComponent::ResolvePendingTransitionTimeout(UAnimMontage* Montage, const float MontageDuration) const
{
	double timeoutSeconds = PendingActivationTimeoutSeconds;
	if (MontageDuration > 0.f)
		timeoutSeconds = MontageDuration;
	else if (IsValid(Montage))
		timeoutSeconds = Montage->GetPlayLength();

	if (timeoutSeconds <= 0.0)
		return timeoutSeconds;

	const double marginMultiplier = 1.0 + FMath::Max(0.0, static_cast<double>(PendingActivationTimeoutMarginPercent));
	return timeoutSeconds * marginMultiplier;
}

bool UMounteaEquipmentComponent::IsPendingActivationExpired() const
{
	// TODO: Investigate how to protect properly animation based workflow.
	// Timeout expiration is intentionally disabled for now.
	// Pending transitions are consumed by the first AnimAttachItem call.
	return false;
}

void UMounteaEquipmentComponent::ResetPendingActivationIfExpired()
{
	if (IsPendingActivationExpired())
	{
		PendingActivation.Reset();
		ResetCurrentTransitionType();
	}
}

bool UMounteaEquipmentComponent::IsTransitionInProgress(const EEquipmentTransitionType RequestedTransitionType) const
{
	if (PendingActivation.IsValid())
	{
		if (RequestedTransitionType == EEquipmentTransitionType::EET_None)
			return true;

		return PendingActivation.TransitionType == RequestedTransitionType;
	}

	if (CurrentTransitionType == EEquipmentTransitionType::EET_None)
		return false;

	if (RequestedTransitionType == EEquipmentTransitionType::EET_None)
		return true;

	return CurrentTransitionType == RequestedTransitionType;
}

void UMounteaEquipmentComponent::SetCurrentTransitionType(const EEquipmentTransitionType NewTransitionType)
{
	CurrentTransitionType = NewTransitionType;
}

void UMounteaEquipmentComponent::ResetCurrentTransitionType()
{
	CurrentTransitionType = EEquipmentTransitionType::EET_None;
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

UAnimMontage* UMounteaEquipmentComponent::ResolveTransitionMontage(const FEquipmentTransitionContext& Context,
	const EEquipmentTransitionType TransitionType) const
{
	if (!Context.EquipItemInterface.GetObject())
		return nullptr;

	UObject* itemObject = Context.EquipItemInterface.GetObject();
	switch (TransitionType)
	{
	case EEquipmentTransitionType::EET_Activate:
		return IMounteaAdvancedEquipmentItemInterface::Execute_GetActivationAnimation(itemObject);
	case EEquipmentTransitionType::EET_Deactivate:
		if (IMounteaAdvancedEquipmentItemInterface::Execute_DoesUseDeactivationAnimation(itemObject))
		{
			if (UAnimMontage* deactivationMontage = IMounteaAdvancedEquipmentItemInterface::Execute_GetDeactivationAnimation(itemObject))
				return deactivationMontage;
		}
		return IMounteaAdvancedEquipmentItemInterface::Execute_GetActivationAnimation(itemObject);
	default:
		return nullptr;
	}
}

bool UMounteaEquipmentComponent::TryStartTransitionMontage(const FInventoryItem& ItemDefinition,
	const FEquipmentTransitionContext& Context, const EEquipmentTransitionType TransitionType)
{
	if (!ShouldUseDeferredTransition(Context, TransitionType))
		return false;

	UAnimMontage* montage = ResolveTransitionMontage(Context, TransitionType);
	if (!IsValid(montage))
		return false;

	UAnimInstance* animInstance = ResolveOwnerAnimInstance();
	if (!IsValid(animInstance))
		return false;

	const float duration = animInstance->Montage_Play(montage);
	if (duration <= 0.f)
		return false;

	ArmPendingActivation(ItemDefinition, Context, TransitionType, montage, duration);

	FOnMontageEnded endDelegate;
	endDelegate.BindUObject(this, &UMounteaEquipmentComponent::OnTransitionMontageEnded);
	animInstance->Montage_SetEndDelegate(endDelegate, montage);
	return true;
}

bool UMounteaEquipmentComponent::ExecuteItemTransitionRequest(const FInventoryItem& ItemDefinition,
	const FName& TargetSlotId, const EEquipmentTransitionType TransitionType)
{
	ResetPendingActivationIfExpired();

	if (!ItemDefinition.IsItemValid())
		return false;

	if (IsTransitionInProgress())
		return false;

	FEquipmentTransitionContext transitionContext;
	if (!BuildTransitionContextForType(ItemDefinition.GetGuid(), TargetSlotId, TransitionType, transitionContext))
		return false;

	if (ShouldUseDeferredTransition(transitionContext, TransitionType))
	{
		if (TryStartTransitionMontage(ItemDefinition, transitionContext, TransitionType))
			return true;

		UAnimMontage* transitionMontage = ResolveTransitionMontage(transitionContext, TransitionType);
		if (!IsValid(transitionMontage))
		{
			ArmPendingActivation(ItemDefinition, transitionContext, TransitionType);
			return true;
		}

		const TCHAR* transitionLabel = TransitionType == EEquipmentTransitionType::EET_Activate
			? TEXT("Activate Equipment Item")
			: TEXT("Deactivate Equipment Item");
		LOG_WARNING(TEXT("[%s]: Failed to play transition montage. Falling back to immediate transition."), transitionLabel)
	}

	if (!IsAuthority())
	{
		if (TransitionType == EEquipmentTransitionType::EET_Activate)
			Server_ActivateEquipmentItem(ItemDefinition, transitionContext.ResolvedTargetSlotId);
		else if (TransitionType == EEquipmentTransitionType::EET_Deactivate)
			Server_DeactivateEquipmentItem(ItemDefinition, transitionContext.ResolvedTargetSlotId);
		else
			return false;

		return true;
	}

	return ExecuteEquipmentStateTransition(transitionContext);
}

bool UMounteaEquipmentComponent::ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	return ExecuteItemTransitionRequest(ItemDefinition, TargetSlotId, EEquipmentTransitionType::EET_Activate);
}

bool UMounteaEquipmentComponent::DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	return ExecuteItemTransitionRequest(ItemDefinition, TargetSlotId, EEquipmentTransitionType::EET_Deactivate);
}

void UMounteaEquipmentComponent::Server_ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	Execute_ActivateEquipmentItem(this, ItemDefinition, TargetSlotId);
}

void UMounteaEquipmentComponent::Server_DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	Execute_DeactivateEquipmentItem(this, ItemDefinition, TargetSlotId);
}

void UMounteaEquipmentComponent::Server_AnimAttachItem_Implementation(const FGuid& ItemGuid, const FName& TargetSlotId,
	const EEquipmentTransitionType TransitionType)
{
	FEquipmentTransitionContext transitionContext;
	if (!BuildTransitionContextForType(ItemGuid, TargetSlotId, TransitionType, transitionContext))
		return;

	ExecuteEquipmentStateTransition(transitionContext);
}

void UMounteaEquipmentComponent::Server_AnimQuickItemUsed_Implementation(const FGuid& ItemGuid)
{
	ConsumeQuickUsePlaceholderActor(ItemGuid, false);
}

bool UMounteaEquipmentComponent::AnimAttachItem_Implementation()
{
	ResetPendingActivationIfExpired();

	if (!PendingActivation.IsValid())
		return false;

	const FPendingEquipmentActivation pendingData = PendingActivation;
	PendingActivation.Reset();
	ResetCurrentTransitionType();

	if (!IsAuthority())
	{
		// Client-side prediction prevents temporary world-space floating before server replication arrives.
		FEquipmentTransitionContext transitionContext;
		if (BuildTransitionContextForType(
			pendingData.ItemGuid,
			pendingData.TargetSlotId,
			pendingData.TransitionType,
			transitionContext))
		{
			ExecuteEquipmentStateTransition(transitionContext, true);
		}

		Server_AnimAttachItem(pendingData.ItemGuid, pendingData.TargetSlotId, pendingData.TransitionType);
		return true;
	}

	FEquipmentTransitionContext transitionContext;
	if (!BuildTransitionContextForType(
		pendingData.ItemGuid,
		pendingData.TargetSlotId,
		pendingData.TransitionType,
		transitionContext))
	{
		return false;
	}

	return ExecuteEquipmentStateTransition(transitionContext);
}

bool UMounteaEquipmentComponent::AnimQuickItemUsed_Implementation()
{
	const FGuid quickUseItemGuid = QuickUsePlaceholderItemGuid;
	const bool bConsumed = ConsumeQuickUsePlaceholderActor(quickUseItemGuid, true);
	if (!bConsumed)
		return false;

	if (!IsAuthority())
		Server_AnimQuickItemUsed(quickUseItemGuid);

	return true;
}

bool UMounteaEquipmentComponent::RegisterQuickUsePlaceholderActor(const FGuid& ItemGuid, AActor* PlaceholderActor)
{
	if (!ItemGuid.IsValid() || !IsValid(PlaceholderActor))
		return false;

	if (PlaceholderActor == GetOwner())
		return false;

	if (QuickUsePlaceholderActor.IsValid() && QuickUsePlaceholderActor.Get() != PlaceholderActor)
	{
		LOG_WARNING(TEXT("[Register Quick Use Placeholder Actor]: Replacing existing quick use placeholder actor."))
		if (AActor* existingPlaceholder = QuickUsePlaceholderActor.Get(); IsValid(existingPlaceholder))
			existingPlaceholder->Destroy();
	}

	QuickUsePlaceholderActor = PlaceholderActor;
	QuickUsePlaceholderItemGuid = ItemGuid;
	return true;
}

bool UMounteaEquipmentComponent::ConsumeQuickUsePlaceholderActor(const FGuid& ItemGuid, const bool bIgnoreItemGuidMismatch)
{
	AActor* placeholderActor = QuickUsePlaceholderActor.Get();
	if (!IsValid(placeholderActor))
		return false;

	if (!bIgnoreItemGuidMismatch && ItemGuid.IsValid() && QuickUsePlaceholderItemGuid.IsValid() && ItemGuid != QuickUsePlaceholderItemGuid)
		return false;

	QuickUsePlaceholderActor.Reset();
	QuickUsePlaceholderItemGuid.Invalidate();

	if (placeholderActor != GetOwner())
		placeholderActor->Destroy();

	return true;
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

void UMounteaEquipmentComponent::OnTransitionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	PendingActivation.Reset();
	ResetCurrentTransitionType();
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
