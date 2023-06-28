#pragma once

#include "Engine/DeveloperSettings.h"
#include "MIEPopupConfig.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class UMIEPopupConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UMIEPopupConfig()
	{
	}

	UPROPERTY(config)
	FString PluginVersionUpdate = "";
	
};
