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
#include "Definitions/MounteaInventoryBaseCommands.h"
#include "Engine/LocalPlayer.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "Interfaces/UserInterface/MounteaAdvancedInventorySharedHUDInterface.h"
#include "Interfaces/Widgets/Crafting/MounteaAdvancedInventoryCraftingWidgetInterface.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Settings/MounteaAdvancedCraftingUIConfig.h"
#include "Statics/MounteaInventorySystemStatics.h"
#include "Statics/MounteaInventoryUIStatics.h"
#include "Subsystems/MounteaAdvancedCraftingUISubsystem.h"
#include "Subsystems/MounteaAdvancedInventorySharedHUDSubsystem.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaCraftingUIStatics.h"


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

	if (GetOwner() &&
		(GetOwnerRole() == ROLE_Authority || GetOwnerRole() == ROLE_AutonomousProxy) &&
		UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
	{
		const auto craftingParticipant = GetOwner()->FindComponentByInterface(UMounteaAdvancedCraftingParticipantInterface::StaticClass());
		if (!IsValid(craftingParticipant))
		{
			LOG_ERROR(TEXT("[Crafting UI] Unable to find Crafting Participant for Actor %s, component disabled"), *(GetOwner() ? GetOwner()->GetName() : TEXT("'no owner'")))
			return;
		}
		Execute_SetCraftingParticipant(this, craftingParticipant);
		
		APlayerController* playerController = UMounteaInventoryUIStatics::FindPlayerController(GetOwner(), 3);
		if (!IsValid(playerController))
		{
			LOG_ERROR(TEXT("[MounteaCraftingParticipantUIComponent] Cannot find valid Player Controller. Crafting UI Subsystem registration failed!"))
			return;
		}

		const ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
		UMounteaAdvancedCraftingUISubsystem* craftingUISubsystem = IsValid(localPlayer)
			? localPlayer->GetSubsystem<UMounteaAdvancedCraftingUISubsystem>()
			: nullptr;
		if (!IsValid(craftingUISubsystem))
		{
			LOG_ERROR(TEXT("[MounteaCraftingParticipantUIComponent] Cannot find 'Crafting UI Subsystem'. UI might not work properly!"))
			return;
		}	

		craftingUISubsystem->RegisterCraftingUIManager(this);
	}
}

void UMounteaCraftingParticipantUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	switch (EndPlayReason)
	{
		case EEndPlayReason::EndPlayInEditor:
			break;
		case EEndPlayReason::RemovedFromWorld:
		case EEndPlayReason::Destroyed:
		case EEndPlayReason::LevelTransition:
		case EEndPlayReason::Quit:
		{
			if (GetOwner() &&
				(GetOwnerRole() == ROLE_Authority || GetOwnerRole() == ROLE_AutonomousProxy) &&
				UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
			{
				APlayerController* playerController = UMounteaInventoryUIStatics::FindPlayerController(GetOwner(), 3);
				if (!IsValid(playerController))
				{
					LOG_ERROR(TEXT("[MounteaCraftingParticipantUIComponent] Cannot find valid Player Controller. Crafting UI Subsystem unregistration failed!"))
					break;
				}

				const ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
				UMounteaAdvancedCraftingUISubsystem* craftingUISubsystem = IsValid(localPlayer)
					? localPlayer->GetSubsystem<UMounteaAdvancedCraftingUISubsystem>()
					: nullptr;
				if (!IsValid(craftingUISubsystem))
				{
					LOG_ERROR(TEXT("[MounteaCraftingParticipantUIComponent] Cannot find 'Crafting UI Subsystem'. UI will UNREGISTER improperly!"))
					break;
				}

				craftingUISubsystem->UnregisterCraftingUIManager(this);
			}
			break;
		}
	}

	Super::EndPlay(EndPlayReason);
}

bool UMounteaCraftingParticipantUIComponent::CreateCraftingWidget_Implementation()
{
	const UMounteaAdvancedCraftingUIConfig* craftingUIConfig = UMounteaCraftingUIStatics::GetCraftingUISettingsConfig();
	if (!craftingUIConfig)
	{
		LOG_ERROR(TEXT("[CreateCraftingWidget] Unable to load Crafting Config!"))
		return false;
	}

	auto widgetClass = craftingUIConfig->CraftingUserInterfaceClass.LoadSynchronous();
	if (!IsValid(widgetClass))
	{
		LOG_ERROR(TEXT("[CreateCraftingWidget] Unable to load Crafting UI Class from Config!"))
		return false;
	}
	if (!widgetClass->ImplementsInterface(UMounteaAdvancedInventoryCraftingWidgetInterface::StaticClass()))
	{
		LOG_ERROR(TEXT("[CreateCraftingWidget] Crafting UI Class must implement `MounteaAdvancedInventoryCraftingWidgetInterface`!"))
		return false;
	}

	FString message = TEXT("");
	bool bSuccess = true;

	int seachDepth = 0;
	APlayerController* playerController = UMounteaInventoryUIStatics::FindPlayerController(GetOwner(), seachDepth);
	if (!playerController || !playerController->IsLocalController())
	{
		message = !playerController ? TEXT("Invalid Player Controller!") : TEXT("UI can be shown only to Local Players!");
		bSuccess = false;
	}
	if (!bSuccess)
	{
		LOG_ERROR(TEXT("[CreateCraftingWidget] Failed to find Player Controller. Message:\n%s"), *message)
		return false;
	}

	if (!playerController->IsLocalController())
	{
		LOG_WARNING(TEXT("[CreateCraftingWidget] UI can be created for Local Players only!"))
		return false;
	}

	auto newWidget = CreateWidget<UUserWidget>(playerController, widgetClass);
	if (!newWidget)
	{
		LOG_ERROR(TEXT("[CreateCraftingWidget] Failed to create Crafting Widget!"))
		return false;
	}
	if (!newWidget->Implements<UMounteaAdvancedInventoryCraftingWidgetInterface>())
	{
		LOG_ERROR(TEXT("[CreateCraftingWidget] Crafting UI must implement `MounteaAdvancedInventoryCraftingWidgetInterface`!"))
		return false;
	}

	CraftingWidget = newWidget;

	TScriptInterface<IMounteaAdvancedInventoryCraftingWidgetInterface> craftingWidget = CraftingWidget;
	ensure(craftingWidget.GetObject() != nullptr);

	const auto sharedHUDSubsystem = GetSharedHUDSubsystem();
	if (!IsValid(sharedHUDSubsystem) || !IsValid(IMounteaAdvancedInventorySharedHUDInterface::Execute_GetWrapperWidget(sharedHUDSubsystem)))
	{
		LOG_WARNING(TEXT("[MounteaCraftingParticipantUIComponent] Cannot find 'Shared HUD Subsystem'. UI might not work properly"))
		if (CraftingWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
			IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(CraftingWidget, InventoryUICommands::General::Create, nullptr);
	}
	else
		IMounteaAdvancedInventorySharedHUDInterface::Execute_ExecuteWidgetCommand(sharedHUDSubsystem, InventoryUICommands::General::Create, CraftingWidget);

	return true;
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

bool UMounteaCraftingParticipantUIComponent::SetCraftingParticipant_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant)
{
	if (Participant == CraftingParticipant)
		return false;
	CraftingParticipant = Participant;
	return true;
}

UMounteaAdvancedInventorySharedHUDSubsystem* UMounteaCraftingParticipantUIComponent::GetSharedHUDSubsystem() const
{
	AActor* ownerActor = GetOwner();
	if (!IsValid(ownerActor))
		return nullptr;

	const APlayerController* playerController = UMounteaInventoryUIStatics::FindPlayerController(ownerActor, 3);
	if (!IsValid(playerController))
		return nullptr;

	const ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
	return IsValid(localPlayer) ? localPlayer->GetSubsystem<UMounteaAdvancedInventorySharedHUDSubsystem>() : nullptr;
}

