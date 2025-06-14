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

bool UMounteaAttachableComponent::IsValidAttachable() const
{
	return !DisplayName.IsEmpty() && !Id.IsNone();
}

bool UMounteaAttachableComponent::IsEmpty() const
{
	return State == EAttachmentSlotState::EASS_Empty && !AttachedTo.GetObject();
}

bool UMounteaAttachableComponent::IsOccupied() const
{
	return State == EAttachmentSlotState::EASS_Occupied && AttachedTo.GetObject();
}

bool UMounteaAttachableComponent::IsLocked() const
{
	return State == EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAttachableComponent::CanAttach() const
{
	return IsValidAttachable() && IsEmpty() && !IsLocked();
}

bool UMounteaAttachableComponent::AttachTo(TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> Target, const FName& SlotId)
{
	if (!CanAttach() || !Target.GetObject())
		return false;

	if (!Target->Execute_TryAttach(Target.GetObject(), SlotId, this))
		return false;

	AttachedTo = Target;
	State = EAttachmentSlotState::EASS_Occupied;
	return true;
}

bool UMounteaAttachableComponent::AttachTo(TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> Target)
{
	if (!CanAttach() || !Target.GetObject())
		return false;

	const FName FoundSlotId = Target->Execute_FindFirstFreeSlotWithTags(Target.GetObject(), Tags);
	if (FoundSlotId.IsNone())
		return false;

	return AttachTo(Target, FoundSlotId);
}

bool UMounteaAttachableComponent::Detach()
{
	if (!IsOccupied() || !AttachedTo.GetObject())
		return false;

	const FName SlotId = AttachedTo->Execute_GetSlotIdForAttachable(AttachedTo.GetObject(), this);
	if (!SlotId.IsNone())
		AttachedTo->Execute_TryDetach(AttachedTo.GetObject(), SlotId);

	AttachedTo = nullptr;
	State = EAttachmentSlotState::EASS_Empty;
	return true;
}

void UMounteaAttachableComponent::Disable()
{
	if (!IsEmpty())
		Detach();

	State = EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAttachableComponent::HasTag(const FGameplayTag& Tag) const
{
	return Tags.HasTag(Tag);
}

bool UMounteaAttachableComponent::MatchesTags(const FGameplayTagContainer& OtherTags, const bool bRequireAll) const
{
	return bRequireAll ? Tags.HasAll(OtherTags) : Tags.HasAny(OtherTags);
}
