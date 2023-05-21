// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaInventoryEquipmentSettings.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

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

	UPROPERTY(Config, EditAnywhere, Category="1. User Interface", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface"))
	TSoftClassPtr<UUserWidget> DefaultInventoryWidgetClass;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. User Interface")
	TSet<FString> InventoryWidgetCommands;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. User Interface")
	TMap<EInventoryUpdateResult, FText> InventoryUpdateMessages;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. User Interface")
	TSet<FString> ItemsWidgetCommands;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "1. User Interface")
	TMap<EItemUpdateResult, FText> ItemUpdateMessages;

	UPROPERTY(config, EditDefaultsOnly, Category = "2. Notifications")
	int32 MinDisplayWeight = 1;
	
	UPROPERTY(config, EditDefaultsOnly, Category = "2. Notifications")
	TMap<EInventoryUpdateResult, FInventoryNotificationData> InventoryUpdateData;
	UPROPERTY(config, EditDefaultsOnly, Category = "2. Notifications")
	TMap<EItemUpdateResult, FInventoryNotificationData> ItemUpdateData;

	UPROPERTY(config, EditDefaultsOnly, Category = "1. User Interface")
	FIntPoint BaseInventoryDimensions;
	
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
	}
#endif
};

#undef LOCTEXT_NAMESPACE