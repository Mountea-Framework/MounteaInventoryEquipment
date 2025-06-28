// All rights reserved Dominik Morse 2024

#include "Components/MounteaAttachableComponent.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"

UMounteaAttachableComponent::UMounteaAttachableComponent()
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

void UMounteaAttachableComponent::SetState_Implementation(const EAttachmentSlotState NewState)
{
	if (NewState != State)
		State = NewState;
}

bool UMounteaAttachableComponent::IsValidAttachable_Implementation() const
{
	return !DisplayName.IsEmpty() && !Id.IsNone();
}

bool UMounteaAttachableComponent::IsEmpty_Implementation() const
{
	return State == EAttachmentSlotState::EASS_Empty && !AttachedTo.GetObject();
}

bool UMounteaAttachableComponent::IsOccupied_Implementation() const
{
	return State == EAttachmentSlotState::EASS_Occupied && AttachedTo.GetObject();
}

bool UMounteaAttachableComponent::IsLocked_Implementation() const
{
	return State == EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAttachableComponent::CanAttach_Implementation() const
{
	return IsValidAttachable() && IsEmpty() && !IsLocked();
}

bool UMounteaAttachableComponent::AttachToSlot_Implementation(
	const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	if (!Execute_CanAttach(this) || !IsValid(Target.GetObject()))
		return false;

	if (!Target->Execute_TryAttach(Target.GetObject(), SlotId, this))
		return false;

	AttachedTo = Target;
	Execute_SetState(this, EAttachmentSlotState::EASS_Occupied);
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
	if (!IsOccupied() || !AttachedTo.GetObject())
		return false;

	const FName slotId = AttachedTo->Execute_GetSlotIdForAttachable(AttachedTo.GetObject(), this);
	if (!slotId.IsNone())
		AttachedTo->Execute_TryDetach(AttachedTo.GetObject(), slotId);

	AttachedTo = nullptr;
	State = EAttachmentSlotState::EASS_Empty;
	return true;
}

void UMounteaAttachableComponent::Disable_Implementation()
{
	if (!IsEmpty())
		Detach();

	State = EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAttachableComponent::HasTag_Implementation(const FGameplayTag& Tag) const
{
	return Tags.HasTag(Tag);
}

bool UMounteaAttachableComponent::MatchesTags_Implementation(const FGameplayTagContainer& OtherTags, const bool bRequireAll) const
{
	return bRequireAll ? Tags.HasAll(OtherTags) : Tags.HasAny(OtherTags);
}
