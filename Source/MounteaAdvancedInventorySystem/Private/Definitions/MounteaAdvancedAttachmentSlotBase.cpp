// Fill out your copyright notice in the Description page of Project Settings.


#include "Definitions/MounteaAdvancedAttachmentSlotBase.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"

UMounteaAdvancedAttachmentSlotBase::UMounteaAdvancedAttachmentSlotBase() : 
	SlotName(NAME_None),
	State(EAttachmentSlotState::EASS_Empty),
	SlotType(EAttachmentSlotType::EAST_Socket)
{
}
