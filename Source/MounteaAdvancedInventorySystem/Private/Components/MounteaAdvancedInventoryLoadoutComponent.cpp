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

#include "Definitions/MounteaAdvancedInventoryLoadout.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventorySystemStatics.h"

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
	// TODO: 
	// 1. Get all items
	// 2. Load them all to inventory
	// 3. From the Inventory get all
	// 4. Attach those that are relevant to Equipment
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
	const auto items = Execute_GetLoadoutItems(this);
	if (items.Num() == 0)
	{
		LOG_WARNING(TEXT("[LoadLoadout] No items to load!"))
		return false;
	}
	
	TMap<int32, bool> failedItems;
	failedItems.Reserve(items.Num());
	for (const auto& item : items)
	{
		if (!RelatedInventory->Execute_AddItemFromTemplate(RelatedInventory.GetObject(), item->ItemTemplate, item->BaseQuantity, item->BaseDurability))
			failedItems.Add(items.Find(item), false);
	}
	
	for (const auto& failure : failedItems)
	{
		if (!items.IsValidIndex(failure.Key))
			continue;
		LOG_WARNING(TEXT("[LoadLoadout] Failed to load item from Loadout:\n%s"), *(items[failure.Key]->DisplayName.ToString()))
	}
	
	if (!IsValid(RelatedEquipment.GetObject()))
		return failedItems.Num() == 0;

	TArray<UMounteaAdvancedInventoryLoadoutItem*> itemsToEquip = {};
	itemsToEquip.Reserve(items.Num());
	for (const auto& item : items)
	{
		if (failedItems.Contains(items.Find(item)))
			continue;
		if (!IsValid(item->ItemTemplate))
			continue;
		if (!item->ItemTemplate->EquipmentItemType.IsValid() || item->ItemTemplate->AttachmentSlots.Num() == 0)
			continue;
		itemsToEquip.Add(item);
	}
	
	TArray<FInventoryItem> inventoryItems = {};
	inventoryItems.Reserve(itemsToEquip.Num());
	for (const auto& item : itemsToEquip)
	{
		const auto inventoryItem = RelatedInventory->Execute_FindItem(RelatedInventory.GetObject(), FInventoryItemSearchParams(item->ItemTemplate));
		if (inventoryItem.IsItemValid())
			inventoryItems.Add(inventoryItem);
	}
	
	// TODO: make it work with pre-defined slots!
	for (const auto& item : inventoryItems)
	{
		RelatedEquipment->Execute_EquipItem(RelatedEquipment.GetObject(), item);
	}
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

