// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Definitions/MounteaInventoryItem.h"

#include "Definitions/MounteaInventoryItemCategory.h"
#include "Definitions/MounteaInventoryItemRarity.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Net/UnrealNetwork.h"

#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

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
	return true;
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
				UMounteaInventoryItemAction* NewAction = NewObject<UMounteaInventoryItemAction>(GetPackage()->GetOuter(), SourceAction->GetClass());
				NewAction->CopyFromOther(ItrCategoryAction.ItemAction);

				FMounteaItemAction NewActionDefinition;
				NewActionDefinition.ItemAction = NewAction;
				
				CurrentActions.Add(NewActionDefinition);
			}
		}

		ItemActions = CurrentActions;
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
	case EItemDataSource::EIDS_SourceItem:
		ClearDataTable();
		CopyTagsFromTypes();
		break;
	case EItemDataSource::Default:
	default: break;
	}
}

#if WITH_EDITOR

void UMounteaInventoryItemBase::SetValidDataEditor()
{
	EnsureValidConfig();
}

void UMounteaInventoryItemBase::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);

	EnsureValidConfig();
	
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