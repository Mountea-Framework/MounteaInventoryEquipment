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


#include "Decorations/MounteaInventoryItemAction.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Helpers/MounteaJsonObject.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Statics/MounteaAdvancedInventoryJsonStatics.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

namespace MounteaItemActionPayload
{
	const FName OwningInventoryField(TEXT("owningInventory"));
	const FName ItemGuidField(TEXT("itemGuid"));
}

bool UMounteaInventoryItemAction::ResolveItemFromPayload(const UMounteaJsonObject* Payload, UObject*& OutInventory, FMounteaInventoryItem& OutItem)
{
	OutInventory = nullptr;
	OutItem = FMounteaInventoryItem();

	if (!IsValid(Payload))
		return false;

	FGuid itemGuid;
	if (!Payload->GetGuidField(MounteaItemActionPayload::ItemGuidField, itemGuid) || !itemGuid.IsValid())
		return false;

	// Reading does not mutate the payload; the JSON statics take a non-const pointer
	UMounteaJsonObject* mutablePayload = const_cast<UMounteaJsonObject*>(Payload);
	if (!UMounteaAdvancedInventoryJsonStatics::GetObjectJsonField(mutablePayload, MounteaItemActionPayload::OwningInventoryField, OutInventory))
		return false;

	if (!IsValid(OutInventory) || !OutInventory->Implements<UMounteaAdvancedInventoryInterface>())
		return false;

	OutItem = IMounteaAdvancedInventoryInterface::Execute_FindItem(OutInventory, FInventoryItemSearchParams(itemGuid));
	return OutItem.IsItemValid();
}

bool UMounteaInventoryItemAction::InitializeInventoryAction_Implementation(const UMounteaJsonObject* Payload)
{
	UObject* inventory = nullptr;
	FMounteaInventoryItem item;
	
	if(ResolveItemFromPayload(Payload, inventory, item))
	{
		ContextPayload = const_cast<UMounteaJsonObject*>(Payload);
		return true;
	}
	
	return false;
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryItemAction::GetOwningInventory_Implementation() const
{
	return nullptr;
}

bool UMounteaInventoryItemAction::IsAllowed_Implementation(const UMounteaJsonObject* Payload) const
{
	UObject* inventory = nullptr;
	FMounteaInventoryItem item;
	if (!ResolveItemFromPayload(Payload, inventory, item))
		return false;

	return IsValid(item.GetTemplate());
}

FText UMounteaInventoryItemAction::GetDisallowedReason_Implementation(const UMounteaJsonObject* Payload) const
{
	if (!IsValid(Payload))
		return FText::FromString(TEXT("Invalid target item"));

	return Super::GetDisallowedReason(Payload);
}

bool UMounteaInventoryItemAction::ExecuteInventoryAction_Implementation(const UMounteaJsonObject* Payload)
{
	if (!IsAllowed(Payload))
	{
		LOG_WARNING(TEXT("[%s] Action not allowed: %s"), *GetName(), *GetDisallowedReason(Payload).ToString())
		return false;
	}
	
	return true;
}

