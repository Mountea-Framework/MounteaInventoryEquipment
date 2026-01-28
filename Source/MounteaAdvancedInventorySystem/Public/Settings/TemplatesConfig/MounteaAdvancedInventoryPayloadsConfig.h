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
 * Defines a payload class binding identified by a pair of Gameplay Tags.
 * 
 * FPayloadConfig represents a single payload mapping entry where the combination of
 * PayloadCreator and PayloadReceiver uniquely identifies the configuration.
 * 
 * The associated `PayloadClass` specifies which UObject-derived payload type should be used
 * for the given Creator/Receiver tag pair. A soft class pointer is used to avoid hard-loading
 * assets until they are explicitly needed.
 */
USTRUCT(Blueprintable, BlueprintType)
struct FPayloadConfig
{
	GENERATED_BODY()
	
public:
	
	/** Gameplay tag identifying the source (creator) domain of the payload. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Payload Definitions",
		meta=(NoResetToDefault),
		meta=(Categories="Mountea_Inventory"))
	FGameplayTag PayloadCreator;
	
	/** Gameplay tag identifying the target (receiver) domain of the payload. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Payload Definitions",
		meta=(NoResetToDefault),
		meta=(Categories="Mountea_Inventory"))
	FGameplayTag PayloadReceiver;
	
	/**
	 * Soft reference to the UObject-derived payload class associated with this Creator/Receiver pair.
	 *
	 * Soft class pointers are used to prevent hard dependencies and allow the payload class
	 * to be loaded on demand at runtime.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Payload Definitions",
		meta=(ForceShowPluginContent = true))
	TSoftClassPtr<UObject> PayloadClass;
	
public:
	
	FORCEINLINE bool IsValid() const
	{
		return PayloadCreator.IsValid() && PayloadReceiver.IsValid() && !PayloadClass.IsNull();
	}
	
	friend FORCEINLINE bool operator==(const FPayloadConfig& A, const FPayloadConfig& B)
	{
		return A.PayloadCreator == B.PayloadCreator
			&& A.PayloadReceiver == B.PayloadReceiver;
	}
	
	friend FORCEINLINE uint32 GetTypeHash(const FPayloadConfig& Value)
	{
		return HashCombine(GetTypeHash(Value.PayloadCreator), GetTypeHash(Value.PayloadReceiver));
	}
};

/**
 * UMounteaAdvancedInventoryPayloadsConfig manages payload definitions for the Inventory & Equipment system.
 *
 * This data asset allows you to define which payload class should be used for a specific combination
 * of Creator and Receiver Gameplay Tags. Each entry represents a single payload configuration,
 * making it easy to set up data-driven payload behavior directly in the editor.
 *
 * Payload classes are referenced using soft pointers, so they are only loaded when needed,
 * helping to avoid unnecessary hard dependencies and keeping startup lightweight.
 *
 * Typical usage:
 * - Define Creator/Receiver tag pairs in the editor.
 * - Assign a payload class to each pair.
 * - At runtime, query the configuration to resolve the appropriate payload class.
 *
 * @see [Inventory & Equipment Payloads Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see FPayloadConfig
 */
UCLASS(ClassGroup = (Mountea), meta = (DisplayName = "Inventory & Equipment Payloads Config"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryPayloadsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Collection of payload configurations.
	 *
	 * Each entry defines a Creator/Receiver Gameplay Tag pair and the payload class associated with it.
	 * The combination of both tags uniquely identifies a configuration.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Payload Definitions",
		meta=(ShowOnlyInnerProperties),
		meta=(NoResetToDefault),
		meta=(TitleProperty="{PayloadCreator} | {PayloadReceiver}"))
	TSet<FPayloadConfig> PayloadConfigs;
};