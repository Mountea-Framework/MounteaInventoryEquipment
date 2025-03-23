// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"

namespace InventoryUICommands
{
	const FString CreateInventoryWidget	(TEXT("Create Inventory Widget"));
	const FString RemoveInventoryWidget	(TEXT("Remove Inventory Widget"));
	const FString OpenInventoryWidget	(TEXT("Open Inventory Widget"));
	const FString CloseInventoryWidget	(TEXT("Close Inventory Widget"));
	const FString ToggleInventoryWidget	(TEXT("Toggle Inventory Widget"));
	const FString RefreshInventoryDisplay	(TEXT("Refresh Inventory Display"));
	const FString ShowItemDetails	(TEXT("Show Item Details"));
	const FString SortInventoryItems	 (TEXT("Sort Inventory Items"));
	const FString FilterInventoryItems	(TEXT("Filter Inventory Items"));
	const FString ClearInventorySelection	(TEXT("Clear Inventory Selection"));
	const FString ShowInventoryNotification	(TEXT("Show Inventory Notification"));

	inline const TMap<FString, FString> CommandsMap = {
		{ TEXT("CreateDialogueWidget"), CreateInventoryWidget },
		{ TEXT("RemoveDialogueWidget"), RemoveInventoryWidget },
		{ TEXT("OpenInventoryWidget"), OpenInventoryWidget },
		{ TEXT("CloseInventoryWidget"), CloseInventoryWidget },
		{ TEXT("ToggleInventoryWidget"), ToggleInventoryWidget },
		{ TEXT("RefreshInventoryDisplay"), RefreshInventoryDisplay },
		{ TEXT("ShowItemDetails"), ShowItemDetails },
		{ TEXT("SortInventoryItems"), SortInventoryItems },
		{ TEXT("FilterInventoryItems"), FilterInventoryItems },
		{ TEXT("ClearInventorySelection"), ClearInventorySelection },
		{ TEXT("ShowInventoryNotification"), ShowInventoryNotification }
	};

	inline TArray<FString> GetAllCommandTypes()
	{
		TArray<FString> Keys;
		CommandsMap.GenerateValueArray(Keys);
		return Keys;
	}
}
