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


#include "Components/MounteaCraftingStationComponent.h"

#include "Net/UnrealNetwork.h"
#include "Definitions/MounteaCraftingBaseEnums.h"

UMounteaCraftingStationComponent::UMounteaCraftingStationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaCraftingStationComponent::SetComponentTickEnabled(false);

	bAutoActivate = true;

	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append( { TEXT("Mountea"), TEXT("Crafting") } );
}

void UMounteaCraftingStationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMounteaCraftingStationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMounteaCraftingStationComponent, CraftingStationState);
	DOREPLIFETIME(UMounteaCraftingStationComponent, CraftingParticipants);
}

void UMounteaCraftingStationComponent::OnRep_CraftingStationState()
{
}

bool UMounteaCraftingStationComponent::IsCraftingPlaceOccupied_Implementation() const
{
	return CraftingStationState == ECraftingStationState::EASS_Occupied;
}

bool UMounteaCraftingStationComponent::SetCraftingStationState_Implementation(const ECraftingStationState NewCraftingStationState)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_SetCraftingStationState(NewCraftingStationState);
		return true;
	}

	switch (NewCraftingStationState)
	{
		case ECraftingStationState::EASS_Empty:
			if (CraftingParticipants.Num() > 0)
				return false;
			if (CraftingStationState == ECraftingStationState::EASS_Empty)
				return false;
		case ECraftingStationState::EASS_Inactive:
			if (CraftingStationState == ECraftingStationState::EASS_Inactive)
				return false;
			break;
		case ECraftingStationState::EASS_Occupied:
			if (CraftingStationState == ECraftingStationState::EASS_Occupied)
				return false;
			break;
	}

	CraftingStationState = NewCraftingStationState;
	return true;
}

bool UMounteaCraftingStationComponent::CanBeUsed_Implementation() const
{
	if (CraftingStationState == ECraftingStationState::EASS_Inactive)
		return false;

	if (CraftingStationState == ECraftingStationState::EASS_Occupied && CraftingParticipants.Num() >= CraftingPlaceCapacity)
		return false;

	return true;
}

bool UMounteaCraftingStationComponent::StartUsing_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_StartUsing(Participant);
		return true;
	}

	if (CraftingParticipants.Contains(Participant))
		return false;

	if (!Execute_CanBeUsed(this))
		return false;

	CraftingParticipants.Add(Participant);
	Execute_SetCraftingStationState(this, ECraftingStationState::EASS_Occupied);

	return true;
}

bool UMounteaCraftingStationComponent::StopUsing_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_StopUsing(Participant);
		return true;
	}

	if (Participant.GetObject() == nullptr)
		return false;
	if (!CraftingParticipants.Contains(Participant))
		return false;
	CraftingParticipants.Remove(Participant);
	Execute_SetCraftingStationState(this, ECraftingStationState::EASS_Empty);

	return true;
}

void UMounteaCraftingStationComponent::Server_StartUsing_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant)
{
	Execute_StartUsing(this, Participant);
}

void UMounteaCraftingStationComponent::Server_StopUsing_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant)
{
	Execute_StopUsing(this, Participant);
}

void UMounteaCraftingStationComponent::Server_SetCraftingStationState_Implementation(ECraftingStationState NewState)
{
	Execute_SetCraftingStationState(this, NewState);
}
