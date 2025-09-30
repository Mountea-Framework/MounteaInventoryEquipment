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
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Engine.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
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

	const auto equipmentConfig = advanedSettings->EquipmentSettingsConfig.LoadSynchronous();
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
		equipmentConfig = advanedSettings->EquipmentSettingsConfig.LoadSynchronous();
	
	
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