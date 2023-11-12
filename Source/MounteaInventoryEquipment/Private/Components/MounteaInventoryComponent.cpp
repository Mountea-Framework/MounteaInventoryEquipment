// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaInventoryInstancedItem.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Engine/ActorChannel.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Helpers/MounteaInventoryItemBFL.h"
#include "Interfaces/MounteaInventoryWBPInterface.h"

#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
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
		TSet<UMounteaInstancedItem*> AllItems;
		TArray<FItemSlot> AllSlots = InventorySlots;
		AllSlots.Append(NullifiedSlots);

		// Setup all Items, even from Slots that should be empty at this point
		for (auto const& Itr : AllSlots)
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
		NullifiedSlots.Empty();
	}

	return bUpdated;
}

TSubclassOf<UMounteaBaseUserWidget> UMounteaInventoryComponent::GetInventoryUIClass_Implementation() const
{
	return InventoryWBPClass;
}

UMounteaBaseUserWidget* UMounteaInventoryComponent::GetInventoryUI_Implementation() const
{
	return InventoryWBP;
}

bool UMounteaInventoryComponent::LoadInventoryFromDataTable_Implementation(const UMounteaInventoryItemsTable* SourceTable)
{
	return true;
}

void UMounteaInventoryComponent::SaveInventory_Implementation()
{
	// TODO
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
		Result.ResultID = 400; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item.");
		return Result;
	}

	// Check if there is Authority
	if (!Execute_DoesHaveAuthority(this))
	{
		AddItemToInventory_Server(Item, Quantity);
		
		Result.OptionalPayload = Item;
		Result.ResultID = 102; // Corresponds to "Processing"
		Result.ResultText = LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request.");
		return Result;
	}
	
    // Check if Quantity is larger than 0
    if (Quantity <= 0)
    {
        Result.OptionalPayload = Item;
        Result.ResultID = 400; // Corresponds to "Bad Request"
        Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidIQuantity", "Quantity less than 1 is forbidden.");
        return Result;
    }

    // Check if the item can be added at all
    if (!Execute_CanAddItem(this, Item, Quantity))
    {
        Result.OptionalPayload = Item;
        Result.ResultID = 403; // Corresponds to "Forbidden"
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

            Result.OptionalPayload = ExistingItem;
            Result.ResultID = 200; // Corresponds to "OK"
            Result.ResultText =  LOCTEXT("InventoryUpdateResult_ItemUpdated", "Added all of the item to the existing stack.");
        }
        else
        {
            // If only part of the Item can be added
            ExistingItem->SetQuantity(MaxAddAmount); // Add the maximum possible quantity to the existing item
            Item->ModifyQuantity(-MaxAddAmount); // Decrease the source item's quantity

            Result.OptionalPayload = ExistingItem;
            Result.ResultID = 206; // Analogous to "Partial Content"
            Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemUpdatedPartly", "Added part of the item to the existing stack. Some quantity remains in the source item.");
        }
    }
    else
    {
        // If the item doesn't exist in the inventory
        if (Quantity <= MaxAddAmount)
        {
            // Directly add the entire source item to the inventory
        	// TODO: implement logic to automatically split items to stacks
        	const FItemSlot NewSlot = FItemSlot(Item);

            Result.OptionalPayload = Item;
            Result.ResultID = 201; // Corresponds to "Created"
            Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemCreated", "Added the entire item to the inventory.");
        }
        else
        {
            // If only part of the Item can be added
            UMounteaInstancedItem* NewItemInstance = NewObject<UMounteaInstancedItem>(GetWorld());
        	
        	FItemInitParams ItemInitParams;
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
            		break;;
            }
        	
        	const bool bNewItemSuccess = NewItemInstance->Execute_InitializeNewItem(NewItemInstance, ItemInitParams);

        	if (bNewItemSuccess)
        	{
        		// TODO: implement logic to automatically split items to stacks
		        const FItemSlot NewSlot = FItemSlot(NewItemInstance);
        		
        		
        		InventorySlots.Add(NewSlot); // Add the new instance to the inventory
        		Item->ModifyQuantity(-MaxAddAmount); // Decrease the source item's quantity

        		Result.OptionalPayload = NewItemInstance;
        		Result.ResultID = 206; // Analogous to "Partial Content"
        		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemUpdatedPartly", "Added part of the item to the existing stack. Some quantity remains in the source item.");
        	}
	        else
	        {
	        	Result.OptionalPayload = Item;
	        	Result.ResultID = 500; // Analogous to "Unknown issue"
	        	Result.ResultText =  LOCTEXT("InventoryUpdateResult_ItemUnknownIssue", "Unhandled Exception.");
	        }
        }
    }
		
    return Result;
}

FInventoryUpdateResult UMounteaInventoryComponent::RemoveItemFromInventory_Implementation(UMounteaInstancedItem* Item)
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_RemoveItemFromInventory );

	FInventoryUpdateResult Result;

	// Validate the request
	if (!Item)
	{
		Result.ResultID = 400; // Bad Request
		Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item.");
		return Result;
	}

	// Check if there is Authority
	if (!Execute_DoesHaveAuthority(this))
	{
		RemoveItemFromInventory_Server(Item);
		
		Result.OptionalPayload = Item;
		Result.ResultID = 102; // Corresponds to "Processing"
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
		Result.ResultID = 404;
		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemNotFound", "The item was not found in the inventory.");

		return Result;
	}
	
	// If the item exists in the inventory, remove it.
	if (const FItemSlot* ExistingItemSlot = InventorySlots.FindByKey(Item))
	{
		// Keep track of this slot being nullified to it gets updated once before it's removed - useful for UI
		NullifiedSlots.AddUnique(*ExistingItemSlot);
		
		// Assuming InventorySlots is a TArray or TSet of FItemSlot
		InventorySlots.Remove(*ExistingItemSlot);

		Result.ResultID = 200;
		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemRemoved", "The item has been successfully removed from the inventory.");
	}
	else
	{
		Result.ResultID = 404;
		Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemNotFound", "The item was not found in the inventory.");
	}

	return Result;
}

FInventoryUpdateResult UMounteaInventoryComponent::ReduceItemInInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UMounteaInventoryComponent_ReduceItemInInventory);

    FInventoryUpdateResult Result;

    // Validate the request
    if (!Item)
    {
        Result.ResultID = 400; // Bad Request
        Result.ResultText = LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid item or quantity.");
    	
        return Result;
    }
	
	// Check if there is Authority
	if (!Execute_DoesHaveAuthority(this))
	{
		ReduceItemInInventory_Server(Item, Quantity);
		
		Result.OptionalPayload = Item;
		Result.ResultID = 102; // Corresponds to "Processing"
		Result.ResultText = LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request.");
		
		return Result;
	}

    // Search for the item in the inventory
    const FItemSlot* FoundSlot = InventorySlots.FindByKey(Item);
	
    if (!FoundSlot)
    {
        Result.OptionalPayload = Item;
        Result.ResultID = 404; // Not Found
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
        	
        	Result.ResultID = 207; // Corresponds to "Multi-Status
        	Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemQuantityReducedRemoved", "The item has been reduced and removed from the inventory.");
        }
        else
        {
            Result.ResultID = 200; // OK
            Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemQuantityReduced", "The item quantity has been reduced in the inventory.");
        }
    }
	// Remove the item slot if the quantity is zero or below
    else
    {
    	Execute_RemoveItemFromInventory(this, FoundSlot->Item);
        	
    	Result.ResultID = 207; // Corresponds to "Multi-Status
    	Result.ResultText = LOCTEXT("InventoryUpdateResult_ItemQuantityReducedRemoved", "The item has been reduced and removed from the inventory.");
    }

    return Result;
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

bool UMounteaInventoryComponent::CanAddItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_CanAddItem );
	
	// Check if the Item is valid.
	if (!UMounteaInventoryItemBFL::IsItemValid(Item))
	{
		return false;
	}

	// Check if we have this type of item in the inventory already
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByGUID = true;
		Filter.Guid = Item->GetGuid();
		Filter.bSearchByItem = true;
		Filter.Item = Item;
	}

	const bool bHasItem = Execute_HasItem(this, Filter);

	// Calculate the maxAddAmount value
	const int32 MaxAddAmount = UMounteaInventoryItemBFL::CalculateAddAmount(Item, Quantity);

	// This means there is already no space
	if (MaxAddAmount == 0)
	{
		return false;
	}

	// If the item is stackable
	if (Item->GetItemData().RequiredData.ItemQuantity.bIsStackable)
	{
		// If the current item amount we want to add is less than or equal to the max 
		// amount we can add to the stack, return true.
		if (Quantity <= MaxAddAmount)
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

bool UMounteaInventoryComponent::CanExecuteCosmetics() const
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

bool UMounteaInventoryComponent::HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_HasItem );

	if (SearchFilter.IsValid())
	{
		if (UMounteaInventoryEquipmentBPF::GetSettings()->MultithreadingThreshold < Items.Num())
		{
			return HasItem_Multithreading(SearchFilter);
		}
		else
		{
			return HasItem_Simple(SearchFilter);
		}
	}

	return false;
}

bool UMounteaInventoryComponent::HasItem_Simple(const FItemRetrievalFilter& SearchFilter) const
{
	// BREAKING
	/*if (SearchFilter.IsValid())
	{
		// Search by Item
		if (SearchFilter.bSearchByItem)
		{
			if (Items.Contains(SearchFilter.Item))
			{
				return true;
			}
		}

		// Search by Class
		if (SearchFilter.bSearchByClass)
		{
			for (const auto& Itr : Items)
			{
				if (Itr && Itr->IsA(SearchFilter.Class))
				{
					return true;
				}
			}
		}

		// Search by Tag
		if (SearchFilter.bSearchByTag)
		{
			for (const auto& Itr : Items)
			{
				if (Itr && Itr->ItemData.RequiredData.ItemFlags.HasAny(SearchFilter.Tags))
				{
					return true;
				}
			}
		}

		// Search by GUID
		if (SearchFilter.bSearchByGUID)
		{
			for (const auto& Itr : Items)
			{
				if (Itr && Itr->GetGuid() == SearchFilter.Guid)
				{
					return true;
				}
			}
		}

		return false;
	}*/
	
	return false;
}

bool UMounteaInventoryComponent::HasItem_Multithreading(const FItemRetrievalFilter& SearchFilter) const
{
	if (!SearchFilter.IsValid())
		return false;

	TAtomic<bool> ItemFound(false);
	
	const int32 NumChunks = FMath::CeilToInt(static_cast<float>(Items.Num()) / ChunkSize);

	auto SearchFunction = [&](const int32 ChunkIndex)
	{
		if (ItemFound.Load())  
			return;

		const int32 StartIndex = ChunkIndex * ChunkSize;
		const int32 EndIndex = FMath::Min(StartIndex + ChunkSize, Items.Num());

		for (int32 i = StartIndex; i < EndIndex; i++)
		{
			if (ItemFound.Load())  
				return;

			const auto& Item = Items[i];

			// Search by Item
			/*if (SearchFilter.bSearchByItem && Item == SearchFilter.Item)
			{
				ItemFound = true;
				return;
			}*/

			// Search by Class
			if (SearchFilter.bSearchByClass && Item && Item->IsA(SearchFilter.Class))
			{
				ItemFound = true;
				return;
			}

			// Search by Tag
			if (SearchFilter.bSearchByTag && Item && Item->ItemData.RequiredData.ItemFlags.HasAny(SearchFilter.Tags))
			{
				ItemFound = true;
				return;
			}

			// Search by GUID
			if (SearchFilter.bSearchByGUID && Item && Item->GetGuid() == SearchFilter.Guid)
			{
				ItemFound = true;
				return;
			}
		}
	};

	ParallelFor(NumChunks, SearchFunction);

	return ItemFound.Load();
}

UMounteaInventoryItemBase* UMounteaInventoryComponent::FindItem_Implementation(const FItemRetrievalFilter& SearchFilter) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_FindItem );
	
	if (SearchFilter.IsValid())
	{
		if (UMounteaInventoryEquipmentBPF::GetSettings()->MultithreadingThreshold < Items.Num())
		{
			return FindItem_Multithreading(SearchFilter);
		}
		else
		{
			return FindItem_Simple(SearchFilter);
		}
	}

	return nullptr;
}

UMounteaInventoryItemBase* UMounteaInventoryComponent::FindItem_Simple(const FItemRetrievalFilter& SearchFilter) const
{
	if (SearchFilter.IsValid())
	{
		// Search by Item
		if (SearchFilter.bSearchByItem)
		{
			/*if (Items.Contains(SearchFilter.Item))
			{
				return SearchFilter.Item;
			}*/
		}

		// Search by Class
		if (SearchFilter.bSearchByClass)
		{
			for (const auto& Itr : Items)
			{
				if (Itr && Itr->IsA(SearchFilter.Class))
				{
					return Itr;
				}
			}
		}

		// Search by Tag
		if (SearchFilter.bSearchByTag)
		{
			for (const auto& Itr : Items)
			{
				if (Itr && Itr->ItemData.RequiredData.ItemFlags.HasAny(SearchFilter.Tags))
				{
					return Itr;
				}
			}
		}

		// Search by GUID
		if (SearchFilter.bSearchByGUID)
		{
			for (const auto& Itr : Items)
			{
				if (Itr && Itr->GetGuid() == SearchFilter.Guid)
				{
					return Itr;
				}
			}
		}		
	}

	return nullptr;
}

UMounteaInventoryItemBase* UMounteaInventoryComponent::FindItem_Multithreading(const FItemRetrievalFilter& SearchFilter) const
{
	if (!SearchFilter.IsValid())
		return nullptr;

	TAtomic<UMounteaInventoryItemBase*> FoundItem(nullptr);
	
	const int32 NumChunks = FMath::CeilToInt(static_cast<float>(Items.Num()) / ChunkSize);

	auto SearchFunction = [&](const int32 ChunkIndex)
	{
		if (FoundItem.Load())  
			return;

		const int32 StartIndex = ChunkIndex * ChunkSize;
		const int32 EndIndex = FMath::Min(StartIndex + ChunkSize, Items.Num());

		for (int32 i = StartIndex; i < EndIndex; i++)
		{
			if (FoundItem.Load())  
				return;

			const auto& Item = Items[i];

			// Search by Item
			/*if (SearchFilter.bSearchByItem && Item == SearchFilter.Item)
			{
				FoundItem = Item;
				return;
			}*/

			// Search by Class
			if (SearchFilter.bSearchByClass && Item && Item->IsA(SearchFilter.Class))
			{
				FoundItem = Item;
				return;
			}

			// Search by Tag
			if (SearchFilter.bSearchByTag && Item && Item->ItemData.RequiredData.ItemFlags.HasAny(SearchFilter.Tags))
			{
				FoundItem = Item;
				return;
			}

			// Search by GUID
			if (SearchFilter.bSearchByGUID && Item && Item->GetGuid() == SearchFilter.Guid)
			{
				FoundItem = Item;
				return;
			}
		}
	};

	ParallelFor(NumChunks, SearchFunction);

	return FoundItem.Load();
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryComponent::GetItems_Simple(const FItemRetrievalFilter OptionalFilter) const
{
	if (OptionalFilter.IsValid())
	{
		TArray<UMounteaInventoryItemBase*> FoundItems;
			
		// Search by Item
		if (OptionalFilter.bSearchByItem)
		{
			for (const auto& Item : Items)
			{
				/*if (Item == OptionalFilter.Item)
				{
					FoundItems.Add(Item);
				}*/
			}
		}

		// Search by Class
		if (OptionalFilter.bSearchByClass)
		{
			for (const auto& Item : Items)
			{
				if (Item && Item->IsA(OptionalFilter.Class))
				{
					FoundItems.Add(Item);
				}
			}
		}

		// Search by Tag
		if (OptionalFilter.bSearchByTag)
		{
			for (const auto& Item : Items)
			{
				if (Item && Item->ItemData.RequiredData.ItemFlags.HasAny(OptionalFilter.Tags))
				{
					FoundItems.Add(Item);
				}
			}
		}

		// Search by GUID
		if (OptionalFilter.bSearchByGUID)
		{
			for (const auto& Item : Items)
			{
				if (Item && Item->GetGuid() == OptionalFilter.Guid)
				{
					FoundItems.Add(Item);
				}
			}
		}

		return FoundItems;
	}

	return Items;
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryComponent::GetItems_Multithreading(const FItemRetrievalFilter OptionalFilter) const
{
	if (!OptionalFilter.IsValid())
		return TArray<UMounteaInventoryItemBase*>();
	
	const int32 NumChunks = FMath::CeilToInt(static_cast<float>(Items.Num()) / ChunkSize);

	// Thread-safe container to collect items from each thread/chunk
	FCriticalSection CriticalSection;
	TArray<UMounteaInventoryItemBase*> ReturnValues;

	auto SearchFunction = [&](const int32 ChunkIndex)
	{
		const int32 StartIndex = ChunkIndex * ChunkSize;
		const int32 EndIndex = FMath::Min(StartIndex + ChunkSize, Items.Num());
		TArray<UMounteaInventoryItemBase*> TempList;

		for (int32 i = StartIndex; i < EndIndex; i++)
		{
			auto& Item = Items[i];

			// Search by Class
			if (OptionalFilter.bSearchByClass && Item && Item->IsA(OptionalFilter.Class))
			{
				TempList.Add(Item);
			}

			// Search by Item
			/*else if (OptionalFilter.bSearchByItem && Item == OptionalFilter.Item)
			{
				TempList.Add(Item);
			}*/

			// Search by Tag
			else if (OptionalFilter.bSearchByTag && Item && Item->ItemData.RequiredData.ItemFlags.HasAny(OptionalFilter.Tags))
			{
				TempList.Add(Item);
			}

			// Search by GUID
			else if (OptionalFilter.bSearchByGUID && Item && Item->GetGuid() == OptionalFilter.Guid)
			{
				TempList.Add(Item);
			}
		}

		// Add items found in this chunk to the main list
		CriticalSection.Lock();
		ReturnValues.Append(TempList);
		CriticalSection.Unlock();
	};

	ParallelFor(NumChunks, SearchFunction);

	return ReturnValues;
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryComponent::GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaInventoryComponent_GetItems );
	
	if (OptionalFilter.IsValid())
	{
		if (UMounteaInventoryEquipmentBPF::GetSettings()->MultithreadingThreshold < Items.Num())
		{
			return GetItems_Multithreading(OptionalFilter);
		}
		else
		{
			return GetItems_Simple(OptionalFilter);
		}
	}

	return Items;
}

bool UMounteaInventoryComponent::AddOrUpdateItem_Implementation(UMounteaInventoryItemBase* NewItem, const int32& Quantity)
{
	return TryAddItem(NewItem, Quantity);
}

bool UMounteaInventoryComponent::AddItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& NewItems)
{
	bool bSatisfied = true;
	for (const auto& Itr : NewItems)
	{
		if (!Execute_AddOrUpdateItem(this, Itr.Key, Itr.Value))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

bool UMounteaInventoryComponent::AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass, const int32& Quantity)
{
	if (ItemClass == nullptr) return false;

	UMounteaInventoryItemBase* NewItem = NewObject<UMounteaInventoryItemBase>(GetWorld(), ItemClass);
	//NewItem->SetWorld(GetWorld());
	
	return TryAddItem(NewItem, Quantity);
}

bool UMounteaInventoryComponent::AddItemsFromClass_Implementation(TMap<TSubclassOf<UMounteaInventoryItemBase>, int32>& NewItemsClasses)
{
	bool bSatisfied = true;
	for (const auto& Itr : NewItemsClasses)
	{
		if (!Execute_AddItemFromClass(this, Itr.Key, Itr.Value))
		{
			bSatisfied = false;
		}
	}

	return bSatisfied;
}

bool UMounteaInventoryComponent::RemoveItem_Implementation(UMounteaInventoryItemBase* AffectedItem, const int32& Quantity)
{
	return TryRemoveItem(AffectedItem, Quantity);
}

bool UMounteaInventoryComponent::RemoveItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& AffectedItems)
{
	bool bSatisfied = true;
	for (const auto& Itr : AffectedItems)
	{
		if (!Execute_RemoveItem(this, Itr.Key, Itr.Value))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

void UMounteaInventoryComponent::RequestNetworkRefresh_Implementation()
{
	ReplicatedItemsKey++;

	OnRep_Items();
}

void UMounteaInventoryComponent::ProcessItemAction_Implementation(UMounteaInventoryItemAction* Action, UMounteaInventoryItemBase* Item, FMounteaDynamicDelegateContext Context) 
{
	if (!Action)	return;

	if (!Item) return;

	if (!GetOwner()) return;

	Action->InitializeAction(Item, Context);
	
	Action->ProcessAction(Item);
}

void UMounteaInventoryComponent::OnRep_Items()
{
	for (const auto& Itr : InventorySlots)
	{
		if (Itr.Item)
		{
			Itr.Item->SetWorld(GetWorld());

			TScriptInterface<IMounteaInventoryInterface> OwningInterface = this;
			Itr.Item->SetOwningInventory(OwningInterface);
		}
	}

	FInventoryUpdateResult UpdateResult;
	UpdateResult.ResultID = 200;
	UpdateResult.ResultText = LOCTEXT("InventoryNotificationData_Success_Replication", "Inventory Replicated.");
	
	OnInventoryUpdated.Broadcast(UpdateResult);
}

void UMounteaInventoryComponent::TryAddItem_Server_Implementation(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	if (TryAddItem(Item, Quantity))
	{
		//TODO: Item Addeeeeeeeed
		return;
	}
	else
	{
		//TODO: Something is wrong :(
		return;
	}
}

bool UMounteaInventoryComponent::TryAddItem_Server_Validate(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	// There is currently no protection,
	// however, in the future some might be implemented.
	// This function is here just for future proofing.
	return true;
}

void UMounteaInventoryComponent::ReduceItemInInventory_Server_Implementation(UMounteaInstancedItem* Item, const int32& Quantity)
{
	Execute_ReduceItemInInventory(this, Item, Quantity);
}

bool UMounteaInventoryComponent::ReduceItemInInventory_Server_Validate(UMounteaInstancedItem* Item, const int32& Quantity)
{ return true; }

bool UMounteaInventoryComponent::TryAddItem(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	if (!GetOwner()) return false;

	if (!GetOwner()->HasAuthority())
	{
		TryAddItem_Server(Item, Quantity);
		return true;
	}
	
	const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();

	if (!Settings)
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoSettings", "Failed - No Inventory Settings");
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		
		return false;
	}
	
	if (!Item)
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoItem", "Failed - Invalid Item!");
	
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		return false;
	}
	
	if (!GetOwner())
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoOwner", "Failed - Inventory failed to initialize!");
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoAuth", "Failed - Client requests are disabled!");
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		
		return false;
	}
	
	FItemRetrievalFilter Filter;
	{
		//Filter.bSearchByItem = true;
		//Filter.Item = Item;
		//Filter.bSearchByClass = true;
		//Filter.Class = Item->StaticClass();
		//Filter.bSearchByTag = false;
		//Filter.Tag = Item->GetFirstTag();
	}

	bool bSuccess = false;
	if (!Execute_HasItem(this, Filter))
	{
		bSuccess = TryAddItem_NewItem(Item, Quantity);
	}
	else
	{
		FItemRetrievalFilter SearchFilter;
		//SearchFilter.bSearchByItem = true;
		//SearchFilter.Item = Item;
		SearchFilter.bSearchByGUID = true;
		SearchFilter.Guid = Item->GetGuid();
			
		UMounteaInventoryItemBase* ExistingItem = Execute_FindItem(this, SearchFilter);
		
		bSuccess = TryAddItem_UpdateExisting(ExistingItem, Item, Quantity);
	}
	
	return bSuccess;
}

void UMounteaInventoryComponent::TryRemoveItem_Server_Implementation(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	if (TryRemoveItem(Item, Quantity))
	{
		//TODO: Item Addeeeeeeeed
		return;
	}
	else
	{
		//TODO: Something is wrong :(
		return;
	}
}

bool UMounteaInventoryComponent::TryRemoveItem_Server_Validate(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	// There is currently no protection,
	// however, in the future some might be implemented.
	// This function is here just for future proofing.
	return true;
}

bool UMounteaInventoryComponent::TryRemoveItem(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	if (!GetOwner()) return false;

	if (!GetOwner()->HasAuthority())
	{
		TryRemoveItem_Server(Item, Quantity);
		return true;
	}
	
	const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();

	if (!Settings)
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("MounteaInventoryComponent_FailedSettings", "Failed - No Inventory Settings");
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		
		return false;
	}
	
	if (!Item)
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoItem", "Failed - Invalid Item!");
	
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		return false;
	}
	
	if (!GetOwner())
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoOwner", "Failed - Inventory failed to initialize!");
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoAuth", "Failed - Client requests are disabled!");
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		
		return false;
	}
	
	FItemRetrievalFilter Filter;
	{
		//Filter.bSearchByItem = true;
		//Filter.Item = Item;
		Filter.bSearchByGUID = true;
		Filter.Guid = Item->GetGuid();
		Filter.bSearchByTag = true;
		// Filter.Tags = Item->GetTags(); BREAKING
	}
	
	if (!Execute_HasItem(this, Filter))
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;

		UpdateResult.UpdateMessage = LOCTEXT("MounteaInventoryComponent_UpdateItem_NoItem", "Failed - Cannot update Item outside Inventory!");
		OnInventoryUpdated.Broadcast(UpdateResult);*/
		
		return false;
	}

	return RemoveItem_Internal(Item, Quantity);
}

bool UMounteaInventoryComponent::AddItem_Internal(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Items.Add(Item);
		
		const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);

		FOnInventoryUpdated ItemUpdatedResult;
		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_AddItem;
		
		OnItemAdded.Broadcast(Item, ItemUpdatedResult);*/
		ReplicatedItemsKey++;
		OnRep_Items();

		// Item->InitializeNewItem(this); BREAKING
		
		return true;
	}
	
	return false;
}

bool UMounteaInventoryComponent::UpdateItem_Internal(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);

		FOnInventoryUpdated ItemUpdatedResult;
		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_UpdateItem;
		
		OnItemUpdated.Broadcast(Item, ItemUpdatedResult);*/
		ReplicatedItemsKey++;
		OnRep_Items();
		
		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::RemoveItem_Internal(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();

	// Sanity check
	if (!Item)
	{
		/*FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoItem", "Failed - Invalid Item input!");

		OnInventoryUpdated.Broadcast(UpdateResult);*/
		return false;
	}
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FOnInventoryUpdated ItemUpdatedResult;
		
		int32 AmountToRemove = -1;
		if (Quantity == 0) //TODO
		{
			// AmountToRemove = Item->ItemData.RequiredData.ItemQuantity.CurrentQuantity; BREAKING
		}
		else
		{
			AmountToRemove = Quantity;
		}

		AmountToRemove = FMath::Abs(AmountToRemove);
		const int32 MaxToRemove = 0; // Item->ItemData.RequiredData.ItemQuantity.CurrentQuantity; BREAKING
		AmountToRemove =  FMath::Abs(FMath::Min(AmountToRemove, MaxToRemove));

		AmountToRemove = MaxToRemove - AmountToRemove;
		
		// Item->SetQuantity(AmountToRemove); BREAKING
		
		// if (Item->ItemData.RequiredData.ItemQuantity.CurrentQuantity <= 0) BREAKING
		if (Item->ItemData.RequiredData.ItemQuantity.MaxQuantity > 0)
		{
			/*ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_RemovedItem;
			OnItemUpdated.Broadcast(Item, ItemUpdatedResult);
			
			OnItemRemoved.Broadcast(ItemUpdatedResult);
			
			FInventoryUpdateResult UpdateResult;
			UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
			UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Success_RemovedItem", "Inventory Item Removed");

			OnInventoryUpdated.Broadcast(UpdateResult);*/
			
			ReplicatedItemsKey++;
			// Item->NetFlush(); BREAKING
			
			OnRep_Items();

			RemovedItems.Add(Item);
			Items.Remove(Item);

			return true;
		}

		/*ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_UpdateItem;

		OnItemUpdated.Broadcast(Item, ItemUpdatedResult);*/
		ReplicatedItemsKey++;
		OnRep_Items();
		
		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::TryAddItem_NewItem(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	// Sanity check
	if (!Item) return false;

	if (Quantity < 0)
	{
		return TryRemoveItem(Item, Quantity);
	}

	const int32 MaxToAdd = UMounteaInventoryEquipmentBPF::CalculateMaxAddQuantity(Item, nullptr, Quantity);
	// Item->SetQuantity(MaxToAdd); BREAKING
	
	// In this case we actually can just add the item with no further checks
	return AddItem_Internal(Item);
}

bool UMounteaInventoryComponent::TryAddItem_UpdateExisting(UMounteaInventoryItemBase* Existing, UMounteaInventoryItemBase* NewItem, const int32 Quantity)
{
	// Sanity check
	if (!NewItem) return false;
	if (!Existing) return false;

	if (Quantity < 0)
	{
		return TryRemoveItem(Existing, Quantity);
	}

	const int32 MaxAddQuantity = UMounteaInventoryEquipmentBPF::CalculateMaxAddQuantity(Existing, NewItem, Quantity);
	if (MaxAddQuantity == 0) return false;
	
	// Existing->AddQuantity(MaxAddQuantity); BREAKING
	
	return UpdateItem_Internal(Existing);
}

void UMounteaInventoryComponent::PostInventoryUpdated_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	PostInventoryUpdated_Client(UpdateContext);
}

void UMounteaInventoryComponent::PostInventoryUpdated_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	if (!CanExecuteCosmetics()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostInventoryUpdated_Client_RequestUpdate", UpdateContext);
	
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

void UMounteaInventoryComponent::PostItemAdded_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!CanExecuteCosmetics()) return;
	
	if (UpdateContext.OptionalPayload)
	{
		// Item->GetItemAddedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful); BREAKING

		PostItemAdded_Client(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemRemoved_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!CanExecuteCosmetics()) return;
	
	if (UpdateContext.OptionalPayload)
	{
		// Item->GetItemUpdatedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful); BREAKING

		PostItemRemoved_Client(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemUpdated_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!CanExecuteCosmetics()) return;
	
	if (UpdateContext.OptionalPayload)
	{
		// Item->GetItemUpdatedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful); BREAKING

		PostItemUpdated_Client(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemAdded_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
 	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	if (!CanExecuteCosmetics()) return;

	// Item->InitializeNewItem(this); BREAKING
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestItemSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemAdded_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestItemSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemRemoved_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;

	if (!CanExecuteCosmetics()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestItemSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemRemoved_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestItemSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemUpdated_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;

	if (!CanExecuteCosmetics()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestItemSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemUpdated_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestItemSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemAdded_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);

	// Item->InitializeNewItem(this); BREAKING
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets, UpdateContext.OptionalPayload);
		
		RequestItemNotification(UpdateContext);
		
		// Item->GetItemAddedHandle().Broadcast(UpdateContext.UpdateMessage.ToString()); BREAKING
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

		RequestItemNotification(UpdateContext);

		// Item->GetItemRemovedHandle().Broadcast(UpdateContext.UpdateMessage.ToString()); BREAKING
		OnItemRemoved_Client.Broadcast(UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);
	
	// Item->InitializeNewItem(this); BREAKING
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets, UpdateContext.OptionalPayload);
		
		RequestItemNotification(UpdateContext);
		
		// Item->GetItemUpdatedHandle().Broadcast(UpdateContext.UpdateMessage.ToString()); BREAKING
		OnItemUpdated_Client.Broadcast(UpdateContext);
	}
}

void UMounteaInventoryComponent::RequestInventoryNotification(const FInventoryUpdateResult& UpdateContext) const
{
	const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		
	FInventoryNotificationData Data; // = *Settings->InventoryUpdateData.Find(UpdateContext.InventoryUpdateResult); BREAKING
	if (Data.Weight >= Settings->MinDisplayWeight)
	{
		if (!UpdateContext.ResultText.IsEmpty())
		{
			// Data.NotificationText = UpdateContext.UpdateMessage;
		}

		IMounteaInventoryWBPInterface::Execute_CreateInventoryNotification(InventoryWBP, Data);
	}
}

void UMounteaInventoryComponent::RequestItemNotification(const FInventoryUpdateResult& UpdateContext) const
{
	const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
	FInventoryNotificationData Data; // = *Settings->ItemUpdateData.Find(UpdateContext.ItemUpdateResult); BREAKING
	if (!UpdateContext.ResultText.IsEmpty())
	{
		// Data.NotificationText = UpdateContext.UpdateMessage;
	}

	IMounteaInventoryWBPInterface::Execute_CreateInventoryNotification(InventoryWBP, Data);
}

void UMounteaInventoryComponent::ClientRefreshInventory_Implementation()
{
	ReplicatedItemsKey++;
}


#undef LOCTEXT_NAMESPACE
