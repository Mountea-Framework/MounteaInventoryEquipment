// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"

namespace InventoryUICommands
{
	const FString CreateInventoryWidget(TEXT("CreateInventoryWidget"));
	const FString RemoveInventoryWidget(TEXT("RemoveInventoryWidget"));
	const FString OpenInventoryWidget(TEXT("OpenInventoryWidget"));
	const FString CloseInventoryWidget(TEXT("CloseInventoryWidget"));
	const FString ToggleInventoryWidget(TEXT("ToggleInventoryWidget"));
	const FString RefreshInventoryDisplay(TEXT("RefreshInventoryDisplay"));
	const FString ShowItemDetails(TEXT("ShowItemDetails"));
	const FString SortInventoryItems(TEXT("SortInventoryItems"));
	const FString FilterInventoryItems(TEXT("FilterInventoryItems"));
	const FString ClearInventorySelection(TEXT("ClearInventorySelection"));
	const FString ShowInventoryNotification(TEXT("ShowInventoryNotification"));

	inline const TSet<FString> CommandsSet = {
		CreateInventoryWidget,
		RemoveInventoryWidget,
		OpenInventoryWidget,
		CloseInventoryWidget,
		ToggleInventoryWidget,
		RefreshInventoryDisplay,
		ShowItemDetails,
		SortInventoryItems,
		FilterInventoryItems,
		ClearInventorySelection,
		ShowInventoryNotification
	};

	inline TArray<FString> GetAllCommandTypes()
	{
		return CommandsSet.Array();
	}
}
