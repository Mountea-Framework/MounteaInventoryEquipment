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


#include "Components/MounteaCraftingParticipantUIComponent.h"

#include "Blueprint/UserWidget.h"


UMounteaCraftingParticipantUIComponent::UMounteaCraftingParticipantUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaCraftingParticipantUIComponent::SetComponentTickEnabled(false);
	
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(false);
	SetActiveFlag(true);

	ComponentTags.Append( { TEXT("Mountea"), TEXT("Crafting"), TEXT("UI") } );
}
void UMounteaCraftingParticipantUIComponent::BeginPlay()
{
	Super::BeginPlay();	
}

bool UMounteaCraftingParticipantUIComponent::CreateWrapperWidget_Implementation()
{
	// TODO: first attempt to reach `UMounteaAdvancedInventoryUISubsystem`
	// if valid, request UI from Inventory Manager (later to be replaced by HUD Manager)
	// if valid, use that 
	// if not valid, follow same pattern as inventory (invoking crafting outside inventory is valid approach)
	return false;
}

UUserWidget* UMounteaCraftingParticipantUIComponent::GetWrapperWidget_Implementation() const
{
	// TODO: first attempt to reach `UMounteaAdvancedInventoryUISubsystem`
	// if valid, request UI from Inventory Manager (later to be replaced by HUD Manager)
	// if valid, use that 
	return WrapperWidget;
}

void UMounteaCraftingParticipantUIComponent::RemoveWrapperWidget_Implementation()
{
	// TODO: first attempt to reach `UMounteaAdvancedInventoryUISubsystem`
	// if valid, request UI from Inventory Manager (later to be replaced by HUD Manager)
	// if valid, use that 
	// if not valid, follow same pattern as inventory (invoking crafting outside inventory is valid approach)
}

bool UMounteaCraftingParticipantUIComponent::CreateCraftingWidget_Implementation()
{
	// TODO: Get global config
	// get crafting UI config
	// validate
	// create widget and populate
	// return true
	return false;
}

void UMounteaCraftingParticipantUIComponent::RemoveCraftingWidget_Implementation()
{
	if (IsValid(CraftingWidget))
	{
		// TODO: execute event using generic widget etc.
		CraftingWidget = nullptr;
	}
}

bool UMounteaCraftingParticipantUIComponent::SetCraftingWidget_Implementation(UUserWidget* NewCraftingWidget)
{
	if (CraftingWidget == NewCraftingWidget)
		return false;
	CraftingWidget = NewCraftingWidget;
	return true;
}

