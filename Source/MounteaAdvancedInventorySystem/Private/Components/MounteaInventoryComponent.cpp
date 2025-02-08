// All rights reserved Dominik Morse 2024


#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaAdvancedInventoryNotification.h"
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

bool UMounteaInventoryComponent::AddItem_Implementation(const FInventoryItem& Item, bool bAutoStack)
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

bool UMounteaInventoryComponent::RemoveItem_Implementation(const FGuid& ItemGuid)
{
	const auto ItemToRemove = FindItem(ItemGuid);
	if (!ItemToRemove.IsItemValid())
		 return false;

	InventoryItems.Items.RemoveAll([ItemGuid](const FInventoryItem& Item)
	{
		 return Item.GetGuid() == ItemGuid;
	});

	OnItemRemoved.Broadcast(ItemToRemove);
	return true;
}

bool UMounteaInventoryComponent::CanAddItem_Implementation(const FInventoryItem& Item) const
{
	return Item.IsItemValid() && Item.GetTemplate() != nullptr;
}

FInventoryItem UMounteaInventoryComponent::FindItem_Implementation(const FGuid& ItemGuid)
{
	const auto FoundItem = InventoryItems.Items.FindByPredicate([ItemGuid](const FInventoryItem& Item)
	{
		 return Item.GetGuid() == ItemGuid;
	});

	return FoundItem ? *FoundItem : FInventoryItem();
}

TArray<FInventoryItem> UMounteaInventoryComponent::GetAllItems_Implementation() const
{
	return InventoryItems.Items;
}

bool UMounteaInventoryComponent::IncreaseItemQuantity_Implementation(const FGuid& ItemGuid, int32 Amount)
{
	for (auto& Item : InventoryItems.Items)
	{
		 if (Item.GetGuid() == ItemGuid)
		 {
			  const int32 OldQuantity = Item.GetQuantity();
			  if (Item.SetQuantity(OldQuantity + Amount))
			  {
					OnItemQuantityChanged.Broadcast(Item, OldQuantity, Item.GetQuantity());
					return true;
			  }
		 }
	}
	return false;
}

bool UMounteaInventoryComponent::DecreaseItemQuantity_Implementation(const FGuid& ItemGuid, int32 Amount)
{
	for (auto& Item : InventoryItems.Items)
	{
		 if (Item.GetGuid() == ItemGuid)
		 {
			  const int32 OldQuantity = Item.GetQuantity();
			  const int32 NewQuantity = OldQuantity - Amount;
			  
			  if (NewQuantity <= 0)
					return RemoveItem(ItemGuid);
					
			  if (Item.SetQuantity(NewQuantity))
			  {
					OnItemQuantityChanged.Broadcast(Item, OldQuantity, NewQuantity);
					return true;
			  }
		 }
	}
	return false;
}

FInventoryItem UMounteaInventoryComponent::SplitItemStack_Implementation(const FGuid& ItemGuid, int32 SplitAmount)
{
	auto ExistingItem = FindItem(ItemGuid);
	if (!ExistingItem.IsItemValid() || SplitAmount <= 0 || ExistingItem.GetQuantity() <= SplitAmount)
		 return FInventoryItem();

	FInventoryItem NewItem(ExistingItem.GetTemplate(), SplitAmount);
	if (DecreaseItemQuantity(ItemGuid, SplitAmount))
	{
		 AddItem(NewItem, false);
		 return NewItem;
	}
	
	return FInventoryItem();
}

bool UMounteaInventoryComponent::ModifyItemDurability_Implementation(const FGuid& ItemGuid, float DeltaDurability)
{
	for (auto& Item : InventoryItems.Items)
	{
		 if (Item.GetGuid() == ItemGuid)
		 {
			  const float OldDurability = Item.GetDurability();
			  const float NewDurability = FMath::Clamp(OldDurability + DeltaDurability, 0.f, 1.f);
			  
			  if (Item.SetDurability(NewDurability))
			  {
					OnItemDurabilityChanged.Broadcast(Item, OldDurability, NewDurability);
					return true;
			  }
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
