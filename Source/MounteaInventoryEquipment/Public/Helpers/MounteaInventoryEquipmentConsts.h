// All rights reserved Dominik Pavlicek 2023.

#pragma once

namespace MounteaInventoryEquipmentConsts
{
	namespace MounteaInventoryWidgetCommands
	{
		namespace InventoryCommands
		{
			const FString InitializeInventoryWidget				(TEXT("InitializeInventoryWidget"));
			const FString CreateInventoryWidget				(TEXT("CreateInventoryWidget"));
			const FString ShowInventoryWidget				(TEXT("ShowInventoryWidget"));
			const FString HideInventoryWidget					(TEXT("HideInventoryWidget"));
			const FString RefreshInventoryWidget				(TEXT("RefreshInventoryWidget"));
			const FString RefreshItemsWidgets				(TEXT("RefreshItemsWidgets"));

			const FString RemoveItemWidget					(TEXT("RemoveItemWidget"));
		}

		namespace ItemCommands
		{
			const FString AddNewItem								(TEXT("AddNewItem"));
			const FString UpdateItem								(TEXT("UpdateItem"));
			const FString RemoveItem								(TEXT("RemoveItem"));
		}

		namespace ItemTooltipCommands
		{
			const FString CleanupTooltip							(TEXT("CleanupTooltip"));
			const FString ShowTooltip								(TEXT("ShowTooltip"));
			const FString HideTooltip								(TEXT("HideTooltip"));
		}

		namespace ItemActionCommands
		{
			const FString InitializeAction							(TEXT("InitializeAction"));
		}

		
	}

	namespace MounteaEquipmentWidgetCommands
	{
		namespace EquipmentCommands
		{
			const FString InitializeEquipmentWidget				(TEXT("InitializeEquipmentWidget"));
			const FString CreateEquipmentWidget				(TEXT("CreateEquipmentWidget"));
			const FString ShowEquipmentWidget				(TEXT("ShowEquipmentWidget"));
			const FString HideEquipmentWidget					(TEXT("HideEquipmentWidget"));
			const FString RefreshEquipmentWidget				(TEXT("RefreshEquipmentWidget"));
			const FString RefreshItemsWidgets				(TEXT("RefreshItemsWidgets"));

			const FString RemoveItemWidget					(TEXT("RemoveItemWidget"));
		}
	}
	namespace MounteaInventoryNotifications
	{
		namespace InventoryNotifications
		{
			const FString UpdateSuccessful						(TEXT("UpdateSuccessful"));
			const FString UpdateFailed							(TEXT("UpdateFailed"));
		}

		namespace ItemNotifications
		{
			const FString NewItemAdded							(TEXT("NewItemAdded"));
			const FString ItemUpdated								(TEXT("ItemUpdated"));
			const FString ItemRemoved							(TEXT("ItemRemoved"));

			const FString ItemAddFailed_Invalid				(TEXT("ItemAddFailed_Invalid"));
			const FString ItemAddFailed_Stacking			(TEXT("ItemAddFailed_Stacking"));
			const FString ItemAddFailed_QuantityMax		(TEXT("ItemAddFailed_QuantityMax"));
		}
	}

	namespace NodeColours
	{
		const FLinearColor Call = FLinearColor::Yellow;
		const FLinearColor Listen = FLinearColor::Green;
		const FLinearColor Stop = FLinearColor::Red;
	}

	namespace NodeIcons
	{
		const FString Listen						(TEXT("Symbols.Check"));
		const FString Call							(TEXT("Symbols.Contact"));
		const FString Stop							(TEXT("Icons.Denied"));
	}
}
