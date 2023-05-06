// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "MounteaInventoryEquipmentSettings.generated.h"

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
	TSet<FString> ItemsWidgetCommands;

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
		}

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryEquipmentSettings, ItemsWidgetCommands))
		{
			if (!ItemsWidgetCommands.Contains(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem))
			{
				ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem);
			}
		}
	}
#endif
};
