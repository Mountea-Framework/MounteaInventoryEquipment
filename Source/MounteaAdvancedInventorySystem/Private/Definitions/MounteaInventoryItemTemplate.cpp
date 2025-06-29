// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools


#include "Definitions/MounteaInventoryItemTemplate.h"

#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

UMounteaInventoryItemTemplate::UMounteaInventoryItemTemplate():
	Guid(FGuid::NewGuid()),
	ItemCategory(TEXT("Miscellaneous")),
	ItemRarity(TEXT("Common")),
	ItemFlags(0),
	MaxQuantity(1),
	MaxStackSize(99),
	SpawnActor(nullptr),
	ItemMesh(nullptr),
	bHasDurability(false),
	MaxDurability(0),
	BaseDurability(0),
	DurabilityPenalization(0),
	DurabilityToPriceCoefficient(0),
	bHasPrice(false),
	BasePrice(0),
	SellPriceCoefficient(1.f),
	bHasWeight(false),
	Weight(0)
{
}

TArray<FString> UMounteaInventoryItemTemplate::GetAllowedCategories()
{
	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
	{
		TArray<FString> returnValues;
		returnValues.Add(TEXT("Miscellaneous"));
		return returnValues;
	}
	return inventorySettings->GetAllowedCategories();
}

TArray<FString> UMounteaInventoryItemTemplate::GetAllowedSubCategories() const
{
	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
		return TArray<FString>();
	
	auto inventorySettingsConfig = inventorySettings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(inventorySettingsConfig))
		return TArray<FString>();

	if (inventorySettingsConfig->AllowedCategories.Num() <= 0)
		return TArray<FString>();

	TArray<FString> returnValues;
	if (const auto categoryConfiguration = inventorySettingsConfig->AllowedCategories.Find(ItemCategory))
		categoryConfiguration->SubCategories.GetKeys(returnValues);
	
	return returnValues;
}

TArray<FString> UMounteaInventoryItemTemplate::GetAllowedRarities()
{
	TArray<FString> returnValues;
	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
	{
		returnValues.Add(TEXT("Common"));
		return returnValues;
	}
	return inventorySettings->GetAllowedRarities();
}

#if WITH_EDITOR

void UMounteaInventoryItemTemplate::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!inventorySettings)
		return;
	auto inventorySettingsConfig = inventorySettings->InventorySettingsConfig.LoadSynchronous();
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemTemplate, ItemCategory))
	{
		if (inventorySettingsConfig)
		{
			auto categoryConfiguration = inventorySettingsConfig->AllowedCategories.Find(ItemCategory);
			ItemFlags = categoryConfiguration->CategoryData.CategoryFlags;
		}

		ItemSubCategory = TEXT("");
	}
}

#endif
