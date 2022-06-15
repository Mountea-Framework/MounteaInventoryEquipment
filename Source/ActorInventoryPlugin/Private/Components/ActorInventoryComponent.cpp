// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryComponent.h"

#include "Definitions/InventoryItem.h"
#include "Helpers/ActorInventoryBPFLibrary.h"
#include "Widgets/InventoryWidget.h"

UActorInventoryComponent::UActorInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	InventoryWidgetClass = UInventoryWidget::StaticClass();
}

void UActorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryManager = UActorInventoryBPFLibrary::GetInventoryManager(this);
}

void UActorInventoryComponent::AddItemToInventory(UInventoryItem* Item)
{
	if (IsItemInInventory(Item))
	{
		return;
	}

	InventoryItems.Emplace(Item);
	OnInventoryUpdated.Broadcast(this);
}

void UActorInventoryComponent::AddItemsToInventory(const TArray<UInventoryItem*>& ListOfItems)
{
	for (UInventoryItem* Itr : ListOfItems)
	{
		AddItemToInventory(Itr);
	}
}

bool UActorInventoryComponent::IsItemInInventory(const UInventoryItem* Item) const
{
	return InventoryItems.Contains(Item);
}

void UActorInventoryComponent::LoadInventoryContent(const UDataTable* SourceTable)
{
	if (SourceTable && SourceTable->RowStruct)
	{
		const UScriptStruct* InventoryRowStruct = SourceTable->GetRowStruct();
		if (InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct()))
		{
			static const FString ContextString(TEXT("ContextString"));
			for (const FName Itr : SourceTable->GetRowNames())
			{
				const FInventoryItemData* Row = SourceTable->FindRow<FInventoryItemData>(Itr, ContextString);
				
				if (Row)
				{
					UInventoryItem* NewItem = NewObject<UInventoryItem>(UInventoryItem::StaticClass());

					if (NewItem)
					{
						NewItem->SetItem(*Row);
						AddItemToInventory(NewItem);
					}
				}
			}
		} 
	}
}

FOnInventoryUpdated& UActorInventoryComponent::GetUpdateEventHandle()
{
	return OnInventoryUpdated;
}

void UActorInventoryComponent::SetInventoryWidgetClass(const TSubclassOf<UInventoryWidget> NewInventoryWidgetClass)
{
	InventoryWidgetClass = NewInventoryWidgetClass;
}

void UActorInventoryComponent::SetInventoryWidgetPtr(UInventoryWidget* NewInventoryWidget)
{
	InventoryWidget = NewInventoryWidget;
}

UInventoryWidget* UActorInventoryComponent::GetInventoryWidgetPtr() const
{
	return InventoryWidget;
}

TSubclassOf<UInventoryWidget> UActorInventoryComponent::GetInventoryWidgetClass() const
{
	return InventoryWidgetClass;
}
