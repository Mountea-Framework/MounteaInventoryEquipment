// All rights reserved Dominik Pavlicek 2022.

#pragma once

namespace MounteaInventoryEquipmentConsts
{
	namespace MounteaInventoryWidgetCommands
	{
		namespace InventoryCommands
		{
			const FString InitializeInventoryWidget		(TEXT("InitializeInventoryWidget"));
			const FString CreateInventoryWidget		(TEXT("CreateInventoryWidget"));
			const FString HideInventoryWidget			(TEXT("HideInventoryWidget"));
			const FString RefreshInventoryWidget		(TEXT("RefreshInventoryWidget"));
		}

		namespace ItemCommands
		{
			const FString AddNewItem		(TEXT("AddNewItem"));
		}
	}

	namespace MounteaInventoryNotifications
	{
		namespace InventoryNotifications
		{
			const FString NewItemAdded							(TEXT("NewItemAdded"));
			const FString ItemUpdated								(TEXT("ItemUpdated"));
			const FString ItemRemoved							(TEXT("ItemRemoved"));
			
			const FString ItemAddFailed_Stacking			(TEXT("ItemAddFailed_Stacking"));
			const FString ItemAddFailed_QuantityMax		(TEXT("ItemAddFailed_QuantityMax"));
		}
	}
}
