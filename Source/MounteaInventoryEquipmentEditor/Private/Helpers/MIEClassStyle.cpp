// All rights reserved Dominik Pavlicek 2022.


#include "MIEClassStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FMIEClassStyle::StyleInstance = nullptr;

void FMIEClassStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMIEClassStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FMIEClassStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMIEClassStyle::Get()
{
	return *StyleInstance;
}

FName FMIEClassStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MIEClassStyleSet"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

TSharedRef<FSlateStyleSet> FMIEClassStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MIEClassStyleSet"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaInventoryEquipment")->GetBaseDir() / TEXT("Resources"));

	Style->Set("MIEClassStyleSet.Icon.MounteaItem", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon64x64));
	Style->Set("MIEClassStyleSet.Icon.MounteaItem.small", new IMAGE_BRUSH(TEXT("Mountea_Logo"), Icon16x16));
	
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT