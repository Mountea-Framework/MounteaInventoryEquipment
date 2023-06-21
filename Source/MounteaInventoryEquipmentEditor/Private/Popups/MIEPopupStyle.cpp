// All rights reserved Dominik Pavlicek 2022.


#include "MIEPopupStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FMIEPopupStyle::StyleSet = nullptr;

void FMIEPopupStyle::Initialize()
{
	if (!StyleSet.IsValid() )
	{
		Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
	}
}

void FMIEPopupStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( StyleSet->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( StyleSet->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

void FMIEPopupStyle::Create()
{
	StyleSet = MakeShareable(new FSlateStyleSet("MIEPopupStyleSet"));
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaInventoryEquipment")->GetBaseDir() / TEXT("Resources"));

	StyleSet->Set("MIEStyleSet.Icon.HeartIcon", new IMAGE_BRUSH(TEXT("HeartIcon"), Icon12x12));
	StyleSet->Set("MIEStyleSet.Icon.MoneyIcon", new IMAGE_BRUSH(TEXT("MoneyIcon"), Icon12x12));
	StyleSet->Set("MIEStyleSet.Icon.SupportDiscord", new IMAGE_BRUSH(TEXT("DiscordIcon"), Icon12x12));
	StyleSet->Set("MIEStyleSet.Icon.UBIcon", new IMAGE_BRUSH(TEXT("UnrealBucketIcon"), Icon12x12));
	StyleSet->Set("MIEStyleSet.Icon.Close", new IMAGE_BRUSH(TEXT("CloseIcon"), Icon12x12));
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT