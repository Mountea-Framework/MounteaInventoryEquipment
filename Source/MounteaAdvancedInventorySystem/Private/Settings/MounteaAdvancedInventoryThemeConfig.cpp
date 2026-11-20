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


#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#include "Fonts/CompositeFont.h"

EDataValidationResult UMounteaAdvancedInventoryThemeConfig::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	const auto report = [&Context](const FString& Message)
	{
		Context.AddError(FText::FromString(Message));
	};

	const auto checkPaint = [this, &report](FName Role)
	{
		if (!Role.IsNone() && !this->Paints.Contains(Role))
			report(FString::Printf(TEXT("Missing paint role '%s'."), *Role.ToString()));
	};

	const auto checkBrush = [this, &report](FName Role)
	{
		if (!Role.IsNone() && !this->Brushes.Contains(Role))
			report(FString::Printf(TEXT("Missing brush role '%s'."), *Role.ToString()));
	};

	for (const auto& palette : this->Palette)
	{
		const FLinearColor& colour = palette.Value;

		if (palette.Key.IsNone() || !FMath::IsFinite(colour.R) || !FMath::IsFinite(colour.G)
			|| !FMath::IsFinite(colour.B) || !FMath::IsFinite(colour.A) || !FMath::IsNearlyEqual(colour.A, 1.f))
			report(FString::Printf(TEXT("Palette '%s' requires a named opaque finite linear colour."), *palette.Key.ToString()));
	}

	for (const auto& paint : this->Paints)
	{
		if (paint.Key.IsNone() || paint.Value.Ink.IsNone() || !this->Palette.Contains(paint.Value.Ink))
			report(FString::Printf(TEXT("Paint '%s' requires a name and existing palette ink."), *paint.Key.ToString()));

		if (!FMath::IsFinite(paint.Value.Opacity) || paint.Value.Opacity < 0.f || paint.Value.Opacity > 1.f)
			report(FString::Printf(TEXT("Paint '%s' opacity must be within [0, 1]."), *paint.Key.ToString()));
	}

	for (const auto& text : this->Typography)
	{
		checkPaint(text.Value.Foreground);
		checkPaint(text.Value.ShadowColour);

		const FSlateFontInfo& font = text.Value.Font;
		const FCompositeFont* compositeFont = font.GetCompositeFont();

		if (text.Key.IsNone() || !compositeFont || compositeFont->DefaultTypeface.Fonts.IsEmpty()
			|| !FMath::IsFinite(font.Size) || font.Size <= 0.f)
			report(FString::Printf(TEXT("Typography '%s' requires a name and usable font."), *text.Key.ToString()));

		if (compositeFont && !font.TypefaceFontName.IsNone())
		{
			bool faceFound = false;

			for (const FTypefaceEntry& face : compositeFont->DefaultTypeface.Fonts)
			{
				faceFound |= face.Name == font.TypefaceFontName;
			}

			if (!faceFound)
				report(FString::Printf(TEXT("Typography '%s' refers to missing font face '%s'."), *text.Key.ToString(), *font.TypefaceFontName.ToString()));
		}
	}

	for (const auto& brush : this->Brushes)
	{
		checkPaint(brush.Value.Tint);
		checkPaint(brush.Value.OutlineTint);

		if (brush.Key.IsNone())
			report(TEXT("Brush roles must have names."));
	}

	for (const auto& control : this->Controls)
	{
		if (control.Key.IsNone())
			report(TEXT("Control roles must have names."));

		for (const auto& state : control.Value.States)
		{
			checkPaint(state.Value.Foreground);
			checkPaint(state.Value.IconTint);
			checkBrush(state.Value.Background);
			checkBrush(state.Value.FocusDecoration);
		}

		const auto& slider = control.Value.Slider;
		checkBrush(slider.NormalBar);
		checkBrush(slider.HoveredBar);
		checkBrush(slider.DisabledBar);
		checkBrush(slider.NormalThumb);
		checkBrush(slider.HoveredThumb);
		checkBrush(slider.DisabledThumb);

		const auto& progress = control.Value.Progress;
		checkBrush(progress.Background);
		checkBrush(progress.Fill);
		checkBrush(progress.Marquee);

		const auto& editableText = control.Value.EditableText;
		checkPaint(editableText.ForegroundPaint);
		checkBrush(editableText.Background);
		checkBrush(editableText.HoveredBackground);
		checkBrush(editableText.FocusedBackground);
		checkBrush(editableText.ReadOnlyBackground);

		if (!editableText.TypeRole.IsNone() && !this->Typography.Contains(editableText.TypeRole))
			report(FString::Printf(TEXT("Control '%s' refers to missing typography '%s'."), *control.Key.ToString(), *editableText.TypeRole.ToString()));
	}

	for (const auto& opacity : this->GroupOpacity)
	{
		if (opacity.Key.IsNone() || !FMath::IsFinite(opacity.Value) || opacity.Value < 0.f || opacity.Value > 1.f)
			report(TEXT("Group opacity requires a named role and a finite value within [0, 1]."));
	}

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
