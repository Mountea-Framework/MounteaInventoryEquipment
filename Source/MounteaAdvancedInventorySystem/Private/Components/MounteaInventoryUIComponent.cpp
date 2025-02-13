// All rights reserved Dominik Morse 2024


#include "Components/MounteaInventoryUIComponent.h"

#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventorySystemStatics.h"

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
	// TODO: Get default class from Config and then save UI
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
	// TODO: Create new notification in the Container
}

void UMounteaInventoryUIComponent::RemoveInventoryNotifications_Implementation()
{
	// TODO: Clear the container
}

