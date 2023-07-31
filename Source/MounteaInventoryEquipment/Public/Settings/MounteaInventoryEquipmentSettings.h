// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "MounteaInventoryThemeConfig.h"

#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Helpers/MounteaInventoryHelpers.h"

#include "MounteaInventoryEquipmentSettings.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

class UMounteaInventoryConfig;
class UMounteaInventoryItemsTable;
class UMounteaInventoryItemConfig;
class UMounteaTransactionPayload;
struct FItemUpdateResult;

/**
 * 
 */
UCLASS(config = MounteaSettings, DisplayName="Mountea Inventory & Equipment Settings")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryEquipmentSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UMounteaInventoryEquipmentSettings();

public:

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSoftObjectPtr<UMounteaInventoryThemeConfig> ThemeConfig;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false, ShowTreeView=true), AdvancedDisplay)
	TSoftClassPtr<UMounteaInventoryItemBase> DefaultItemClass;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false, ShowTreeView=true))
	TSoftClassPtr<UMounteaInventoryConfig> DefaultInventoryConfigClass;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false, ShowTreeView=true))
	TSoftClassPtr<UMounteaInventoryItemConfig> DefaultItemConfigClass;
	
	UPROPERTY(Config, EditAnywhere, Category="1. Required", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface", AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryWidgetClass;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSoftObjectPtr<UMounteaInventoryItemsTable> DefaultInventoryItemDefinitionsTable;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSet<TSoftObjectPtr<UMounteaInventoryItemCategory>> InventoryCategories;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TSet<TSoftObjectPtr<UMounteaInventoryItemRarity>> InventoryRarities;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TMap<FGameplayTag,FGameplayTagContainer> CompatibleInventoryFlags;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. Optional")
	uint8 bUIDebug : 1;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. Optional")
	uint8 bDragDropAllowed : 1;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TSet<FString> InventoryWidgetCommands;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TMap<EInventoryUpdateResult, FText> InventoryUpdateMessages;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TSet<FString> ItemsWidgetCommands;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TMap<EItemUpdateResult, FText> ItemUpdateMessages;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. User Interface")
	TSet<FString> ItemTooltipWidgetCommands;

	UPROPERTY(config, EditDefaultsOnly, Category = "4. Input")
	TArray<FKey> DragKeys;

	UPROPERTY(config, EditDefaultsOnly, Category = "4. Input")
	TArray<FKey> ActionRequestKeys;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "5. Notifications")
	int32 MinDisplayWeight = 1;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "5. Notifications")
	TMap<EInventoryUpdateResult, FInventoryNotificationData> InventoryUpdateData;
	UPROPERTY(config, EditDefaultsOnly, Category = "5. Notifications")
	TMap<EItemUpdateResult, FInventoryNotificationData> ItemUpdateData;

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


public:

#if WITH_EDITOR
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

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, InventoryUpdateMessages))
		{
			if (!InventoryUpdateMessages.Contains(EInventoryUpdateResult::EIUR_Success))
			{
				InventoryUpdateMessages.Add(EInventoryUpdateResult::EIUR_Success, LOCTEXT("InventoryUpdateMessages_Success", "Inventory Updated"));
			}
			if (!InventoryUpdateMessages.Contains(EInventoryUpdateResult::EIUR_Failed))
			{
				InventoryUpdateMessages.Add(EInventoryUpdateResult::EIUR_Failed,  LOCTEXT("InventoryUpdateMessages_Failed", "Inventory Update Failed"));
			}
		}

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, ItemUpdateMessages))
		{
			if (!ItemUpdateMessages.Contains(EItemUpdateResult::EIUR_Success_AddItem))
			{
				ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_AddItem, LOCTEXT("ItemUpdateMessages_SuccessAdd", "Success - Add Item"));
			}
			if (!ItemUpdateMessages.Contains(EItemUpdateResult::EIUR_Success_UpdateItem))
			{
				ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_UpdateItem, LOCTEXT("ItemUpdateMessages_SuccessUpdate", "Success - Update Item"));
			}
			if (!ItemUpdateMessages.Contains(EItemUpdateResult::EIUR_Success_RemovedItem))
			{
				ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_RemovedItem,  LOCTEXT("InventoryUpdateMessages_RemovedItem", "Success - Removed Item"));
			}
			if (!ItemUpdateMessages.Contains(EItemUpdateResult::EIUR_Success_SomeAdd))
			{
				ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_SomeAdd,  LOCTEXT("InventoryUpdateMessages_SomeAddItem", "Success - Partially Added"));
			}
			if (!ItemUpdateMessages.Contains(EItemUpdateResult::EIUR_Failed_InvalidItem))
			{
				ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Failed_InvalidItem,  LOCTEXT("InventoryUpdateMessages_InvalidItem", "Failed - Invalid Item"));
			}
			if (!ItemUpdateMessages.Contains(EItemUpdateResult::EIUR_Failed_LimitReached))
			{
				ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Failed_LimitReached,  LOCTEXT("InventoryUpdateMessages_InvalidItem", "Failed - Max Quantity"));
			}
		}

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, InventoryUpdateData))
		{
			if (!InventoryUpdateData.Contains(EInventoryUpdateResult::EIUR_Failed))
			{
				InventoryUpdateData.Add
				(
					EInventoryUpdateResult::EIUR_Failed,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryNotificationData_Failed", "Inventory Update Failed"), 3.f, FLinearColor(FColor::Red))
				);
			}
			if (!InventoryUpdateData.Contains(EInventoryUpdateResult::EIUR_Success))
			{
				InventoryUpdateData.Add
				(
					EInventoryUpdateResult::EIUR_Success,
					FInventoryNotificationData(nullptr, LOCTEXT("InventoryNotificationData_Success", "Inventory Updated"), 1.5f, FLinearColor(FColor::White))
				);
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
	}
#endif
};

#undef LOCTEXT_NAMESPACE