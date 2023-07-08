// All rights reserved Dominik Pavlicek 2022.


#include "MIEEditorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FMIEEditorStyle::StyleInstance = nullptr;

void FMIEEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMIEEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FMIEEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMIEEditorStyle::Get()
{
	return *StyleInstance;
}

FName FMIEEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MIEStyleSet"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

TSharedRef<FSlateStyleSet> FMIEEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MIEStyleSet"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaInventoryEquipment")->GetBaseDir() / TEXT("Resources"));

	Style->Set("MIEStyleSet.Icon.MounteaItem", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon64x64));
	Style->Set("MIEStyleSet.Icon.MounteaItem.small", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon16x16));
	
	Style->Set("MIEStyleSet.Buttons.Documentation", new IMAGE_BRUSH(TEXT("Documentation"), Icon200x70));
	Style->Set("MIEStyleSet.Buttons.Update", new IMAGE_BRUSH(TEXT("RefreshIcon"), Icon64x64));
	Style->Set("MIEStyleSet.Buttons.Documentation.small", new IMAGE_BRUSH(TEXT("DocumentationIcon"), Icon12x12));
	Style->Set("MIEStyleSet.Icon.Browse", new IMAGE_BRUSH(TEXT("BrowseIcon"), Icon12x12));
	Style->Set("MIEStyleSet.Icon.Edit", new IMAGE_BRUSH(TEXT("EditIcon"), Icon12x12));

	const FButtonStyle MounteaButtonStyle = FButtonStyle()
		.SetNormal(BOX_BRUSH("RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(1, 1, 1, 0.1f)))
		.SetHovered(BOX_BRUSH("RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(1, .55f, 0, 0.2f)))
		.SetPressed(BOX_BRUSH("RoundedSelection_16x", 4.0f / 16.0f,  FLinearColor(1, .55f, 0, 0.4f)));

	Style->Set("MIEStyleSet.Buttons.Style", MounteaButtonStyle);
	
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT