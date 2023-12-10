// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemCategory.h"
#include "Definitions/MounteaInventoryItemRarity.h"

#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Helpers/BlueprintFunctionLibraries/MounteaInventoryEquipmentBFL.h"

#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "Settings/Config/MounteaDefaultsConfig.h"

void UMounteaInventoryItemBase::PostInitProperties()
{
	UDataAsset::PostInitProperties();

	bool bIsEditorNoPlay = false;
#if WITH_EDITOR
	bIsEditorNoPlay = UMounteaInventoryEquipmentBFL::IsEditorNoPlay();
#endif
	
	if (bIsEditorNoPlay) // This code gets executed only when opening new Asset in Editor
	{
		if (ItemDataSource == EItemDataSource::EIDS_SourceTable && SourceTable == nullptr && UMounteaInventoryEquipmentBFL::GetSettings() != nullptr)
		{
			SourceTable = UMounteaInventoryEquipmentBFL::GetSettings()->GetDefaultInventoryItemDefinitionsTable().LoadSynchronous();
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
	return ItemGuid.IsValid() && ItemData.IsValid();
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
		ItemData.RequiredData = Row->RequiredData;
		ItemData.OptionalData = Row->OptionalData;
	}
	else
	{
		//TODO: Show error that no valid data found
		
		ItemData = FMounteaInventoryItemData();
	}
}

void UMounteaInventoryItemBase::ClearMappedValues()
{
	ItemData = FMounteaInventoryItemData();
}

void UMounteaInventoryItemBase::CopyTagsFromTypes()
{
	if (ItemData.RequiredData.ItemCategory)
	{
		ItemData.RequiredData.ItemFlags.AppendTags(ItemData.RequiredData.ItemCategory->CompatibleGameplayTags);
	}

	if (ItemData.RequiredData.ItemRarity)
	{
		ItemData.RequiredData.ItemFlags.AddTag(ItemData.RequiredData.ItemRarity->RarityGameplayTag);
	}
}

void UMounteaInventoryItemBase::EnsureValidConfig()
{
	if (ItemConfig.ItemConfig == nullptr)
	{
		bool bFound = false;
		const TSubclassOf<UMounteaInventoryItemConfig> Class = UMounteaInventoryEquipmentBFL::GetDefaults()->DefaultInventoryItemConfigClass.LoadSynchronous();
		ItemConfig.ItemConfig = UMounteaInventoryEquipmentBFL::GetItemConfig(this, Class, bFound);
	}
	
	// Copy Actions Categories
	{
		TArray<FMounteaItemAction> CurrentActions = GetItemActionsDefinitions();
		ItemActions.Empty();

		if (ItemData.RequiredData.ItemCategory)
		{
			TArray<FMounteaItemAction> CategoryActions = ItemData.RequiredData.ItemCategory->GetCategoryActionsDefinitions();
			
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
		if (!ItemData.RequiredData.ItemQuantity.bIsStackable)
		{
			ItemData.RequiredData.ItemQuantity.MaxQuantity = 1;
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