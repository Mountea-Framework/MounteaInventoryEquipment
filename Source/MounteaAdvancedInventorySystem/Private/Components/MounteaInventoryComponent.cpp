// All rights reserved Dominik Morse 2024

#include "Components/MounteaInventoryComponent.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Net/UnrealNetwork.h"

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
}

AActor* UMounteaInventoryComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

bool UMounteaInventoryComponent::AddItem_Implementation(const FInventoryItem& Item, const bool bAutoStack)
{
	if (!CanAddItem(Item))
	{
		  Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
				 EInventoryNotificationType::EINT_ItemNotUpdated,
				 EInventoryNotificationCategory::EINC_Error,
				 NSLOCTEXT("Inventory", "ItemNotAdded", "Cannot add item to inventory"),
				 Item.GetGuid(),
				 this,
				 Item.GetQuantity(),
				 0
		  ));
		  return false;
	}

	const int32 RequestedAmount = Item.GetQuantity();
	int32 AmountToAdd = RequestedAmount;

	if(bAutoStack)
	{
		  // Stack logic
		  // AmountToAdd = CalculateActualAddAmount(Item, RequestedAmount);
	}

	if(AmountToAdd == 0)
	{
		  Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
				 EInventoryNotificationType::EINT_ItemNotUpdated,
				 EInventoryNotificationCategory::EINC_Error,
				 NSLOCTEXT("Inventory", "ItemNotAdded", "Cannot add item to inventory"),
				 Item.GetGuid(),
				 this,
				 RequestedAmount,
				 0
		  ));
		  return false;
	}

	if(AmountToAdd < RequestedAmount)
	{
		  Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
				 EInventoryNotificationType::EINT_ItemPartiallyAdded,
				 EInventoryNotificationCategory::EINC_Warning,
				 NSLOCTEXT("Inventory", "PartialAdd", "Could only add {0} of {1} items"),
				 Item.GetGuid(),
				 this,
				 RequestedAmount,
				 AmountToAdd
		  ));
	}
	else
	{
		  Execute_ProcessInventoryNotification(this, FInventoryNotificationData(
				 EInventoryNotificationType::EINT_ItemAdded,
				 EInventoryNotificationCategory::EINC_Info,
				 NSLOCTEXT("Inventory", "ItemAdded", "Successfully added {0} items"),
				 Item.GetGuid(),
				 this,
				 RequestedAmount,
				 AmountToAdd
		  ));
	}

	// Add item logic
	return true;
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

	return Execute_AddItem(this, FInventoryItem(Template, Quantity, Durability, this), true);
}

bool UMounteaInventoryComponent::RemoveItem_Implementation(const FGuid& ItemGuid)
{
	const auto ItemToRemove = Execute_FindItem(this, FInventoryItemSearchParams(ItemGuid));
	if (!ItemToRemove.IsItemValid())
		  return false;

	InventoryItems.Items.RemoveAll([ItemGuid](const FInventoryItem& Item)
	{
		  return Item.GetGuid() == ItemGuid;
	});

	OnItemRemoved.Broadcast(ItemToRemove);
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
	return Item.IsItemValid() && Item.GetTemplate() != nullptr;
}

bool UMounteaInventoryComponent::CanAddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* const Template, const int32 Quantity) const
{
	if (!IsValid(Template))
		return false;

	return Execute_CanAddItem(this, FInventoryItem(Template, Quantity, Template->BaseDurability, nullptr));
}

FInventoryItem UMounteaInventoryComponent::FindItem_Implementation(const FInventoryItemSearchParams& SearchParams)
{
	const auto FoundItem = InventoryItems.Items.FindByPredicate([&SearchParams](const FInventoryItem& Item)
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

	return FoundItem ? *FoundItem : FInventoryItem();
}

TArray<FInventoryItem> UMounteaInventoryComponent::FindItems_Implementation(const FInventoryItemSearchParams& SearchParams)
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
	auto inventoryItem = Execute_FindItem(this, FInventoryItemSearchParams(ItemGuid));
	if (inventoryItem.IsItemValid())
	{
		const int32 OldQuantity = inventoryItem.GetQuantity();
		if (inventoryItem.SetQuantity(OldQuantity + Amount))
		{
			OnItemQuantityChanged.Broadcast(inventoryItem, OldQuantity, inventoryItem.GetQuantity());
			return true;
		}
	}
	return false;
}

bool UMounteaInventoryComponent::DecreaseItemQuantity_Implementation(const FGuid& ItemGuid, const int32 Amount)
{
	auto inventoryItem = Execute_FindItem(this, FInventoryItemSearchParams(ItemGuid));
	if (inventoryItem.IsItemValid())
	{
		const int32 OldQuantity = inventoryItem.GetQuantity();
		const int32 NewQuantity = OldQuantity - Amount;
				 
		if (NewQuantity <= 0)
			return Execute_RemoveItem(this, ItemGuid);
						 
		if (inventoryItem.SetQuantity(NewQuantity))
		{
			OnItemQuantityChanged.Broadcast(inventoryItem, OldQuantity, NewQuantity);
			return true;
		}
	}
	return false;
}

bool UMounteaInventoryComponent::ModifyItemDurability_Implementation(const FGuid& ItemGuid, const float DeltaDurability)
{
	auto inventoryItem = Execute_FindItem(this, FInventoryItemSearchParams(ItemGuid));
	if (inventoryItem.IsItemValid())
	{
		const float OldDurability = inventoryItem.GetDurability();
		const float NewDurability = FMath::Clamp(OldDurability + DeltaDurability, 0.f, 1.f);
				 
		if (inventoryItem.SetDurability(NewDurability))
		{
			OnItemDurabilityChanged.Broadcast(inventoryItem, OldDurability, NewDurability);
			return true;
		}
	}
	return false;
}

void UMounteaInventoryComponent::ClearInventory_Implementation()
{
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