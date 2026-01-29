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


#include "Components/MounteaInventoryUIComponent.h"

#include "Algo/AnyOf.h"
#include "Blueprint/UserWidget.h"
#include "Decorations/MounteaSelectableInventoryItemAction.h"

#include "Definitions/MounteaInventoryBaseCommands.h"

#include "Helpers/MounteaAdvancedInventoryWidgetPayload.h"

#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/Widgets/MounteaInventorySystemWrapperWidgetInterface.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Widgets/Inventory/MounteaAdvancedInventoryWidgetInterface.h"
#include "Interfaces/Widgets/Notification/MounteaInventoryNotificationContainerWidgetInterface.h"
#include "Interfaces/Widgets/Notification/MounteaInventoryNotificationWidgetInterface.h"

#include "Logs/MounteaAdvancedInventoryLog.h"

#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventoryUIConfig.h"

#include "Statics/MounteaInventorySystemStatics.h"
#include "Statics/MounteaInventoryUIStatics.h"
#include "Subsystems/MounteaAdvancedInventoryUISubsystem.h"

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

	if (GetOwner() && 
		(GetOwnerRole() == ROLE_Authority || GetOwnerRole() == ROLE_AutonomousProxy) && 
		UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
	{
		Execute_EmptyItemActionsQueue(this); // Do NOT receive any queue items
		
		{
			auto inventoryComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedInventoryInterface::StaticClass());
			if (!IsValid(inventoryComponent))
				LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory' component in Parent! UI will NOT work!"))
			else
			{
				Execute_SetParentInventory(this, inventoryComponent);
				ensureMsgf(ParentInventory.GetObject() != nullptr, TEXT("[MounteaInventoryUIComponent] Failed to update 'ParentInventory'"));

				ParentInventory->GetOnNotificationProcessedEventHandle().AddUniqueDynamic(this, &UMounteaInventoryUIComponent::CreateInventoryNotification);
			
				ParentInventory->GetOnItemAddedEventHandle().AddUniqueDynamic(this, &UMounteaInventoryUIComponent::ProcessItemAdded);
				ParentInventory->GetOnItemRemovedEventHandle().AddUniqueDynamic(this, &UMounteaInventoryUIComponent::ProcessItemRemoved);

				ParentInventory->GetOnItemDurabilityChangedEventHandle().AddUniqueDynamic(this, &UMounteaInventoryUIComponent::ProcessItemDurabilityChanged);
				ParentInventory->GetOnItemQuantityChangedEventHandle().AddUniqueDynamic(this, &UMounteaInventoryUIComponent::ProcessItemQuantityChanged);
			}
		
			const auto inventoryUISubsystem = UMounteaInventoryUIStatics::GetInventoryUISubsystem(
				UMounteaInventoryUIStatics::FindPlayerController(GetOwner(), 3));
			if (!inventoryUISubsystem)
				LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory UI Subsystem'. UI will NOT work!"))
			else
				inventoryUISubsystem->RegisterInventoryUIManager(this);
		}
	}
}

void UMounteaInventoryUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Execute_EmptyItemActionsQueue(this); // we don't care if this runs on server I guess?
	
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
					const auto inventoryUISubsystem = UMounteaInventoryUIStatics::GetInventoryUISubsystem(
						UMounteaInventoryUIStatics::FindPlayerController(GetOwner(), 3));
					if (!inventoryUISubsystem)
						LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory UI Subsystem'. UI will UNREGISTER properly!"))
					else
						inventoryUISubsystem->UnregisterInventoryUIManager(this);
				}
			}
			break;
	}
	
	Super::EndPlay(EndPlayReason);
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryUIComponent::GetParentInventory_Implementation() const
{
	return ParentInventory;
}

void UMounteaInventoryUIComponent::SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	if (ParentInventory != NewParentInventory) ParentInventory = NewParentInventory;
}

bool UMounteaInventoryUIComponent::CreateWrapperWidget_Implementation()
{
	const UMounteaAdvancedInventoryUIConfig* inventoryUIConfig = UMounteaInventoryUIStatics::GetInventoryUISettingsConfig();
	if (!inventoryUIConfig)
	{
		LOG_ERROR(TEXT("[CreateWrapperWidget] Unable to load Inventory Config!"))
		return false;
	}

	auto widgetClass = inventoryUIConfig->UserInterfaceWrapperClass.LoadSynchronous();
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
		LOG_ERROR(TEXT("[CreateWrapperWidget] Failed to find Player Controller. Message:\n%s"), *Message)
		return false;
	}

	if (!playerController->IsLocalController())
	{
		LOG_WARNING(TEXT("[CreateWrapperWidget] UI Wrapper Can be created for Local Players only!"))
		return false;
	}

	auto newWidget = CreateWidget<UUserWidget>(playerController, widgetClass);
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

	WrapperWidget = newWidget;
	ensure(WrapperWidget != nullptr);
	
	WrapperWidget->SetIsFocusable(true);
	
	TScriptInterface<IMounteaInventorySystemWrapperWidgetInterface> wrapperWidget = WrapperWidget;
	ensure(wrapperWidget.GetObject() != nullptr);
	
	wrapperWidget->Execute_InitializeWrapperWidget(WrapperWidget, this);
	
	if (WrapperWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(WrapperWidget, InventoryUICommands::Wrapper::Create, nullptr);

	return true;
}

void UMounteaInventoryUIComponent::RemoveWrapperWidget_Implementation()
{
	TScriptInterface<IMounteaInventorySystemWrapperWidgetInterface> wrapperInterface = WrapperWidget;
	ensure(wrapperInterface.GetObject() != nullptr);
	
	wrapperInterface->Execute_RemoveWrapperWidget(WrapperWidget);
	
	if (WrapperWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(WrapperWidget, InventoryUICommands::Wrapper::Remove, nullptr);
	else
	{
		WrapperWidget->RemoveFromParent();
		WrapperWidget = nullptr;
	}
}

bool UMounteaInventoryUIComponent::CreateInventoryWidget_Implementation()
{
	const UMounteaAdvancedInventoryUIConfig* inventoryUIConfig = UMounteaInventoryUIStatics::GetInventoryUISettingsConfig();
	if (!inventoryUIConfig)
	{
		LOG_ERROR(TEXT("[CreateInventoryWidget] Unable to load Inventory Config!"))
		return false;
	}

	auto widgetClass = inventoryUIConfig->InventoryWidgetClass.LoadSynchronous();
	if (!IsValid(widgetClass))
	{
		LOG_ERROR(TEXT("[CreateInventoryWidget] Unable to load Inventory UI Class from Config!"))
		return false;
	}
	if (!widgetClass->ImplementsInterface(UMounteaAdvancedInventoryWidgetInterface::StaticClass()))
	{
		LOG_ERROR(TEXT("[CreateInventoryWidget] Inventory UI Class must implement `MounteaAdvancedInventoryWidgetInterface`!"))
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
		LOG_ERROR(TEXT("[CreateInventoryWidget] Failed to find Player Controller. Message:\n%s"), *Message)
		return false;
	}

	if (!playerController->IsLocalController())
	{
		LOG_WARNING(TEXT("[CreateInventoryWidget] UI Can be created for Local Players only!"))
		return false;
	}

	auto newWidget = CreateWidget<UUserWidget>(playerController, widgetClass);
	if (!newWidget)
	{
		LOG_ERROR(TEXT("[CreateInventoryWidget] Failed to create Inventory Widget!"))
		return false;
	}
	if (!newWidget->Implements<UMounteaAdvancedInventoryWidgetInterface>())
	{
		LOG_ERROR(TEXT("[CreateInventoryWidget] Inventory UI must implement `MounteaAdvancedInventoryWidgetInterface`!"))
		return false;
	}

	InventoryWidget = newWidget;
	
	TScriptInterface<IMounteaAdvancedInventoryWidgetInterface> inventoryWidget = InventoryWidget;
	ensure(inventoryWidget.GetObject() != nullptr);
	
	if (InventoryWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::General::Create, nullptr);

	return true;
}

void UMounteaInventoryUIComponent::RemoveInventoryWidget_Implementation()
{
	TScriptInterface<IMounteaAdvancedInventoryWidgetInterface> inventoryWrapper = InventoryWidget;
	ensure(inventoryWrapper.GetObject() != nullptr);
	
	if (WrapperWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::General::Remove, nullptr);
	else
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
	}
}

bool UMounteaInventoryUIComponent::SetInventoryWidget_Implementation(UUserWidget* NewInventoryWidget)
{
	if (InventoryWidget == NewInventoryWidget)
		return false;
	
	InventoryWidget = NewInventoryWidget;
	return true;
}

void UMounteaInventoryUIComponent::SetActiveItemWidget_Implementation(UWidget* NewActiveItemWidget)
{
	if (ActiveItemWidget != NewActiveItemWidget)
		ActiveItemWidget = NewActiveItemWidget;
	
	if (IsValid(ActiveItemWidget))
		ActiveItemWidget->SetFocus();
}

UUserWidget* UMounteaInventoryUIComponent::GetNotificationContainer_Implementation() const
{
	return InventoryNotificationContainerWidget;
}

void UMounteaInventoryUIComponent::SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer)
{
	if (InventoryNotificationContainerWidget != NewNotificationContainer)
		InventoryNotificationContainerWidget = NewNotificationContainer;
}

void UMounteaInventoryUIComponent::CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData)
{
	if (!IsValid(InventoryNotificationContainerWidget))
	{
		LOG_WARNING(TEXT("[CreateInventoryNotification] Invalid Notification Container!"))
		return;
	}

	TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface> notificationContainerInterface = InventoryNotificationContainerWidget;
	ensure(notificationContainerInterface.GetObject() != nullptr);
	
	const UMounteaAdvancedInventoryUIConfig* Config = GetDefault<UMounteaAdvancedInventorySettings>()->AdvancedInventoryUISettingsConfig.LoadSynchronous();
	if (!Config)
	{
		LOG_ERROR(TEXT("[CreateInventoryNotification] Unable to load Inventory Config!"))
		return;
	}

	if (Config->NotificationWidgetClass.IsNull())
	{
		LOG_ERROR(TEXT("[CreateInventoryNotification] Unable to load `NotificationWidgetClass` from Config!"))
		return;
	}
	
	auto notificationClass = Config->NotificationWidgetClass.LoadSynchronous();
	auto notificationWidget = CreateWidget(InventoryNotificationContainerWidget, notificationClass);
	if (!IsValid(notificationWidget))
	{
		LOG_ERROR(TEXT("[CreateInventoryNotification] Failed to Create Inventory Notification!"))
		return;
	}

	IMounteaInventoryNotificationWidgetInterface::Execute_CreateNotification(notificationWidget, NotificationData, InventoryNotificationContainerWidget);

	notificationContainerInterface->Execute_AddNotification(InventoryNotificationContainerWidget, notificationWidget);
}

void UMounteaInventoryUIComponent::RemoveInventoryNotifications_Implementation()
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

void UMounteaInventoryUIComponent::ProcessItemAdded_Implementation(const FInventoryItem& AddedItem)
{
	if (!IsValid(InventoryWidget))
	{
		LOG_WARNING(TEXT("[ProcessItemAdded] Invalid Inventory UI!")) return;
	}

	if (InventoryWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
	{
		UMounteaAdvancedInventoryWidgetPayload* newPayload = NewObject<UMounteaAdvancedInventoryWidgetPayload>();
		newPayload->PayloadData.Add(AddedItem.Guid);
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::Items::Added, newPayload);
	}
}

void UMounteaInventoryUIComponent::ProcessItemModified_Implementation(const FInventoryItem& ModifiedItem)
{
	if (!IsValid(InventoryWidget))
	{
		LOG_WARNING(TEXT("[ProcessItemAdded] Invalid Inventory UI!"))
		return;
	}

	if (InventoryWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
	{
		UMounteaAdvancedInventoryWidgetPayload* newPayload = NewObject<UMounteaAdvancedInventoryWidgetPayload>();
		newPayload->PayloadData.Add(ModifiedItem.Guid);
		newPayload->PayloadQuantities.Add(ModifiedItem.Quantity);
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::Items::Modified, newPayload);
	}
}

void UMounteaInventoryUIComponent::ProcessItemRemoved_Implementation(const FInventoryItem& RemovedItem)
{
	if (!IsValid(InventoryWidget))
	{
		LOG_WARNING(TEXT("[ProcessItemAdded] Invalid Inventory UI!")) 
		return;
	}

	if (InventoryWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
	{
		UMounteaAdvancedInventoryWidgetPayload* newPayload = NewObject<UMounteaAdvancedInventoryWidgetPayload>();
		newPayload->PayloadData.Add(RemovedItem.Guid);
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::Items::Removed, newPayload);
	}
}

void UMounteaInventoryUIComponent::CategorySelected_Implementation(const FString& SelectedCategoryId)
{
	if (ActiveCategoryId.Equals(SelectedCategoryId, ESearchCase::IgnoreCase)) return;
	ActiveCategoryId = SelectedCategoryId;
	ActiveItemGuid = FGuid();	
	ActiveItemWidget = nullptr;
	
	if (IsValid(InventoryWidget) && InventoryWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
	{
		TScriptInterface<IMounteaInventoryGenericWidgetInterface> genericWidget = InventoryWidget;
		genericWidget->Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::Categories::Selected, nullptr);
	}

	OnCategorySelected.Broadcast(SelectedCategoryId);
}

void UMounteaInventoryUIComponent::ItemSelected_Implementation(const FGuid& SelectedItem)
{
	if (ActiveItemGuid == SelectedItem) return;
	ActiveItemGuid = SelectedItem;
	
	if (IsValid(InventoryWidget) && InventoryWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
	{
		TScriptInterface<IMounteaInventoryGenericWidgetInterface> genericWidget = InventoryWidget;
		genericWidget->Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::Items::Selected, nullptr);
	}

	OnItemSelected.Broadcast(SelectedItem);
}

void UMounteaInventoryUIComponent::AddCustomItemToMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (FInventoryUICustomData* customData = CustomItemsMap.Find(ItemTag))
	{
		if (!customData->StoredIds.Contains(ItemId))
			customData->StoredIds.Add(ItemId);
	}
	else
	{
		FInventoryUICustomData newData;
		newData.StoredIds.Add(ItemId);

		CustomItemsMap.Add(ItemTag, MoveTemp(newData));
	}	
}

void UMounteaInventoryUIComponent::ProcessItemDurabilityChanged(const FInventoryItem& Item, const float OldDurability, const float NewDurability)
{
	Execute_ProcessItemModified(this, Item);
}

void UMounteaInventoryUIComponent::ProcessItemQuantityChanged(const FInventoryItem& Item, const int32 OldQuantity, const int32 NewQuantity)
{
	Execute_ProcessItemModified(this, Item);
}

bool UMounteaInventoryUIComponent::RemoveCustomItemFromMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	FInventoryUICustomData* foundData = CustomItemsMap.Find(ItemTag);
	if (!foundData || foundData->StoredIds.IsEmpty())
		return false;

	const int32 removedCount = foundData->StoredIds.RemoveAll([&ItemId](const FGuid& storedId)
	{
		return storedId == ItemId;
	});

	if (removedCount > 0 && foundData->StoredIds.Num() == 0)
		CustomItemsMap.Remove(ItemTag);

	return removedCount > 0;
}

bool UMounteaInventoryUIComponent::IsItemStoredInCustomMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	const FInventoryUICustomData* foundData = CustomItemsMap.Find(ItemTag);
	if (!foundData)
		return false;
	
	return Algo::AnyOf(foundData->StoredIds, [&ItemId](const FGuid& storedId)
	{
		return storedId == ItemId;
	});
}

void UMounteaInventoryUIComponent::ExecuteWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload)
{
	if (WrapperWidget && WrapperWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(WrapperWidget, Command, OptionalPayload);
}

bool UMounteaInventoryUIComponent::EnqueueItemAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload)
{
	if (!IsValid(ItemAction))
		return false;
    
	FActionQueueEntry entry;
	entry.Action = ItemAction;
	entry.Payload = Payload;
	entry.CreationTime = FDateTime::Now();
    
	ActionsQueue.Enqueue(MoveTemp(entry));
    
	return true;
}

void UMounteaInventoryUIComponent::CompleteQueuedAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload)
{
	if (!IsValid(ItemAction))
		return;
    
	ItemAction->ExecuteQueuedAction(Payload);
	ActionsQueue.Remove(ItemAction);
}

void UMounteaInventoryUIComponent::CancelQueuedAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction)
{
	if (!IsValid(ItemAction))
		return;
	
	ItemAction->CancelInventoryAction();
	ActionsQueue.Remove(ItemAction);
}

/* TODO: do some cleanup, like every 30 seconds? Make it into UI Config?
void UMounteaInventoryUIComponent::CleanupExpiredActions_Implementation(float MaxAgeSeconds)
{
	FTimespan maxAge = FTimespan::FromSeconds(MaxAgeSeconds);
	FDateTime now = FDateTime::Now();
    
	for (int32 i = ActionsQueue.Pending.Num() - 1; i >= 0; --i)
	{
		const FActionQueueEntry& entry = ActionsQueue.Pending[i];
		if ((now - entry.CreationTime) > maxAge && entry.Action)
			Execute_CancelQueuedAction(this, entry.Action.Get());
	}
}
*/

void UMounteaInventoryUIComponent::EmptyItemActionsQueue_Implementation()
{
	ActionsQueue.Clear();
}

TArray<UMounteaSelectableInventoryItemAction*> UMounteaInventoryUIComponent::GetItemActionsQueue_Implementation() const
{
	TArray<UMounteaSelectableInventoryItemAction*> result;
	result.Reserve(ActionsQueue.Pending.Num());
    
	for (const FActionQueueEntry& entry : ActionsQueue.Pending)
	{
		if (entry.Action)
			result.Add(entry.Action.Get());
	}
    
	return result;
}
