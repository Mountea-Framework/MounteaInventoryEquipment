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
	// Wrapper lifecycle
	namespace Wrapper
	{
		inline const FString Create(TEXT("CreateWrapper"));
		inline const FString Remove(TEXT("RemoveWrapper"));

		inline const TSet<FString> All = {
			Create,
			Remove
		};
	}

	// Inventory widget (main inventory UI)
	namespace Inventory
	{
		inline const FString Open(TEXT("OpenInventoryWidget"));
		inline const FString Close(TEXT("CloseInventoryWidget"));
		inline const FString Toggle(TEXT("ToggleInventoryWidget"));

		inline const FString Refresh(TEXT("RefreshInventory"));

		inline const FString SortItems(TEXT("SortInventoryItems"));
		inline const FString FilterItems(TEXT("FilterInventoryItems"));

		inline const FString ClearSelection(TEXT("ClearInventorySelection"));

		inline const TSet<FString> All = {
			Open,
			Close,
			Toggle,
			Refresh,
			SortItems,
			FilterItems,
			ClearSelection
		};
	}

	// Equipment widget
	namespace Equipment
	{
		inline const FString Open(TEXT("OpenEquipmentWidget"));
		inline const FString Close(TEXT("CloseEquipmentWidget"));

		inline const TSet<FString> All = {
			Open,
			Close
		};
	}

	// Modal windows
	namespace Modal
	{
		inline const FString Add(TEXT("AddModalWindow"));
		inline const FString Remove(TEXT("RemoveModalWindow"));

		inline const TSet<FString> All = {
			Add,
			Remove
		};
	}

	// Item-level events/actions
	namespace Items
	{
		inline const FString Added(TEXT("ItemAdded"));
		inline const FString Removed(TEXT("ItemRemoved"));
		inline const FString Modified(TEXT("ItemModified"));

		inline const FString Selected(TEXT("ItemSelected"));
		inline const FString ShowDetails(TEXT("ShowItemDetails"));

		inline const TSet<FString> All = {
			Added,
			Removed,
			Modified,
			Selected,
			ShowDetails
		};
	}

	// Category-level actions
	namespace Categories
	{
		inline const FString Selected(TEXT("CategorySelected"));
		inline const FString Refresh(TEXT("RefreshCategory"));

		inline const TSet<FString> All = {
			Selected,
			Refresh
		};
	}

	// Notifications
	namespace Notifications
	{
		inline const FString Show(TEXT("ShowInventoryNotification"));

		inline const TSet<FString> All = {
			Show
		};
	}
	
	// ItemPreview
	namespace ItemPreview
	{
		inline const FString ClearPreview(TEXT("ClearItemPreview"));

		inline const TSet<FString> All = {
			ClearPreview
		};
	}

	// General
	namespace General
	{
		inline const FString Create(TEXT("CreateWidget"));
		inline const FString Remove(TEXT("RemoveWidget"));
		inline const FString Toggle(TEXT("ToggleWidget"));
		inline const FString Refresh(TEXT("RefreshWidget"));
		inline const FString Show(TEXT("ShowWidget"));
		inline const FString Hide(TEXT("HideWidget"));

		inline const TSet<FString> All = {
			Create,
			Remove,
			Toggle,
			Refresh,
			Show,
			Hide
		};
	}
	
	// Aggregate set
	inline const TSet<FString> AllCommands = []()
	{
		TSet<FString> Result;

		Result.Append(Wrapper::All);
		Result.Append(Inventory::All);
		Result.Append(Equipment::All);
		Result.Append(Modal::All);
		Result.Append(Items::All);
		Result.Append(Categories::All);
		Result.Append(Notifications::All);
		Result.Append(General::All);
		Result.Append(ItemPreview::All);

		return Result;
	}();
	
	inline TArray<FString> GetAllCommandTypes()
	{
		return AllCommands.Array();
	}
}