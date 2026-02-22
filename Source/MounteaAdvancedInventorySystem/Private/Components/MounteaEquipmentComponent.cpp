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
		const bool bCanUseFallback = !fallbackSlotId.IsNone() &&
			fallbackSlotId != SlotId &&
			Execute_IsValidSlot(this, fallbackSlotId) &&
			!Execute_IsSlotOccupied(this, fallbackSlotId);

		if (bCanUseFallback)
		{
			const bool bDetached = Execute_TryDetach(this, SlotId);
			if (!bDetached)
				return false;

			if (Execute_TryAttach(this, fallbackSlotId, attachmentObject))
			{
				// Reset equipment state to Equipped when moving to fallback (e.g. Active → Equipped)
				const TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface = UMounteaEquipmentStatics::FindEquipmentItemInterface(attachmentObject);
				if (equipItemInterface.GetObject())
					IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), EEquipmentItemState::EES_Equipped);

				return true;
			}
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

bool UMounteaEquipmentComponent::ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!ItemDefinition.IsItemValid())
		return false;

	if (PendingActivation.IsValid())
		return false;

	// Find the slot where this item currently lives
	UMounteaAdvancedAttachmentSlot* currentSlot = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface;
	if (!UMounteaEquipmentStatics::FindEquippedItemSlotAndInterface(this, ItemDefinition.GetGuid(), currentSlot, equipItemInterface))
		return false;

	const EEquipmentItemState currentState = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(equipItemInterface.GetObject());
	if (currentState != EEquipmentItemState::EES_Equipped)
		return false;

	// Resolve target slot: explicit param > reverse fallback lookup (find which slot uses current as its fallback)
	FName resolvedTargetSlotId = TargetSlotId;
	if (resolvedTargetSlotId.IsNone())
		resolvedTargetSlotId = UMounteaEquipmentStatics::ResolveActiveSlotId(currentSlot->SlotName);

	const bool bNeedsSlotSwitch = !resolvedTargetSlotId.IsNone() && resolvedTargetSlotId != currentSlot->SlotName;

	// Play cosmetic montage if applicable (on any machine — client, server, or standalone)
	const bool bItemAutoActivates = IMounteaAdvancedEquipmentItemInterface::Execute_DoesAutoActive(equipItemInterface.GetObject());
	const bool bRequiresEvent = IMounteaAdvancedEquipmentItemInterface::Execute_DoesRequireActivationEvent(equipItemInterface.GetObject());

	if (!bItemAutoActivates && bRequiresEvent)
	{
		UAnimMontage* montage = IMounteaAdvancedEquipmentItemInterface::Execute_GetActivationAnimation(equipItemInterface.GetObject());
		if (IsValid(montage))
		{
			if (const AActor* owner = GetOwner())
			{
				if (USkeletalMeshComponent* mesh = owner->FindComponentByClass<USkeletalMeshComponent>())
				{
					if (UAnimInstance* animInstance = mesh->GetAnimInstance())
					{
						const float duration = animInstance->Montage_Play(montage);
						if (duration > 0.f)
						{
							PendingActivation.ItemGuid = ItemDefinition.GetGuid();
							PendingActivation.SourceSlotId = currentSlot->SlotName;
							PendingActivation.TargetSlotId = resolvedTargetSlotId;
							PendingActivation.Montage = montage;
							PendingActivation.bIsActivating = true;

							FOnMontageEnded endDelegate;
							endDelegate.BindUObject(this, &UMounteaEquipmentComponent::OnActivationMontageEnded);
							animInstance->Montage_SetEndDelegate(endDelegate, montage);
						}
					}
				}
			}
		}
	}

	if (!IsAuthority())
	{
		Server_ActivateEquipmentItem(ItemDefinition, resolvedTargetSlotId);
		return true;
	}

	// Server/standalone: slot switch (if needed) + state change
	if (bNeedsSlotSwitch)
	{
		UMounteaAdvancedAttachmentSlot* targetSlot = Execute_GetSlot(this, resolvedTargetSlotId);
		if (!IsValid(targetSlot) || !UMounteaEquipmentStatics::SwitchEquippedItemSlot(this, currentSlot, targetSlot))
			return false;

		// Re-resolve interface after re-attachment
		equipItemInterface = UMounteaEquipmentStatics::FindEquipmentItemInterface(targetSlot->Attachment);
	}

	if (equipItemInterface.GetObject())
		IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), EEquipmentItemState::EES_Active);

	return true;
}

bool UMounteaEquipmentComponent::DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	if (!ItemDefinition.IsItemValid())
		return false;

	if (PendingActivation.IsValid())
		return false;

	// Find the slot where this item currently lives
	UMounteaAdvancedAttachmentSlot* currentSlot = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> equipItemInterface;
	if (!UMounteaEquipmentStatics::FindEquippedItemSlotAndInterface(this, ItemDefinition.GetGuid(), currentSlot, equipItemInterface))
		return false;

	const EEquipmentItemState currentState = IMounteaAdvancedEquipmentItemInterface::Execute_GetEquipmentItemState(equipItemInterface.GetObject());
	if (currentState != EEquipmentItemState::EES_Active)
		return false;

	// Resolve target slot: explicit param > fallback lookup from current slot
	FName resolvedTargetSlotId = TargetSlotId;
	if (resolvedTargetSlotId.IsNone())
		resolvedTargetSlotId = UMounteaEquipmentStatics::ResolveFallbackSlotId(currentSlot->SlotName);

	const bool bNeedsSlotSwitch = !resolvedTargetSlotId.IsNone() && resolvedTargetSlotId != currentSlot->SlotName;

	if (!IsAuthority())
	{
		Server_DeactivateEquipmentItem(ItemDefinition, resolvedTargetSlotId);
		return true;
	}

	// Server/standalone: slot switch (if needed) + state change
	if (bNeedsSlotSwitch)
	{
		UMounteaAdvancedAttachmentSlot* targetSlot = Execute_GetSlot(this, resolvedTargetSlotId);
		if (!IsValid(targetSlot) || !UMounteaEquipmentStatics::SwitchEquippedItemSlot(this, currentSlot, targetSlot))
			return false;

		// Re-resolve interface after re-attachment
		equipItemInterface = UMounteaEquipmentStatics::FindEquipmentItemInterface(targetSlot->Attachment);
	}

	if (equipItemInterface.GetObject())
		IMounteaAdvancedEquipmentItemInterface::Execute_SetEquipmentItemState(equipItemInterface.GetObject(), EEquipmentItemState::EES_Equipped);

	return true;
}

void UMounteaEquipmentComponent::Server_ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	Execute_ActivateEquipmentItem(this, ItemDefinition, TargetSlotId);
}

void UMounteaEquipmentComponent::Server_DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId)
{
	Execute_DeactivateEquipmentItem(this, ItemDefinition, TargetSlotId);
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
