// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FMounteaAdvancedInventoryItemTemplate_AssetAction : public FAssetTypeActions_Base
{
	
public:
	FMounteaAdvancedInventoryItemTemplate_AssetAction();

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;
};
