// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "MounteaThemeConfigHelpers.generated.h"

/**
 * Helper structure exposing Widget text settings to set them globally in Dialogue Settings.
 */
USTRUCT(BlueprintType)
struct FTextSettings
{
	GENERATED_BODY()

	/**
	 * Slate Color Settings.
	 * Defines color of the Font for UI.
	 * Default:
	 * * Color: White
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles", meta=(DisplayName="Color and Oppacity"))
	FSlateColor FontColor;
	/**
	 * Slate Font Info settings.
	 * Should be filled by default with:
	 * * Font: Roboot
	 * * Size 16
	 * * Typeface: 'Regular'
	 * * Outline: 1
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles", meta=(ForceShowEngineContent))
	FSlateFontInfo SubtitlesFont;
	/**
	 * Shadow Offset Settings.
	 * Defines shadow offset on X and Y axis.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles")
	FVector2D ShadowOffset;
	/**
	 * Shadow Color Settings.
	 * Default:
	 * * Color: Black
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles")
	FLinearColor ShadowColor;

	/**
	 * Settings GUID.
	 * Defines whether settings are valid or not.
	 * Invalid settings are:
	 * * With no Font
	 * 
	 * Invalid settings are ignored!
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Subtitles")
	FGuid SettingsGUID;

public:

	FTextSettings() : FontColor(FLinearColor::White), ShadowOffset(1.5f, 1.25f), ShadowColor(FLinearColor::Black)
	{
		SubtitlesFont = FCoreStyle::GetDefaultFontStyle("Regular", 16, FFontOutlineSettings(1));
	};
};

USTRUCT(BlueprintType)
struct FMounteaThemeConfigRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Text Brush")
	FTextSettings CategoryTextBrush;

	UPROPERTY(EditAnywhere, Category="Icon Brush")
	FSlateBrush CategoryIconsBrush;
};