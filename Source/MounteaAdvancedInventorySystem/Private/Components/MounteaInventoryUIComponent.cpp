// All rights reserved Dominik Morse 2024


#include "Components/MounteaInventoryUIComponent.h"

#include "Blueprint/UserWidget.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/Widgets/MounteaInventoryBaseWidgetInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Statics/MounteaInventorySystemStatics.h"
#include "Statics/MounteaInventoryUIStatics.h"

class UMounteaAdvancedInventorySettings;

UMounteaInventoryUIComponent::UMounteaInventoryUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaInventoryUIComponent::SetComponentTickEnabled(false);
	
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(false);
	SetActiveFlag(true);

	ComponentTags.Append( { TEXT("Mountea"), TEXT("Inventory"), TEXT("UI") } );
}

void UMounteaInventoryUIComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
	{
		auto inventoryComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedInventoryInterface::StaticClass());
		if (!IsValid(inventoryComponent))
		{
			LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory' component in Parent! UI will NOT work!"))
		}
		else
		{
			Execute_SetParentInventory(this, inventoryComponent);
			ensureMsgf(ParentInventory.GetObject() != nullptr, TEXT("[MounteaInventoryUIComponent] Failed to update 'ParentInventory'"));

			ParentInventory->GetOnNotificationProcessedEventHandle().AddUniqueDynamic(this, &UMounteaInventoryUIComponent::UMounteaInventoryUIComponent::CreateInventoryNotification);
		}
	}
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryUIComponent::GetParentInventory_Implementation() const
{
	return ParentInventory;
}

void UMounteaInventoryUIComponent::SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	// TODO: broadcast
	if (ParentInventory != NewParentInventory)
		ParentInventory = NewParentInventory;
}

bool UMounteaInventoryUIComponent::CreateInventoryUI_Implementation()
{
	const UMounteaAdvancedInventorySettingsConfig* Config = GetDefault<UMounteaAdvancedInventorySettings>()->InventorySettingsConfig.LoadSynchronous();
	if (!Config)
	{
		LOG_ERROR(TEXT("[Create Inventory UI] Unable to load Inventory Config!"))
		return false;
	}

	auto widgetClass = Config->InventoryWidgetClass.LoadSynchronous();
	if (!IsValid(widgetClass))
	{
		LOG_ERROR(TEXT("[Create Inventory UI] Unable to load Inventory UI Class from Config!"))
		return false;
	}
	if (!widgetClass->ImplementsInterface(UMounteaInventoryBaseWidgetInterface::StaticClass()))
	{
		LOG_ERROR(TEXT("[Create Inventory UI] Base Inventory UI Class must implement `MounteaInventoryBaseWidgetInterface`!"))
		return false;
	}

	FString Message = TEXT("");
	bool bSuccess = true;
	
	int seachDepth = 0;
	APlayerController* playerController = UMounteaInventoryUIStatics::FindPlayerController(GetOwner(), seachDepth);
	if (!playerController || !playerController->IsLocalController())
	{
		Message = !playerController ? TEXT("Invalid Player Controller!") : TEXT("UI can be shown only to Local Players!");
		bSuccess = false;
	}
	if (!bSuccess)
	{
		LOG_ERROR(TEXT("[Create Inventory UI] Failed to find Player Controller. Message:\n%s"), *Message)
		return false;
	}

	if (!playerController->IsLocalController())
	{
		LOG_WARNING(TEXT("[Create Inventory UI] UI Can be created for Local Players only!"))
		return false;
	}

	auto newWidget = CreateWidget<UUserWidget>(playerController, widgetClass);
	if (!newWidget->Implements<UMounteaInventoryBaseWidgetInterface>())
	{
		LOG_ERROR(TEXT("[Create Inventory UI] Base Inventory UI  must implement `MounteaInventoryBaseWidgetInterface`!"))
		return false;
	}

	InventoryWidget = newWidget;

	// TODO: Inventory Generic Widget and execute InitializeInventoryUI command

	return true;
}

void UMounteaInventoryUIComponent::RemoveInventoryUI_Implementation()
{
	// TODO: Remove UI from viewport
}

void UMounteaInventoryUIComponent::SetInventoryUIVisibility_Implementation(const ESlateVisibility NewVisibility)
{
	// TODO: Execute command to show/hide
}

UUserWidget* UMounteaInventoryUIComponent::GetNotificationContainer_Implementation() const
{
	return InventoryNotificationContainerWidget;
}

void UMounteaInventoryUIComponent::SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer)
{
}

void UMounteaInventoryUIComponent::CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData)
{
	//LOG_WARNING(TEXT("Notification: %s"), *NotificationData.ToString())
	// TODO: Create new notification in the Container
}

void UMounteaInventoryUIComponent::RemoveInventoryNotifications_Implementation()
{
	// TODO: Clear the container
}

