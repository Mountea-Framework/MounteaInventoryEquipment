// All rights reserved Dominik Pavlicek 2023.


#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Engine/ActorChannel.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Interfaces/MounteaInventoryWBPInterface.h"

#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryComponent"

UMounteaInventoryComponent::UMounteaInventoryComponent()
{	
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
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

	DOREPLIFETIME(UMounteaInventoryComponent, Items);
	DOREPLIFETIME(UMounteaInventoryComponent, InventoryConfig);
	DOREPLIFETIME(UMounteaInventoryComponent, OtherInventory);
}

bool UMounteaInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bUpdated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	//Check if the array of items needs to replicate
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		TArray<UMounteaInventoryItemBase*> AllItems = Items;
		AllItems.Append(RemovedItems);
		
		for (const auto& Item : AllItems)
		{
			if (Item && Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->GetRepKey()))
			{
				bUpdated |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}

		RemovedItems.Empty();

		if (Channel->KeyNeedsToReplicate(InventoryConfig.MounteaInventoryConfig->GetUniqueID(), InventoryConfig.MounteaInventoryConfig->GetRepKey()))
		{
			bUpdated |= Channel->ReplicateSubobject(InventoryConfig.MounteaInventoryConfig, *Bunch, *RepFlags);
		}
	}

	return bUpdated;
}

TSubclassOf<UMounteaBaseUserWidget> UMounteaInventoryComponent::GetInventoryWBPClass_Implementation()
{
	return InventoryWBPClass;
}

UMounteaBaseUserWidget* UMounteaInventoryComponent::GetInventoryWBP_Implementation()
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
	if (SearchFilter.IsValid())
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
				if (Itr && Itr->ItemData.ItemFlags.HasAny(SearchFilter.Tags))
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
				if (Itr && Itr->GetItemGuid() == SearchFilter.Guid)
				{
					return true;
				}
			}
		}

		return false;
	}
	
	return false;
}

bool UMounteaInventoryComponent::HasItem_Multithreading(const FItemRetrievalFilter& SearchFilter) const
{
	if (SearchFilter.IsValid())
	{
		int32 ThreadsPerFilter = UMounteaInventoryEquipmentBPF::GetValidFiltersCount(SearchFilter);
		const int32 MinThreads = UMounteaInventoryEquipmentBPF::GetSettings()->ThreadsLimit;
		const int32 NumThreads = FMath::Min( FPlatformMisc::NumberOfWorkerThreadsToSpawn(), MinThreads);

		ThreadsPerFilter = FMath::Max( NumThreads/ThreadsPerFilter, 1);
			
		TArray<FRunnableThread*> Threads;
		TAtomic<bool> ItemFound(false);

		if (SearchFilter.bSearchByClass)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByTag = false;
			SpecificFilter.bSearchByGUID = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemSearchRunnable* SearchRunnable = new FItemSearchRunnable(Items, SpecificFilter, ItemFound);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("HasItemRunnable_ClassSearch"));
				Threads.Add(Thread);
			}
		}
		if (SearchFilter.bSearchByItem)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByTag = false;
			SpecificFilter.bSearchByGUID = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemSearchRunnable* SearchRunnable = new FItemSearchRunnable(Items, SpecificFilter, ItemFound);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("HasItemRunnable_ItemSearch"));
				Threads.Add(Thread);
			}
		}
		if (SearchFilter.bSearchByTag)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByGUID = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemSearchRunnable* SearchRunnable = new FItemSearchRunnable(Items, SpecificFilter, ItemFound);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("HasItemRunnable_TagSearch"));
				Threads.Add(Thread);
			}
		}
		if (SearchFilter.bSearchByGUID)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByTag = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemSearchRunnable* SearchRunnable = new FItemSearchRunnable(Items, SpecificFilter, ItemFound);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("HasItemRunnable_GUIDSearch"));
				Threads.Add(Thread);
			}
		}

		for (FRunnableThread* Thread : Threads)
		{
			if (Thread)
			{
				Thread->WaitForCompletion();
				delete Thread;
			}
		}

		return ItemFound.Load();
	}
	
	return false;
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
			if (Items.Contains(SearchFilter.Item))
			{
				return SearchFilter.Item;
			}
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
				if (Itr && Itr->ItemData.ItemFlags.HasAny(SearchFilter.Tags))
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
				if (Itr && Itr->GetItemGuid() == SearchFilter.Guid)
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
	if (SearchFilter.IsValid())
	{
		int32 ThreadsPerFilter = UMounteaInventoryEquipmentBPF::GetValidFiltersCount(SearchFilter);
		const int32 MinThreads = UMounteaInventoryEquipmentBPF::GetSettings()->ThreadsLimit;
		const int32 NumThreads = FMath::Min( FPlatformMisc::NumberOfWorkerThreadsToSpawn(), MinThreads);

		ThreadsPerFilter = FMath::Max( NumThreads/ThreadsPerFilter, 1);
			
		TArray<FRunnableThread*> Threads;
		TAtomic<UMounteaInventoryItemBase*> FoundItem(nullptr);

		if (SearchFilter.bSearchByClass)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByTag = false;
			SpecificFilter.bSearchByGUID = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemGetRunnable* SearchRunnable = new FItemGetRunnable(Items, SpecificFilter, FoundItem);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemGetRunnable_ClassSearch"));
				Threads.Add(Thread);
			}
		}
		if (SearchFilter.bSearchByItem)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByTag = false;
			SpecificFilter.bSearchByGUID = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemGetRunnable* SearchRunnable = new FItemGetRunnable(Items, SpecificFilter, FoundItem);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemGetRunnable_ItemSearch"));
				Threads.Add(Thread);
			}
		}
		if (SearchFilter.bSearchByTag)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByGUID = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemGetRunnable* SearchRunnable = new FItemGetRunnable(Items, SpecificFilter, FoundItem);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemGetRunnable_TagSearch"));
				Threads.Add(Thread);
			}
		}
		if (SearchFilter.bSearchByGUID)
		{
			FItemRetrievalFilter SpecificFilter = SearchFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByTag = false;

			for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
			{
				FItemGetRunnable* SearchRunnable = new FItemGetRunnable(Items, SpecificFilter, FoundItem);
				FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemGetRunnable_GUIDSearch"));
				Threads.Add(Thread);
			}
		}

		for (FRunnableThread* Thread : Threads)
		{
			if (Thread)
			{
				Thread->WaitForCompletion();
				delete Thread;
			}
		}

		return FoundItem.Load();
	}

	return nullptr;
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
				if (Item == OptionalFilter.Item)
				{
					FoundItems.Add(Item);
				}
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
				if (Item && Item->ItemData.ItemFlags.HasAny(OptionalFilter.Tags))
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
				if (Item && Item->GetItemGuid() == OptionalFilter.Guid)
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
	int32 ThreadsPerFilter = UMounteaInventoryEquipmentBPF::GetValidFiltersCount(OptionalFilter);
	const int32 MinThreads = UMounteaInventoryEquipmentBPF::GetSettings()->ThreadsLimit;
	const int32 NumThreads = FMath::Min( FPlatformMisc::NumberOfWorkerThreadsToSpawn(), MinThreads);

	ThreadsPerFilter = FMath::Max( NumThreads/ThreadsPerFilter, 1);
			
	TArray<FRunnableThread*> Threads;
	TArray<UMounteaInventoryItemBase*> ReturnValues;

	// Make runnable for each filter task. Each thread will iterate over only 1 filter.
	if (OptionalFilter.bSearchByClass)
	{
		// Create and run the search runnables on separate threads
		for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
		{
			FItemRetrievalFilter SpecificFilter = OptionalFilter;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByTag = false;
			SpecificFilter.bSearchByGUID = false;
					
			FItemsGetRunnable* SearchRunnable = new FItemsGetRunnable(Items, SpecificFilter, ReturnValues);
			FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemsGetRunnable_ClassSearch"), 0, TPri_BelowNormal);
			Threads.Add(Thread);
		}
	}
	if (OptionalFilter.bSearchByItem)
	{
		// Create and run the search runnables on separate threads
		for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
		{
			FItemRetrievalFilter SpecificFilter = OptionalFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByTag = false;
			SpecificFilter.bSearchByGUID = false;
					
			FItemsGetRunnable* SearchRunnable = new FItemsGetRunnable(Items, SpecificFilter, ReturnValues);
			FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemsGetRunnable_ItemSearch"), 0, TPri_BelowNormal);
			Threads.Add(Thread);
		}
	}
	if (OptionalFilter.bSearchByTag)
	{
		// Create and run the search runnables on separate threads
		for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
		{
			FItemRetrievalFilter SpecificFilter = OptionalFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByGUID = false;
					
			FItemsGetRunnable* SearchRunnable = new FItemsGetRunnable(Items, SpecificFilter, ReturnValues);
			FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemsGetRunnable_TagSearch"), 0, TPri_BelowNormal);
			Threads.Add(Thread);
		}
	}
	if (OptionalFilter.bSearchByGUID)
	{
		// Create and run the search runnables on separate threads
		for (int32 ThreadIndex = 0; ThreadIndex < ThreadsPerFilter; ++ThreadIndex)
		{
			FItemRetrievalFilter SpecificFilter = OptionalFilter;
			SpecificFilter.bSearchByClass = false;
			SpecificFilter.bSearchByItem = false;
			SpecificFilter.bSearchByTag = false;
					
			FItemsGetRunnable* SearchRunnable = new FItemsGetRunnable(Items, SpecificFilter, ReturnValues);
			FRunnableThread* Thread = FRunnableThread::Create(SearchRunnable, TEXT("ItemsGetRunnable_GUIDSearch"), 0, TPri_Lowest);
			Threads.Add(Thread);
		}
	}

	// Wait for the threads to complete
	for (FRunnableThread* Thread : Threads)
	{
		if (Thread)
		{
			Thread->WaitForCompletion();
			delete Thread;
		}
	}

	// Collect the found items from the atomic variables
	return ReturnValues;
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

AActor* UMounteaInventoryComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

void UMounteaInventoryComponent::SetInventoryWBPClass_Implementation(TSubclassOf<UMounteaBaseUserWidget> NewInventoryWBPClass)
{
	InventoryWBPClass = NewInventoryWBPClass;
}

void UMounteaInventoryComponent::SetInventoryWBP_Implementation(UMounteaBaseUserWidget* NewWBP)
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
	for (const auto& Itr : Items)
	{
		if (Itr)
		{
			Itr->SetWorld(GetWorld());
		}
	}

	FInventoryUpdateResult UpdateResult;
	UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
	UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Success_Replication", "Inventory Replicated.");
	
	OnInventoryUpdated.Broadcast(UpdateResult);
}

void UMounteaInventoryComponent::OnRep_OtherInventory()
{
	// Call request to update Inventory Flags
	Execute_SetInventoryFlags(this);
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

TScriptInterface<IMounteaInventoryInterface> UMounteaInventoryComponent::GetOtherInventory_Implementation() const
{
	return OtherInventory;
}

void UMounteaInventoryComponent::SetOtherInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewInventory)
{
	if (!GetOwner()) return;


	if (!GetOwner()->HasAuthority())
	{
		SetOtherInventory_Server(NewInventory);
	}
}

bool UMounteaInventoryComponent::SetInventoryFlags_Implementation()
{
	if (!InventoryConfig.MounteaInventoryConfig) return false;
	
	if (OtherInventory.GetObject())
	{
		//TODO:
		// Get all compatible tags of the Current Inventory
		// Find if the Other has any of current one
		// Find if Current one has any of the Other
		// If match, update both
		// If not, return false
	}
	else
	{
		InventoryConfig.MounteaInventoryConfig->ResetFlags();
	}
	return true;
}

bool UMounteaInventoryComponent::DoesHaveAuthority_Implementation() const
{
	if (GetOwner()==nullptr) return false;
	return GetOwner()->HasAuthority();
}

void UMounteaInventoryComponent::SetOtherInventory_Server_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewInventory)
{
	if (NewInventory != OtherInventory)
	{
		OtherInventory = NewInventory;

		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Success_OtherInventory", "Other Inventory Updated.");
		
		OnInventoryUpdated.Broadcast(UpdateResult);
	}
}

bool UMounteaInventoryComponent::SetOtherInventory_Server_Validate(const TScriptInterface<IMounteaInventoryInterface>& NewInventory)
{
	return true;
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
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoSettings", "Failed - No Inventory Settings");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	if (!Item)
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoItem", "Failed - Invalid Item!");
	
		OnInventoryUpdated.Broadcast(UpdateResult);
		return false;
	}
	
	if (!GetOwner())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoOwner", "Failed - Inventory failed to initialize!");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoAuth", "Failed - Client requests are disabled!");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByItem = true;
		Filter.Item = Item;
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
		SearchFilter.bSearchByItem = true;
		SearchFilter.Item = Item;
		SearchFilter.bSearchByGUID = true;
		SearchFilter.Guid = Item->GetItemGuid();
			
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
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("MounteaInventoryComponent_FailedSettings", "Failed - No Inventory Settings");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	if (!Item)
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoItem", "Failed - Invalid Item!");
	
		OnInventoryUpdated.Broadcast(UpdateResult);
		return false;
	}
	
	if (!GetOwner())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoOwner", "Failed - Inventory failed to initialize!");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoAuth", "Failed - Client requests are disabled!");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByItem = true;
		Filter.Item = Item;
		Filter.bSearchByGUID = true;
		Filter.Guid = Item->GetItemGuid();
		Filter.bSearchByTag = true;
		Filter.Tags = Item->GetTags();
	}
	
	if (!Execute_HasItem(this, Filter))
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;

		UpdateResult.UpdateMessage = LOCTEXT("MounteaInventoryComponent_UpdateItem_NoItem", "Failed - Cannot update Item outside Inventory!");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
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
		
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);

		FItemUpdateResult ItemUpdatedResult;
		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_AddItem;
		
		OnItemAdded.Broadcast(Item, ItemUpdatedResult);
		ReplicatedItemsKey++;
		OnRep_Items();

		Item->InitializeNewItem(this);
		
		return true;
	}
	
	return false;
}

bool UMounteaInventoryComponent::UpdateItem_Internal(UMounteaInventoryItemBase* Item, const int32 Quantity)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);

		FItemUpdateResult ItemUpdatedResult;
		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_UpdateItem;
		
		OnItemUpdated.Broadcast(Item, ItemUpdatedResult);
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
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Failed_NoItem", "Failed - Invalid Item input!");

		OnInventoryUpdated.Broadcast(UpdateResult);
		return false;
	}
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FItemUpdateResult ItemUpdatedResult;
		
		int32 AmountToRemove = -1;
		if (Quantity == 0) //TODO
		{
			AmountToRemove = Item->ItemData.ItemQuantity.CurrentQuantity;
		}
		else
		{
			AmountToRemove = Quantity;
		}

		AmountToRemove = FMath::Abs(AmountToRemove);
		const int32 MaxToRemove = Item->ItemData.ItemQuantity.CurrentQuantity;
		AmountToRemove =  FMath::Abs(FMath::Min(AmountToRemove, MaxToRemove));

		AmountToRemove = MaxToRemove - AmountToRemove;
		
		Item->SetQuantity(AmountToRemove);
		
		if (Item->ItemData.ItemQuantity.CurrentQuantity <= 0)
		{
			ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_RemovedItem;
			OnItemUpdated.Broadcast(Item, ItemUpdatedResult);
			
			OnItemRemoved.Broadcast(Item, ItemUpdatedResult);
			
			FInventoryUpdateResult UpdateResult;
			UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIUR_Success;
			UpdateResult.UpdateMessage = LOCTEXT("InventoryNotificationData_Success_RemovedItem", "Inventory Item Removed");

			OnInventoryUpdated.Broadcast(UpdateResult);
			
			ReplicatedItemsKey++;
			Item->NetFlush();
			
			OnRep_Items();

			RemovedItems.Add(Item);
			Items.Remove(Item);

			return true;
		}

		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIUR_Success_UpdateItem;

		OnItemUpdated.Broadcast(Item, ItemUpdatedResult);
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
	Item->SetQuantity(MaxToAdd);
	
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
	
	Existing->AddQuantity(MaxAddQuantity);
	
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

void UMounteaInventoryComponent::PostItemAdded_Implementation(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!CanExecuteCosmetics()) return;
	
	if (Item)
	{
		Item->GetItemAddedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful);

		PostItemAdded_Client(Item, UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemRemoved_Implementation(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!CanExecuteCosmetics()) return;
	
	if (Item)
	{
		Item->GetItemUpdatedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful);

		PostItemRemoved_Client(Item, UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemUpdated_Implementation(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!CanExecuteCosmetics()) return;
	
	if (Item)
	{
		Item->GetItemUpdatedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful);

		PostItemUpdated_Client(Item, UpdateContext);
	}
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

void UMounteaInventoryComponent::PostItemAdded_Client_Implementation(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
 	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!Item) return;
	
	if (!CanExecuteCosmetics()) return;

	Item->InitializeNewItem(this);
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestItemSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemAdded_Client_RequestUpdate", Item, UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestItemSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemRemoved_Client_Implementation(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;

	if (!CanExecuteCosmetics()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestItemSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemRemoved_Client_RequestUpdate", Item, UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestItemSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemUpdated_Client_Implementation(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;

	if (!CanExecuteCosmetics()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestItemSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemUpdated_Client_RequestUpdate", Item, UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestItemSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostItemAdded_Client_RequestUpdate(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!Item) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);

	Item->InitializeNewItem(this);
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets, Item);
		
		RequestItemNotification(UpdateContext);
		
		Item->GetItemAddedHandle().Broadcast(UpdateContext.UpdateMessage.ToString());
		OnItemAdded_Client.Broadcast(Item, UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemRemoved_Client_RequestUpdate(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!Item) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RemoveItemWidget, Item);

		RequestItemNotification(UpdateContext);

		Item->GetItemRemovedHandle().Broadcast(UpdateContext.UpdateMessage.ToString());
		OnItemRemoved_Client.Broadcast(Item, UpdateContext);
	}
}

void UMounteaInventoryComponent::PostItemUpdated_Client_RequestUpdate(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!Item) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestInventorySyncTimerHandle);
	
	Item->InitializeNewItem(this);
	
	if (InventoryWBP)
	{
		InventoryWBP->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets, Item);
		
		RequestItemNotification(UpdateContext);
		
		Item->GetItemUpdatedHandle().Broadcast(UpdateContext.UpdateMessage.ToString());
		OnItemUpdated_Client.Broadcast(Item, UpdateContext);
	}
}

void UMounteaInventoryComponent::RequestInventoryNotification(const FInventoryUpdateResult& UpdateContext) const
{
	const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		
	FInventoryNotificationData Data = *Settings->InventoryUpdateData.Find(UpdateContext.InventoryUpdateResult);
	if (Data.Weight >= Settings->MinDisplayWeight)
	{
		if (!UpdateContext.UpdateMessage.IsEmpty())
		{
			Data.NotificationText = UpdateContext.UpdateMessage;
		}

		IMounteaInventoryWBPInterface::Execute_CreateInventoryNotification(InventoryWBP, Data);
	}
}

void UMounteaInventoryComponent::RequestItemNotification(const FItemUpdateResult& UpdateContext) const
{
	const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
	FInventoryNotificationData Data = *Settings->ItemUpdateData.Find(UpdateContext.ItemUpdateResult);
	if (!UpdateContext.UpdateMessage.IsEmpty())
	{
		Data.NotificationText = UpdateContext.UpdateMessage;
	}

	IMounteaInventoryWBPInterface::Execute_CreateInventoryNotification(InventoryWBP, Data);
}

void UMounteaInventoryComponent::ClientRefreshInventory_Implementation()
{
	ReplicatedItemsKey++;
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
			InventoryConfig.MounteaInventoryConfig = UMounteaInventoryEquipmentBPF::GetInventoryConfig(this, Class, bFound);
		}
	}
}

#endif

#undef LOCTEXT_NAMESPACE

uint32 FItemSearchRunnable::Run()
{
	// Search by Item
	if (SearchFilter.bSearchByItem)
	{
		if (Items.Contains(SearchFilter.Item))
		{
			ItemFound = true;
			return 1;
		}
	}

	// Search by Class
	if (SearchFilter.bSearchByClass)
	{
		for (const auto& Itr : Items)
		{
			if (Itr && Itr->IsA(SearchFilter.Class))
			{
				ItemFound = true;
				return 1;
			}
		}
	}

	// Search by Tag
	if (SearchFilter.bSearchByTag)
	{
		for (const auto& Itr : Items)
		{
			if (Itr && Itr->ItemData.ItemFlags.HasAny(SearchFilter.Tags))
			{
				ItemFound = true;
				return 1;
			}
		}
	}

	// Search by GUID
	if (SearchFilter.bSearchByGUID)
	{
		for (const auto& Itr : Items)
		{
			if (Itr && Itr->GetItemGuid() == SearchFilter.Guid)
			{
				ItemFound = true;
				return 1;
			}
		}
	}

	return 0;
}

uint32 FItemGetRunnable::Run()
{
	// Search by Item
	if (SearchFilter.bSearchByItem)
	{
		if (Items.Contains(SearchFilter.Item))
		{
			FoundItem = SearchFilter.Item;
			ItemFound = true;
			return 1;
		}
	}

	// Search by Class
	if (SearchFilter.bSearchByClass)
	{
		for (const auto& Itr : Items)
		{
			if (Itr && Itr->IsA(SearchFilter.Class))
			{
				FoundItem = Itr;
				ItemFound = true;
				return 1;
			}
		}
	}

	// Search by Tag
	if (SearchFilter.bSearchByTag)
	{
		for (const auto& Itr : Items)
		{
			if (Itr && Itr->ItemData.ItemFlags.HasAny(SearchFilter.Tags))
			{
				FoundItem = Itr;
				ItemFound = true;
				return 1;
			}
		}
	}

	// Search by GUID
	if (SearchFilter.bSearchByGUID)
	{
		for (const auto& Itr : Items)
		{
			if (Itr && Itr->GetItemGuid() == SearchFilter.Guid)
			{
				FoundItem = Itr;
				ItemFound = true;
				return 1;
			}
		}
	}

	return 0;
}

uint32 FItemsGetRunnable::Run()
{
	// Search by Item
	if (SearchFilter.bSearchByItem)
	{
		for (const auto& Item : Items)
		{
			if (Item == SearchFilter.Item)
			{
				FoundItems.Add(Item);
				ItemFound = true;
			}
		}
	}

	// Search by Class
	if (SearchFilter.bSearchByClass)
	{
		for (const auto& Item : Items)
		{
			if (Item && Item->IsA(SearchFilter.Class))
			{
				FoundItems.Add(Item);
				ItemFound = true;
			}
		}
	}

	// Search by Tag
	if (SearchFilter.bSearchByTag)
	{
		for (const auto& Item : Items)
		{
			if (Item && Item->ItemData.ItemFlags.HasAny(SearchFilter.Tags))
			{
				FoundItems.Add(Item);
				ItemFound = true;
			}
		}
	}

	// Search by GUID
	if (SearchFilter.bSearchByGUID)
	{
		for (const auto& Item : Items)
		{
			if (Item && Item->GetItemGuid() == SearchFilter.Guid)
			{
				FoundItems.Add(Item);
				ItemFound = true;
			}
		}
	}

	return ItemFound ? 1 : 0;
}
