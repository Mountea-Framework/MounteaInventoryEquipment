// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryInteractiveWidgetConfig.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;
class UWorld;

/**
 * UMounteaAdvancedInventoryInteractiveWidgetConfig is a UObject-based configuration class derived from UPrimaryDataAsset.
 * It is designed to manage and store assets related to interactive inventory widget previews in the Mountea Advanced Inventory System.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryInteractiveWidgetConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview Settings")
	TSoftObjectPtr<UWorld> PreviewLevelAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview Settings")
	TSoftObjectPtr<UMaterialInterface> DefaultRenderTargetMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview Settings")
	TSoftObjectPtr<UTextureRenderTarget2D> DefaultRenderTarget;
};
