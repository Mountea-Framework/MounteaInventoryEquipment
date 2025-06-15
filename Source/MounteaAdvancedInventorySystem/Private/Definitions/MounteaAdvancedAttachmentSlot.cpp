// All rights reserved Dominik Morse 2024


#include "Definitions/MounteaAdvancedAttachmentSlot.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Statics/MounteaAttachmentsStatics.h"
#include "Statics/MounteaInventorySystemStatics.h"

UMounteaAdvancedAttachmentSlot::UMounteaAdvancedAttachmentSlot():
	State(EAttachmentSlotState::EASS_Empty), SlotType(EAttachmentSlotType::EAST_Socket)
{
}

UMounteaAdvancedAttachmentSlot::UMounteaAdvancedAttachmentSlot(
	const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& NewParentContainer) :
	State(EAttachmentSlotState::EASS_Empty), SlotType(EAttachmentSlotType::EAST_Socket), ParentContainer(NewParentContainer)
{
}

TArray<FName> UMounteaAdvancedAttachmentSlot::GetAvailableSocketNames() const
{
	switch (SlotType)
	{		
		case EAttachmentSlotType::EAST_Socket:
			break;
		case EAttachmentSlotType::EAST_Component:
		case EAttachmentSlotType::Default:
			return TArray<FName>();
	}
	
	if (!IsValid(ParentContainer.GetObject()))
		return TArray<FName>();

	auto parentActor = ParentContainer->Execute_GetOwningActor(ParentContainer.GetObject());
	if (!IsValid(parentActor))
		return TArray<FName>();

	return UMounteaAttachmentsStatics::GetAvailableSocketNames(parentActor,
		AttachmentTargetOverride.IsNone() ?
		ParentContainer->Execute_GetDefaultAttachmentTarget(ParentContainer.GetObject()) :
		AttachmentTargetOverride);
}

TArray<FName> UMounteaAdvancedAttachmentSlot::GetAvailableTargetNames() const
{
	if (!IsValid(ParentContainer.GetObject()))
		return TArray<FName>();

	auto parentActor = ParentContainer->Execute_GetOwningActor(ParentContainer.GetObject());
	if (!IsValid(parentActor))
		return TArray<FName>();

	return UMounteaAttachmentsStatics::GetAvailableComponentNames(parentActor);
}

bool UMounteaAdvancedAttachmentSlot::IsEmpty() const
{
	return State == EAttachmentSlotState::EASS_Empty && Attachment == nullptr;
}

bool UMounteaAdvancedAttachmentSlot::IsOccupied() const
{
	return State == EAttachmentSlotState::EASS_Occupied && Attachment != nullptr;
}

void UMounteaAdvancedAttachmentSlot::DisableSlot()
{
	if (!IsEmpty())
		Detach();
	State = EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAdvancedAttachmentSlot::IsLocked() const
{
	return State == EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAdvancedAttachmentSlot::Attach(UMounteaAttachableComponent* NewAttachment)
{
	if (!CanAttach() || !NewAttachment)
		return false;

	Attachment = NewAttachment;
	State = EAttachmentSlotState::EASS_Occupied;
	return true;
}

bool UMounteaAdvancedAttachmentSlot::Detach()
{
	if (!IsOccupied())
		return false;

	Attachment = nullptr;
	State = EAttachmentSlotState::EASS_Empty;
	return true;
}
