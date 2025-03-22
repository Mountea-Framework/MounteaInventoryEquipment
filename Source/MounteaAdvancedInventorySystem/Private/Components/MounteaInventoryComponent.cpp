// All rights reserved Dominik Morse 2024

#include "Components/MounteaInventoryComponent.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Net/UnrealNetwork.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaInventoryStatics.h"
#include "Statics/MounteaInventorySystemStatics.h"

UMounteaInventoryComponent::UMounteaInventoryComponent() : InventoryTypeFlag(EInventoryFlags::EIF_Private)
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
	DOREPLIFETIME_CONDITION(UMounteaInventoryComponent, InventoryItems, COND_InitialOrOwner);
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
		Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
			MounteaInventoryNotificationBaseTypes::ItemNotUpdated,
			this,
			Item.GetGuid(),
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

		if (Item.IsItemInInventory())
		{
			if (!Execute_DecreaseItemQuantity(Item.GetOwningInventory().GetObject(), Item.GetGuid(), AmountToAdd))
			{
				Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
					MounteaInventoryNotificationBaseTypes::ItemNotUpdated,
					this,
					Item.GetGuid(),
					0
				));
				return false;
			}
		}

		if (Execute_IncreaseItemQuantity(this, existingItem.GetGuid(), AmountToAdd))
		{
			InventoryItems.MarkArrayDirty();
			return true;
		}
	}
	else
	{
		const int32 AmountToAdd = FMath::Min(Item.GetQuantity(), Item.GetTemplate()->MaxQuantity);
		
		if (Item.IsItemInInventory())
		{
			if (!Execute_DecreaseItemQuantity(Item.GetOwningInventory().GetObject(), Item.GetGuid(), AmountToAdd))
			{
				Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
					MounteaInventoryNotificationBaseTypes::ItemNotUpdated,
					this,
					Item.GetGuid(),
					0
				));
				return false;
			}
		}

		FInventoryItem newItem = Item;
		if (AmountToAdd != Item.GetQuantity())
			newItem.Quantity += AmountToAdd;

		InventoryItems.Items.Add(newItem);
		InventoryItems.Items.Last().SetOwningInventory(this);
		InventoryItems.MarkArrayDirty();

		// TODO: Standalone, but do NOT affect LISTEN SERVER
		/*
		if (IsAuthority() && UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
		{
			const auto NotifType = AmountToAdd < Item.GetQuantity() ? 
				MounteaInventoryNotificationBaseTypes::ItemPartiallyAdded : 
				MounteaInventoryNotificationBaseTypes::ItemAdded;
				
			Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
				NotifType,
				this,
				newItem.GetGuid(),
				AmountToAdd
			));
		}
		*/
		
		OnItemAdded.Broadcast(newItem);
		PostItemAdded_Client(newItem);
		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::AddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity, const float Durability)
{
	if (!IsValid(Template))
	{
		//Invalid item template
		Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
			MounteaInventoryNotificationBaseTypes::ItemNotUpdated,
			this,
			FGuid(),
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
	
	OnItemRemoved.Broadcast(RemovedItem);
	
	if (!IsAuthority())
	{
		RemoveItem_Server(ItemGuid);
		return true;
	}
	
	PostItemRemoved_Client(RemovedItem);
	
	InventoryItems.Items.RemoveAt(ItemIndex);
	InventoryItems.MarkArrayDirty();

	return true;
}

bool UMounteaInventoryComponent::RemoveItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* const Template, const int32 Quantity)
{
	if (!IsValid(Template))
		return false;

	auto inventoryItem = Execute_FindItem(this, FInventoryItemSearchParams(Template));
	if (!inventoryItem.IsItemValid())
	{
		// Cannot find item matching template with sufficient quantity
		Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
				MounteaInventoryNotificationBaseTypes::ItemNotUpdated,
				this,
				FGuid(),
				0
			));
		return false;
	}
	
	if (inventoryItem.Quantity - Quantity <= 0)
	{
		return Execute_RemoveItem(this, inventoryItem.GetGuid());
	}
	else
	{
		if (!IsAuthority())
			ChangeItemQuantity_Server(inventoryItem.GetGuid(), -Quantity);
		else
			Execute_DecreaseItemQuantity(this, inventoryItem.GetGuid(), Quantity);
		return true;
	}	
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
	if (!IsAuthority() || (IsAuthority() && UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld())))
		OnNotificationProcessed.Broadcast(Notification);
	else if (IsAuthority() && !UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
		ProcessInventoryNotification_Client(Notification.ItemGuid, Notification.Type, Notification.DeltaAmount);
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

void UMounteaInventoryComponent::ChangeItemQuantity_Server_Implementation(const FGuid& ItemGuid,
	const int32 DeltaAmount)
{
	if (DeltaAmount < 0)
		Execute_DecreaseItemQuantity(this, ItemGuid, FMath::Abs(DeltaAmount));
	else if (DeltaAmount > 0)
		Execute_IncreaseItemQuantity(this, ItemGuid, FMath::Abs(DeltaAmount));
}

void UMounteaInventoryComponent::ProcessInventoryNotification_Client_Implementation(const FGuid& TargetItem, const FString& NotifType, const int32 QuantityDelta)
{
	// Wait for the next tick before executing the broadcast to avoid timing issues with replication (item might not exist yet)
	GetWorld()->GetTimerManager().SetTimerForNextTick([this, NotifType, TargetItem, QuantityDelta]()
	{
		auto targetItem = Execute_FindItem(this, FInventoryItemSearchParams(TargetItem));
		
		OnNotificationProcessed.Broadcast(UMounteaInventoryStatics::CreateNotificationData(
			NotifType,
			this,
			targetItem.GetGuid(),
			QuantityDelta
		));
	});
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
	if (IsAuthority() && UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
	{
		Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
				MounteaInventoryNotificationBaseTypes::ItemAdded,
				this,
				Item.GetGuid(),
				Item.Quantity
			));
	}
	
	OnItemAdded.Broadcast(Item);
}

void UMounteaInventoryComponent::PostItemRemoved_Client_Implementation(const FInventoryItem& Item)
{
	if (IsAuthority() && UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
	{
		Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
				MounteaInventoryNotificationBaseTypes::ItemRemoved,
				this,
				Item.GetGuid(),
				Item.GetQuantity()
			));
	}
	
	OnItemRemoved.Broadcast(Item);
}

void UMounteaInventoryComponent::PostItemQuantityChanged_Implementation(const FInventoryItem& Item, const int32 OldQuantity, const int32 NewQuantity)
{
	if (IsAuthority() && UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(GetWorld()))
	{
		Execute_ProcessInventoryNotification(this, UMounteaInventoryStatics::CreateNotificationData(
				OldQuantity > NewQuantity ? MounteaInventoryNotificationBaseTypes::ItemRemoved : MounteaInventoryNotificationBaseTypes::ItemAdded,
				this,
				Item.GetGuid(),
				OldQuantity > NewQuantity ? FMath::Abs(OldQuantity - NewQuantity) : -FMath::Abs(OldQuantity - NewQuantity)
			));
	}
	
	OnItemQuantityChanged.Broadcast(Item, OldQuantity, NewQuantity);
}

void UMounteaInventoryComponent::PostItemDurabilityChanged_Implementation(const FInventoryItem& Item, const int32 OldDurability, const int32 NewDurability)
{
	OnItemDurabilityChanged.Broadcast(Item, OldDurability, NewDurability);
}