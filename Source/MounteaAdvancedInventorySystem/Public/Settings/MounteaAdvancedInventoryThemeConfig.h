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
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "MounteaAdvancedInventoryThemeConfig.generated.h"

class USoundBase;

/** Semantic visual roles shared by the inventory, equipment and crafting screens. */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Theme Config")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryThemeConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Opaque linear source inks. Paint opacity supplies the resolved alpha. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme|Palette",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TMap<FName, FLinearColor> Palette;

	/** Semantic ink and opacity roles. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme|Paints",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TMap<FName, FMounteaThemePaint> Paints;

	/** Complete typography by semantic role. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme|Typography",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TMap<FName, FMounteaThemeTextStyle> Typography;

	/** Decorative brush resources and paint overrides. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme|Brushes",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TMap<FName, FMounteaThemeBrushStyle> Brushes;

	/** Control states and typed native style parts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme|Controls",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TMap<FName, FMounteaThemeControlStyle> Controls;

	/** Intentional whole-subtree fades in the inclusive range zero to one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme|GroupOpacity",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TMap<FName, float> GroupOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Audio",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TSoftObjectPtr<USoundBase> ButtonPressedSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Audio",
		meta=(NoResetToDefault, ShowOnlyInnerProperties))
	TSoftObjectPtr<USoundBase> ButtonHoveredSound;

#if WITH_EDITOR
public:
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
