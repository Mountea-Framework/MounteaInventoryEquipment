// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MounteaInventoryEquipmentSettingsEditor.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettingsEditor"

/**
 * 
 */
UCLASS(config = MounteaSettings, DisplayName="Mountea Inventory & Equipment Settings (Editor)")
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaInventoryEquipmentSettingsEditor : public UDeveloperSettings
{
	GENERATED_BODY()

	UMounteaInventoryEquipmentSettingsEditor();

protected:

	UPROPERTY(config, EditDefaultsOnly, Category="Config")
	uint8 bCheckForTagsUpdate : 1;

public:

	FORCEINLINE bool AllowCheckTagUpdate() const { return bCheckForTagsUpdate; };
	
};

#undef LOCTEXT_NAMESPACE