// All rights reserved Dominik Pavlicek 2022.


#include "MIEHelpStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FMIEHelpStyle::StyleInstance = nullptr;

void FMIEHelpStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMIEHelpStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FMIEHelpStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMIEHelpStyle::Get()
{
	return *StyleInstance;
}

FName FMIEHelpStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MIEHelpStyleSet"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

TSharedRef<FSlateStyleSet> FMIEHelpStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MIEHelpStyleSet"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaInventoryEquipment")->GetBaseDir() / TEXT("Resources"));

	Style->Set("MIEHelpStyleSet.Toolbar.HelpIcon", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon40x40));
	Style->Set("MIEHelpStyleSet.Toolbar.HelpIcon.small", new IMAGE_BRUSH(TEXT("InfoIcon_Small"), Icon20x20));
	
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT