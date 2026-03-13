// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools


#include "Components/MounteaAdvancedInventoryLoadoutComponent.h"

#include "Algo/ForEach.h"
#include "Algo/Transform.h"
#include "Definitions/MounteaAdvancedInventoryLoadout.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaEquipmentStatics.h"
#include "Statics/MounteaInventoryStatics.h"

using FResolvedLoadoutItem = TPair<UMounteaAdvancedInventoryLoadoutItem*, FInventoryItem>;

static FString GetLoadoutItemDebugName(const UMounteaAdvancedInventoryLoadoutItem* Item)
{
	if (!IsValid(Item))
		return TEXT("<InvalidItem>");

#if WITH_EDITORONLY_DATA
	if (Item->DisplayName != NAME_None)
		return Item->DisplayName.ToString();
#endif

	return IsValid(Item->ItemTemplate) ? Item->ItemTemplate->DisplayName.ToString() : TEXT("<MissingTemplate>");
}

static void LogFailedLoadoutItems(const TArray<UMounteaAdvancedInventoryLoadoutItem*>& FailedItems, const TCHAR* InvalidItemMessage, const TCHAR* FailedItemMessage)
{
	Algo::ForEach(FailedItems, [InvalidItemMessage, FailedItemMessage](const UMounteaAdvancedInventoryLoadoutItem* Failure)
	{
		if (!IsValid(Failure))
		{
			LOG_WARNING(TEXT("%s"), InvalidItemMessage)
			return;
		}

		LOG_WARNING(TEXT("%s\n%s"), FailedItemMessage, *GetLoadoutItemDebugName(Failure))
	});
}

static void LoadItemsToInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	const TArray<UMounteaAdvancedInventoryLoadoutItem*>& SourceItems,
	TArray<UMounteaAdvancedInventoryLoadoutItem*>& OutLoadedItems,
	TArray<UMounteaAdvancedInventoryLoadoutItem*>& OutFailedItems)
{
	OutLoadedItems.Reserve(SourceItems.Num());
	OutFailedItems.Reserve(SourceItems.Num());

	Algo::ForEach(SourceItems, [&Inventory, &OutLoadedItems, &OutFailedItems](UMounteaAdvancedInventoryLoadoutItem* Item)
	{
		const bool bAdded = IsValid(Item) && UMounteaInventoryStatics::AddItemFromTemplate(Inventory, Item->ItemTemplate, Item->BaseQuantity, Item->BaseDurability);
		if (bAdded)
		{
			OutLoadedItems.Add(Item);
			return;
		}

		OutFailedItems.Add(Item);
	});
}

static bool IsLoadoutItemEquippable(const UMounteaAdvancedInventoryLoadoutItem* Item)
{
	if (!IsValid(Item) || !Item->bAutomaticallyEquip)
		return false;

	const UMounteaInventoryItemTemplate* itemTemplate = Item->ItemTemplate;
	return IsValid(itemTemplate) && itemTemplate->EquipmentItemType.IsValid() && !itemTemplate->AttachmentSlots.IsEmpty();
}

static TArray<UMounteaAdvancedInventoryLoadoutItem*> CollectItemsToEquip(const TArray<UMounteaAdvancedInventoryLoadoutItem*>& LoadedItems)
{
	TArray<UMounteaAdvancedInventoryLoadoutItem*> itemsToEquip;
	itemsToEquip.Reserve(LoadedItems.Num());

	Algo::TransformIf(LoadedItems, itemsToEquip,
		[](const UMounteaAdvancedInventoryLoadoutItem* Item) { return IsLoadoutItemEquippable(Item); },
		[](UMounteaAdvancedInventoryLoadoutItem* Item) { return Item; });

	return itemsToEquip;
}

static TArray<FResolvedLoadoutItem> ResolveInventoryItemsForEquip(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	const TArray<UMounteaAdvancedInventoryLoadoutItem*>& ItemsToEquip)
{
	TArray<FResolvedLoadoutItem> inventoryItems;
	inventoryItems.Reserve(ItemsToEquip.Num());

	Algo::Transform(ItemsToEquip, inventoryItems, [&Inventory](UMounteaAdvancedInventoryLoadoutItem* Item)
	{
		return FResolvedLoadoutItem(
			Item,
			UMounteaInventoryStatics::FindItem(Inventory, FInventoryItemSearchParams(Item->ItemTemplate))
		);
	});

	return inventoryItems;
}

static void EquipResolvedInventoryItems(const TScriptInterface<IMounteaAdvancedEquipmentInterface>& Equipment,
	const TArray<FResolvedLoadoutItem>& InventoryItems,
	TArray<UMounteaAdvancedInventoryLoadoutItem*>& OutFailedEquipments)
{
	OutFailedEquipments.Reserve(InventoryItems.Num());

	Algo::ForEach(InventoryItems, [&Equipment, &OutFailedEquipments](const FResolvedLoadoutItem& ItemPair)
	{
		if (!ItemPair.Value.IsItemValid())
		{
			OutFailedEquipments.Add(ItemPair.Key);
			return;
		}

		bool bEquipped = false;
		if (IsValid(ItemPair.Key) && ItemPair.Key->EquipmentSlot != NAME_None)
			bEquipped = UMounteaEquipmentStatics::EquipItemToSlot(Equipment, ItemPair.Value, ItemPair.Key->EquipmentSlot);

		if (!bEquipped)
			bEquipped = UMounteaEquipmentStatics::EquipItem(Equipment, ItemPair.Value);

		if (!bEquipped)
			OutFailedEquipments.Add(ItemPair.Key);
	});
}

UMounteaAdvancedInventoryLoadoutComponent::UMounteaAdvancedInventoryLoadoutComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaAdvancedInventoryLoadoutComponent::SetComponentTickEnabled(false);

	bAutoActivate = true;

	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append({ TEXT("Mountea"), TEXT("Loadout") });
}

bool UMounteaAdvancedInventoryLoadoutComponent::LoadLoadout_Implementation()
{
	if (!GetOwner()->HasAuthority())
	{
		Server_LoadLoadout();
		return true;
	}
	
	if (!IsValid(LoadoutConfiguration))
	{
		LOG_WARNING(TEXT("[LoadLoadout] Loadout configuration is invalid!"))
		return false;
	}
	if (!IsValid(RelatedInventory.GetObject()))
	{
		LOG_WARNING(TEXT("[LoadLoadout] Inventory is invalid!"))
		return false;
	}
	const TArray<UMounteaAdvancedInventoryLoadoutItem*> items = Execute_GetLoadoutItems(this);
	if (items.IsEmpty())
	{
		LOG_WARNING(TEXT("[LoadLoadout] No items to load!"))
		return false;
	}

	TArray<UMounteaAdvancedInventoryLoadoutItem*> loadedItems;
	TArray<UMounteaAdvancedInventoryLoadoutItem*> failedItems;

	LoadItemsToInventory(RelatedInventory, items, loadedItems, failedItems);
	LogFailedLoadoutItems(
		failedItems,
		TEXT("[LoadLoadout] Failed to load invalid/null item from Loadout!"),
		TEXT("[LoadLoadout] Failed to load item from Loadout:")
	);

	if (!IsValid(RelatedEquipment.GetObject()))
		return failedItems.IsEmpty();

	const TArray<UMounteaAdvancedInventoryLoadoutItem*> itemsToEquip = CollectItemsToEquip(loadedItems);
	if (itemsToEquip.IsEmpty())
		return true;

	const TArray<FResolvedLoadoutItem> inventoryItems = ResolveInventoryItemsForEquip(RelatedInventory, itemsToEquip);

	TArray<UMounteaAdvancedInventoryLoadoutItem*> failedEquipments;
	EquipResolvedInventoryItems(RelatedEquipment, inventoryItems, failedEquipments);
	LogFailedLoadoutItems(
		failedEquipments,
		TEXT("[LoadLoadout] Failed to equip invalid/null item from Loadout!"),
		TEXT("[LoadLoadout] Failed to equip item from Loadout:")
	);

	return true;
}

TArray<UMounteaAdvancedInventoryLoadoutItem*> UMounteaAdvancedInventoryLoadoutComponent::GetLoadoutItems_Implementation() const
{
	if (!IsValid(LoadoutConfiguration))
		return {};
	return LoadoutConfiguration->Items;
}

void UMounteaAdvancedInventoryLoadoutComponent::Server_LoadLoadout_Implementation()
{
	Execute_LoadLoadout(this);
}

void UMounteaAdvancedInventoryLoadoutComponent::InitializeInventoryAndEquipment()
{
	auto inventoryComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedInventoryInterface::StaticClass());
	if (!IsValid(inventoryComponent))
		LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory' component in Parent! Loadouts will NOT work!"))
	else
	{
		RelatedInventory = inventoryComponent;
		ensureMsgf(RelatedInventory.GetObject() != nullptr, TEXT("[MounteaAdvancedInventoryLoadoutComponent] Failed to update 'RelatedInventory'"));
	}
		
	auto equipmentComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedEquipmentInterface::StaticClass());
	if (!IsValid(equipmentComponent))
		LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Equipment' component in Parent! Loadouts will NOT work!"))
	else
	{
		RelatedEquipment = equipmentComponent;
		ensureMsgf(RelatedEquipment.GetObject() != nullptr, TEXT("[MounteaAdvancedInventoryLoadoutComponent] Failed to update 'RelatedEquipment'"));
	}
}

void UMounteaAdvancedInventoryLoadoutComponent::BeginPlay()
{
	Super::BeginPlay();	
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{		
		InitializeInventoryAndEquipment();
		Execute_LoadLoadout(this);
	}
}

