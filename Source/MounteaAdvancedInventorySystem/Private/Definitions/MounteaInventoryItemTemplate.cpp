// All rights reserved Dominik Morse 2024


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
	SellPriceCoefficient(0),
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
	{
		TArray<FString> returnValues;
		returnValues.Add(TEXT(""));
		return returnValues;
	}
	auto inventorySettingsConfig = inventorySettings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(inventorySettingsConfig))
		TArray<FString> returnValues;

	TArray<FString> returnValues;
	if (const auto categoryConfiguration = inventorySettingsConfig->AllowedCategories.Find(ItemCategory))
	{
		for (auto subCategory : categoryConfiguration->SubCategories)
			returnValues.Add(subCategory.ToString());
	}
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
			ItemFlags = categoryConfiguration->CategoryFlags;
		}

		ItemSubCategory = TEXT("");
	}
}

#endif
