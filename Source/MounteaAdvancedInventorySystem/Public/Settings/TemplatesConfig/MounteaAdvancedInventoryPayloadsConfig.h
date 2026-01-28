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
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryPayloadsConfig.generated.h"

/**
 * Enumerates supported payload directions used by the payload configuration system.
 */
UENUM(Blueprintable, BlueprintType)
enum class EPayloadDirection : uint8
{
	EPD_Outgoing	UMETA(DisplayName = "Outgoing", ToolTip="Defines payload which is sent and never expected to be returned."),
	EPD_Incoming	UMETA(DisplayName = "Incoming", ToolTip="Defines payload which is expected to be received from other source."),
	EPD_Generic		UMETA(DisplayName = "Generic",  ToolTip="Defines generic payload which can be used for anything."),
	
	Hidde			UMETA(Hidden)
};

/**
 * Defines payload class mapping for a single gameplay tag configuration.
 *
 * Each entry associates an EPayloadDirection with a payload class reference.
 * Soft class pointers are used to avoid hard-loading assets until needed.
 *
 * Intended usage:
 * - Configure direction -> payload class mappings in editor.
 * - Resolve/load the class on demand when processing payloads.
 */
USTRUCT(Blueprintable, BlueprintType)
struct FPayloadConfig
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Mapping between payload direction and payload class type.
	 *
	 * Key   : Direction semantics for the payload.
	 * Value : Soft class reference to the payload UObject-derived class.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Payload Definitions",
		meta=(ShowOnlyInnerProperties),
		meta=(ForceShowPluginContent = true))
	TMap<EPayloadDirection, TSoftClassPtr<UObject>> PayloadClassMap;
};

/**
 * UMounteaAdvancedInventoryPayloadsConfig manages payload definitions for the Inventory & Equipment system.
 * Payload configs define direction-based payload class mappings (Outgoing/Incoming/Generic) keyed by gameplay tags
 * for data-driven payload resolution and usage across inventory/equipment features.
 *
 * Each entry in `PayloadConfigs` represents a payload “domain” identified by a `FGameplayTag`.
 * The corresponding `FPayloadConfig` then maps `EPayloadDirection` to a soft class reference, allowing
 * payload classes to be loaded on demand without forcing hard dependencies at startup.
 *
 * @see [Inventory & Equipment Payloads Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see FPayloadConfig
 * @see EPayloadDirection
 */
UCLASS(ClassGroup = (Mountea), meta = (DisplayName = "Inventory & Equipment Payloads Config"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryPayloadsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Payload configuration sets keyed by gameplay tag.
	 *
	 * Key   : Gameplay tag identifying the payload domain/type.
	 * Value : Direction-to-class mapping configuration.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Payload Definitions",
		meta=(ShowOnlyInnerProperties))
	TMap<FGameplayTag, FPayloadConfig> PayloadConfigs;
};

// TODO: Create statics getters for tags and inputs, so its easier to query