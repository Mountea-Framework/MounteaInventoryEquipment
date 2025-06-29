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


#include "Components/MounteaAttachmentContainerComponent.h"

#include "Algo/ForEach.h"
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

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
	bReplicateUsingRegisteredSubObjectList = true;
#endif
	
	ApplyParentContainer();
}

void UMounteaAttachmentContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto attachmentTargetComponent = UMounteaAttachmentsStatics::GetAvailableComponentByName(Execute_GetOwningActor(this), DefaultAttachmentTarget);
	if (!IsValid(attachmentTargetComponent))
		LOG_ERROR(TEXT("Default attachment target component '%s' is not valid!"), *DefaultAttachmentTarget.ToString());

	Execute_SetDefaultAttachmentTargetComponent(this, attachmentTargetComponent);

	Algo::ForEach(AttachmentSlots, [this](const auto& AttachmentSlot)
	{
		if (IsValid(AttachmentSlot))
		{
			AttachmentSlot->InitializeAttachmentSlot(this);
			AttachmentSlot->BeginPlay();
		}
	});
}

bool UMounteaAttachmentContainerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
	bool wroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
    
	for (auto& Slot : AttachmentSlots)
	{
		if (IsValid(Slot))
			AddReplicatedSubObject(Slot);
	}
	return wroteSomething;
#else
	bool wroteSomething = true;

	for (UORReplicatedObject* ReplicatedObject : ReplicatedObjects)
	{
		if (IsValid(ReplicatedObject))
			wroteSomething |= Channel->ReplicateSubobject(ReplicatedObject, *Bunch, *RepFlags);
	}

	return wroteSomething;
#endif
}

AActor* UMounteaAttachmentContainerComponent::GetOwningActor_Implementation() const
{
	return UMounteaInventorySystemStatics::GetOwningActor(this);
}

void UMounteaAttachmentContainerComponent::SetDefaultAttachmentTargetComponent_Implementation(
	USceneComponent* NewTarget)
{
	if (NewTarget != DefaultAttachmentTargetComponent)
		DefaultAttachmentTargetComponent = NewTarget;
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
	if (!Attachment) return false;

	if (!GetOwner()->HasAuthority())
	{
		ServerTryAttach(SlotId, Attachment);
		return true;
	}

	const bool bSuccess = TryAttachInternal(SlotId, Attachment);
	if (bSuccess)
		OnAttachmentChanged.Broadcast(SlotId, Attachment, nullptr);
	return bSuccess;
}

void UMounteaAttachmentContainerComponent::ServerTryAttach_Implementation(const FName& SlotId, UObject* Attachment)
{
	Execute_TryAttach(this, SlotId, Attachment);
}

bool UMounteaAttachmentContainerComponent::TryAttachInternal(const FName& SlotId, UObject* Attachment)
{
	if (!Attachment) return false;

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

	if (!GetOwner()->HasAuthority())
	{
		ServerTryDetach(SlotId);
		return true;
	}

	return foundSlot->Detach();
}

void UMounteaAttachmentContainerComponent::ServerTryDetach_Implementation(const FName& SlotId)
{
	Execute_TryDetach(this, SlotId);
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
	//DOREPLIFETIME_CONDITION(UMounteaAttachmentContainerComponent, State, COND_SimulatedOnly);
	DOREPLIFETIME(UMounteaAttachmentContainerComponent, State);
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
