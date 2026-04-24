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
#include "Engine/LocalPlayer.h"

#include "Definitions/MounteaInventoryBaseCommands.h"

#include "Helpers/MounteaAdvancedInventoryWidgetPayload.h"

#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/UserInterface/MounteaAdvancedInventorySharedHUDInterface.h"
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
#include "Subsystems/MounteaAdvancedInventorySharedHUDSubsystem.h"

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
		UIConfig = GetDefault<UMounteaAdvancedInventorySettings>()->AdvancedInventoryUISettingsConfig.LoadSynchronous();
		Execute_EmptyItemActionsQueue(this); // Do NOT receive any queue items
		
		{
			auto inventoryComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedInventoryInterface::StaticClass());
			if (!IsValid(inventoryComponent))
				LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory' component in Parent! UI will NOT work!"))
			else
			{
				Execute_SetParentInventory(this, inventoryComponent);
				ensureMsgf(ParentInventory.GetObject() != nullptr, TEXT("[MounteaInventoryUIComponent] Failed to update 'ParentInventory'"));

				ParentInventory->GetOnNotificationProcessedEventHandle().AddUniqueDynamic(this, &UMounteaInventoryUIComponent::ForwardInventoryNotificationToSubsystem);
			
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

UMounteaAdvancedInventorySharedHUDSubsystem* UMounteaInventoryUIComponent::GetSharedHUDSubsystem() const
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

	const auto sharedHUDSubsystem = GetSharedHUDSubsystem();
	if (!IsValid(sharedHUDSubsystem) || !IsValid(IMounteaAdvancedInventorySharedHUDInterface::Execute_GetWrapperWidget(sharedHUDSubsystem)))
	{
		LOG_WARNING(TEXT("[MounteaInventoryUIComponent] Cannot find 'Shared HUD Subsystem'. UI might not work properly"))
		if (InventoryWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
			IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(InventoryWidget, InventoryUICommands::General::Create, nullptr);
	}
	else
		IMounteaAdvancedInventorySharedHUDInterface::Execute_ExecuteWidgetCommand(sharedHUDSubsystem, InventoryUICommands::General::Create, InventoryWidget);

	return true;
}

void UMounteaInventoryUIComponent::RemoveInventoryWidget_Implementation()
{
	if (!IsValid(InventoryWidget))
		return;
	
	const auto sharedHUDSubsystem = GetSharedHUDSubsystem();
	if (!IsValid(sharedHUDSubsystem) || !IsValid(IMounteaAdvancedInventorySharedHUDInterface::Execute_GetWrapperWidget(sharedHUDSubsystem)))
	{
		LOG_WARNING(TEXT("[MounteaInventoryUIComponent] Cannot find 'Shared HUD Subsystem'. UI might not work properly"))
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
	}	
	else
		IMounteaAdvancedInventorySharedHUDInterface::Execute_ExecuteWidgetCommand(sharedHUDSubsystem, InventoryUICommands::General::Remove, InventoryWidget);
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
	
	if (UIConfig && UIConfig->bAllowAutoFocus && ActiveItemWidget)
		ActiveItemWidget->SetFocus();	
}

void UMounteaInventoryUIComponent::ProcessItemAdded_Implementation(const FMounteaInventoryItem& AddedItem)
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

void UMounteaInventoryUIComponent::ProcessItemModified_Implementation(const FMounteaInventoryItem& ModifiedItem)
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

void UMounteaInventoryUIComponent::ProcessItemRemoved_Implementation(const FMounteaInventoryItem& RemovedItem)
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

void UMounteaInventoryUIComponent::ProcessItemDurabilityChanged(const FMounteaInventoryItem& Item, const float OldDurability, const float NewDurability)
{
	Execute_ProcessItemModified(this, Item);
}

void UMounteaInventoryUIComponent::ForwardInventoryNotificationToSubsystem(const FInventoryNotificationData& NotificationData)
{
	const auto sharedHUDSubsystem = GetSharedHUDSubsystem();
	if (!IsValid(sharedHUDSubsystem) || !IsValid(IMounteaAdvancedInventorySharedHUDInterface::Execute_GetWrapperWidget(sharedHUDSubsystem)))
	{
		LOG_WARNING(TEXT("[MounteaInventoryUIComponent] Cannot find 'Shared HUD Subsystem'. Notification cannot be displayed"))
		return;
	}

	IMounteaAdvancedInventorySharedHUDInterface::Execute_CreateInventoryNotification(sharedHUDSubsystem, NotificationData);
}

void UMounteaInventoryUIComponent::ProcessItemQuantityChanged(const FMounteaInventoryItem& Item, const int32 OldQuantity, const int32 NewQuantity)
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

void UMounteaInventoryUIComponent::EmptyItemActionsQueue_Implementation()
{
	ActionsQueue.Clear();
}

TArray<UMounteaSelectableInventoryItemAction*> UMounteaInventoryUIComponent::GetItemActionsQueue_Implementation() const
{
	TArray<UMounteaSelectableInventoryItemAction*> Result;
	Result.Reserve(ActionsQueue.Pending.Num());

	Algo::TransformIf(
		ActionsQueue.Pending,
		Result,
		[](const FActionQueueEntry& entry) { return IsValid(entry.Action); },
		[](const FActionQueueEntry& entry) { return entry.Action.Get(); }
	);

	return Result;
}

