// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryManagerComponent.h"

#include "Definitions/InventoryItem.h"
#include "Helpers/ActorInventoryPluginLog.h"
//#include "Interfaces/ActorInventoryInterface.h"
#include "Widgets/InventoryWidget.h"

#if WITH_EDITOR
#include "EditorHelper.h"
#endif

UActorInventoryManagerComponent::UActorInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bAllowDragDrop = true;
	bAllowPackSplit = true;
	bAllowGeneralCategory = true;
}

void UActorInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Categories Cleanup
	{
		const UInventoryCategory* GenericCategory = nullptr;
		TArray<UInventoryCategory*> InvalidCategories;
	
		for (int i = 0; i < AllowedCategories.Num(); i++)
		{
			if (AllowedCategories.Array().IsValidIndex(i))
			{
				const auto Itr = AllowedCategories.Array()[i];

				if (Itr == nullptr)
				{
					InvalidCategories.Add(Itr);
				}
			
				if (Itr && Itr->GetCategoryData().IsAllCategories())
				{
					if (!GenericCategory)
					{
						GenericCategory = Itr;
					}
					else
					{
						InvalidCategories.Add(Itr);
					}
				}
			}
		}

		for (const auto Itr : InvalidCategories)
		{
			AllowedCategories.Remove(Itr);
		}
	}
}


bool UActorInventoryManagerComponent::AddItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer) const
{
	if (OwningPlayer)
	{
		TArray<UActorComponent*> InventoryComponents = OwningPlayer->GetComponentsByInterface(UActorInventoryInterface::StaticClass());

		if (InventoryComponents.Num())
		{
			// Only one inventory per player
			UActorComponent* InventoryComponent = InventoryComponents[0];

			if (InventoryComponent)
			{
				TScriptInterface<IActorInventoryInterface> InventoryInterface;
				InventoryInterface.SetObject(InventoryComponent);
				InventoryInterface.SetInterface(Cast<IActorInventoryInterface>(InventoryComponent));

				if (InventoryInterface)
				{
					UInventoryItem* NewItem = NewObject<UInventoryItem>();
					NewItem->SetItem(Item->GetItem());
					
					return InventoryInterface->AddItemToInventory(Item);
				}
			}
		}
	}

	return false;
}

bool UActorInventoryManagerComponent::AddItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer) const
{
	for (UInventoryItem* Itr : Items)
	{
		if (!AddItemToInventory(Itr, OwningPlayer))
		{
			return false;
		}
	}

	return true;
}

void UActorInventoryManagerComponent::RemoveItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer) const
{
	if (OwningPlayer)
	{
		TArray<UActorComponent*> InventoryComponents = OwningPlayer->GetComponentsByInterface(UActorInventoryInterface::StaticClass());

		if (InventoryComponents.Num())
		{
			// Only one inventory per player
			UActorComponent* InventoryComponent = InventoryComponents[0];

			if (InventoryComponent)
			{
				TScriptInterface<IActorInventoryInterface> InventoryInterface;
				InventoryInterface.SetObject(InventoryComponent);
				InventoryInterface.SetInterface(Cast<IActorInventoryInterface>(InventoryComponent));

				if (InventoryInterface)
				{
					UInventoryItem* NewItem = NewObject<UInventoryItem>();
					NewItem->SetItem(Item->GetItem());
					
					InventoryInterface->RemoveItemFromInventory(Item);
				}
			}
		}
	}
}

void UActorInventoryManagerComponent::RemoveItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer) const
{
	if (OwningPlayer)
	{
		for(const auto Itr : Items)
		{
			RemoveItemToInventory(Itr, OwningPlayer);
		}
	}
}

void UActorInventoryManagerComponent::SubtractItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer,
	const int32 Amount) const
{
	if (OwningPlayer)
	{
		TArray<UActorComponent*> InventoryComponents = OwningPlayer->GetComponentsByInterface(UActorInventoryInterface::StaticClass());

		if (InventoryComponents.Num())
		{
			// Only one inventory per player
			UActorComponent* InventoryComponent = InventoryComponents[0];

			if (InventoryComponent)
			{
				TScriptInterface<IActorInventoryInterface> InventoryInterface;
				InventoryInterface.SetObject(InventoryComponent);
				InventoryInterface.SetInterface(Cast<IActorInventoryInterface>(InventoryComponent));

				if (InventoryInterface)
				{
					UInventoryItem* NewItem = NewObject<UInventoryItem>();
					NewItem->SetItem(Item->GetItem());
					
					InventoryInterface->SubtractItemFromInventory(Item, Amount);
				}
			}
		}
	}
}

void UActorInventoryManagerComponent::SubtractItemsToInventory(TMap<UInventoryItem*, int32> Items,
	APlayerController* OwningPlayer) const
{
	if (OwningPlayer)
	{
		for (const auto Itr : Items)
		{
			SubtractItemToInventory(Itr.Key, OwningPlayer, Itr.Value);
		}
	}
}

TArray<UInventoryItem*>  UActorInventoryManagerComponent::GetItemsFromInventory(APlayerController* OwningPlayer) const
{
	if (OwningPlayer)
	{
		TArray<UActorComponent*> InventoryComponents = OwningPlayer->GetComponentsByInterface(UActorInventoryInterface::StaticClass());

		if (InventoryComponents.Num())
		{
			// Only one inventory per player
			UActorComponent* InventoryComponent = InventoryComponents[0];

			if (InventoryComponent)
			{
				TScriptInterface<IActorInventoryInterface> InventoryInterface;
				InventoryInterface.SetObject(InventoryComponent);
				InventoryInterface.SetInterface(Cast<IActorInventoryInterface>(InventoryComponent));

				if (InventoryInterface)
				{
					return InventoryInterface->GetInventoryItems();
				}
			}
		}
	}

	return TArray<UInventoryItem*>();
}



void UActorInventoryManagerComponent::SetInventoryWidgetClass(const TSubclassOf<UInventoryWidget> NewInventoryWidgetClass)
{
	InventoryWidgetClass = NewInventoryWidgetClass;
}

void UActorInventoryManagerComponent::SetInventoryCategoryWidgetClass(
	const TSubclassOf<UInventoryCategoryWidget> NewInventoryCategoryWidgetClass)
{
	InventoryCategoryClass = NewInventoryCategoryWidgetClass;
}

void UActorInventoryManagerComponent::SetInventoryItemSlotWidgetClass(
	const TSubclassOf<UInventoryItemSlot> NewInventoryItemSlotClass)
{
	InventorySlotClass = NewInventoryItemSlotClass;
}

void UActorInventoryManagerComponent::SetInventoryItemTooltipWidgetClass(
	const TSubclassOf<UInventoryItemSlotTooltip> NewInventoryItemSlotTooltipClass)
{
	InventorySlotTooltipClass = NewInventoryItemSlotTooltipClass;
}

void UActorInventoryManagerComponent::SetInventoryItemDragDropWidgetClass(
	const TSubclassOf<UInventoryItemSlotDrag> NewInventoryItemSlotDragDropClass)
{
	InventorySlotDragClass = NewInventoryItemSlotDragDropClass;
}

void UActorInventoryManagerComponent::SetInventoryItemSlotUseClass(
	const TSubclassOf<UInventoryItemSlotUse>& NewInventoryItemSlotUseClass)
{
	InventorySlotUseClass = NewInventoryItemSlotUseClass;
}

bool UActorInventoryManagerComponent::IsValidCategory(const FGuid& CategoryGUID) const
{
	for(const auto Itr : AllowedCategories)
	{
		if (Itr && Itr->GetCategoryGUID() == CategoryGUID)
		{
			return true;
		}
	}

	return false;
}

UObject* UActorInventoryManagerComponent::ProcessTransaction_Implementation(TSubclassOf<UInventoryTransaction> TransactionClass, UInventoryTransactionPayload* TransactionPayload)
{
	if (TransactionClass.Get())
	{
		return TransactionClass.GetDefaultObject()->ProcessTransaction(TransactionPayload);	
	}
	
	return nullptr;
}


TArray<UInventoryCategory*> UActorInventoryManagerComponent::GetAllowedCategories() const
{
	return AllowedCategories.Array();
}

void UActorInventoryManagerComponent::ClearAllowedCategories()
{
	AllowedCategories.Empty();
}

void UActorInventoryManagerComponent::SetAllowedCategories(TSet<UInventoryCategory*>& Categories)
{
	ClearAllowedCategories();
	AllowedCategories = Categories;
}

void UActorInventoryManagerComponent::AddAllowedCategory(UInventoryCategory* Category)
{
	if(!ContainsAllowedCategory(Category))
	{
		AllowedCategories.Emplace(Category);
	}
}

void UActorInventoryManagerComponent::AddAllowedCategories(TSet<UInventoryCategory*>& Categories)
{
	for (const auto Itr : Categories)
	{
		AddAllowedCategory(Itr);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategory(UInventoryCategory* Category)
{
	if (ContainsAllowedCategory(Category))
	{
		AllowedCategories.Remove(Category);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategories(TSet<UInventoryCategory*>& Categories)
{
	for (const auto Itr : Categories)
	{
		RemoveAllowedCategory(Itr);
	}
}


bool UActorInventoryManagerComponent::ValidateCategory(const FGuid& CategoryGUID) const
{
	for (const auto Itr : AllowedCategories)
	{
		if (Itr && Itr->GetCategoryGUID() == CategoryGUID)
		{
			return true;
		}
	}

	return false;
}

TArray<UInventoryItemRarity*> UActorInventoryManagerComponent::GetAllowedRarities() const
{
	return  AllowedRarities.Array();
}

void UActorInventoryManagerComponent::ClearAllowedRarities()
{
	AllowedRarities.Empty();
}

void UActorInventoryManagerComponent::SetAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities)
{
	ClearAllowedRarities();
	AllowedRarities = Rarities;
}

void UActorInventoryManagerComponent::AddAllowedRarity(UInventoryItemRarity* Rarity)
{
	if (!ContainsAllowedRarity(Rarity))
	{
		AllowedRarities.Emplace(Rarity);
	}
}

void UActorInventoryManagerComponent::AddAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities)
{
	for (const auto Itr : Rarities)
	{
		AddAllowedRarity(Itr);	
	}
}

void UActorInventoryManagerComponent::RemoveAllowedRarity(const UInventoryItemRarity* Rarity)
{
	if (ContainsAllowedRarity(Rarity))
	{
		AllowedRarities.Remove(Rarity);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities)
{
	for (const auto Itr : Rarities)
	{
		RemoveAllowedRarity(Itr);
	}
}

#if WITH_EDITOR

bool UActorInventoryManagerComponent::ValidateCategories(const bool bShouldCheckForFlags)
{
	bool bResult = true;

	if (bShouldCheckForFlags)
	{
		const bool bShouldValidate = GetOwner()  && HasAnyFlags(RF_LoadCompleted);

		if (!bShouldValidate)
		{
			return true;
		}
	}
		
	int32 GenericCategories = 0;
	for (const auto Itr : AllowedCategories)
	{
		
		if (Itr == nullptr)
		{
			const FText ErrorMessage = FText::FromString(TEXT("ALLOWED CATEGORIES CONTAIN EMPTY RECORD!"));
			FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
			
			bResult = false;
		}

		if (Itr && Itr->GetCategoryData().IsAllCategories())
		{
			GenericCategories++;
			if (GenericCategories > 1)
			{
				const FText ErrorMessage = FText::FromString(TEXT("ALLOWED CATEGORIES CONTAIN MORE THAN ONE GENERIC CATEGORY!"));
				FEditorHelper::DisplayEditorNotification(ErrorMessage, SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
				bResult = false;
			}
		}
	}

	return bResult;
}

void UActorInventoryManagerComponent::UpdateCategories()
{
	if (bAutoAllowParentCategories)
	{
		TSet<UInventoryCategory*> ParentCategories;
		
		for (const auto Itr : AllowedCategories)
		{
			if (Itr && Itr->GetParentCategory() && !ContainsAllowedCategory(Itr->GetParentCategory()))
			{
				ParentCategories.Add(Itr->GetParentCategory());
			}
		}

		for (const auto Itr : ParentCategories)
		{
			if (!ContainsAllowedCategory(Itr))
			{
				int32 SearchDepth = 1;
				AddParentCategory(Itr, SearchDepth);
			}
		}
	}
}

void UActorInventoryManagerComponent::AddParentCategory(UInventoryCategory* Category, int32& DepthIndex)
{
	if (DepthIndex > MaxRecursionDepth)
	{
		return;
	}
	
	if(Category->GetClass())
	{
		if (ContainsAllowedCategory(Category))
		{
			return;
		}
		else
		{
			AddAllowedCategory(Category);
			
			DepthIndex++;
			
			if (Category && Category->GetParentCategory())
			{
				AddParentCategory(Category->GetParentCategory(), DepthIndex);
			}
		}
	}
}

void UActorInventoryManagerComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == "AllowedCategories")
	{
		UpdateCategories();

		if(!ValidateCategories(true)) return;
	}
	
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

EDataValidationResult UActorInventoryManagerComponent::IsDataValid(TArray<FText>& ValidationErrors)
{
	if (!ValidateCategories(false))
	{
		const FText ErrorMessage = FText::FromString(TEXT("Allowed Categories validation failed!"));
		ValidationErrors.Add(ErrorMessage);
		
		return EDataValidationResult::Invalid;
	}
	return Super::IsDataValid(ValidationErrors);
}
#endif