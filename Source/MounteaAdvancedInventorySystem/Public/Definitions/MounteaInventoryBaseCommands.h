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

#pragma once

#include "CoreMinimal.h"

namespace InventoryUICommands
{
	const FString CreateWrapper(TEXT("CreateWrapper"));
	const FString RemoveWrapper(TEXT("RemoveWrapper"));
	
	const FString OpenInventoryWidget(TEXT("OpenInventoryWidget"));
	const FString CloseInventoryWidget(TEXT("CloseInventoryWidget"));
	
	const FString OpenEquipmentWidget(TEXT("OpenEquipmentWidget"));
	const FString CloseEquipmentWidget(TEXT("CloseEquipmentWidget"));
	
	const FString ToggleInventoryWidget(TEXT("ToggleInventoryWidget"));
	
	const FString RefreshInventory(TEXT("RefreshInventory"));

	const FString AddModalWindow(TEXT("AddModalWindow"));
	const FString RemoveModalWindow(TEXT("RemoveModalWindow"));

	const FString ItemAdded(TEXT("ItemAdded"));
	const FString ItemRemoved(TEXT("ItemRemoved"));
	const FString ItemModified(TEXT("ItemModified"));

	const FString CategorySelected(TEXT("CategorySelected"));

	const FString ItemSelected(TEXT("ItemSelected"));
	const FString ShowItemDetails(TEXT("ShowItemDetails"));
	
	const FString SortInventoryItems(TEXT("SortInventoryItems"));
	const FString FilterInventoryItems(TEXT("FilterInventoryItems"));
	const FString ClearInventorySelection(TEXT("ClearInventorySelection"));
	
	const FString ShowInventoryNotification(TEXT("ShowInventoryNotification"));

	inline const TSet<FString> CommandsSet = {
		CreateWrapper,
		RemoveWrapper,
		
		OpenInventoryWidget,
		CloseInventoryWidget,

		OpenEquipmentWidget,
		CloseEquipmentWidget,

		CategorySelected,
		ItemSelected,

		AddModalWindow,
		RemoveModalWindow,

		ItemAdded,
		ItemRemoved,
		ItemModified,
		
		ToggleInventoryWidget,
		RefreshInventory,
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
