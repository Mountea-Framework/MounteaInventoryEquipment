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


#include "Definitions/MounteaAdvancedAttachmentSlotBase.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Net/UnrealNetwork.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

UMounteaAdvancedAttachmentSlotBase::UMounteaAdvancedAttachmentSlotBase() : 
	SlotName(NAME_None),
	State(EAttachmentSlotState::EASS_Empty),
	SlotType(EAttachmentSlotType::EAST_Socket)
{
}

void UMounteaAdvancedAttachmentSlotBase::BeginPlay_Implementation()
{
	SlotBeginPlay.Broadcast();
}

AActor* UMounteaAdvancedAttachmentSlotBase::GetOwningActor() const
{
	if (!IsValid(ParentContainer.GetObject()))
		return nullptr;

	const auto ownerObject = ParentContainer.GetObject();
	return ParentContainer->Execute_GetOwningActor(ownerObject);
}

void UMounteaAdvancedAttachmentSlotBase::InitializeAttachmentSlot(
	const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Parent)
{
	ParentContainer = Parent;
}

UWorld* UMounteaAdvancedAttachmentSlotBase::GetWorld() const
{
	if (GetOuter() == nullptr)
		return nullptr;
		
	if (Cast<UPackage>(GetOuter()) != nullptr)
		return Cast<UWorld>(GetOuter()->GetOuter());

	if (!IsValid(GetOwningActor()))
		return nullptr;
		
	return GetOwningActor()->GetWorld();
}

void UMounteaAdvancedAttachmentSlotBase::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaAdvancedAttachmentSlotBase, Attachment);
	DOREPLIFETIME(UMounteaAdvancedAttachmentSlotBase, State);

	if (UBlueprintGeneratedClass* blueprintClass = Cast<UBlueprintGeneratedClass>(GetClass()))
		blueprintClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
}
int32 UMounteaAdvancedAttachmentSlotBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject) || !IsSupportedForNetworking())
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UMounteaAdvancedAttachmentSlotBase::CallRemoteFunction(UFunction* Function, void* Parms,
	struct FOutParmRec* OutParms, FFrame* Stack)
{
	AActor* owningActor = GetOwningActor();
	if (UNetDriver* netDriver = owningActor->GetNetDriver())
	{
		netDriver->ProcessRemoteFunction(owningActor, Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}

bool UMounteaAdvancedAttachmentSlotBase::Attach(UObject* NewAttachment)
{
	return (CanAttach() && IsValid(NewAttachment) && CanAttachAttachable(NewAttachment));
}

bool UMounteaAdvancedAttachmentSlotBase::ForceAttach(UObject* NewAttachment)
{
	LastAttachment = Attachment;
	Attachment = NewAttachment;	
	State = EAttachmentSlotState::EASS_Occupied;
	return true;
}

bool UMounteaAdvancedAttachmentSlotBase::Detach()
{
	if (!IsOccupied())
		return false;
	
	Attachment = nullptr;
	State = EAttachmentSlotState::EASS_Empty;
	return true;
}

bool UMounteaAdvancedAttachmentSlotBase::ForceDetach()
{
	Attachment = nullptr;
	State = EAttachmentSlotState::EASS_Empty;
	return true;
}

void UMounteaAdvancedAttachmentSlotBase::DisableSlot()
{
}

bool UMounteaAdvancedAttachmentSlotBase::CanAttachAttachable(const UObject* NewAttachment) const
{
	if (!IsValid(NewAttachment))
	{
		LOG_ERROR(TEXT("Attachment is not valid!"));
		return false;
	}
	
	if (!NewAttachment->Implements<UMounteaAdvancedAttachmentAttachableInterface>())
	{
		LOG_WARNING(TEXT("Attachable %s does not implement the required interface! Attachment will be performed, however, it may not behave as expected."), *NewAttachment->GetName())
		return true;
	}

	if (!IMounteaAdvancedAttachmentAttachableInterface::Execute_CanAttach(NewAttachment))
	{
		LOG_WARNING(TEXT("Attachable object is not compatible with selected Slot!."));
		return false;
	}
	
	if (!MatchesTags(IMounteaAdvancedAttachmentAttachableInterface::Execute_GetTags(NewAttachment), false))
	{
		LOG_WARNING(TEXT("Attachable object does not match selected Slot tags!"));
		return false;
	}
	
	return true;
}

bool UMounteaAdvancedAttachmentSlotBase::IsEmpty() const
{
	return State == EAttachmentSlotState::EASS_Empty && Attachment == nullptr;
}

bool UMounteaAdvancedAttachmentSlotBase::IsOccupied() const
{
	return State == EAttachmentSlotState::EASS_Occupied && Attachment != nullptr;
}

bool UMounteaAdvancedAttachmentSlotBase::IsLocked() const
{
	return State == EAttachmentSlotState::EASS_Locked;
}

void UMounteaAdvancedAttachmentSlotBase::OnRep_State()
{
	UObject* oldAttachment = LastAttachment;
    
	switch (State)
	{
		case EAttachmentSlotState::EASS_Occupied:
			LastAttachment = Attachment;
			break;
		case EAttachmentSlotState::EASS_Empty:
			break;
	}
    
	if (ParentContainer.GetObject())
		ParentContainer->GetOnAttachmentChangedEventHandle().Broadcast(SlotName, Attachment, oldAttachment);
}

TArray<FName> UMounteaAdvancedAttachmentSlotBase::GetAvailableSlotNames() const
{
	const auto advanedSettings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!advanedSettings)
		return {};

	const auto equipmentConfig = advanedSettings->AdvancedEquipmentSettingsConfig.LoadSynchronous();
	if (!IsValid(equipmentConfig))
		return {};

	const auto& allowedSlots = equipmentConfig->AllowedEquipmentSlots;
	TArray<FName> slotNames;
	slotNames.Reserve(allowedSlots.Num());

	Algo::Transform(allowedSlots, slotNames, [](const TPair<FName, FMounteaEquipmentSlotHeaderData>& Pair)
	{
		return Pair.Key;
	});

	return slotNames;
}

#if WITH_EDITOR
void UMounteaAdvancedAttachmentSlotBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	UMounteaAdvancedEquipmentSettingsConfig* equipmentConfig = nullptr;

	if (const auto advanedSettings = GetDefault<UMounteaAdvancedInventorySettings>())
		equipmentConfig = advanedSettings->AdvancedEquipmentSettingsConfig.LoadSynchronous();
	
	
	const FName propertName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (propertName == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedAttachmentSlotBase, SlotName))
	{
		if (IsValid(equipmentConfig))
		{
			if (const FMounteaEquipmentSlotHeaderData* headerData = equipmentConfig->AllowedEquipmentSlots.Find(SlotName))
			{
				SlotTags.Reset();
				SlotTags.AppendTags(headerData->TagContainer);

				DisplayName = headerData->DisplayName;
			}
		}
	}
}
#endif