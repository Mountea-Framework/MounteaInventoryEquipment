// All rights reserved Dominik Morse 2024


#include "Components/MounteaAttachmentContainerComponent.h"

#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaAttachmentsStatics.h"
#include "Statics/MounteaInventorySystemStatics.h"

UMounteaAttachmentContainerComponent::UMounteaAttachmentContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaAttachmentContainerComponent::SetComponentTickEnabled(false);
	
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append( { TEXT("Mountea"), TEXT("Attachment") } );

	ApplyParentContainer();
}

AActor* UMounteaAttachmentContainerComponent::GetOwningActor_Implementation() const
{
	return UMounteaInventorySystemStatics::GetOwningActor(this);
}

bool UMounteaAttachmentContainerComponent::IsValidSlot_Implementation(const FName& SlotId) const
{
	const auto foundSlot = AttachmentSlots.FindRef(SlotId);
	return foundSlot && foundSlot->IsSlotValid();
}

UMounteaAdvancedAttachmentSlot* UMounteaAttachmentContainerComponent::GetSlot_Implementation(const FName& SlotId) const
{
	const auto foundSlot = AttachmentSlots.FindRef(SlotId);
	return foundSlot ? foundSlot : nullptr;
}

bool UMounteaAttachmentContainerComponent::IsSlotOccupied_Implementation(const FName& SlotId) const
{
	const auto foundSlot = AttachmentSlots.FindRef(SlotId);
	return foundSlot && foundSlot->IsOccupied();
}

bool UMounteaAttachmentContainerComponent::DisableSlot_Implementation(const FName& SlotId)
{
	auto foundSlot = AttachmentSlots.FindRef(SlotId);
	if (!foundSlot)
		return false;

	return foundSlot->Detach();
}

bool UMounteaAttachmentContainerComponent::TryAttach_Implementation(const FName& SlotId, UMounteaAttachableComponent* Attachment)
{
	if (!Attachment)
		return false;

	auto foundSlot = AttachmentSlots.FindRef(SlotId);
	return foundSlot && foundSlot->Attach(Attachment);
}

bool UMounteaAttachmentContainerComponent::TryDetach_Implementation(const FName& SlotId)
{
	auto foundSlot = AttachmentSlots.FindRef(SlotId);
	return foundSlot && foundSlot->Detach();
}

bool UMounteaAttachmentContainerComponent::ForceAttach_Implementation(const FName& SlotId, UMounteaAttachableComponent* Attachment)
{
	if (!Attachment)
		return false;

	auto foundSlot = AttachmentSlots.FindRef(SlotId);
	if (!foundSlot)
		return false;

	foundSlot->Attachment = Attachment;
	foundSlot->State = EAttachmentSlotState::EASS_Occupied;
	return true;
}

bool UMounteaAttachmentContainerComponent::ForceDetach_Implementation(const FName& SlotId)
{
	auto foundSlot = AttachmentSlots.FindRef(SlotId);
	if (!foundSlot)
		return false;

	foundSlot->Attachment = nullptr;
	foundSlot->State = EAttachmentSlotState::EASS_Empty;
	return true;
}

FName UMounteaAttachmentContainerComponent::FindFirstFreeSlotWithTags_Implementation(const FGameplayTagContainer& RequiredTags) const
{
	const auto* found = Algo::FindByPredicate(AttachmentSlots,
		[&](const TPair<FName, UMounteaAdvancedAttachmentSlot*>& pair)
		{
			const UMounteaAdvancedAttachmentSlot* slot = pair.Value;
			return slot != nullptr && slot->CanAttach() && slot->MatchesTags(RequiredTags, true);
		});

	return found ? found->Key : NAME_None;
}

FName UMounteaAttachmentContainerComponent::GetSlotIdForAttachable_Implementation(const UMounteaAttachableComponent* Attachable) const
{
	if (!Attachable)
		return NAME_None;

	const auto* Found = Algo::FindByPredicate(AttachmentSlots,
		[&](const TPair<FName, UMounteaAdvancedAttachmentSlot*>& Pair)
		{
			return Pair.Value != nullptr && Pair.Value->IsOccupied() && Pair.Value->Attachment == Attachable;
		});

	return Found ? Found->Key : NAME_None;
}

void UMounteaAttachmentContainerComponent::ClearAll_Implementation()
{
	for (auto& pair : AttachmentSlots)
	{
		pair.Value->Detach();
	}
}

void UMounteaAttachmentContainerComponent::ApplyParentContainer()
{
	for (auto& pair : AttachmentSlots)
	{
		if (!pair.Value || pair.Value->ParentContainer.GetObject()) continue;
		pair.Value->ParentContainer = this;
	}
}

TArray<FName> UMounteaAttachmentContainerComponent::GetAvailableTargetNames() const
{
	const AActor* ownerActor = UMounteaInventorySystemStatics::GetOwningActor(this);

	if (!IsValid(ownerActor))
	{
		LOG_ERROR(TEXT("[%s] No valid owner actor"), *GetName());
		return TArray<FName>();
	}
	return UMounteaAttachmentsStatics::GetAvailableComponentNames(ownerActor);
}

#if WITH_EDITOR

void UMounteaAttachmentContainerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaAttachmentContainerComponent, AttachmentSlots))
	{
		ApplyParentContainer();
	}
}

#endif
