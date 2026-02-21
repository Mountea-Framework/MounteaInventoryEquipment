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


#include "Components/MounteaEquipmentItemComponent.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Net/UnrealNetwork.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Statics/MounteaEquipmentStatics.h"

UMounteaEquipmentItemComponent::UMounteaEquipmentItemComponent() : 
	EquipmentItemState(EEquipmentItemState::EES_Idle),
	bAutoActivates(1),
	bRequiresActivationEvent(0)
{
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);
}

// TODO: Handle server and client
bool UMounteaEquipmentItemComponent::SetEquipmentItemState_Implementation(const EEquipmentItemState NewEquipmentItemState)
{
	if (NewEquipmentItemState != EquipmentItemState)
	{
		EquipmentItemState = NewEquipmentItemState;
		OnEquipmentItemStateChanged.Broadcast(this, NewEquipmentItemState);
		return true;
	}
	
	return false;
}

bool UMounteaEquipmentItemComponent::SetAutoActive_Implementation(const bool bValue)
{
	if (bValue != bAutoActivates)
	{
		bAutoActivates = bValue;
		return true;
	}
	
	return false;
}

bool UMounteaEquipmentItemComponent::SetRequiresActivationEvent_Implementation(const bool bValue)
{
	if (bValue != bRequiresActivationEvent)
	{
		bRequiresActivationEvent = bValue;
		return true;
	}
	return false;
}

UAnimationAsset* UMounteaEquipmentItemComponent::GetActivationAnimation_Implementation() const
{
	if (ActivationAnimation.ToSoftObjectPath().IsValid())
		return Cast<UAnimationAsset>(ActivationAnimation.LoadSynchronous());
	return nullptr;
}

bool UMounteaEquipmentItemComponent::SetActivationAnimation_Implementation(UAnimationAsset* NewActivateAnimation)
{
	if (ActivationAnimation != NewActivateAnimation)
	{
		ActivationAnimation = NewActivateAnimation;
		return true;
	}
	return false;
}

void UMounteaEquipmentItemComponent::SetEquippedItemId_Implementation(const FGuid& NewEquippedItemId)
{
	if (NewEquippedItemId != EquippedItemId)
		EquippedItemId = NewEquippedItemId;
}

TArray<FName> UMounteaEquipmentItemComponent::GetAvailableSlots()
{
	const auto* equipmentConfig = UMounteaEquipmentStatics::GetEquipmentSettingsConfig();
	if (!equipmentConfig)
		return { NAME_None };

	TArray<FName> returnValue{ NAME_None };
	TArray<FName> slotKeys;
	equipmentConfig->AllowedEquipmentSlots.GetKeys(slotKeys);

	returnValue.Reserve(slotKeys.Num() + 1);
	returnValue.Append(MoveTemp(slotKeys));

	return returnValue;
}

void UMounteaEquipmentItemComponent::OnRep_EquipmentItemState()
{
	OnEquipmentItemStateChanged.Broadcast(this, EquipmentItemState);
}

void UMounteaEquipmentItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UMounteaEquipmentItemComponent, EquipmentItemState);
}
