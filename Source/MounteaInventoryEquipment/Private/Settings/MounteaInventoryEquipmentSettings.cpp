// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Settings/MounteaInventoryEquipmentSettings.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Setup/MounteaInventoryConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

UMounteaInventoryEquipmentSettings::UMounteaInventoryEquipmentSettings()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory & Equipment");

	// INVENTORY COMMANDS
	{
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget);
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget);
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::ShowInventoryWidget);
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget);
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets);
		InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RemoveItemWidget);
	}

	// EQUIPMENT COMMANDS
	{
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::InitializeEquipmentWidget);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::CreateEquipmentWidget);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::ShowEquipmentWidget);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::HideEquipmentWidget);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshEquipmentWidget);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshEquipmentWidget);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshItemsWidgets);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::UnequipItemWidget);
		EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::EquipItemWidget);
	}

	// ITEM COMMANDS
	{
		ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem);
		ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::UpdateItem);
		ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::RemoveItem);
	}

	// ITEM TOOLTIP COMMANDS
	{
		ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::CleanupTooltip);
		ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::ShowTooltip);
		ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::HideTooltip);
	}

	// INVENTORY UPDATE DATA SETUP
	{
		InventoryUpdateData.Add
				   (
					   MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing,
					   FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request."), 3.f, FLinearColor(FColor::White), 0)
				   );
		InventoryUpdateData.Add
					(
						MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK,
						FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemUpdated", "Added all of the item to the existing stack."), 3.f, FLinearColor(FColor::White), 2)
					);
		InventoryUpdateData.Add
					(
						MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Created,
						FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemCreated", "New item created in Inventory."), 3.f, FLinearColor(FColor::White), 2)
					);
		InventoryUpdateData.Add
					(
						MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_CreatedPart,
						FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemUpdatedPartly", "Added part of the item to the existing stack. Some quantity remains in the source item."), 3.f, FLinearColor(FColor::White), 2)
					);
		InventoryUpdateData.Add
						(
							MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_MutliStatus,
							FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemQuantityReducedRemoved", "The item has been reduced and removed from the inventory."), 3.f, FLinearColor(FColor::White), 2)
						);
		InventoryUpdateData.Add
					(
						MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest,
						FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid Request. Item does not exist."), 3.f, FLinearColor(FColor::Red), 10)
					);
		InventoryUpdateData.Add
					(
						MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden,
						FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_InvalidRequest", "Action is forbidden. Action cannot be processed."), 3.f, FLinearColor(FColor::Red), 10)
					);
		InventoryUpdateData.Add
					(
						MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound,
						FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemNotFound", "The item was not found in the inventory."), 3.f, FLinearColor(FColor::Red), 10)
					);
		InventoryUpdateData.Add
					(
						MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue,
						FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemUnknownIssue", "Unhandled Exception."), 3.f, FLinearColor(FColor::Red), 10)
					);
	}
	
	MinDisplayWeight = 1;

	bUIDebug = false;
}

#undef LOCTEXT_NAMESPACE