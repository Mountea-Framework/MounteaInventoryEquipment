// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventorySettingsConfig.generated.h"

struct FInventoryRarity;
struct FInventoryCategory;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings Config")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettingsConfig();

public:

	UPROPERTY(EditAnywhere, Category="Rarity")
	TMap<FString, FInventoryRarity> AllowedRarities;

	UPROPERTY(EditAnywhere, Category="Rarity")
	TMap<FString, FInventoryCategory> AllowedCategories;
};
