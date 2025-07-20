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

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryInteractiveWidgetConfig.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;
class AMounteaAdvancedInventoryItemPreviewRenderer;
class UMounteaAdvancedInventoryInteractableObjectWidget;
class UWorld;

/**
 * UMounteaAdvancedInventoryInteractiveWidgetConfig manages assets and classes for 3D inventory item previews.
 * Interactive widget configs define preview renderers, materials, render targets, and widget classes
 * for sophisticated 3D item visualization and interaction within inventory interfaces.
 *
 * @see [3D Item Preview Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see UMounteaAdvancedInventoryInteractableObjectWidget
 * @see AMounteaAdvancedInventoryItemPreviewRenderer
 */
UCLASS(ClassGroup = (Mountea), meta = (DisplayName = "Interactive Widget Config"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryInteractiveWidgetConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview Settings")
	TSoftClassPtr<UMounteaAdvancedInventoryInteractableObjectWidget> InteractiveWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview Settings")
	TSoftClassPtr<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview Settings")
	TSoftObjectPtr<UMaterialInterface> DefaultRenderTargetMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview Settings")
	TSoftObjectPtr<UTextureRenderTarget2D> DefaultRenderTarget;
};
