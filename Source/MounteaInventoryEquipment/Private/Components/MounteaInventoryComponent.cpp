// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Components/MounteaInventoryComponent.h"

#include "Async/Async.h"
#include "Definitions/MounteaInventoryInstancedItem.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Engine/ActorChannel.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Helpers/MounteaInventoryItemBFL.h"
#include "Interfaces/UI/MounteaInventoryWBPInterface.h"

#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "Settings/Config/MounteaDefaultsConfig.h"
#include "WBP/MounteaBaseUserWidget.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryComponent"

UMounteaInventoryComponent::UMounteaInventoryComponent()
{	
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;

	ReplicatedItemsKey = 0;
}

void UMounteaInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OnInventoryUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostInventoryUpdated);
	
	OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemAdded);
	OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemRemoved);
	OnItemUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemUpdated);
}

void UMounteaInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//FDoRepLifetimeParams Params;
	//Params.bIsPushBased = true;
	//Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	//DOREPLIFETIME_WITH_PARAMS(UMounteaInventoryComponent, Items, Params)
	
	DOREPLIFETIME(UMounteaInventoryComponent, InventorySlots);
	DOREPLIFETIME(UMounteaInventoryComponent, InventoryFlags);
}

bool UMounteaInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bUpdated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	//Check if the array of items needs to replicate
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		// Setup all Items, even from Slots that should be empty at this point
		TSet<UMounteaInstancedItem*> AllItems;
		for (auto const& Itr : ModifiedSlots)
		{
			if (Itr.Item)
			{
				AllItems.Add(Itr.Item);
			}
		}
		
		for (const auto& Item : AllItems)
		{
			if (Item && Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->GetRepKey()))
			{
				bUpdated |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}

		// Cleanup all Nullified Slots
		ModifiedSlots.Empty();
	}

	return bUpdated;
}

TSubclassOf<UMounteaBaseUserWidget> UMounteaInventoryComponent::GetInventoryUIClass_Implementation() const
{
	if (InventoryWBPClass)
	{
		return InventoryWBPClass;
	}
	
	const UMounteaInventoryEquipmentSettings* Settings = UMounteaInventoryEquipmentBPF::GetSettings();

	if (!Settings) return nullptr;

	const UMounteaDefaultsConfig* DefaultsConfig = Settings->DefaultsConfig.LoadSynchronous();

	if (!DefaultsConfig) return nullptr;

	return DefaultsConfig->DefaultInventoryClass.LoadSynchronous();
}

UMounteaBaseUserWidget* UMounteaInventoryComponent::GetInventoryUI_Implementation() const
{
	return InventoryWBP;
}

void UMounteaInventoryComponent::PostInitProperties()
{
	Super::PostInitProperties();

	bool bIsEditorNoPlay = false;
#if WITH_EDITOR
	bIsEditorNoPlay = UMounteaInventoryEquipmentBPF::IsEditorNoPlay();
#endif
	
	if (bIsEditorNoPlay) // This code gets executed only when opening new Asset in Editor
	{
		if (InventoryConfig.MounteaInventoryConfig == nullptr)
		{
			bool bFound = false;
			const TSubclassOf<UMounteaInventoryConfig> Class = UMounteaInventoryEquipmentBPF::GetSettings()->DefaultInventoryConfigClass.LoadSynchronous();
			InventoryConfig.MounteaInventoryConfig = UMounteaInventoryEquipmentBPF::GetInventoryConfig(this, Class, bFound);
		}
	}
}

FInventoryUpdateResult UMounteaInventoryComponent::AddItemToInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity)
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_AddItemToInventory );
	
	// Create an empty result struct.
    FInventoryUpdateResult Result;
	
	// Validate the request
	if (!Item)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item.");
		
		return Result;
	}

	// Check if there is Authority
	if (!Execute_DoesHaveAuthority(this))
	{
		AddItemToInventory_Server(Item, Quantity);
		
		Result.OptionalPayload = Item;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing; // Corresponds to "Processing"
		Result.ResultText = LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request.");
		
		return Result;
	}
	
    // Check if Quantity is larger than 0
    if (Quantity <= 0)
    {
        Result.OptionalPayload = Item;
        Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Corresponds to "Bad Request"
        Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidQuantity", "Quantity less than 1 is forbidden.");
    	
        return Result;
    }

    // Check if the item can be added at all
    if (!Execute_CanAddItem(this, FItemTransfer(Item, Quantity)))
    {
        Result.OptionalPayload = Item;
        Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
        Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");
    	
        return Result;
    }

    // Check if a similar item already exists in the inventory
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByGUID = true;
    	Filter.Guid = Item->GetGuid();
    	Filter.bSearchByItem = true;
    	Filter.Item = Item;
	}
	
    UMounteaInstancedItem* ExistingItem = Execute_SearchSingleItem(this, Filter);

    // Calculate the max addable amount
    int32 MaxAddAmount = UMounteaInventoryItemBFL::CalculateAddAmount(Item, Quantity);

    // If the similar item already exists in the inventory
    if (ExistingItem != nullptr)
    {
        // If entire quantity of Item can be added to existing item
        if (Quantity <= MaxAddAmount)
        {
            ExistingItem->SetQuantity(Quantity); // Increment the quantity of the existing item
            Item->SetQuantity(0); // Set the source item's quantity to 0, effectively marking it for destruction

        	// Check if we have this Item in Inventory, and if so then remove it
			if (Execute_CanRemoveItem(this, Item))
			{
				RemoveItemFromInventory(Item);
			}
        	
            Result.OptionalPayload = ExistingItem;
            Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK; // Corresponds to "OK"
            Result.ResultText =  LOCTEXT("InventoryUpdateResult_ItemUpdated", "Added all of the item to the existing stack.");
        	
        	if (FItemSlot* Slot = InventorySlots.FindByKey(ExistingItem))
        	{
        		UMounteaInventoryItemBFL::UpdateStacksInSlot(*Slot, Quantity);

        		ModifiedSlots.AddUnique(*Slot);
        	}
	        else
	        {
	        	// Find an empty slot or create a new one if none exist
	        	const int32 EmptyIndex = UMounteaInventoryItemBFL::FindEmptySlot(InventorySlots, ExistingItem);
	        	if (EmptyIndex != INDEX_NONE)
	        	{
	        		FItemSlot& EmptySlot = InventorySlots[EmptyIndex];
	        		
	        		UMounteaInventoryItemBFL::UpdateStacksInSlot(EmptySlot, Quantity);

	        		ModifiedSlots.AddUnique(EmptySlot);
	        	}
		        else
		        {
		        	FItemSlot NewSlot = UMounteaInventoryItemBFL::MakeNewSlot(ExistingItem, Quantity);
		        	
		        	UMounteaInventoryItemBFL::UpdateStacksInSlot(NewSlot, Quantity);
		        	
		        	InventorySlots.AddUnique(NewSlot);
		        	ModifiedSlots.AddUnique(NewSlot);
		        }
	        }
        }
        else
        {
            // If only part of the Item can be added
            ExistingItem->SetQuantity(MaxAddAmount); // Add the maximum possible quantity to the existing item
            Item->ModifyQuantity(-MaxAddAmount); // Decrease the source item's quantity

        	// Check if we have this Item in Inventory, and if so then remove it
        	if (Item->GetQuantity() <= 0 && Execute_CanRemoveItem(this, Item))
        	{
        		RemoveItemFromInventory(Item);
        	}
        	
            Result.OptionalPayload = ExistingItem;
            Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_CreatedPart; // Analogous to "Partial Content"
            Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemUpdatedPartly", "Added part of the item to the existing stack. Some quantity remains in the source item.");

        	if (FItemSlot* Slot = InventorySlots.FindByKey(ExistingItem))
        	{
        		UMounteaInventoryItemBFL::UpdateStacksInSlot(*Slot, MaxAddAmount);

        		ModifiedSlots.AddUnique(*Slot);
        	}
        	else
        	{
        		// Find an empty slot or create a new one if none exist
        		const int32 EmptyIndex = UMounteaInventoryItemBFL::FindEmptySlot(InventorySlots, ExistingItem);
        		if (EmptyIndex != INDEX_NONE)
        		{
        			FItemSlot& EmptySlot = InventorySlots[EmptyIndex];
	        		
        			UMounteaInventoryItemBFL::UpdateStacksInSlot(EmptySlot, MaxAddAmount);

        			ModifiedSlots.AddUnique(EmptySlot);
        		}
        		else
        		{
        			FItemSlot NewSlot = UMounteaInventoryItemBFL::MakeNewSlot(ExistingItem, MaxAddAmount);
		        	
        			UMounteaInventoryItemBFL::UpdateStacksInSlot(NewSlot, MaxAddAmount);
		        	
        			InventorySlots.AddUnique(NewSlot);
        			ModifiedSlots.AddUnique(NewSlot);
        		}
        	}
        }

    	OnItemUpdated.Broadcast(Result);
    }
    else
    {
        // If the item doesn't exist in the inventory
        if (Quantity <= MaxAddAmount)
        {
        	// Directly add the entire source item to the inventory
        	FItemSlot* NewSlot = nullptr;

        	// Find an empty slot or create a new one if none exist
        	const int32 EmptyIndex = UMounteaInventoryItemBFL::FindEmptySlot(InventorySlots, Item);
        	if (EmptyIndex != INDEX_NONE)
        	{
        		NewSlot = &InventorySlots[EmptyIndex];        		
        	}
	        else
	        {
	        	InventorySlots.AddUnique(UMounteaInventoryItemBFL::MakeNewSlot(Item, Quantity));
	        	
	        	// Use the new slot
	        	NewSlot = &InventorySlots.Last();
	        }
        	
        	// Assuming NewSlot is now pointing to the correct slot, either found or newly created
        	UMounteaInventoryItemBFL::UpdateStacksInSlot(*NewSlot, Quantity);        	
        	
            Result.OptionalPayload = Item;
            Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Created; // Corresponds to "Created"
            Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemCreated", "Added the entire item to the inventory.");
        	
        	Item->SetWorld(GetWorld());
        	Item->SetOwningInventory(this);

        	ModifiedSlots.AddUnique(*NewSlot);
        	
        	OnItemAdded.Broadcast(Result);
        }
        else
        {
            // If only part of the Item can be added
            UMounteaInstancedItem* NewItemInstance = NewObject<UMounteaInstancedItem>(GetWorld());
        	
        	FItemInitParams ItemInitParams;
	        {
		        ItemInitParams.Quantity = MaxAddAmount;
            	ItemInitParams.OwningInventory = this;
            	switch (Item->GetItemDataSource())
            	{
            		case EItemDataSource::EIDS_SourceTable:
            			ItemInitParams.SourceTable = Item->SourceTable;
            			ItemInitParams.SourceRow = Item->SourceRow;
            			break;
            		case EItemDataSource::EIDS_SourceItem:
            			ItemInitParams.SourceItem = Item->SourceItem;
            			break;
            		case EItemDataSource::Default:
            		default:
						break;
            	}
	        }
        	
			// Try make a new item
        	if (NewItemInstance->Execute_InitializeNewItem(NewItemInstance, ItemInitParams))
        	{
        		FItemSlot* NewSlot;

        		// Find an empty slot or create a new one if none exist
        		int32 EmptyIndex = UMounteaInventoryItemBFL::FindEmptySlot(InventorySlots, NewItemInstance);
        		if (EmptyIndex != INDEX_NONE)
        		{
        			NewSlot = &InventorySlots[EmptyIndex];
        		}
        		else
        		{
        			InventorySlots.AddUnique(UMounteaInventoryItemBFL::MakeNewSlot(NewItemInstance, MaxAddAmount));
        			NewSlot = &InventorySlots.Last();
        		}

        		// Update the stacks in the slot with the new item instance
        		UMounteaInventoryItemBFL::UpdateStacksInSlot(*NewSlot, MaxAddAmount);
        
        		// Adjust the quantity of the original item
        		Item->ModifyQuantity(-MaxAddAmount);

        		Result.OptionalPayload = NewItemInstance;
        		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_CreatedPart; // Analogous to "Partial Content"
        		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemUpdatedPartly", "Added part of the item to the existing stack. Some quantity remains in the source item.");
        	}
	        else
	        {
	        	Result.OptionalPayload = nullptr;
	        	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue; // Analogous to "Unknown issue"
	        	Result.ResultText =  LOCTEXT("InventoryUpdateResult_ItemUnknownIssue", "Unhandled Exception.");

	        	return Result;
	        }

        	OnItemAdded.Broadcast(Result);
        }
    }

	// Remove OptionalPayload so UI does not need to refresh - UI is refreshing only if OptionalPayload is not null
	Result.OptionalPayload = this;
	OnInventoryUpdated.Broadcast(Result);
	
    return Result;
}

TArray<FInventoryUpdateResult> UMounteaInventoryComponent::AddItemsToInventory_Implementation(const TMap<UMounteaInstancedItem*, int32>& Items, int32 TransactionTypeFlags)
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_AddItemsToInventory );

	TArray<FInventoryUpdateResult> Results;

	// Bitmask Corrections
	{
		// Define the maximum valid value for TransactionTypeFlags
		const uint8 MaxTransactionTypeValue =
			static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction) |
			static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction) |
			static_cast<uint8>(EInventoryTransactionType::EITT_Sync) |
			static_cast<uint8>(EInventoryTransactionType::EITT_Async);
	
		// Check for empty value
		if (TransactionTypeFlags == static_cast<uint8>(EInventoryTransactionType::None))
		{
			TransactionTypeFlags = static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction) | static_cast<uint8>(EInventoryTransactionType::EITT_Sync);
		}

		// Check for both FullTransaction and PartialTransaction being selected
		if ((TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction)) != 0 && (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction)) != 0)
		{
			TransactionTypeFlags &= ~static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction);
		}

		// Check for both Sync and Async being selected
		if ((TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Sync)) != 0 && (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Async)) != 0)
		{
			TransactionTypeFlags &= ~static_cast<uint8>(EInventoryTransactionType::EITT_Async);
		}
	}
	
	// Extracting the boolean values from the bitmask
	const bool bFullOnly = (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction)) != 0;
	const bool bIsSync = (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Sync)) != 0;
	
	if (bIsSync)
	{
		if (bFullOnly)
		{
			// First validate
			for (auto const& Itr : Items)
			{
				// Create an empty result struct.
				FInventoryUpdateResult Result;

				if (Itr.Key== nullptr)
				{
					Result.OptionalPayload = Itr.Key;
					Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
					Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");
			
					return  Results;
				}
		
				// Check if the item can be added at all
				if (!Execute_CanAddItem(this, FItemTransfer(Itr.Key, Itr.Value)))
				{
					Result.OptionalPayload = Itr.Key;
					Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
					Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

					return  Results;
				}

				// Check if a similar item already exists in the inventory
				FItemRetrievalFilter Filter;
				{
					Filter.bSearchByGUID = true;
					Filter.Guid = Itr.Key->GetGuid();
					Filter.bSearchByItem = true;
					Filter.Item = Itr.Key;
				}

				if (const UMounteaInstancedItem* ExistingItem = Execute_SearchSingleItem(this, Filter))
				{
					// Calculate the max addable amount
					const int32 MaxAddAmount = UMounteaInventoryItemBFL::CalculateAddAmount(ExistingItem, Itr.Value);
					if (MaxAddAmount <= 0)
					{
						Result.OptionalPayload = Itr.Key;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

						return  Results;
					}
				}
			}
		}
		
		// Add items
		for (auto const& Itr : Items)
		{
			const FInventoryUpdateResult IndividualResult = Execute_AddItemToInventory(this, Itr.Key, Itr.Value);

			Results.Add(IndividualResult);
		}
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this, Items, &Results, bFullOnly]() mutable
		{
			bool bSatisfied = true;
			if (bFullOnly)
			{
				// First validate
				for (auto const& Itr : Items)
				{
					// Create an empty result struct.
					FInventoryUpdateResult Result;

					if (Itr.Key== nullptr)
					{
						Result.OptionalPayload = Itr.Key;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

						Results.Add(Result);
						bSatisfied = false;
						break;
					}
		
					// Check if the item can be added at all
					if (!Execute_CanAddItem(this, FItemTransfer(Itr.Key, Itr.Value)))
					{
						Result.OptionalPayload = Itr.Key;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

						Results.Add(Result);
						bSatisfied = false;
						break;
					}

					// Check if a similar item already exists in the inventory
					FItemRetrievalFilter Filter;
					{
						Filter.bSearchByGUID = true;
						Filter.Guid = Itr.Key->GetGuid();
						Filter.bSearchByItem = true;
						Filter.Item = Itr.Key;
					}

					if (const UMounteaInstancedItem* ExistingItem = Execute_SearchSingleItem(this, Filter))
					{
						// Calculate the max addable amount
						const int32 MaxAddAmount = UMounteaInventoryItemBFL::CalculateAddAmount(ExistingItem, Itr.Value);
						if (MaxAddAmount <= 0)
						{
							Result.OptionalPayload = Itr.Key;
							Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
							Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

							Results.Add(Result);
							bSatisfied = false;
							break;
						}
					}
				}
			}

			if (bSatisfied)
			{
				for (auto const& Itr : Items)
				{
					const FInventoryUpdateResult IndividualResult = Execute_AddItemToInventory(this, Itr.Key, Itr.Value);

					Results.Add(IndividualResult);
				}
			}
		});
	}
	
	return Results;
}

FInventoryUpdateResult UMounteaInventoryComponent::RemoveItemFromInventory_Implementation(UMounteaInstancedItem* Item)
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_RemoveItemFromInventory );

	FInventoryUpdateResult Result;

	// Validate the request
	if (!Item)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item.");
		
		return Result;
	}

	// Check if there is Authority
	if (!Execute_DoesHaveAuthority(this))
	{
		RemoveItemFromInventory_Server(Item);
		
		Result.OptionalPayload = Item;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing; // Corresponds to "Processing"
		Result.ResultText = LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request.");
		
		return Result;
	}
	
	// Search for the item in the inventory.
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByItem = true;
		Filter.Item = Item;
	}

	// Validate whether the item exists
	if (!Execute_HasItem(this, Filter))
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound; // Not Found
		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemNotFound", "The item was not found in the inventory.");

		return Result;
	}
	
	// If the item exists in the inventory, remove it. Here we don't care about Quantities, we just straight up remove the item from inventory
	if (const FItemSlot* ExistingItemSlot = InventorySlots.FindByKey(Item))
	{
		// Keep track of this slot being nullified to it gets updated once before it's removed - useful for UI
		ModifiedSlots.AddUnique(*ExistingItemSlot);
		
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK; // Corresponds to "OK"
		Result.OptionalPayload = ExistingItemSlot->Item;
		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemRemoved", "The item has been successfully removed from the inventory.");

		// Break the connection between the item and the inventory
		ExistingItemSlot->Item->SetOwningInventory(nullptr);
		
		// Cleanup the slot in InventorySlots so it can be used again
		const int32 ExistingSlotIndex = InventorySlots.IndexOfByKey(ExistingItemSlot);
		InventorySlots[ExistingSlotIndex] = FItemSlot();
		
		// Broadcast that an item has been removed. This should be done before broadcasting the general inventory update.
		OnItemRemoved.Broadcast(Result);
	}
	else
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound; // Not Found
		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemNotFound", "The item was not found in the inventory.");

		return Result;
	}
	
	// Remove OptionalPayload so UI does not need to refresh - UI is refreshing only if OptionalPayload is not null
	Result.OptionalPayload = this;
	OnInventoryUpdated.Broadcast(Result);
	
	return Result;
}

TArray<FInventoryUpdateResult> UMounteaInventoryComponent::RemoveItemsFromInventory_Implementation(const TArray<UMounteaInstancedItem*>& Items, int32 TransactionTypeFlags)
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_RemoveItemsFromInventory );

	TArray<FInventoryUpdateResult> Results;

	// Bitmask Corrections
	{
		// Define the maximum valid value for TransactionTypeFlags
		const uint8 MaxTransactionTypeValue =
			static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction) |
			static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction) |
			static_cast<uint8>(EInventoryTransactionType::EITT_Sync) |
			static_cast<uint8>(EInventoryTransactionType::EITT_Async);
	
		// Check for empty value
		if (TransactionTypeFlags == static_cast<uint8>(EInventoryTransactionType::None))
		{
			TransactionTypeFlags = static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction) | static_cast<uint8>(EInventoryTransactionType::EITT_Sync);
		}

		// Check for both FullTransaction and PartialTransaction being selected
		if ((TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction)) != 0 && (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction)) != 0)
		{
			TransactionTypeFlags &= ~static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction);
		}

		// Check for both Sync and Async being selected
		if ((TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Sync)) != 0 && (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Async)) != 0)
		{
			TransactionTypeFlags &= ~static_cast<uint8>(EInventoryTransactionType::EITT_Async);
		}
	}

	// Extracting the boolean values from the bitmask
	const bool bFullOnly = (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction)) != 0;
	const bool bIsSync = (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Sync)) != 0;

	if (bIsSync)
	{
		if (bFullOnly)
		{
			// First validate
			for (auto const& Itr : Items)
			{
				// Create an empty result struct.
				FInventoryUpdateResult Result;

				if (Itr== nullptr)
				{
					Result.OptionalPayload = Itr;
					Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
					Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");
			
					return  Results;
				}
		
				// Check if the item can be added at all
				if (!Execute_CanRemoveItem(this, Itr))
				{
					Result.OptionalPayload = Itr;
					Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
					Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

					return  Results;
				}

				// Check if a similar item already exists in the inventory
				FItemRetrievalFilter Filter;
				{
					Filter.bSearchByGUID = true;
					Filter.Guid = Itr->GetGuid();
					Filter.bSearchByItem = true;
					Filter.Item = Itr;
				}

				const UMounteaInstancedItem* ExistingItem = Execute_SearchSingleItem(this, Filter);
				if (ExistingItem == nullptr)
				{
					Result.OptionalPayload = Itr;
					Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound; // Corresponds to "Not Found"
					Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot find specific Item in inventory.");

					return  Results;
				}
			}
		}
		
		// Add items
		for (auto const& Itr : Items)
		{
			const FInventoryUpdateResult IndividualResult = Execute_RemoveItemFromInventory(this, Itr);

			Results.Add(IndividualResult);
		}
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this, Items, &Results, bFullOnly]() mutable
		{
			bool bSatisfied = true;
			if (bFullOnly)
			{
				// First validate
				for (auto const& Itr : Items)
				{
					// Create an empty result struct.
					FInventoryUpdateResult Result;

					if (Itr== nullptr)
					{
						Result.OptionalPayload = Itr;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

						Results.Add(Result);
						bSatisfied = false;
						break;
					}
		
					// Check if the item can be added at all
					if (!Execute_CanRemoveItem(this, Itr))
					{
						Result.OptionalPayload = Itr;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

						Results.Add(Result);
						bSatisfied = false;
						break;
					}

					// Check if a similar item already exists in the inventory
					FItemRetrievalFilter Filter;
					{
						Filter.bSearchByGUID = true;
						Filter.Guid = Itr->GetGuid();
						Filter.bSearchByItem = true;
						Filter.Item = Itr;
					}

					const UMounteaInstancedItem* ExistingItem = Execute_SearchSingleItem(this, Filter);
					if (ExistingItem == nullptr)
					{
						Result.OptionalPayload = Itr;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound; // Corresponds to "Not Found"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot find specific Item in inventory.");

						Results.Add(Result);
						bSatisfied = false;
						break;
					}
				}
			}

			if (bSatisfied)
			{
				for (auto const& Itr : Items)
				{
					const FInventoryUpdateResult IndividualResult = Execute_RemoveItemFromInventory(this, Itr);

					Results.Add(IndividualResult);
				}
			}
		});
	}
	
	return Results;
}

FInventoryUpdateResult UMounteaInventoryComponent::ReduceItemInInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UMounteaInventoryComponent_ReduceItemInInventory);

    FInventoryUpdateResult Result;

    // Validate the request
    if (!Item)
    {
    	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
        Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item or quantity.");
    	
        return Result;
    }
	
	// Check if there is Authority
	if (!Execute_DoesHaveAuthority(this))
	{
		ReduceItemInInventory_Server(Item, Quantity);
		
		Result.OptionalPayload = Item;
		Result.ResultID = Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing;; // Corresponds to "Processing"
		Result.ResultText = LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request.");
		
		return Result;
	}

    // Search for the item in the inventory
    FItemSlot* FoundSlot = InventorySlots.FindByKey(Item);
	
    if (!FoundSlot)
    {
        Result.OptionalPayload = nullptr;
    	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound; // Not Found
        Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemNotFound", "Item not found in the inventory.");
    	
        return Result;
    }

    // Check if the item has enough quantity
    if (FoundSlot->Item->GetQuantity() >= Quantity)
    {
        // Reduce the item's quantity
        FoundSlot->Item->ModifyQuantity(-Quantity);

        // Remove the item slot if the quantity is zero or below
        if (FoundSlot->Item->GetQuantity() <= 0)
        {
            Execute_RemoveItemFromInventory(this, FoundSlot->Item);
        	
        	Result.ResultID = Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_MutliStatus; // Corresponds to "Multi-Status
        	Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemQuantityReducedRemoved", "The item has been reduced and removed from the inventory.");
        }
        else
        {
        	Result.OptionalPayload = FoundSlot->Item;
        	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK; // Corresponds to "OK"
            Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemQuantityReduced", "The item quantity has been reduced in the inventory.");

        	// Update Stacks in the Slot
        	UMounteaInventoryItemBFL::ReduceQuantityInStacks(*FoundSlot, Quantity);

			// Mark slot as modified
        	ModifiedSlots.Add(*FoundSlot);
        	
        	// Broadcast that an item has been updated
        	OnItemUpdated.Broadcast(Result);
        }
    }
	// Remove the item slot if the quantity is zero or below
    else
    {
    	Execute_RemoveItemFromInventory(this, FoundSlot->Item);
        	
    	Result.ResultID = Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_MutliStatus; // Corresponds to "Multi-Status
    	Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemQuantityReducedRemoved", "The item has been reduced and removed from the inventory.");
    }

	Result.OptionalPayload = this;
	OnInventoryUpdated.Broadcast(Result);
	
    return Result;
}

TArray<FInventoryUpdateResult> UMounteaInventoryComponent::ReduceItemsInInventory_Implementation(const TMap<UMounteaInstancedItem*, int32>& Items, int32 TransactionTypeFlags)
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_ReduceItemsInInventory );

	TArray<FInventoryUpdateResult> Results;

	// Bitmask Corrections
	{
		// Define the maximum valid value for TransactionTypeFlags
		const uint8 MaxTransactionTypeValue =
			static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction) |
			static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction) |
			static_cast<uint8>(EInventoryTransactionType::EITT_Sync) |
			static_cast<uint8>(EInventoryTransactionType::EITT_Async);
	
		// Check for empty value
		if (TransactionTypeFlags == static_cast<uint8>(EInventoryTransactionType::None))
		{
			TransactionTypeFlags = static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction) | static_cast<uint8>(EInventoryTransactionType::EITT_Sync);
		}

		// Check for both FullTransaction and PartialTransaction being selected
		if ((TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction)) != 0 && (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_PartialTransaction)) != 0)
		{
			TransactionTypeFlags &= ~static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction);
		}

		// Check for both Sync and Async being selected
		if ((TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Sync)) != 0 && (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Async)) != 0)
		{
			TransactionTypeFlags &= ~static_cast<uint8>(EInventoryTransactionType::EITT_Async);
		}
	}

	// Extracting the boolean values from the bitmask
	const bool bFullOnly = (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_FullTransaction)) != 0;
	const bool bIsSync = (TransactionTypeFlags & static_cast<uint8>(EInventoryTransactionType::EITT_Sync)) != 0;

	if (bIsSync)
	{
		if (bFullOnly)
		{
			// First validate
			for (auto const& Itr : Items)
			{
				// Create an empty result struct.
				FInventoryUpdateResult Result;

				if (Itr.Key== nullptr)
				{
					Result.OptionalPayload = Itr.Key;
					Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
					Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");
			
					return  Results;
				}
				
				// Check if a similar item already exists in the inventory
				FItemRetrievalFilter Filter;
				{
					Filter.bSearchByGUID = true;
					Filter.Guid = Itr.Key->GetGuid();
					Filter.bSearchByItem = true;
					Filter.Item = Itr.Key;
				}

				const UMounteaInstancedItem* ExistingItem = Execute_SearchSingleItem(this, Filter);
				if (ExistingItem == nullptr)
				{
					Result.OptionalPayload = Itr.Key;
					Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound; // Corresponds to "Not Found"
					Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot find Item in inventory.");

					return  Results;
				}
			}
		}
		
		// Add items
		for (auto const& Itr : Items)
		{
			const FInventoryUpdateResult IndividualResult = Execute_ReduceItemInInventory(this, Itr.Key, Itr.Value);

			Results.Add(IndividualResult);
		}
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this, Items, &Results, bFullOnly]() mutable
		{
			bool bSatisfied = true;
			if (bFullOnly)
			{
				// First validate
				for (auto const& Itr : Items)
				{
					// Create an empty result struct.
					FInventoryUpdateResult Result;

					if (Itr.Key== nullptr)
					{
						Result.OptionalPayload = Itr.Key;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot add the item to inventory.");

						Results.Add(Result);
						bSatisfied = false;
						break;
					}
					
					// Check if a similar item already exists in the inventory
					FItemRetrievalFilter Filter;
					{
						Filter.bSearchByGUID = true;
						Filter.Guid = Itr.Key->GetGuid();
						Filter.bSearchByItem = true;
						Filter.Item = Itr.Key;
					}

					const UMounteaInstancedItem* ExistingItem = Execute_SearchSingleItem(this, Filter);
					if (ExistingItem == nullptr)
					{
						Result.OptionalPayload = Itr.Key;
						Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound; // Corresponds to "Not Found"
						Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Cannot find Item in inventory.");

						Results.Add(Result);
						bSatisfied = false;
						break;
					}
				}
			}

			if (bSatisfied)
			{
				for (auto const& Itr : Items)
				{
					const FInventoryUpdateResult IndividualResult = Execute_ReduceItemInInventory(this, Itr.Key, Itr.Value);

					Results.Add(IndividualResult);
				}
			}
		});
	}
	
	return Results;
}

void UMounteaInventoryComponent::AddItemToInventory_Server_Implementation(UMounteaInstancedItem* Item, const int32& Quantity)
{
	Execute_AddItemToInventory(this, Item, Quantity);
}

bool UMounteaInventoryComponent::AddItemToInventory_Server_Validate(UMounteaInstancedItem* Item, const int32& Quantity)
{ return true; }

void UMounteaInventoryComponent::RemoveItemFromInventory_Server_Implementation(UMounteaInstancedItem* Item)
{
	Execute_RemoveItemFromInventory(this, Item);
}

bool UMounteaInventoryComponent::RemoveItemFromInventory_Server_Validate(UMounteaInstancedItem* Item)
{ return true; }

void UMounteaInventoryComponent::ReduceItemInInventory_Server_Implementation(UMounteaInstancedItem* Item, const int32& Quantity)
{
	Execute_ReduceItemInInventory(this, Item, Quantity);
}

bool UMounteaInventoryComponent::ReduceItemInInventory_Server_Validate(UMounteaInstancedItem* Item, const int32& Quantity)
{ return true; }

bool UMounteaInventoryComponent::CanAddItem_Implementation(const FItemTransfer& Item) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_CanAddItem );
	
	// Check if the Item is valid.
	if (!UMounteaInventoryItemBFL::IsItemValid(Item.Item))
	{
		return false;
	}

	// Check if we have this type of item in the inventory already
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByGUID = true;
		Filter.Guid = Item.Item->GetGuid();
		Filter.bSearchByItem = true;
		Filter.Item = Item.Item;
	}

	const bool bHasItem = Execute_HasItem(this, Filter);

	// Calculate the maxAddAmount value
	const int32 MaxAddAmount = UMounteaInventoryItemBFL::CalculateAddAmount(Item.Item, Item.Quantity);

	// This means there is already no space
	if (MaxAddAmount == 0)
	{
		return false;
	}

	// If the item is stackable
	if (Item.Item->GetItemData().RequiredData.ItemQuantity.bIsStackable)
	{
		// If the current item amount we want to add is less than or equal to the max 
		// amount we can add to the stack, return true.
		if (Item.Quantity <= MaxAddAmount)
		{
			return true;
		}
		// If not, but there's partial space, return true as well, since partial addition is allowed. Valid MaxAddAmount has already been validated before.
		else
		{
			return true;
		}
	}
	// If the item is not stackable
	else
	{
		// If we already have this item in inventory, return false as we can't add another non-stacking item of the same type.
		if (bHasItem)
		{
			return false;
		}
		// If we don't have the item, it's the creation process. Valid MaxAddAmount has already been validated before.
		else
		{
			return true;
		}
	}
}

bool UMounteaInventoryComponent::CanRemoveItem_Implementation(UMounteaInstancedItem* Item) const
{
	// Check if the ItemToRemove is valid.
	if (Item == nullptr)
	{
		return false;
	}
	
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByItem = true;
		Filter.Item = Item;
	}

	// If no matching item is found in the inventory, return false.
	if (!Execute_HasItem(this, Filter))
	{
		return false;
	}

	// Return true as the item exists in the inventory and can potentially be removed.
	return true;
}

bool UMounteaInventoryComponent::CanReduceItem_Implementation(const FItemTransfer& Item) const
{
	if (Item.Item == nullptr)
	{
		return false;
	}

	if (Item.Quantity <= 0)
	{
		return false;
	}

	FItemRetrievalFilter SearchFilter;
	SearchFilter.Item = Item.Item;
	SearchFilter.bSearchByItem = true;
	SearchFilter.Guid = Item.Item->GetGuid();
	SearchFilter.bSearchByGUID = true;

	
	const auto ExistingItem = Execute_SearchSingleItem(this, SearchFilter);
	if (ExistingItem == nullptr)
	{
		return false;
	}

	return ExistingItem->GetQuantity() >= Item.Quantity;
}

bool UMounteaInventoryComponent::HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_HasItem );

	if (SearchFilter.IsValid())
	{
		return Execute_SearchSingleItem(this, SearchFilter) != nullptr;
	}

	return false;
}

UMounteaInstancedItem* UMounteaInventoryComponent::SearchSingleItem_Implementation(const FItemRetrievalFilter& SearchFilter) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_SearchSingleItem );
	
	// If we are searching specifically by a UMounteaInstancedItem
	if (SearchFilter.bSearchByItem && SearchFilter.Item != nullptr)
	{
		const FItemSlot* Slot = InventorySlots.FindByKey(SearchFilter.Item);
		if (Slot && Slot->IsValid())
		{
			return Slot->Item;
		}
	}

	// If we are searching specifically by FGuid
	if (SearchFilter.bSearchByGUID)
	{
		const FItemSlot* Slot =  InventorySlots.FindByKey(SearchFilter.Guid);
		if (Slot && Slot->IsValid())
		{
			return Slot->Item;
		}
	}

	// Search by other conditions
	for (const FItemSlot& Slot : InventorySlots)
	{
		UMounteaInstancedItem* Item = Slot.Item;
		if (!Item) continue;

		if (SearchFilter.bSearchByClass && Item->GetSourceItem() && Item->GetSourceItem()->IsA(SearchFilter.Class))
		{
			return Item;
		}

		if (SearchFilter.bSearchByTag && Item->GetItemFlags().HasAny(SearchFilter.Tags))
		{
			return Item;
		}
	}

	// No matching item found in this case
	return nullptr;
}

TArray<UMounteaInstancedItem*> UMounteaInventoryComponent::SearchMultipleItems_Implementation(const FItemRetrievalFilter& SearchFilter) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_SearchMultipleItems );
	
	TArray<UMounteaInstancedItem*> ResultArray;

	// If we are searching specifically by a UMounteaInstancedItem
	if (SearchFilter.bSearchByItem && SearchFilter.Item != nullptr)
	{
		const FItemSlot* Slot = InventorySlots.FindByKey(SearchFilter.Item);
		if (Slot && Slot->IsValid())
		{
			ResultArray.AddUnique(Slot->Item);
		}
	}

	// If we are searching specifically by FGuid
	if (SearchFilter.bSearchByGUID)
	{
		const FItemSlot* Slot =  InventorySlots.FindByKey(SearchFilter.Guid);
		if (Slot && Slot->IsValid())
		{
			ResultArray.AddUnique(Slot->Item);
		}
	}
	
	// Search by other conditions
	for (const FItemSlot& Slot : InventorySlots)
	{
		UMounteaInstancedItem* Item = Slot.Item;
		if (!Item) continue;

		if (SearchFilter.bSearchByClass && Item->GetSourceItem() && Item->GetSourceItem()->IsA(SearchFilter.Class))
		{
			ResultArray.AddUnique(Item);
		}

		if (SearchFilter.bSearchByTag && Item->GetItemFlags().HasAny(SearchFilter.Tags))
		{
			ResultArray.AddUnique(Item);
		}
	}

	// Add other search conditions here as necessary

	return ResultArray;
}

TArray<UMounteaInstancedItem*> UMounteaInventoryComponent::GetItems_Implementation() const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_GetItems );
	
	TArray<UMounteaInstancedItem*> AllItems;
	if (const FItemSlot* ValidSlot = InventorySlots.FindByPredicate(
			[](const FItemSlot& Slot)
			{
				return Slot.IsValid();
			}))
	{
		AllItems.Add(ValidSlot->Item);
	}

	return AllItems;
}

FGameplayTagContainer UMounteaInventoryComponent::GetInventoryFlags_Implementation() const
{
	return InventoryFlags;
}

bool UMounteaInventoryComponent::HasFlag_Implementation(const FGameplayTag& SearchedFlag, const bool bSearchExact)
{
	return bSearchExact ? InventoryFlags.HasTagExact(SearchedFlag) : InventoryFlags.HasTag(SearchedFlag);
}

bool UMounteaInventoryComponent::HasFlags_Implementation(const FGameplayTagContainer& SearchedFlags, const bool bSearchExact, const bool bSearchFast)
{
	if (bSearchExact)
	{
		return bSearchFast ? InventoryFlags.HasAnyExact(SearchedFlags) : InventoryFlags.HasAllExact(SearchedFlags);
	}
	else
	{
		return bSearchFast ? InventoryFlags.HasAny(SearchedFlags) : InventoryFlags.HasAll(SearchedFlags);
	}
}

AActor* UMounteaInventoryComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

void UMounteaInventoryComponent::SetInventoryUIClass_Implementation(TSubclassOf<UMounteaBaseUserWidget> NewInventoryWBPClass)
{
	InventoryWBPClass = NewInventoryWBPClass;
}

void UMounteaInventoryComponent::SetInventoryUI_Implementation(UMounteaBaseUserWidget* NewWBP)
{
	if (InventoryWBP && InventoryWBP != NewWBP)
	{
		TScriptInterface<IMounteaInventoryWBPInterface> InventoryWBPInterface;
		InventoryWBPInterface.SetInterface(Cast<IMounteaInventoryWBPInterface>(InventoryWBP));

		const TScriptInterface<IMounteaInventoryInterface> OwningInventory;
		InventoryWBPInterface->Execute_SetOwningInventory(InventoryWBP, OwningInventory);
	}
	
	InventoryWBP = NewWBP;
	if (InventoryWBP)
	{
		TScriptInterface<IMounteaInventoryWBPInterface> InventoryWBPInterface;
		InventoryWBPInterface.SetInterface(Cast<IMounteaInventoryWBPInterface>(InventoryWBP));
		
		InventoryWBPInterface->Execute_SetOwningInventory(InventoryWBP, this);
	}
}

UMounteaInventoryConfig* UMounteaInventoryComponent::GetInventoryConfig_Implementation( TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult) const
{
	if (ClassFilter == nullptr)
	{
		bResult = false;
		return nullptr;
	}

	bResult = true;
	if (InventoryConfig.MounteaInventoryConfig == nullptr)
	{
		return NewObject<UMounteaInventoryConfig>(GetPackage(), ClassFilter);
	}

	return InventoryConfig.MounteaInventoryConfig->IsA(ClassFilter) ? InventoryConfig.MounteaInventoryConfig : NewObject<UMounteaInventoryConfig>(GetPackage(), ClassFilter);
}

TSubclassOf<UMounteaInventoryConfig> UMounteaInventoryComponent::GetInventoryConfigClass_Implementation() const
{
	if (InventoryConfig.MounteaInventoryConfig)
	{
		return InventoryConfig.MounteaInventoryConfig->StaticClass();
	}

	return nullptr;
}

bool UMounteaInventoryComponent::SetInventoryFlags_Implementation(const FGameplayTagContainer& NewFlags)
{
	// TODO: replicate
	
	InventoryFlags.Reset();

	InventoryFlags.AppendTags(NewFlags);
	
	return true;
}

bool UMounteaInventoryComponent::SetInventoryFlag_Implementation(const FGameplayTag& NewFlag)
{
	// TODO: replicate

	if (InventoryFlags.HasTag(NewFlag))
	{
		return false;
	}

	InventoryFlags.AddTag(NewFlag);

	return true;
}

bool UMounteaInventoryComponent::DoesHaveAuthority_Implementation() const
{
	if (GetOwner()==nullptr) return false;
	return GetOwner()->HasAuthority();
}

bool UMounteaInventoryComponent::IsAuthorityOrAutonomousProxy() const
{
	if (!GetOwner()) return false;
	
	switch (GetOwnerRole())
	{
		case ROLE_MAX:
		case ROLE_None:
		case ROLE_Authority:
		case ROLE_AutonomousProxy: return true;
		case ROLE_SimulatedProxy: break;
		default: break;
	}

	return false;
}

void UMounteaInventoryComponent::PostInventoryUpdated_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!IsAuthorityOrAutonomousProxy()) return;
	
	if (UpdateContext.OptionalPayload)
	{
		PostInventoryUpdated_Client(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemAdded_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!IsAuthorityOrAutonomousProxy()) return;

	if (UpdateContext.OptionalPayload)
	{
		const TScriptInterface<IMounteaInventoryInstancedItemInterface> ItemInterfacePayload = UpdateContext.OptionalPayload;
		ItemInterfacePayload->NetFlush();

		PostItemAdded_Client(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemRemoved_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!IsAuthorityOrAutonomousProxy()) return;

	if (UpdateContext.OptionalPayload)
	{
		const TScriptInterface<IMounteaInventoryInstancedItemInterface> ItemInterfacePayload = UpdateContext.OptionalPayload;
		ItemInterfacePayload->NetFlush();

		PostItemRemoved_Client(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemUpdated_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!IsAuthorityOrAutonomousProxy()) return;

	if (UpdateContext.OptionalPayload)
	{
		const TScriptInterface<IMounteaInventoryInstancedItemInterface> ItemInterfacePayload = UpdateContext.OptionalPayload;
		ItemInterfacePayload->NetFlush();

		PostItemUpdated_Client(UpdateContext);
	}
}

void UMounteaInventoryComponent::RequestInventoryNotification(const FInventoryUpdateResult& UpdateContext) const
{
	const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
	FInventoryNotificationData Data = *Settings->InventoryUpdateData.Find(UpdateContext.ResultID);

	if (Data.Weight >= Settings->MinDisplayWeight)
	{
		if (!UpdateContext.ResultText.IsEmpty())
		{
			Data.NotificationText = UpdateContext.ResultText;
		}

		IMounteaInventoryWBPInterface::Execute_CreateInventoryNotification(InventoryWBP, Data);
	}
}

void UMounteaInventoryComponent::PostInventoryUpdated_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	if (!IsAuthorityOrAutonomousProxy()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostInventoryUpdated_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestInventorySyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemAdded_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	if (!IsAuthorityOrAutonomousProxy()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemAdded_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestInventorySyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemRemoved_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;

	if (!IsAuthorityOrAutonomousProxy()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemRemoved_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestInventorySyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemUpdated_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;

	if (!IsAuthorityOrAutonomousProxy()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemUpdated_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestInventorySyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostInventoryUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);
	
	OnInventoryUpdated_Client.Broadcast(UpdateContext);

	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);

		RequestInventoryNotification(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemAdded_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets, UpdateContext.OptionalPayload);
		
		RequestInventoryNotification(UpdateContext);
		
		const TScriptInterface<IMounteaInventoryInstancedItemInterface> ItemInterface = UpdateContext.OptionalPayload;
		if (ItemInterface.GetObject())
		{
			ItemInterface->GetEventHandle_OnItemAdded().Broadcast(UpdateContext.ResultText.ToString());
		}
		OnItemAdded_Client.Broadcast(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemRemoved_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RemoveItemWidget, UpdateContext.OptionalPayload);

		RequestInventoryNotification(UpdateContext);

		const TScriptInterface<IMounteaInventoryInstancedItemInterface> ItemInterface = UpdateContext.OptionalPayload;
		if (ItemInterface.GetObject())
		{
			ItemInterface->GetEventHandle_OnItemRemoved().Broadcast(UpdateContext.ResultText.ToString());
		}
		OnItemRemoved_Client.Broadcast(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets, UpdateContext.OptionalPayload);
		
		RequestInventoryNotification(UpdateContext);

		const TScriptInterface<IMounteaInventoryInstancedItemInterface> ItemInterface = UpdateContext.OptionalPayload;
		if (ItemInterface.GetObject())
		{
			ItemInterface->GetEventHandle_OnItemModified().Broadcast(UpdateContext.ResultText.ToString());
		}

		OnItemUpdated_Client.Broadcast(UpdateContext);
	}
}

void UMounteaInventoryComponent::RequestNetworkRefresh_Implementation()
{
	if (!Execute_DoesHaveAuthority(this))
	{
		RequestNetworkRefresh_Server();
		return;
	}

	ReplicatedItemsKey++;
}

void UMounteaInventoryComponent::RequestNetworkRefresh_Server_Implementation()
{
	RequestNetworkRefresh();
}

void UMounteaInventoryComponent::OnRep_Items()
{
	if (const FItemSlot* DirtySlot  = InventorySlots.FindByPredicate(
			[this](const FItemSlot& Slot)
			{
				if (!this)
				{
					return false;
				}
				
				return
				Slot.IsValid() &&
				Slot.Item->GetWorld() == nullptr;
			}))
	{
		// If the slot contains an item, perform necessary updates
		if (DirtySlot && DirtySlot->Item)
		{
			// Set the world context for the item, ensuring it is properly located in the game world.
			DirtySlot->Item->SetWorld(GetWorld());
		}
	}
}

FInventoryUpdateResult UMounteaInventoryComponent::ProcessItemAction_Implementation(UMounteaInventoryItemAction* Action, UMounteaInstancedItem* Item, FMounteaDynamicDelegateContext Context)
{
	FInventoryUpdateResult Result;

	// Validate the request
	if (!Action)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid Action.");
		
		return Result;
	}
	
	// Validate the request
	if (!Item)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item.");
		
		return Result;
	}

	const AActor* OwningActor = Execute_GetOwningActor(this);
	if (!OwningActor)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item, there is no owner!.");
		
		return Result;
	}

	const UWorld* OwningWorld = GetWorld();
	if (!OwningWorld)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item, there is no world!.");
		
		return Result;
	}

	if (Action->DoesRequireAuthority() && !Execute_DoesHaveAuthority(this))
	{
		ProcessItemAction_Server(Action, Item, Context);

		Result.OptionalPayload = Action;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing; // Corresponds to "Processing"
		Result.ResultText = LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request.");
		
		return Result;
	}
	
	Result = UMounteaInventoryItemBFL::ProcessItemAction(Action, Item, Context);

	return Result;
}

void UMounteaInventoryComponent::ProcessItemAction_Server_Implementation(UMounteaInventoryItemAction* Action, UMounteaInstancedItem* Item, FMounteaDynamicDelegateContext Context)
{
	Execute_ProcessItemAction(this, Action, Item, Context);
}

#if WITH_EDITOR

void UMounteaInventoryComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == TEXT("MounteaInventoryConfig"))
	{
		if (InventoryConfig.MounteaInventoryConfig == nullptr)
		{
			bool bFound = false;
			const TSubclassOf<UMounteaInventoryConfig> Class = UMounteaInventoryEquipmentBPF::GetSettings()->DefaultInventoryConfigClass.LoadSynchronous();
			InventoryConfig.MounteaInventoryConfig = UMounteaInventoryEquipmentBPF::GetInventoryConfig(GetPackage()->GetOuter(), Class, bFound);
		}
	}
}

#endif

/*===============================================================================
		SUBJECT OF CHANGE
		
		Following functions are using outdated, wrong class definitions and functions.
===============================================================================*/

bool UMounteaInventoryComponent::LoadInventoryFromDataTable_Implementation(const UMounteaInventoryItemsTable* SourceTable)
{
	return true;
}

void UMounteaInventoryComponent::SaveInventory_Implementation()
{
	// TODO
}

#undef LOCTEXT_NAMESPACE
