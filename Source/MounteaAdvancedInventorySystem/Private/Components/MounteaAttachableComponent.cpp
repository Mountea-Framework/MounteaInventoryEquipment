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

#include "Components/MounteaAttachableComponent.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"

UMounteaAttachableComponent::UMounteaAttachableComponent() : 
	Id(NAME_None),
	DisplayName(FText::GetEmpty()),
	State(EAttachmentState::EAS_Detached)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaAttachableComponent::SetComponentTickEnabled(false);

	bAutoActivate = true;

	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append({ TEXT("Mountea"), TEXT("Attachable") });
}

void UMounteaAttachableComponent::SetId_Implementation(const FName& NewId)
{
	if (Id != NewId)
		Id = NewId;
}

void UMounteaAttachableComponent::SetDisplayName_Implementation(const FText& NewDisplayName)
{
	if (!NewDisplayName.EqualTo(DisplayName))
		DisplayName = NewDisplayName;
}

void UMounteaAttachableComponent::SetTags_Implementation(const FGameplayTagContainer& NewTags)
{
	Tags.Reset();
	Tags.AppendTags(NewTags);
}

void UMounteaAttachableComponent::SetState_Implementation(const EAttachmentState NewState)
{
	if (NewState != State)
		State = NewState;
}

bool UMounteaAttachableComponent::IsValidAttachable_Implementation() const
{
	return !DisplayName.IsEmpty() && !Id.IsNone();
}

bool UMounteaAttachableComponent::CanAttach_Implementation() const
{
	return IsValidAttachable() && State != EAttachmentState::EAS_Attached;
}

bool UMounteaAttachableComponent::AttachToSlot_Implementation(
	const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	if (!Execute_CanAttach(this) || !IsValid(Target.GetObject()))
		return false;

	if (!Target->Execute_TryAttach(Target.GetObject(), SlotId, this))
		return false;

	AttachedTo = Target;
	Execute_SetState(this, EAttachmentState::EAS_Attached);

	OnAttachableAttached.Broadcast(Target, SlotId);
	return true;
}

bool UMounteaAttachableComponent::AttachToContainer_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target)
{
	if (!CanAttach() || !Target.GetObject())
		return false;

	const FName foundSlotId = Target->Execute_FindFirstFreeSlotWithTags(Target.GetObject(), Tags);
	if (foundSlotId.IsNone())
		return false;

	return Execute_AttachToSlot(this, Target, foundSlotId);
}

bool UMounteaAttachableComponent::Detach_Implementation()
{
	if (State != EAttachmentState::EAS_Attached || !AttachedTo.GetObject())
		return false;

	const FName slotId = AttachedTo->Execute_GetSlotIdForAttachable(AttachedTo.GetObject(), this);
	if (!slotId.IsNone())
		AttachedTo->Execute_TryDetach(AttachedTo.GetObject(), slotId);

	AttachedTo = nullptr;
	State = EAttachmentState::EAS_Detached;

	OnAttachableDetached.Broadcast(AttachedTo);
	return true;
}

bool UMounteaAttachableComponent::HasTag_Implementation(const FGameplayTag& Tag) const
{
	return Tags.HasTag(Tag);
}

bool UMounteaAttachableComponent::MatchesTags_Implementation(const FGameplayTagContainer& OtherTags, const bool bRequireAll) const
{
	return bRequireAll ? Tags.HasAll(OtherTags) : Tags.HasAny(OtherTags);
}
