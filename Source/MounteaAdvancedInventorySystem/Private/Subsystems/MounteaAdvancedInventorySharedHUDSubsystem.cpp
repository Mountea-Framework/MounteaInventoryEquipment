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


#include "Subsystems/MounteaAdvancedInventorySharedHUDSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Definitions/MounteaInventoryBaseCommands.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Widgets/MounteaInventorySystemWrapperWidgetInterface.h"
#include "Interfaces/Widgets/Notification/MounteaInventoryNotificationContainerWidgetInterface.h"
#include "Interfaces/Widgets/Notification/MounteaInventoryNotificationWidgetInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Settings/MounteaAdvancedInventoryUIConfig.h"
#include "Statics/MounteaInventoryUIStatics.h"

void UMounteaAdvancedInventorySharedHUDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMounteaAdvancedInventorySharedHUDSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UMounteaAdvancedInventorySharedHUDSubsystem::CreateWrapperWidget_Implementation()
{
	if (!IsValid(GetWorld()))
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Invalid World!"))
		return false;
	}	
	if (!IsValid(GetLocalPlayer()))
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Invalid Local Player!"))
		return false;
	}
	
	const auto localPlayerController = GetLocalPlayer()->GetPlayerController(GetWorld());
	if (!IsValid(localPlayerController))
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Invalid Player Controller!"))
		return false;
	}
	UMounteaAdvancedInventoryUIConfig* inventoryUIConfig = UMounteaInventoryUIStatics::GetInventoryUISettingsConfig();
	if (!inventoryUIConfig)
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Unable to load Inventory Config!"))
		return false;
	}
	UIConfig = inventoryUIConfig;
	
	auto widgetClass = UIConfig->UserInterfaceWrapperClass.LoadSynchronous();
	if (!IsValid(widgetClass))
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Unable to load UI Wrapper Class from Config!"))
		return false;
	}
	if (!widgetClass->ImplementsInterface(UMounteaInventorySystemWrapperWidgetInterface::StaticClass()))
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Base UI Wrapper Class must implement `MounteaInventorySystemBaseWidgetInterface`!"))
		return false;
	}

	FString returnMessage = TEXT("");
	bool bSuccess = true;
	
	int seachDepth = 0;
	const auto playerController = UMounteaInventoryUIStatics::FindPlayerController(localPlayerController, seachDepth);
	if (!playerController || !playerController->IsLocalController())
	{
		returnMessage = !playerController ? TEXT("Invalid Player Controller!") : TEXT("UI can be shown only to Local Players!");
		bSuccess = false;
	}
	if (!bSuccess)
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Failed to find Player Controller. Message:\n%s"), *returnMessage)
		return false;
	}

	if (!playerController->IsLocalController())
	{
		LOG_WARNING(TEXT("[CreateWrapperWidget] UI Wrapper Can be created for Local Players only!"))
		return false;
	}

	auto newWidget = CreateWidget<UUserWidget>(localPlayerController, widgetClass);
	if (!newWidget)
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Failed to create Wrapper Widget!"))
		return false;
	}
	if (!newWidget->Implements<UMounteaInventorySystemWrapperWidgetInterface>())
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Base UI wrapper must implement `MounteaInventorySystemBaseWidgetInterface`!"))
		return false;
	}

	SharedUserInterface = newWidget;
	ensure(SharedUserInterface != nullptr);
	
	SharedUserInterface->SetFocus();
	
	TScriptInterface<IMounteaInventorySystemWrapperWidgetInterface> sharedUserInterface = SharedUserInterface;
	ensure(sharedUserInterface.GetObject() != nullptr);
	
	sharedUserInterface->Execute_InitializeWrapperWidget(SharedUserInterface, this);
	
	if (SharedUserInterface->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(SharedUserInterface, InventoryUICommands::Wrapper::Create, nullptr);

	return true;
}

void UMounteaAdvancedInventorySharedHUDSubsystem::RemoveWrapperWidget_Implementation()
{
	TScriptInterface<IMounteaInventorySystemWrapperWidgetInterface> wrapperInterface = SharedUserInterface;
	ensure(wrapperInterface.GetObject() != nullptr);
	
	wrapperInterface->Execute_RemoveWrapperWidget(SharedUserInterface);
	
	if (SharedUserInterface->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(SharedUserInterface, InventoryUICommands::Wrapper::Remove, nullptr);
	else
	{
		SharedUserInterface->RemoveFromParent();
		SharedUserInterface = nullptr;
	}
}

UWidget* UMounteaAdvancedInventorySharedHUDSubsystem::GetNotificationContainer_Implementation() const
{
	return InventoryNotificationContainerWidget;
}

void UMounteaAdvancedInventorySharedHUDSubsystem::SetNotificationContainer_Implementation(UWidget* NewNotificationContainer)
{
	if (InventoryNotificationContainerWidget != NewNotificationContainer)
		InventoryNotificationContainerWidget = NewNotificationContainer;
}

void UMounteaAdvancedInventorySharedHUDSubsystem::CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData)
{
	if (!IsValid(InventoryNotificationContainerWidget))
	{
		LOG_WARNING(TEXT("[CreateInventoryNotification] Invalid Notification Container!"))
		return;
	}

	TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface> notificationContainerInterface = InventoryNotificationContainerWidget;
	ensure(notificationContainerInterface.GetObject() != nullptr);
	
	if (!UIConfig)
	{
		LOG_ERROR(TEXT("[CreateInventoryNotification] Unable to load Inventory Config!"))
		return;
	}

	if (UIConfig->NotificationWidgetClass.IsNull())
	{
		LOG_ERROR(TEXT("[CreateInventoryNotification] Unable to load `NotificationWidgetClass` from Config!"))
		return;
	}
	
	auto notificationClass = UIConfig->NotificationWidgetClass.LoadSynchronous();
	auto notificationWidget = CreateWidget(InventoryNotificationContainerWidget, notificationClass);
	if (!IsValid(notificationWidget))
	{
		LOG_ERROR(TEXT("[CreateInventoryNotification] Failed to Create Inventory Notification!"))
		return;
	}

	IMounteaInventoryNotificationWidgetInterface::Execute_CreateNotification(notificationWidget, NotificationData, InventoryNotificationContainerWidget);

	notificationContainerInterface->Execute_AddNotification(InventoryNotificationContainerWidget, notificationWidget);
}

void UMounteaAdvancedInventorySharedHUDSubsystem::RemoveInventoryNotifications_Implementation()
{
	if (!IsValid(InventoryNotificationContainerWidget))
	{
		LOG_WARNING(TEXT("[CreateInventoryNotification] Invalid Notification Container!"))
		return;
	}

	TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface> notificationContainerInterface = InventoryNotificationContainerWidget;
	ensure(notificationContainerInterface.GetObject() != nullptr);

	IMounteaInventoryNotificationContainerWidgetInterface::Execute_ClearNotifications(InventoryNotificationContainerWidget);
}

void UMounteaAdvancedInventorySharedHUDSubsystem::ExecuteWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload)
{
	if (SharedUserInterface && SharedUserInterface->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(SharedUserInterface, Command, OptionalPayload);
}

void UMounteaAdvancedInventorySharedHUDSubsystem::SetWidgetStates_Implementation(const FGameplayTagContainer& NewStates)
{
	if (WidgetStatesContainer != NewStates)
		WidgetStatesContainer = NewStates;
}

bool UMounteaAdvancedInventorySharedHUDSubsystem::AddWidgetStateTag_Implementation(const FGameplayTag& Tag)
{
	if (WidgetStatesContainer.HasTag(Tag))
		return false;
	WidgetStatesContainer.AddTag(Tag);
	return true;
}

bool UMounteaAdvancedInventorySharedHUDSubsystem::RemoveWidgetStateTag_Implementation(const FGameplayTag& Tag)
{
	return WidgetStatesContainer.RemoveTag(Tag);
}

bool UMounteaAdvancedInventorySharedHUDSubsystem::HasWidgetStateTag_Implementation(const FGameplayTag& Tag, bool bExactMatch) const
{
	return bExactMatch ? WidgetStatesContainer.HasTagExact(Tag) : WidgetStatesContainer.HasTag(Tag);
}

bool UMounteaAdvancedInventorySharedHUDSubsystem::AppendWidgetStateTags_Implementation(const FGameplayTagContainer& TagsToAppend)
{
	WidgetStatesContainer.AppendTags(TagsToAppend);
	return true;
}