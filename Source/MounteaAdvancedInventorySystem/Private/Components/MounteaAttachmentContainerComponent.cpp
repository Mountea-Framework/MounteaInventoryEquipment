// All rights reserved Dominik Morse 2024


#include "Components/MounteaAttachmentContainerComponent.h"

#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Misc/DataValidation.h"
#include "Net/UnrealNetwork.h"
#include "Statics/MounteaAttachmentsStatics.h"
#include "Statics/MounteaInventorySystemStatics.h"
#include "UObject/ObjectSaveContext.h"

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
	const auto foundSlot = *AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot->SlotName == SlotId;
	});
	return foundSlot && foundSlot->IsSlotValid();
}

UMounteaAdvancedAttachmentSlot* UMounteaAttachmentContainerComponent::GetSlot_Implementation(const FName& SlotId) const
{
	const auto foundSlot = *AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot->SlotName == SlotId;
	});
	return foundSlot ? foundSlot : nullptr;
}

bool UMounteaAttachmentContainerComponent::IsSlotOccupied_Implementation(const FName& SlotId) const
{
	const auto foundSlot = *AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot->SlotName == SlotId;
	});
	return foundSlot && foundSlot->IsOccupied();
}

bool UMounteaAttachmentContainerComponent::DisableSlot_Implementation(const FName& SlotId)
{
	auto foundSlot = *AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot->SlotName == SlotId;
	});
	if (!foundSlot)
		return false;

	return foundSlot->Detach();
}

bool UMounteaAttachmentContainerComponent::TryAttach_Implementation(const FName& SlotId, UObject* Attachment)
{
	if (!Attachment)
		return false;

	auto foundSlot = AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot && Slot->SlotName == SlotId;
	});
	return foundSlot && (*foundSlot)->Attach(Attachment);
}

bool UMounteaAttachmentContainerComponent::TryDetach_Implementation(const FName& SlotId)
{
	auto foundSlot = *AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot->SlotName == SlotId;
	});
	if (!foundSlot)
		return false;

	return foundSlot->Detach();
}

bool UMounteaAttachmentContainerComponent::ForceAttach_Implementation(const FName& SlotId, UObject* Attachment)
{
	if (!Attachment)
		return false;

	auto foundSlot = *AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot->SlotName == SlotId;
	});
	if (!foundSlot)
		return false;

	foundSlot->Attachment = Attachment;
	foundSlot->State = EAttachmentSlotState::EASS_Occupied;
	return true;
}

bool UMounteaAttachmentContainerComponent::ForceDetach_Implementation(const FName& SlotId)
{
	auto foundSlot = *AttachmentSlots.FindByPredicate([SlotId](const UMounteaAdvancedAttachmentSlot* Slot) {
		return Slot->SlotName == SlotId;
	});
	if (!foundSlot)
		return false;

	foundSlot->Attachment = nullptr;
	foundSlot->State = EAttachmentSlotState::EASS_Empty;
	return true;
}

FName UMounteaAttachmentContainerComponent::FindFirstFreeSlotWithTags_Implementation(const FGameplayTagContainer& RequiredTags) const
{
	const auto* found = Algo::FindByPredicate(AttachmentSlots,
		[&](const UMounteaAdvancedAttachmentSlot* slot)
		{
			return slot != nullptr && slot->CanAttach() && slot->MatchesTags(RequiredTags, true);
		});

	return found ? (*found)->SlotName : NAME_None;
}

FName UMounteaAttachmentContainerComponent::GetSlotIdForAttachable_Implementation(const UMounteaAttachableComponent* Attachable) const
{
	if (!Attachable)
		return NAME_None;

	const auto* Found = Algo::FindByPredicate(AttachmentSlots,
		[&](const UMounteaAdvancedAttachmentSlot* Slot)
		{
			return Slot != nullptr && Slot->IsOccupied() && Slot->Attachment == Attachable;
		});

	return Found ? (*Found)->SlotName : NAME_None;	
}

FName UMounteaAttachmentContainerComponent::GetFirstEmptySlot_Implementation() const
{
	const auto* Found = Algo::FindByPredicate(AttachmentSlots,
		[&](const UMounteaAdvancedAttachmentSlot* Slot)
		{
			return Slot != nullptr && !Slot->IsOccupied();
		});

	return Found ? (*Found)->SlotName : NAME_None;
}

void UMounteaAttachmentContainerComponent::ClearAll_Implementation()
{
	for (auto& attachmentSlot : AttachmentSlots)
	{
		attachmentSlot->Detach();
	}
}

void UMounteaAttachmentContainerComponent::ApplyParentContainer()
{
	for (auto& slot : AttachmentSlots)
	{
		if (!slot || slot->ParentContainer.GetObject()) continue;
		slot->ParentContainer = this;
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

void UMounteaAttachmentContainerComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// TODO: update if needed
	DOREPLIFETIME_CONDITION(UMounteaAttachmentContainerComponent, State, COND_SimulatedOnly);
	DOREPLIFETIME(UMounteaAttachmentContainerComponent, AttachmentSlots);
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

EDataValidationResult UMounteaAttachmentContainerComponent::IsDataValid(FDataValidationContext& Context) const
{
	for (const auto& slot : AttachmentSlots)
	{
		slot->IsDataValid(Context);
	}
	Super::IsDataValid(Context);
	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}

#endif
