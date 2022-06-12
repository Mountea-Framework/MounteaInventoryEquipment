// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInventoryItemComponent.h"

#include "Definitions/InventoryItem.h"
#include "Helpers/ActorInventoryPluginLog.h"

FInventoryItemData UActorInventoryItemComponent::GetItemDefinition() const
{
	FInventoryItemData ReturnDefinition;
	switch (SetupMode)
	{
		case EInventoryItemSetup::EIIS_FromItem:
			if (SourceItem)
			{
				if (const UInventoryItem* Item = Cast<UInventoryItem>(SourceItem->GetDefaultObject(true)))
				{
					ReturnDefinition = Item->GetItem();
				}
			}
			break;
		case EInventoryItemSetup::EIIS_FromDataTable:
			if (SourceItemTable.DataTable)
			{
				static const FString ContextString(TEXT(""));
				const FInventoryItemData* Row = SourceItemTable.DataTable->FindRow<FInventoryItemData>(SourceItemTable.RowContents, ContextString);
				if (ContextString.IsEmpty())
				{
					ReturnDefinition = *Row;
				}
			}
			break;
		case EInventoryItemSetup::EIIS_Default:
			break;
		default: ;
	}

	return ReturnDefinition;
}

void UActorInventoryItemComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == "DataTable")
	{
		const UDataTable* Table = SourceItemTable.DataTable;
		
		if (Table)
		{
			if (Table->GetRowStruct())
			{
				const UScriptStruct* InventoryRowStruct = Table->GetRowStruct();
				if (InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct()))
				{
					AInvP_LOG(Warning, TEXT("[UActorInventoryItemComponent] Valid Table"))
				}
				else
				{
					AInvP_LOG(Warning, TEXT("[UActorInventoryItemComponent] Invalid Table"))

					//SourceItemTable.DataTable = nullptr;
				}
			}
		}
	}
}