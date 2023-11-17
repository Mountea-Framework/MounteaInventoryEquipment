// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "MounteaInventoryThemeConfig.h"

#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Helpers/MounteaInventoryHelpers.h"

#include "MounteaInventoryEquipmentSettings.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

class UMounteaEquipmentConfigData;
class UMounteaInventoryConfig;
class UMounteaInventoryItemsTable;
class UMounteaInventoryItemConfig;
class UMounteaTransactionPayload;
struct FItemUpdateResult;

/**
 * 
 */
UCLASS(config = MounteaSettings, DefaultConfig, DisplayName="Mountea Inventory & Equipment Settings")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryEquipmentSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UMounteaInventoryEquipmentSettings();

public:

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSet<TSoftObjectPtr<UMounteaInventoryThemeConfig>> ThemeConfigs;

	UPROPERTY(Config, EditAnywhere, Category="1. Required", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface", AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryWidgetClass;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSet<TSoftObjectPtr<UMounteaInventoryItemCategory>> InventoryCategories;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSet<TSoftObjectPtr<UMounteaInventoryItemRarity>> InventoryRarities;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSoftObjectPtr<UMounteaEquipmentConfigData> EquipmentConfigData;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. Optional")
	uint8 bUIDebug : 1;

#pragma region COMMANDS
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TSet<FString> InventoryWidgetCommands;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TSet<FString> EquipmentWidgetCommands;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TSet<FString> ItemsWidgetCommands;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TSet<FString> ItemTooltipWidgetCommands;
	
#pragma endregion

	UPROPERTY(config, EditDefaultsOnly, Category = "5. Notifications")
	TMap<int32, FInventoryNotificationData> InventoryUpdateData;

#if WITH_EDITOR

public:
	
	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("MounteaInventoryEquipment", "MounteaInventoryEquipmentSettingsSection", "Mountea Inventory & Equipment Settings");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("MounteaInventoryEquipment", "MounteaInventoryEquipmentSettingsDescription", "Default values for Mountea Inventory & Equipment Settings.");
	}

	virtual FName GetContainerName() const override
	{
		return "Project";
	}

protected:
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, InventoryWidgetCommands))
		{
			if (!InventoryWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets))
			{
				InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::ShowInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::ShowInventoryWidget);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RemoveItemWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RemoveItemWidget);
			}
		}

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, EquipmentWidgetCommands))
		{
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::InitializeEquipmentWidget))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::InitializeEquipmentWidget);
			}
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::CreateEquipmentWidget))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::CreateEquipmentWidget);
			}
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::ShowEquipmentWidget))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::ShowEquipmentWidget);
			}
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::HideEquipmentWidget))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::HideEquipmentWidget);
			}
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshEquipmentWidget))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshEquipmentWidget);
			}
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshItemsWidgets))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshItemsWidgets);
			}
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::UnequipItemWidget))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::UnequipItemWidget);
			}
			if (!EquipmentWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::EquipItemWidget))
			{
				EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::EquipItemWidget);
			}
		}

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, ItemsWidgetCommands))
		{
			if (!ItemsWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem))
			{
				ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem);
			}
			if (!ItemsWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::UpdateItem))
			{
				ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::UpdateItem);
			}
			if (!ItemsWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::RemoveItem))
			{
				ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::RemoveItem);
			}
		}
		
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, ItemTooltipWidgetCommands))
		{
			if (!ItemTooltipWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::CleanupTooltip))
			{
				ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::CleanupTooltip);
			}
			
			if (!ItemTooltipWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::ShowTooltip))
			{
				ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::ShowTooltip);
			}
			
			if (!ItemTooltipWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::HideTooltip))
			{
				ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::HideTooltip);
			}
		}

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, InventoryUpdateData))
		{
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request."), 3.f, FLinearColor(FColor::White), 0)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemUpdated", "Added all of the item to the existing stack."), 3.f, FLinearColor(FColor::White), 2)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Created))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Created,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemCreated", "New item created in Inventory."), 3.f, FLinearColor(FColor::White), 2)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_CreatedPart))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_CreatedPart,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemUpdatedPartly", "Added part of the item to the existing stack. Some quantity remains in the source item."), 3.f, FLinearColor(FColor::White), 2)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_MutliStatus))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_MutliStatus,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemQuantityReducedRemoved", "The item has been reduced and removed from the inventory."), 3.f, FLinearColor(FColor::White), 2)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid Request. Item does not exist."), 3.f, FLinearColor(FColor::Red), 10)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_InvalidRequest", "Action is forbidden. Action cannot be processed."), 3.f, FLinearColor(FColor::Red), 10)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemNotFound", "The item was not found in the inventory."), 3.f, FLinearColor(FColor::Red), 10)
				);
			}
			if (!InventoryUpdateData.Contains(MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound))
			{
				InventoryUpdateData.Add
				(
					MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_NotFound,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryUpdateResult_ItemUnknownIssue", "Unhandled Exception."), 3.f, FLinearColor(FColor::Red), 10)
				);
			}
		}
	}
	
#endif
	
/*===============================================================================
		IN PROGRESS
		
		Following properties are already being updated.
===============================================================================*/

public:
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSoftObjectPtr<UMounteaInventoryThemeConfig> ThemeConfig;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false, ShowTreeView=true), AdvancedDisplay)
	TSoftClassPtr<UMounteaInventoryItemBase> DefaultItemClass;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false, ShowTreeView=true))
	TSoftClassPtr<UMounteaInventoryConfig> DefaultInventoryConfigClass;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false, ShowTreeView=true))
	TSoftClassPtr<UMounteaInventoryItemConfig> DefaultItemConfigClass;
	
/*===============================================================================
		SUBJECT OF CHANGE
		
		Following functions are using outdated, wrong class definitions and functions.
===============================================================================*/

public:
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSoftObjectPtr<UMounteaInventoryItemsTable> DefaultInventoryItemDefinitionsTable;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. Optional")
	uint8 bDragDropAllowed : 1;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TMap<EInventoryUpdateResult, FText> InventoryUpdateMessages;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "4. Input")
	TArray<FKey> DragKeys;

	UPROPERTY(config, EditDefaultsOnly, Category = "4. Input")
	TArray<FKey> ActionRequestKeys;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "5. Notifications")
	int32 MinDisplayWeight = 1;
		
	/**
	 * The maximum number of threads used for parallel execution.
	 * Increasing this value can improve performance in certain scenarios,
	 * but using too many threads may introduce diminishing returns and
	 * potential performance issues due to overhead and contention.
	 *
	 * ❗ Be cautious when adjusting this value. Setting it too high
	 *     can lead to resource exhaustion and decreased performance.
	 *
	 * ❓ It is recommended to profile and experiment to find the optimal
	 *     thread count for your specific workload and hardware setup.
	 *
	 * @see FPlatformMisc::NumberOfWorkerThreadsToSpawn
	 */
	UPROPERTY(config, EditDefaultsOnly, Category = "6. Optimization", meta=(UIMin=1, ClampMin=1), AdvancedDisplay)
	int32 ThreadsLimit = 2;
	
	/**
	* The MultithreadingThreshold variable determines the minimum number of items required in the inventory to enable multithreading.
	* When the number of items in the inventory is below this threshold, multithreading will be disabled.
	* ❗ Warning: Modifying this value without proper knowledge may affect performance and stability.
	* ❓ Note: The value should be set to a reasonable number based on the expected size of the inventory.
	*/
	UPROPERTY(config, EditDefaultsOnly, Category = "6. Optimization", meta=(UIMin=100, ClampMin=100), AdvancedDisplay)
	int32 MultithreadingThreshold = 1000;
	
};

#undef LOCTEXT_NAMESPACE