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

#include "MounteaAdvancedInventoryDeveloperStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define SVG_BRUSH( RelativePath, ... ) FSlateVectorImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".svg") ), __VA_ARGS__ )
#define SVG_BRUSH_TINT( RelativePath, Size, Tint ) FSlateVectorImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".svg") ), Size, Tint )
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

TSharedPtr<FSlateStyleSet> FMounteaAdvancedInventoryDeveloperStyle::StyleSet = nullptr;

void FMounteaAdvancedInventoryDeveloperStyle::Create()
{
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	
	StyleSet = MakeShareable(new FSlateStyleSet(GetAppStyleSetName()));
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaAdvancedInventorySystem")->GetBaseDir() / TEXT("Resources"));

	StyleSet->Set("MAISStyleSet.MounteaLogo.Small", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon16x16));
	StyleSet->Set("MAISStyleSet.MounteaLogo", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon40x40));

	StyleSet->Set("MDSStyleSet.K2Node_ValidateIcon.Small", new SVG_BRUSH_TINT(TEXT("WebIcons/shield-check"), Icon20x20, FLinearColor::White));
	StyleSet->Set("MDSStyleSet.K2Node_ValidateIcon", new SVG_BRUSH_TINT(TEXT("WebIcons/shield-check"), Icon40x40, FLinearColor::White));

	StyleSet->Set("MDSStyleSet.K2Node_SetterIcon.Small", new SVG_BRUSH_TINT(TEXT("WebIcons/sunrise"), Icon20x20, FLinearColor::White));
	StyleSet->Set("MDSStyleSet.K2Node_SetterIcon", new SVG_BRUSH_TINT(TEXT("WebIcons/sunrise"), Icon40x40, FLinearColor::White));
	
	StyleSet->Set("MDSStyleSet.K2Node_GetterIcon.Small", new SVG_BRUSH_TINT(TEXT("WebIcons/sunset"), Icon20x20, FLinearColor::White));
	StyleSet->Set("MDSStyleSet.K2Node_GetterIcon", new SVG_BRUSH_TINT(TEXT("WebIcons/sunset"), Icon40x40, FLinearColor::White));
	
	StyleSet->Set("MAISStyleSet.K2Node_SwitchIcon.Small", new SVG_BRUSH_TINT(TEXT("WebIcons/trending-up-down"), Icon20x20, FLinearColor::White));
	StyleSet->Set("MAISStyleSet.K2Node_SwitchIcon", new SVG_BRUSH_TINT(TEXT("WebIcons/trending-up-down"), Icon40x40, FLinearColor::White));
	
	StyleSet->Set("MAISStyleSet.K2Node_SetPropertyByName.Small", new SVG_BRUSH_TINT(TEXT("WebIcons/circuit-board"), Icon20x20, FLinearColor::White));
	StyleSet->Set("MAISStyleSet.K2Node_SetPropertyByName", new SVG_BRUSH_TINT(TEXT("WebIcons/circuit-board"), Icon40x40, FLinearColor::White));
}

void FMounteaAdvancedInventoryDeveloperStyle::Initialize()
{
	if (!StyleSet.IsValid() )
	{
		Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());

		StyleSet->SetParentStyleName(FAppStyle::GetAppStyleSetName());
		FAppStyle::SetAppStyleSet(*StyleSet);
	}
}

void FMounteaAdvancedInventoryDeveloperStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}