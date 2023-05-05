// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MounteaInventoryEquipmentSettings.generated.h"

namespace MounteaInventoryWidgetCommands
{
	namespace InventoryCommands
	{
		const FString InitializeInventoryWidget		(TEXT("InitializeInventoryWidget"));
		const FString CreateInventoryWidget		(TEXT("CreateInventoryWidget"));
		const FString HideInventoryWidget		(TEXT("HideInventoryWidget"));
	}

	namespace ItemCommands
	{
		const FString AddNewItem		(TEXT("AddNewItem"));
	}
	
}

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
			if (!InventoryWidgetCommands.Contains(MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget);
			}
			if (!InventoryWidgetCommands.Contains(MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget))
			{
				InventoryWidgetCommands.Add(MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget);
			}
		}
	}
#endif
};
