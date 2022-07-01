// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInventoryItemComponent.h"

#include "Definitions/InventoryItem.h"

#if WITH_EDITOR
#include "InventoryEditorNotifications/Public/EditorHelper.h"
#endif

void UActorInventoryItemComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto TempItem = NewObject<UInventoryItem>();
	TempItem->SetItem(GetItemDefinition());
	
	if (TempItem && TempItem->IsValidItem())
	{
		Item = TempItem;
	}
	else
	{
		Item = nullptr;
	}
}

FInventoryItemData UActorInventoryItemComponent::GetItemDefinition() const
{
	FInventoryItemData ReturnDefinition;
	switch (SetupMode)
	{
		case EInventoryItemSetup::EIIS_FromItem:
			if (SourceItem)
			{
				if (UInventoryItem* TempItem = Cast<UInventoryItem>(SourceItem->GetDefaultObject(true)))
				{
					ReturnDefinition = TempItem->GetItem();
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
#if WITH_EDITOR
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == "DataTable")
	{
		if (const auto DataTable = SourceItemRow.DataTable)
		{
			if (DataTable->GetRowStruct())
			{
				const UScriptStruct* InventoryRowStruct = DataTable->GetRowStruct();
				if (!(InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct())))
				{
					const FString ErrorMessage = FString::Printf(TEXT("INVALID DATA TABLE: %s structure is not supported!"), *InventoryRowStruct->GetDisplayNameText().ToString());
					FEditorHelper::DisplayWarningEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f);
				}
			}
		}
	}
#endif
}
