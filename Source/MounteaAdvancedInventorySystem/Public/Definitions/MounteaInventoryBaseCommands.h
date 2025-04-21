// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"

namespace InventoryUICommands
{
	const FString CreateInventoryWidgetWrapper(TEXT("CreateInventoryWidgetWrapper"));
	const FString RemoveInventoryWidgetWrapper(TEXT("RemoveInventoryWidgetWrapper"));
	
	const FString OpenInventoryWidget(TEXT("OpenInventoryWidget"));
	const FString CloseInventoryWidget(TEXT("CloseInventoryWidget"));
	
	const FString OpenEquipmentWidget(TEXT("OpenEquipmentWidget"));
	const FString CloseEquipmentWidget(TEXT("CloseEquipmentWidget"));
	
	const FString ToggleInventoryWidget(TEXT("ToggleInventoryWidget"));
	
	const FString RefreshInventoryDisplay(TEXT("RefreshInventoryDisplay"));

	const FString CategorySelected(TEXT("CategorySelected"));

	const FString ItemSelected(TEXT("ItemSelected"));
	const FString ShowItemDetails(TEXT("ShowItemDetails"));
	
	const FString SortInventoryItems(TEXT("SortInventoryItems"));
	const FString FilterInventoryItems(TEXT("FilterInventoryItems"));
	const FString ClearInventorySelection(TEXT("ClearInventorySelection"));
	
	const FString ShowInventoryNotification(TEXT("ShowInventoryNotification"));

	inline const TSet<FString> CommandsSet = {
		CreateInventoryWidgetWrapper,
		RemoveInventoryWidgetWrapper,
		
		OpenInventoryWidget,
		CloseInventoryWidget,

		OpenEquipmentWidget,
		CloseEquipmentWidget,

		CategorySelected,
		ItemSelected,
		
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
