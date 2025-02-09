// All rights reserved Dominik Morse 2024

#include "Components/MounteaInventoryComponent.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Net/UnrealNetwork.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

UMounteaInventoryComponent::UMounteaInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaInventoryComponent::SetComponentTickEnabled(false);
	
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append( { TEXT("Mountea"), TEXT("Inventory") } );
}

void UMounteaInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMounteaInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMounteaInventoryComponent, InventoryItems);
}

void UMounteaInventoryComponent::OnRep_InventoryItems()
{
	// Process replicated Items
	// TODO refresh UI by sending Command
}

AActor* UMounteaInventoryComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

bool UMounteaInventoryComponent::AddItem_Implementation(const FInventoryItem& Item)
{
	if (!Execute_CanAddItem(this, Item))
	{
		Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
			EInventoryNotificationType::EINT_ItemNotUpdated,
			EInventoryNotificationCategory::EINC_Error,
			NSLOCTEXT("Inventory", "CannotAddItem", "Cannot add item to inventory"),
			Item.GetGuid(),
			this,
			Item.GetQuantity(),
			0
		));
		return false;
	}

	if (!IsAuthority())
	{
		AddItem_Server(Item);
		return true;
	}
	
	auto existingItem = Execute_FindItem(this, FInventoryItemSearchParams(Item.GetGuid()));
	if (!existingItem.IsItemValid())
		existingItem = Execute_FindItem(this, FInventoryItemSearchParams(Item.GetTemplate()));

	if (existingItem.IsItemValid())
	{
		const int32 AvailableSpace = Item.GetTemplate()->MaxQuantity - existingItem.GetQuantity();
		const int32 AmountToAdd = FMath::Min(Item.GetQuantity(), AvailableSpace);

		// Handle source inventory for partial quantity
		if (Item.IsItemInInventory())
		{
			if (!Execute_DecreaseItemQuantity(Item.GetOwningInventory().GetObject(), Item.GetGuid(), AmountToAdd))
			{
				Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
					EInventoryNotificationType::EINT_ItemNotUpdated,
					EInventoryNotificationCategory::EINC_Error,
					NSLOCTEXT("Inventory", "SourceInventoryError", "Failed to update source inventory"),
					Item.GetGuid(),
					this,
					AmountToAdd,
					0
				));
				return false;
			}
		}

		if (Execute_IncreaseItemQuantity(this, existingItem.GetGuid(), AmountToAdd))
		{
			const auto NotifType = AmountToAdd < Item.GetQuantity() ? 
				EInventoryNotificationType::EINT_ItemPartiallyAdded : 
				EInventoryNotificationType::EINT_ItemAdded;

			Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
				NotifType,
				EInventoryNotificationCategory::EINC_Info,
				NSLOCTEXT("Inventory", "ItemQuantityUpdated", "Updated item quantity"),
				existingItem.GetGuid(),
				this,
				existingItem.GetQuantity(),
				AmountToAdd
			));

			InventoryItems.MarkArrayDirty();
			OnItemAdded.Broadcast(existingItem);

			// TODO: Don't spam client if same as server
			PostItemAdded_Client(existingItem);
			return true;
		}
	}
	else
	{
		// For new items, we can add with full or capped quantity
		const int32 AmountToAdd = FMath::Min(Item.GetQuantity(), Item.GetTemplate()->MaxQuantity);
		
		// Handle source inventory
		if (Item.IsItemInInventory())
		{
			if (!Execute_DecreaseItemQuantity(Item.GetOwningInventory().GetObject(), Item.GetGuid(), AmountToAdd))
			{
				Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
					EInventoryNotificationType::EINT_ItemNotUpdated,
					EInventoryNotificationCategory::EINC_Error,
					NSLOCTEXT("Inventory", "SourceInventoryError", "Failed to update source inventory"),
					Item.GetGuid(),
					this,
					AmountToAdd,
					0
				));
				return false;
			}
		}

		// Create new item with correct quantity
		FInventoryItem newItem = Item;
		if (AmountToAdd != Item.GetQuantity())
			newItem.Quantity += AmountToAdd;

		InventoryItems.Items.Add(newItem);
		InventoryItems.Items.Last().SetOwningInventory(this);
		InventoryItems.MarkArrayDirty();

		const auto NotifType = AmountToAdd < Item.GetQuantity() ? 
			EInventoryNotificationType::EINT_ItemPartiallyAdded : 
			EInventoryNotificationType::EINT_ItemAdded;

		Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
			NotifType,
			EInventoryNotificationCategory::EINC_Info,
			NSLOCTEXT("Inventory", "ItemAdded", "Successfully added items"),
			newItem.GetGuid(),
			this,
			AmountToAdd,
			AmountToAdd
		));

		OnItemAdded.Broadcast(newItem);

		// TODO: Don't spam client if same as server
		PostItemAdded_Client(newItem);
		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::AddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity, const float Durability)
{
	if (!IsValid(Template))
	{
		Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
			EInventoryNotificationType::EINT_ItemNotUpdated,
			EInventoryNotificationCategory::EINC_Error,
			NSLOCTEXT("Inventory", "InvalidTemplate", "Invalid item template"),
			FGuid(),
			this,
			Quantity,
			0
		));
		return false;
	}

	return Execute_AddItem(this, FInventoryItem(Template, Quantity, Durability, nullptr));
}

bool UMounteaInventoryComponent::RemoveItem_Implementation(const FGuid& ItemGuid)
{
	const int32 ItemIndex = Execute_FindItemIndex(this, FInventoryItemSearchParams(ItemGuid));
	if (ItemIndex == INDEX_NONE)
		return false;

	const FInventoryItem RemovedItem = InventoryItems.Items[ItemIndex];
	InventoryItems.Items.RemoveAt(ItemIndex);
	InventoryItems.MarkArrayDirty();

	OnItemRemoved.Broadcast(RemovedItem);
	return true;
}

bool UMounteaInventoryComponent::RemoveItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* const Template, const int32 Quantity)
{
	if (!IsValid(Template))
		return false;

	auto inventoryItem = Execute_FindItem(this, FInventoryItemSearchParams(Template));
	if (inventoryItem.IsItemValid())
	{
		return Execute_RemoveItem(this, inventoryItem.GetGuid());
	}
   
	Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
		EInventoryNotificationType::EINT_ItemNotUpdated,
		EInventoryNotificationCategory::EINC_Error,
		NSLOCTEXT("Inventory", "RemoveTemplateFailed", "Cannot find item matching template with sufficient quantity"),
		FGuid(),
		this,
		Quantity,
		0
	));
	return false;
}

bool UMounteaInventoryComponent::CanAddItem_Implementation(const FInventoryItem& Item) const
{
	if (!Item.IsItemValid() || !Item.GetTemplate())
		return false;
	
	auto existingItem = Execute_FindItem(this, FInventoryItemSearchParams(Item.GetGuid()));
	if (!existingItem.IsItemValid()) 
		existingItem = Execute_FindItem(this, FInventoryItemSearchParams(Item.GetTemplate()));

	if (existingItem.IsItemValid())
		return existingItem.GetQuantity() < existingItem.GetTemplate()->MaxQuantity;

	return true;
}

bool UMounteaInventoryComponent::CanAddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* const Template, const int32 Quantity) const
{
	if (!IsValid(Template))
		return false;

	return Execute_CanAddItem(this, FInventoryItem(Template, Quantity, Template->BaseDurability, nullptr));
}

FInventoryItem UMounteaInventoryComponent::FindItem_Implementation(const FInventoryItemSearchParams& SearchParams) const
{
	const auto foundItem = InventoryItems.Items.FindByPredicate([&SearchParams](const FInventoryItem& Item)
	{
		if (SearchParams.bSearchByGuid && Item.GetGuid() != SearchParams.ItemGuid)
			return false;

		if (SearchParams.bSearchByTemplate && Item.GetTemplate() != SearchParams.Template)
			return false;

		if (SearchParams.bSearchByTags)
		{
			if (SearchParams.bRequireAllTags)
				return Item.GetCustomData().HasAll(SearchParams.Tags);
			else
				return Item.GetCustomData().HasAny(SearchParams.Tags);
		}

		return true;
	});

	return foundItem ? *foundItem : FInventoryItem();
}

int32 UMounteaInventoryComponent::FindItemIndex_Implementation(const FInventoryItemSearchParams& SearchParams) const
{
	return InventoryItems.Items.IndexOfByPredicate([&SearchParams](const FInventoryItem& Item)
	{
		if (SearchParams.bSearchByGuid && Item.GetGuid() != SearchParams.ItemGuid)
			return false;

		if (SearchParams.bSearchByTemplate && Item.GetTemplate() != SearchParams.Template)
			return false;

		if (SearchParams.bSearchByTags)
		{
			if (SearchParams.bRequireAllTags)
				return Item.GetCustomData().HasAll(SearchParams.Tags);
			else
				return Item.GetCustomData().HasAny(SearchParams.Tags);
		}

		return true;
	});
}

TArray<FInventoryItem> UMounteaInventoryComponent::FindItems_Implementation(const FInventoryItemSearchParams& SearchParams) const
{
	TArray<FInventoryItem> returnResult;
	// TODO: Implementation

	return returnResult;
}

TArray<FInventoryItem> UMounteaInventoryComponent::GetAllItems_Implementation() const
{
	return InventoryItems.Items;
}

bool UMounteaInventoryComponent::IncreaseItemQuantity_Implementation(const FGuid& ItemGuid, const int32 Amount)
{
	if (!IsAuthority())
		return false;
	
	const int32 index = Execute_FindItemIndex(this, FInventoryItemSearchParams(ItemGuid));
	if (index != INDEX_NONE && InventoryItems.Items.IsValidIndex(index))
	{
		auto& inventoryItem = InventoryItems.Items[index];
		
		const int32 OldQuantity = inventoryItem.GetQuantity();
		if (inventoryItem.SetQuantity(OldQuantity + Amount))
		{
			InventoryItems.MarkItemDirty(inventoryItem);
			OnItemQuantityChanged.Broadcast(inventoryItem, OldQuantity, inventoryItem.GetQuantity());

			// TODO: Don't spam client if same as server
			PostItemQuantityChanged(inventoryItem, OldQuantity, inventoryItem.GetQuantity());
			return true;
		}
	}
	return false;
}

bool UMounteaInventoryComponent::DecreaseItemQuantity_Implementation(const FGuid& ItemGuid, const int32 Amount)
{
	if (!IsAuthority())
		return false;
	
	const int32 index = Execute_FindItemIndex(this, FInventoryItemSearchParams(ItemGuid));
	if (index != INDEX_NONE && InventoryItems.Items.IsValidIndex(index))
	{
		auto& inventoryItem = InventoryItems.Items[index];
		
		const int32 OldQuantity =inventoryItem.GetQuantity();
		const int32 NewQuantity = OldQuantity - Amount;
				 
		if (NewQuantity <= 0)
			return Execute_RemoveItem(this, ItemGuid);
						 
		if (inventoryItem.SetQuantity(NewQuantity))
		{
			InventoryItems.MarkItemDirty(inventoryItem);
			OnItemQuantityChanged.Broadcast(inventoryItem, OldQuantity, NewQuantity);

			// TODO: Don't spam client if same as server
			PostItemQuantityChanged(inventoryItem, OldQuantity, NewQuantity);
			return true;
		}
	}
	return false;
}

bool UMounteaInventoryComponent::ModifyItemDurability_Implementation(const FGuid& ItemGuid, const float DeltaDurability)
{
	if (!IsAuthority())
		return false;
	
	const int32 index = Execute_FindItemIndex(this, FInventoryItemSearchParams(ItemGuid));
	if (index != INDEX_NONE && InventoryItems.Items.IsValidIndex(index))
	{
		auto& inventoryItem = InventoryItems.Items[index];
		const float OldDurability = inventoryItem.GetDurability();
		const float NewDurability = FMath::Clamp(OldDurability + DeltaDurability, 0.f, 1.f);
				 
		if (inventoryItem.SetDurability(NewDurability))
		{
			InventoryItems.MarkItemDirty(inventoryItem);
			OnItemDurabilityChanged.Broadcast(inventoryItem, OldDurability, NewDurability);

			// TODO: Don't spam client if same as server
			PostItemDurabilityChanged(inventoryItem, OldDurability, NewDurability);
			return true;
		}
	}
	return false;
}

void UMounteaInventoryComponent::ClearInventory_Implementation()
{
	// TODO: Server call
	if (!IsAuthority())
		return;
	
	for (const auto& Item : InventoryItems.Items)
	{
		  OnItemRemoved.Broadcast(Item);
	}
	InventoryItems.Items.Empty();
}

void UMounteaInventoryComponent::ProcessInventoryNotification_Implementation(const FInventoryNotificationData& Notification)
{
	LOG_WARNING(TEXT("%s"), *Notification.NotificationText.ToString());
	// TODO: Create new Notification Widget
}

bool UMounteaInventoryComponent::IsAuthority() const
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->GetWorld())
		return false;
    
	const ENetMode NetMode = Owner->GetWorld()->GetNetMode();
	
	if (NetMode == NM_Standalone)
		return true;
	
	if (Owner->HasAuthority())
		return true;
        
	return false;
}

void UMounteaInventoryComponent::AddItem_Server_Implementation(const FInventoryItem& Item)
{
	Execute_AddItem(this, Item);
}

void UMounteaInventoryComponent::RemoveItem_Server_Implementation(const FGuid& ItemGuid)
{
	Execute_RemoveItem(this, ItemGuid);
}

void UMounteaInventoryComponent::PostItemAdded_Client_Implementation(const FInventoryItem& Item)
{
	OnItemAdded.Broadcast(Item);
}

void UMounteaInventoryComponent::PostItemRemoved_Client_Implementation(const FInventoryItem& Item)
{
	OnItemRemoved.Broadcast(Item);
}

void UMounteaInventoryComponent::PostItemQuantityChanged_Implementation(const FInventoryItem& Item, const int32 OldQuantity, const int32 NewQuantity)
{
	OnItemQuantityChanged.Broadcast(Item, OldQuantity, NewQuantity);
}

void UMounteaInventoryComponent::PostItemDurabilityChanged_Implementation(const FInventoryItem& Item, const int32 OldDurability, const int32 NewDurability)
{
	OnItemDurabilityChanged.Broadcast(Item, OldDurability, NewDurability);
}