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

#include "Decorations/MounteaInventoryItemAction.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventoryStatics.h"

UMounteaInventoryItemAction::UMounteaInventoryItemAction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	ItemActionData.ActionDisplayName = FText::FromString(TEXT("Default Action"));
	ItemActionData.ActionDescription = FText::FromString(TEXT("A basic inventory item action"));
	ItemActionData.ActionPriority = 0;
	ItemActionData.bIsVisibleByDefault = true;
	ItemActionData.bRequiresConfirmation = false;
}

bool UMounteaInventoryItemAction::InitializeItemAction_Implementation(const FInventoryItem& NewTargetItem,
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewOwningInventory, UObject* ContextPayload)
{
	if (!NewTargetItem.IsItemValid() || NewTargetItem.OwningInventory != NewOwningInventory || !IsValid(NewOwningInventory.GetObject()))
		return false;
	
	CurrentTargetItem = NewTargetItem;

	return true;
}

bool UMounteaInventoryItemAction::IsActionVisible_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	return ItemActionData.bIsVisibleByDefault;
}

bool UMounteaInventoryItemAction::IsAllowed_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	if (!IsActionVisible(TargetItem))
		return false;

	if (!IsValid(TargetItem.Template))
		return false;

	auto allowedActions = UMounteaInventoryStatics::GetItemActions(CurrentTargetItem);

	const bool bFound = Algo::FindByPredicate(allowedActions, [this](const TSoftClassPtr<UObject>& action)
	{
		return action == GetClass();
	}) != nullptr;

	return bFound;
}

FText UMounteaInventoryItemAction::GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return FText::FromString(TEXT("Invalid target item"));

	return FText::FromString(TEXT("Action is not currently available"));
}

bool UMounteaInventoryItemAction::ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem)
{
	UAbilitySystemComponent* asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
	{
		LOG_ERROR(TEXT("[%s] No AbilitySystemComponent found"), *GetName())
		return false;
	}

	if (!TargetItem.IsItemValid())
	{
		LOG_ERROR(TEXT("[%s] Invalid target item"), *GetName())
		return false;
	}

	CurrentTargetItem = TargetItem;

	FGameplayEventData eventData;
	eventData.Instigator = GetAvatarActorFromActorInfo();
	eventData.Target = GetAvatarActorFromActorInfo();
	
	eventData.ContextHandle = FGameplayEffectContextHandle();
	if (eventData.ContextHandle.IsValid())
	{
		eventData.ContextHandle.Get()->AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	}

	FGameplayTag eventTag = FGameplayTag::RequestGameplayTag("Ability.Event.InventoryAction");

	FGameplayAbilitySpec* spec = asc->FindAbilitySpecFromClass(GetClass());
	if (spec)
		return asc->TriggerAbilityFromGameplayEvent(spec->Handle, asc->AbilityActorInfo.Get(), eventTag, &eventData, *asc);
	else
	{
		FGameplayAbilitySpecHandle handle = asc->GiveAbility(FGameplayAbilitySpec(GetClass(), 1, INDEX_NONE));
		return asc->TriggerAbilityFromGameplayEvent(handle, asc->AbilityActorInfo.Get(), eventTag, &eventData, *asc);
	}
}

bool UMounteaInventoryItemAction::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	if (!CurrentTargetItem.IsItemValid())
		return false;

	if (!IsActionVisible(CurrentTargetItem))
		return false;

	return true;
}

void UMounteaInventoryItemAction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	LOG_INFO(TEXT("[%s] Executing action on item %s"), *GetName(), *CurrentTargetItem.GetGuid().ToString());

	ReceiveExecuteAction(CurrentTargetItem);
	
	const bool success = ProcessAction(GetAvatarActorFromActorInfo(), CurrentTargetItem);
	
	if (success)
	{
		ApplyActionEffects();
		ReceiveActionCompleted(CurrentTargetItem);
	}
	else
		ReceiveActionFailed(CurrentTargetItem, FText::FromString(TEXT("Action processing failed")));
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, !success);
}

void UMounteaInventoryItemAction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled)
	{
		ReceiveActionFailed(CurrentTargetItem, FText::FromString(TEXT("Action was cancelled")));
		LOG_WARNING(TEXT("[%s] Action was cancelled"), *GetName())
	}

	CurrentTargetItem = FInventoryItem();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UMounteaInventoryItemAction::ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem)
{
	LOG_WARNING(TEXT("[%s] ProcessAction_Implementation called on base class"), *GetName())
	return false;
}

EInventoryItemActionCallback UMounteaInventoryItemAction::GetInventoryItemActionCallback_Implementation() const
{
	return static_cast<EInventoryItemActionCallback>(ItemActionData.InventoryItemActionCallback);
}

void UMounteaInventoryItemAction::AddActionFlag_Implementation(const EInventoryItemActionCallback FlagToAdd)
{
	ItemActionData.InventoryItemActionCallback |= static_cast<uint8>(FlagToAdd);
}

void UMounteaInventoryItemAction::RemoveActionFlag_Implementation(const EInventoryItemActionCallback FlagToRemove)
{
	ItemActionData.InventoryItemActionCallback &= ~static_cast<uint8>(FlagToRemove);
}

void UMounteaInventoryItemAction::ClearAllActionFlags_Implementation()
{
	ItemActionData.InventoryItemActionCallback = 0;
}


void UMounteaInventoryItemAction::ApplyActionEffects()
{
	UAbilitySystemComponent* asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
	{
		LOG_WARNING(TEXT("[%s] No AbilitySystemComponent found"), *GetName())
		return;
	}
	
	for (const auto& effectClass : ItemActionData.ActionEffects)
	{
		if (!effectClass.IsValid())
			continue;
		
		FGameplayEffectContextHandle contextHandle = asc->MakeEffectContext();
		contextHandle.AddSourceObject(this);
			
		FGameplayEffectSpecHandle specHandle = asc->MakeOutgoingSpec(effectClass.LoadSynchronous(), 1.0f, contextHandle);
		if (specHandle.IsValid())
			asc->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());
	}
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryItemAction::GetOwningInventory_Implementation() const
{
	if (CurrentTargetItem.IsItemValid())
		return CurrentTargetItem.GetOwningInventory();

	return nullptr;
}