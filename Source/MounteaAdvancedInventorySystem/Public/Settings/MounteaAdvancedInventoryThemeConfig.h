// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryThemeConfig.generated.h"

/**
 * @class UMounteaAdvancedInventoryThemeConfig
 * @brief A configuration class for defining color themes used in the Mountea Advanced Inventory System.
 *
 * This class extends UPrimaryDataAsset and provides a comprehensive set of configurable colors
 * for different UI elements and states in the Inventory System. The configuration contains primary,
 * secondary, and tertiary levels for each theme or UI state, allowing for a highly customizable visual
 * experience.
 *
 * The configurations include:
 * - Background Colors: Defines the primary, secondary, tertiary, and disabled colors for UI backgrounds.
 * - State Colors: Defines distinct color schemes for Normal, Hovered, Active, Focused, and Disabled states.
 * - Text Colors: Configures text color options for primary, secondary, and tertiary text elements.
 *
 * Each property is exposed to Blueprints as both editable and readable to allow seamless integration
 * with the editor and runtime systems.
 *
 * The color definitions utilize FLinearColor to specify both standard RGB values and optional Alpha for transparency.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Theme Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining colours."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryThemeConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// Background Colors
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundPrimary = FLinearColor(0.094f, 0.094f, 0.094f); // #181818

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundSecondary = FLinearColor(0.118f, 0.118f, 0.118f); // #1e1e1e

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundTertiary = FLinearColor(0.145f, 0.145f, 0.145f); // #252525

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Background")
	FLinearColor BackgroundDisabled = FLinearColor(0.408f, 0.396f, 0.396f); // #3c3b3b

	// Normal State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Normal")
	FLinearColor PrimaryNormal = FLinearColor(0.102f, 0.102f, 0.102f); // #1a1a1a

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Normal")
	FLinearColor SecondaryNormal = FLinearColor(0.145f, 0.145f, 0.145f); // #252525

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Normal")
	FLinearColor TertiaryNormal = FLinearColor(0.188f, 0.188f, 0.188f); // #303030

	// Hovered State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor PrimaryHovered = FLinearColor(0.953f, 0.573f, 0.216f); // #f39237

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor PrimaryHoverBorder = FLinearColor(0.878f, 0.878f, 0.878f, 0.5f); // #e0e0e080

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor SecondaryHovered = FLinearColor(0.239f, 0.239f, 0.239f); // #3d3d3d

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor SecondaryHoverBorder = FLinearColor(0.878f, 0.878f, 0.878f, 0.5f); // #e0e0e080

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Hovered")
	FLinearColor TertiaryHovered = FLinearColor(0.278f, 0.278f, 0.278f); // #474747

	// Active State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Active")
	FLinearColor PrimaryActive = FLinearColor(0.302f, 0.302f, 0.302f); // #4d4d4d

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Active")
	FLinearColor SecondaryActive = FLinearColor(0.341f, 0.341f, 0.341f); // #575757

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Active")
	FLinearColor TertiaryActive = FLinearColor(0.380f, 0.380f, 0.380f); // #616161

	// Focus State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Focus")
	FLinearColor PrimaryFocus = FLinearColor(0.4f, 0.4f, 0.4f); // #666666

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Focus")
	FLinearColor SecondaryFocus = FLinearColor(0.439f, 0.439f, 0.439f); // #707070

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Focus")
	FLinearColor TertiaryFocus = FLinearColor(0.478f, 0.478f, 0.478f); // #7a7a7a

	// Disabled State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Disabled")
	FLinearColor PrimaryDisabled = FLinearColor(0.533f, 0.533f, 0.533f); // #888888

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Disabled")
	FLinearColor SecondaryDisabled = FLinearColor(0.573f, 0.573f, 0.573f); // #929292

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State|Disabled")
	FLinearColor TertiaryDisabled = FLinearColor(0.612f, 0.612f, 0.612f); // #9c9c9c

	// Text Colors
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	FLinearColor PrimaryText = FLinearColor::White; // #ffffff

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	FLinearColor SecondaryText = FLinearColor(0.878f, 0.878f, 0.878f); // #e0e0e0

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
	FLinearColor TertiaryText = FLinearColor(0.690f, 0.690f, 0.690f); // #b0b0b0

};
