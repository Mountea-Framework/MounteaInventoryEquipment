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
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedEquipmentSettingsConfig.generated.h"

class AActor;

/**
 * UMounteaAdvancedEquipmentSettingsConfig manages configuration for the Mountea Advanced Equipment System.
 * Equipment settings define allowed equipment slots with metadata for comprehensive equipment
 * management and attachment system configuration within the inventory framework.
 *
 * @see [Equipment Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see FMounteaEquipmentSlotHeaderData
 * @see UMounteaAdvancedInventorySettings
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Equipment Settings Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Equipment System defining equipment types, slots and other settings."))

class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedEquipmentSettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UMounteaAdvancedEquipmentSettingsConfig();
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🧩 Slots",
		meta=(ForceInlineRow),
		meta=(NoResetToDefault), 
		meta=(ShowOnlyInnerProperties),
		meta=(TitleProperty="DisplayName"))
	TMap<FName, FMounteaEquipmentSlotHeaderData> AllowedEquipmentSlots;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🧩 Slots",
		meta=(TitleProperty="Allowed Attachment Targets"))
	TSet<TSoftClassPtr<USceneComponent>> AllowedAttachmentTargets;

	/** Default visual actor class for quick-use items (temporary placeholder consumed by animation notifies). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="⚡ Quick Use",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedQuickUseItemInterface"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=0))
	TSoftClassPtr<AActor> DefaultQuickUseItemClass = nullptr;

protected:
	
	UFUNCTION()
	TArray<FName> GetFallbackSlotOptions() const;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
