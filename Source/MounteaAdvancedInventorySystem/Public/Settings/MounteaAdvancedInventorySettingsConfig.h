// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventorySettingsConfig.generated.h"

enum class EInventoryType : uint8;
struct FInventoryRarity;
struct FInventoryCategory;
struct FInventoryTypeConfig;

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

	UPROPERTY(EditAnywhere, Category="Types")
	TMap<EInventoryType, FInventoryTypeConfig> AllowedInventoryTypes;

	UPROPERTY(EditAnywhere, Category="Rarities")
	TMap<FString, FInventoryRarity> AllowedRarities;

	UPROPERTY(EditAnywhere, Category="Categories")
	TMap<FString, FInventoryCategory> AllowedCategories;
};
