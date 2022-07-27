// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FInventoryCategoryAssetActions : public FAssetTypeActions_Base
{
	
public:
	virtual UClass* GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
};
