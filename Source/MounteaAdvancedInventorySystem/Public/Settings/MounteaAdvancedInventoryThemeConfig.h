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
#include "MounteaAdvancedInventoryThemeConfig.generated.h"

/**
 * UMounteaAdvancedInventoryThemeConfig defines visual and audio theming for inventory user interfaces.
 * Theme configs provide color palettes, text styling, and audio assets for consistent visual presentation
 * across different UI states including normal, hovered, active, focus, and disabled appearances.
 *
 * @see [UI Theming](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see UMounteaAdvancedInventorySettingsConfig
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Theme Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining colours."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryThemeConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// Background Colors
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundPrimary = FColor::FromHex("#181818");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundSecondary = FColor::FromHex("#1e1e1e");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundTertiary = FColor::FromHex("#252525");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundDisabled = FColor::FromHex("#3c3b3b");

	// Normal State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Normal")
	FLinearColor PrimaryNormal = FColor::FromHex("#1a1a1a");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Normal")
	FLinearColor SecondaryNormal = FColor::FromHex("#252525");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Normal")
	FLinearColor TertiaryNormal = FColor::FromHex("#303030");

	// Hovered State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor PrimaryHovered = FColor::FromHex("#f39237");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor PrimaryHoverBorder = FColor::FromHex("#e0e0e0");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor SecondaryHovered = FColor::FromHex("#3d3d3d");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor SecondaryHoverBorder = FColor::FromHex("#e0e0e0");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor TertiaryHovered = FColor::FromHex("#474747");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor TertiaryHoverBorder = FColor::FromHex("#e0e0e0");

	// Active State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Active")
	FLinearColor PrimaryActive = FColor::FromHex("#4d4d4d");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Active")
	FLinearColor SecondaryActive = FColor::FromHex("#575757");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Active")
	FLinearColor TertiaryActive = FColor::FromHex("#616161");

	// Focus State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Focus")
	FLinearColor PrimaryFocus = FColor::FromHex("#666666");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Focus")
	FLinearColor SecondaryFocus = FColor::FromHex("#707070");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Focus")
	FLinearColor TertiaryFocus = FColor::FromHex("#7a7a7a");

	// Disabled State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Disabled")
	FLinearColor PrimaryDisabled = FColor::FromHex("#888888");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Disabled")
	FLinearColor SecondaryDisabled = FColor::FromHex("#929292");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Disabled")
	FLinearColor TertiaryDisabled = FColor::FromHex("#9c9c9c");

	// Text Colors
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	FLinearColor PrimaryText = FColor::FromHex("#D0C8B0FF");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	FLinearColor SecondaryText = FColor::FromHex("#9C7E4AFF");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	FLinearColor TertiaryText = FColor::FromHex("#8F7443FF");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	int PrimaryTextSize = 16;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	int SecondaryTextSize = 14;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	int TertiaryTextSize = 12;

	// Accent
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Highlight")
	FLinearColor Accent = FColor::FromHex("#9C7E4AFF");

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> ButtonPressedSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> ButtonHoveredSound;
};
