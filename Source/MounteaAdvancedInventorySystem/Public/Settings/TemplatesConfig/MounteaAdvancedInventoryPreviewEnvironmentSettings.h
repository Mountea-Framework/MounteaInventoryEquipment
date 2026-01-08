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
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryPreviewEnvironmentSettings.generated.h"

/**
 * UMounteaAdvancedInventoryPreviewEnvironmentSettings provides data-driven configuration
 * for preview scene environments in the Mountea Advanced Inventory System.
 * 
 * This data asset defines lighting, post-processing, and additional actor configurations
 * that compose the visual environment for 3D inventory item previews.
 *
 * Key features include:
 * - Configurable directional and ambient sky lighting
 * - Optional post-processing effects for enhanced visuals
 * - Support for spawning additional environmental actors (backdrops, props, etc.)
 * - Data-driven approach eliminates need for Blueprint subclassing
 * - Reusable configurations across different preview contexts
 *
 * @see [3D Item Preview](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemPreview)
 * @see AMounteaAdvancedInventoryPreviewEnvironment
 * @see UMounteaAdvancedInventoryInteractableObjectWidget
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings UI Config", 
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining visual components."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryPreviewEnvironmentSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Defines the directional light configuration for the preview environment.
	 * Directional lights serve as the primary illumination source, simulating
	 * sunlight or key lighting to reveal form and detail of preview items.
	 * Default settings match Unreal Engine's standard preview lighting setup.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
	FMounteaPreviewDirectionalLightSettings DirectionalLight;

	/**
	 * Defines the ambient sky light configuration for the preview environment.
	 * Sky lights provide soft ambient illumination and optional image-based lighting
	 * through cubemaps, creating realistic material appearance with environmental reflections.
	 * Essential for proper visualization of metallic and reflective materials.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
	FMounteaPreviewSkyLightSettings SkyLight;

	/**
	 * Defines the post-processing configuration for the preview environment.
	 * Post-processing effects enhance visual quality through tone mapping, color grading,
	 * bloom, depth of field, and other cinematic effects. Disabled by default for performance.
	 * Enable only when specific visual enhancements are required for preview quality.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PostProcess")
	FMounteaPreviewPostProcessSettings PostProcess;
	
	/**
	 * Specifies a collection of additional actor classes to spawn in the preview environment.
	 * Enables placement of environmental elements like backdrops, floors, decorative props,
	 * or additional light sources that enhance the preview scene's visual context.
	 * Actors are spawned at world origin; use actor-specific positioning logic as needed.
	 * Commonly used for:
	 * - Ground planes or pedestals for item display
	 * - Backdrop meshes for environmental context
	 * - Additional fill lights or accent lighting
	 * - Decorative elements for thematic preview scenes
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Extras")
	TSet<TSoftClassPtr<AActor>> ExtraActorsToSpawn;	
};
