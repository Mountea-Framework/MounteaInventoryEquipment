// All rights reserved Dominik Morse 2024

#include "Decorations/MounteaInventoryItemAction.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

UMounteaInventoryItemAction::UMounteaInventoryItemAction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	ActionDisplayName = FText::FromString(TEXT("Default Action"));
	ActionDescription = FText::FromString(TEXT("A basic inventory item action"));
	ActionPriority = 0;
	bIsVisibleByDefault = true;
	bRequiresConfirmation = false;
}

bool UMounteaInventoryItemAction::ExecuteInventoryAction(const FInventoryItem& TargetItem)
{
	UAbilitySystemComponent* asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
	{
		LOG_ERROR(TEXT("[%s] No AbilitySystemComponent found"), *GetName());
		return false;
	}

	if (!TargetItem.IsItemValid())
	{
		LOG_ERROR(TEXT("[%s] Invalid target item"), *GetName());
		return false;
	}

	CurrentTargetItem = TargetItem;

	FGameplayEventData eventData;
	eventData.Instigator = GetAvatarActorFromActorInfo();
	eventData.Target = GetAvatarActorFromActorInfo();
	
	eventData.ContextHandle = FGameplayEffectContextHandle();
	if (eventData.ContextHandle.IsValid())
		eventData.ContextHandle.Get()->AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

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
	ApplyActionEffects();
	ReceiveActionCompleted(CurrentTargetItem);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UMounteaInventoryItemAction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled)
	{
		ReceiveActionFailed(CurrentTargetItem, FText::FromString(TEXT("Action was cancelled")));
		LOG_WARNING(TEXT("[%s] Action was cancelled"), *GetName());
	}

	CurrentTargetItem = FInventoryItem();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UMounteaInventoryItemAction::IsActionVisible_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	return bIsVisibleByDefault;
}

FText UMounteaInventoryItemAction::GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const
{
	UAbilitySystemComponent* asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
		return FText::FromString(TEXT("No ability system available"));

	if (!TargetItem.IsItemValid())
		return FText::FromString(TEXT("Invalid target item"));

	FGameplayTagContainer failureTags;
	if (!CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), nullptr, nullptr, &failureTags))
	{
		if (failureTags.HasTag(FGameplayTag::RequestGameplayTag("Ability.ActivateFail.Cost")))
			return FText::FromString(TEXT("Insufficient resources"));
		else if (failureTags.HasTag(FGameplayTag::RequestGameplayTag("Ability.ActivateFail.Cooldown")))
			return FText::FromString(TEXT("Action is on cooldown"));
		else if (failureTags.HasTag(FGameplayTag::RequestGameplayTag("Ability.ActivateFail.BlockedByTags")))
			return FText::FromString(TEXT("Action is blocked"));
	}

	return FText::FromString(TEXT("Action is not currently available"));
}

void UMounteaInventoryItemAction::ApplyActionEffects()
{
	UAbilitySystemComponent* asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
	{
		LOG_WARNING(TEXT("[%s] No AbilitySystemComponent found"), *GetName());
		return;
	}

	for (const TSubclassOf<UGameplayEffect>& effectClass : ActionEffects)
	{
		if (effectClass)
		{
			FGameplayEffectContextHandle contextHandle = asc->MakeEffectContext();
			contextHandle.AddSourceObject(this);
			
			FGameplayEffectSpecHandle specHandle = asc->MakeOutgoingSpec(effectClass, 1.0f, contextHandle);
			if (specHandle.IsValid())
				asc->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());
		}
	}
}

FInventoryItem UMounteaInventoryItemAction::GetTargetItem() const
{
	return CurrentTargetItem;
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryItemAction::GetOwningInventory() const
{
	if (CurrentTargetItem.IsItemValid())
		return CurrentTargetItem.GetOwningInventory();

	return nullptr;
}

UInventoryActionEventData* UInventoryActionEventData::CreateEventData(const FInventoryItem& Item)
{
	UInventoryActionEventData* EventData = NewObject<UInventoryActionEventData>();
	EventData->TargetItem = Item;
	return EventData;
}