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


#include "Statics/MounteaCraftingUIStatics.h"

#include "Blueprint/UserWidget.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantUIInterface.h"
#include "Settings/MounteaAdvancedCraftingUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaInventoryUIStatics.h"
#include "Subsystems/MounteaAdvancedCraftingUISubsystem.h"

bool UMounteaCraftingUIStatics::IsValidCraftingUIHandler(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedCraftingParticipantUIInterface>();
}

UMounteaAdvancedCraftingUISubsystem* UMounteaCraftingUIStatics::GetCraftingUISubsystem(UObject* Context)
{
	if (!IsValid(Context))
		return nullptr;

	APlayerController* playerController = Cast<APlayerController>(Context);

	if (!IsValid(playerController))
	{
		if (AActor* actor = Cast<AActor>(Context))
			playerController = UMounteaInventoryUIStatics::FindPlayerController(actor, 3);
		else if (const UActorComponent* actorComp = Cast<UActorComponent>(Context))
		{
			if (AActor* ownerActor = actorComp->GetOwner())
				playerController = UMounteaInventoryUIStatics::FindPlayerController(ownerActor, 3);
		}
		else if (const UUserWidget* userWidget = Cast<UUserWidget>(Context))
			playerController = UMounteaInventoryUIStatics::FindPlayerController(userWidget->GetOwningPlayer(), 2);
	}

	if (!IsValid(playerController))
		return nullptr;

	const ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
	return IsValid(localPlayer) ? localPlayer->GetSubsystem<UMounteaAdvancedCraftingUISubsystem>() : nullptr;
}

UMounteaAdvancedCraftingUIConfig* UMounteaCraftingUIStatics::GetCraftingUISettingsConfig()
{
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return nullptr;
	return settings->AdvancedCraftingUISettingsConfig.LoadSynchronous();
}

bool UMounteaCraftingUIStatics::CreateCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target)
{
	return IsValidCraftingUIHandler(Target.GetObject()) ? IMounteaAdvancedCraftingParticipantUIInterface::Execute_CreateCraftingWidget(Target.GetObject()) : false;
}

UUserWidget* UMounteaCraftingUIStatics::GetCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target)
{
	return IsValidCraftingUIHandler(Target.GetObject()) ? IMounteaAdvancedCraftingParticipantUIInterface::Execute_GetCraftingWidget(Target.GetObject()) : nullptr;
}

bool UMounteaCraftingUIStatics::SetCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target, UUserWidget* NewCraftingWidget)
{
	return IsValidCraftingUIHandler(Target.GetObject()) ? IMounteaAdvancedCraftingParticipantUIInterface::Execute_SetCraftingWidget(Target.GetObject(), NewCraftingWidget) : false;
}

void UMounteaCraftingUIStatics::RemoveCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target)
{
	if (IsValidCraftingUIHandler(Target.GetObject()))
		IMounteaAdvancedCraftingParticipantUIInterface::Execute_RemoveCraftingWidget(Target.GetObject());
}

FString UMounteaCraftingUIStatics::GetCraftingSelectedCategoryId(UObject* Target)
{
	return IsValidCraftingUIHandler(Target) ? IMounteaAdvancedCraftingParticipantUIInterface::Execute_GetCraftingSelectedCategoryId(Target) : TEXT("none");
}

bool UMounteaCraftingUIStatics::CraftingCategorySelected(UObject* Target,const FString& SelectedCategoryId)
{
	return IsValidCraftingUIHandler(Target) ? IMounteaAdvancedCraftingParticipantUIInterface::Execute_CraftingCategorySelected(Target, SelectedCategoryId) : false;
}

FGuid UMounteaCraftingUIStatics::GetActiveCraftableItemGuid_Implementation(UObject* Target)
{
	return IsValidCraftingUIHandler(Target) ? IMounteaAdvancedCraftingParticipantUIInterface::Execute_GetActiveCraftableItemGuid(Target) : FGuid();
}

bool UMounteaCraftingUIStatics::CraftableItemSelected(UObject* Target, const FGuid& SelectedItem)
{
	return IsValidCraftingUIHandler(Target) ? IMounteaAdvancedCraftingParticipantUIInterface::Execute_CraftableItemSelected(Target, SelectedItem) : false;
}
