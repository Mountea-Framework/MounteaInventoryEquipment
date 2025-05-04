// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class  FMounteaAdvancedInventoryThemeConfig_AssetAction : public FAssetTypeActions_Base
{
public:
	FMounteaAdvancedInventoryThemeConfig_AssetAction();

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;
};
