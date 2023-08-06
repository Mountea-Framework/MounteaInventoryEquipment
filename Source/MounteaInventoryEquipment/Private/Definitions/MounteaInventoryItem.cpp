// All rights reserved Dominik Pavlicek 2023.


#include "Definitions/MounteaInventoryItem.h"

#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Net/UnrealNetwork.h"

#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Helpers/MounteaInventoryEquipmentBPF.h"


UMounteaInventoryItemBase::UMounteaInventoryItemBase()
{
	RepKey = 0;

	FWorldDelegates::OnPostWorldCreation.AddUObject(this, &UMounteaInventoryItemBase::PostWorldCreated);
}

UMounteaInventoryItemBase::~UMounteaInventoryItemBase()
{
	FWorldDelegates::OnPostWorldCreation.RemoveAll(this);
}

void UMounteaInventoryItemBase::PostWorldCreated(UWorld* NewWorld)
{
	if (FApp::IsGame())
	{
		SetWorld(NewWorld);

		OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemAdded);
		OnItemInitialized.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemInitialized);
		OnItemModified.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemModified);
		OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemRemoved);
		
		SetValidData();

		EnsureValidConfig();
		InitializeItemActions();

		OnItemBeginPlay(TEXT("Item has been initialized"));
	}
}

void UMounteaInventoryItemBase::PostInitProperties()
{
	UDataAsset::PostInitProperties();

	bool bIsEditorNoPlay = false;
#if WITH_EDITOR
	bIsEditorNoPlay = UMounteaInventoryEquipmentBPF::IsEditorNoPlay();
#endif
	
	if (bIsEditorNoPlay) // This code gets executed only when opening new Asset in Editor
	{
		if (ItemDataSource == EItemDataSource::EIDS_SourceTable && SourceTable == nullptr)
		{
			SourceTable = UMounteaInventoryEquipmentBPF::GetDefaultItemsTable();
		}

		EnsureValidConfig();
	}
}

TArray<UMounteaInventoryItemAction*> UMounteaInventoryItemBase::GetItemActions() const
{
	TArray<UMounteaInventoryItemAction*> ReturnValues;
	for (const auto& Itr : ItemActions)
	{
		if (Itr.ItemAction && !ReturnValues.Contains(Itr.ItemAction))
		{
			ReturnValues.Add(Itr.ItemAction);
		}
	}

	return ReturnValues;
}

bool UMounteaInventoryItemBase::IsValid(UObject* WorldContextObject) const
{
	return GetWorld() ? true : false;
}

void UMounteaInventoryItemBase::AddQuantity(const int32 Amount)
{
	const int32 Current = ItemData.ItemQuantity.CurrentQuantity;
	const int32 Requested = Current + Amount;

	if (Current == Requested) return;
	
	const int32 Max = ItemData.ItemQuantity.bIsStackable ? ItemData.ItemQuantity.MaxQuantity : 1;
	
	ItemData.ItemQuantity.CurrentQuantity = FMath::Min(Requested, Max);

	OnItemModified.Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::ItemNotifications::ItemUpdated);
}

void UMounteaInventoryItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity == ItemData.ItemQuantity.CurrentQuantity) return;
	
	ItemData.ItemQuantity.CurrentQuantity = FMath::Min(ItemData.ItemQuantity.MaxQuantity, NewQuantity);

	OnItemModified.Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::ItemNotifications::ItemUpdated);
}

int32 UMounteaInventoryItemBase::GetQuantity() const
{
	return ItemData.ItemQuantity.CurrentQuantity;
}

bool UMounteaInventoryItemBase::OwnerHasAuthority() const
{
	if (!OwningInventory)
	{
		if (!GEngine) return false;
		if (!World) return false;

		const FString ModeName = GetEnumValueAsString(TEXT("ENetRole"), GEngine->GetNetMode(World));

		UE_LOG(LogTemp, Error, TEXT("%s"), *ModeName)
		if (GEngine->GetNetMode(World) == NM_Client)
		{
			return false;
		}

		return true;
	}

	return OwningInventory->Execute_DoesHaveAuthority(OwningInventory.GetObject());
}

void UMounteaInventoryItemBase::SetWorldFromLevel(ULevel* FromLevel)
{
	if (FromLevel == nullptr)
	{
		return;
	}
	
	if (FromLevel->GetWorld() != World)
	{
		SetWorld(FromLevel->GetWorld());
	}
}

void UMounteaInventoryItemBase::InitializeItemActions()
{
	TArray<UMounteaInventoryItemAction*> Actions = GetItemActions();
	for (UMounteaInventoryItemAction* ItrAction : Actions)
	{
		FMounteaDynamicDelegateContext Context;
		Context.Command = MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemActionCommands::InitializeAction;
		Context.Payload = this;
		ItrAction->InitializeAction(this, Context);
	}
}

void UMounteaInventoryItemBase::SetWorld(UWorld* NewWorld)
{
	World = NewWorld;

	InitializeItemActions();
}

void UMounteaInventoryItemBase::InitializeNewItem(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory)
{
	if (NewOwningInventory.GetObject() == nullptr)
	{
		return;
	}

	if (const AActor* InventoryOwner = NewOwningInventory->Execute_GetOwningActor(NewOwningInventory.GetObject()))
	{
		World = InventoryOwner->GetWorld();
	}
	else
	{
		World = NewOwningInventory.GetObject()->GetWorld();
	}
	
	OwningInventory = NewOwningInventory;
	
	MarkDirtyForReplication();

	FString Message = ItemData.ItemName.ToString();
	Message.Append(": Initialization completed.");
	OnItemInitialized.Broadcast(Message);
}

void UMounteaInventoryItemBase::OnRep_Item()
{
	if (OwningInventory.GetObject() == nullptr) return;
	
	FString Message = ItemData.ItemName.ToString();
	Message.Append(" has been modified.");
		
	if (const AActor* InventoryOwner = OwningInventory->Execute_GetOwningActor(OwningInventory.GetObject()))
	{
		World = InventoryOwner->GetWorld();
	}
	else
	{
		World = OwningInventory.GetObject()->GetWorld();
	}
	
	OnItemModified.Broadcast(Message);
}

void UMounteaInventoryItemBase::OnItemBeginPlay_Implementation(const FString& Message)
{
	OnItemInitialized.Broadcast(Message);
}

void UMounteaInventoryItemBase::ItemAdded(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::ItemInitialized(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::ItemModified(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::ItemRemoved(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryItemBase, ItemData);
	DOREPLIFETIME(UMounteaInventoryItemBase, ItemDataSource);
	DOREPLIFETIME(UMounteaInventoryItemBase, OwningInventory);
	DOREPLIFETIME_CONDITION(UMounteaInventoryItemBase, World, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInventoryItemBase, ItemActions, COND_InitialOnly);
	DOREPLIFETIME(UMounteaInventoryItemBase, ItemConfig);
}

bool UMounteaInventoryItemBase::IsSupportedForNetworking() const
{
	return true;
}

void UMounteaInventoryItemBase::MarkDirtyForReplication()
{
	RepKey++;
	
	if (OwningInventory.GetInterface())
	{
		OwningInventory->Execute_RequestNetworkRefresh(OwningInventory.GetObject());
	}
}

void UMounteaInventoryItemBase::ClearDataTable()
{
	SourceTable = nullptr;
	SourceRow = FName();
}

void UMounteaInventoryItemBase::CopyFromTable()
{
	if (SourceTable == nullptr)
	{
		ClearDataTable();
		return;
	}
	
	FString Context;
	if (const FMounteaInventoryItemData* Row = GetRow<FMounteaInventoryItemData>(SourceRow, SourceTable))
	{
		ItemData = Row->RequiredData;
		ItemOptionalData = Row->OptionalData;
	}
	else
	{
		//TODO: Show error that no valid data found
		
		ItemData = FMounteaInventoryItemRequiredData();
		ItemOptionalData = FMounteaInventoryItemOptionalData();
	}
}

void UMounteaInventoryItemBase::SetValidData()
{
	switch (ItemDataSource)
	{
		case EItemDataSource::EIDS_SourceTable:
			ClearMappedValues();
			CopyFromTable();
			CopyTagsFromTypes();
			break;
		case EItemDataSource::EIDS_ManualInput:
			ClearDataTable();
			CopyTagsFromTypes();
			break;
		case EItemDataSource::Default:
		default: break;
	}
}

void UMounteaInventoryItemBase::ClearMappedValues()
{
	ItemData = FMounteaInventoryItemRequiredData();
	ItemOptionalData = FMounteaInventoryItemOptionalData();
}

void UMounteaInventoryItemBase::CopyTagsFromTypes()
{
	if (ItemData.ItemCategory)
	{
		ItemData.ItemFlags.AppendTags(ItemData.ItemCategory->CompatibleGameplayTags);
	}

	if (ItemData.ItemRarity)
	{
		ItemData.ItemFlags.AddTag(ItemData.ItemRarity->RarityGameplayTag);
	}
}

void UMounteaInventoryItemBase::EnsureValidConfig()
{
	if (ItemConfig.ItemConfig == nullptr)
	{
		bool bFound = false;
		const TSubclassOf<UMounteaInventoryItemConfig> Class = UMounteaInventoryEquipmentBPF::GetSettings()->DefaultItemConfigClass.LoadSynchronous();
		ItemConfig.ItemConfig = UMounteaInventoryEquipmentBPF::GetItemConfig(this, Class, bFound);
	}
	
	// Copy Actions Categories
	{
		TArray<FMounteaItemAction> CurrentActions = GetItemActionsDefinitions();
		ItemActions.Empty();

		if (ItemData.ItemCategory)
		{
			TArray<FMounteaItemAction> CategoryActions = ItemData.ItemCategory->GetCategoryActionsDefinitions();
			
			for (const FMounteaItemAction ItrCategoryAction: CategoryActions)
			{
				if (!ItrCategoryAction.ItemAction) continue;
				if (CurrentActions.Contains(ItrCategoryAction)) continue;

				const UMounteaInventoryItemAction* SourceAction = ItrCategoryAction.ItemAction; 
				UMounteaInventoryItemAction* NewAction = NewObject<UMounteaInventoryItemAction>(GetPackage(), SourceAction->GetClass());
				NewAction->CopyFromOther(ItrCategoryAction.ItemAction);

				FMounteaItemAction NewActionDefinition;
				NewActionDefinition.ItemAction = NewAction;
				
				CurrentActions.Add(NewActionDefinition);
			}
		}

		ItemActions = CurrentActions;
	}
}

#if WITH_EDITOR

void UMounteaInventoryItemBase::SetValidDataEditor()
{
	EnsureValidConfig();
	SetValidData();
}

void UMounteaInventoryItemBase::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);

	EnsureValidConfig();
	SetValidData();
	
	ItemGuid = FGuid::NewGuid();
}

void UMounteaInventoryItemBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName().ToString() == TEXT("bIsStackable"))
	{
		if (!ItemData.ItemQuantity.bIsStackable)
		{
			ItemData.ItemQuantity.MaxQuantity = 1;
		}
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, ItemDataSource))
	{
		SetValidData();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, SourceTable))
	{
		SourceRow = FName();
		SetValidData();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, SourceRow))
	{
		SetValidData();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, ItemConfig))
	{
		EnsureValidConfig();
	}
}
#endif