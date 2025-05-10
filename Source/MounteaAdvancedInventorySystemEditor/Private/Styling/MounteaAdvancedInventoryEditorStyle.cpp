// All rights reserved Dominik Morse (Pavlicek) 2024

#include "MounteaAdvancedInventoryEditorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

TSharedPtr<FSlateStyleSet> FMounteaAdvancedInventoryEditorStyle::StyleSet = nullptr;

void FMounteaAdvancedInventoryEditorStyle::Create()
{
	const FVector2D Icon12x12(12.0f, 12.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);
	const FVector2D Icon128x128(128.f, 128.f);
	const FVector2D Icon200x70(200.f, 70.f);
	
	StyleSet = MakeShareable(new FSlateStyleSet(GetAppStyleSetName()));
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaAdvancedInventorySystem")->GetBaseDir() / TEXT("Resources"));

	StyleSet->Set("MAISStyleSet.MounteaLogo.Small", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon16x16));
	StyleSet->Set("MAISStyleSet.MounteaLogo", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon40x40));

	StyleSet->Set("MAISStyleSet.K2Node_ValidateIcon.Small", new IMAGE_BRUSH(TEXT("K2Node_ValidateIcon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.K2Node_ValidateIcon", new IMAGE_BRUSH(TEXT("K2Node_ValidateIcon"), Icon40x40));

	StyleSet->Set("MAISStyleSet.K2Node_SetterIcon.Small", new IMAGE_BRUSH(TEXT("K2Node_SetterIcon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.K2Node_SetterIcon", new IMAGE_BRUSH(TEXT("K2Node_SetterIcon"), Icon40x40));
	
	StyleSet->Set("MAISStyleSet.K2Node_GetterIcon.Small", new IMAGE_BRUSH(TEXT("K2Node_GetterIcon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.K2Node_GetterIcon", new IMAGE_BRUSH(TEXT("K2Node_GetterIcon"), Icon40x40));

	StyleSet->Set("MAISStyleSet.Help", new IMAGE_BRUSH(TEXT("Help_Icon"), Icon40x40));
	StyleSet->Set("MAISStyleSet.Help.Small", new IMAGE_BRUSH(TEXT("Help_Icon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.Help.large", new IMAGE_BRUSH(TEXT("Help_Icon"), Icon64x64));

	StyleSet->Set("MAISStyleSet.Dialoguer", new IMAGE_BRUSH(TEXT("Dialoguer_Icon"), Icon40x40));
	StyleSet->Set("MAISStyleSet.Dialoguer.Small", new IMAGE_BRUSH(TEXT("Dialoguer_Icon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.Dialoguer.large", new IMAGE_BRUSH(TEXT("Dialoguer_Icon"), Icon64x64));

	StyleSet->Set("MAISStyleSet.Launcher", new IMAGE_BRUSH(TEXT("MPLIcon"), Icon40x40));

	StyleSet->Set("MAISStyleSet.Wiki", new IMAGE_BRUSH(TEXT("Wiki_Icon"), Icon40x40));
	StyleSet->Set("MAISStyleSet.Wiki.Small", new IMAGE_BRUSH(TEXT("Wiki_Icon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.Wiki.large", new IMAGE_BRUSH(TEXT("Wiki_Icon"), Icon64x64));

	StyleSet->Set("MAISStyleSet.Youtube", new IMAGE_BRUSH(TEXT("youtube_icon"), Icon40x40));
	StyleSet->Set("MAISStyleSet.Youtube.Small", new IMAGE_BRUSH(TEXT("youtube_icon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.Youtube.large", new IMAGE_BRUSH(TEXT("youtube_icon"), Icon64x64));

	StyleSet->Set("MAISStyleSet.Settings", new IMAGE_BRUSH(TEXT("settings_icon"), Icon40x40));
	StyleSet->Set("MAISStyleSet.Settings.Small", new IMAGE_BRUSH(TEXT("settings_icon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.Settings.large", new IMAGE_BRUSH(TEXT("settings_icon"), Icon64x64));

	StyleSet->Set("MAISStyleSet.Config", new IMAGE_BRUSH(TEXT("configIcon"), Icon40x40));
	StyleSet->Set("MAISStyleSet.Config.Small", new IMAGE_BRUSH(TEXT("configIcon"), Icon16x16));
	StyleSet->Set("MAISStyleSet.Config.large", new IMAGE_BRUSH(TEXT("configIcon"), Icon64x64));

	StyleSet->Set("MAISStyleSet.InventorySystemIcon", new IMAGE_BRUSH(TEXT("InventorySystem_Icon"), Icon16x16));
}

void FMounteaAdvancedInventoryEditorStyle::Initialize()
{
	if (!StyleSet.IsValid() )
	{
		Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());

		StyleSet->SetParentStyleName(FAppStyle::GetAppStyleSetName());
		FAppStyle::SetAppStyleSet(*StyleSet);
	}
}

void FMounteaAdvancedInventoryEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}