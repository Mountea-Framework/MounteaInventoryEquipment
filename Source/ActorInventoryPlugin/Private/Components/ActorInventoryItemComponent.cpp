// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInventoryItemComponent.h"

#include "Definitions/InventoryItem.h"
#include "Helpers/ActorInventoryPluginLog.h"

void UActorInventoryItemComponent::BeginPlay()
{
	Super::BeginPlay();
}

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
			if (SourceItemRow.DataTable)
			{
				static const FString ContextString(TEXT(""));
				const FInventoryItemData* Row = SourceItemRow.DataTable->FindRow<FInventoryItemData>(SourceItemRow.RowName, ContextString);
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
		// Todo: Show message that the table is invalid
		// ...
	}
}
