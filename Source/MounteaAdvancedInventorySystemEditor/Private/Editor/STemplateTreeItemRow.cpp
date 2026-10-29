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

#include "Editor/STemplateTreeItemRow.h"

#include "AssetRegistry/AssetData.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Styling/AppStyle.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "UObject/Package.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryTreeRows"

const FTableRowStyle* MounteaInventoryTreeRows::GetRowStyle()
{
	return &FMounteaAdvancedInventoryEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("MAISStyleSet.CustomTableRow");
}

FText MounteaInventoryTreeRows::CategoryLabel(const FString& Category)
{
	return FText::FromString(Category).ToUpper();
}

FText MounteaInventoryTreeRows::DisplayName(const FText& Name, bool IsModified)
{
	const FString text =
	    IsModified ? FText::Format(LOCTEXT("ModifiedName", "{0} *"), Name).ToString() : Name.ToString();
	return FText::FromString(text.Len() > 32 ? text.Left(32) + TEXT("...") : text);
}

FString MounteaInventoryTreeRows::AssetPath(const UObject* Asset)
{
	if (!Asset)
		return FString();

	return Asset->HasAnyFlags(RF_Transient) ? TEXT("Unsaved") : Asset->GetPackage()->GetName();
}

FReply MounteaInventoryTreeRows::LocateAsset(TWeakObjectPtr<UObject> Asset)
{
	if (Asset.IsValid() && !Asset->HasAnyFlags(RF_Transient))
	{
		TArray<FAssetData> assets;
		assets.Add(FAssetData(Asset.Get()));
		FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get().SyncBrowserToAssets(assets);
	}

	return FReply::Handled();
}

TSharedRef<SWidget> MounteaInventoryTreeRows::MakeCategoryContent(const FString& Category, TAttribute<int32> Count)
{
	return SNew(SBorder)
	    .BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop"))
	    .BorderBackgroundColor(FLinearColor::Transparent)
	    .Padding(FMargin(6, 4))[SNew(SHorizontalBox) +
	                            SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)[SNew(STextBlock)
	                                    .Text(CategoryLabel(Category))
	                                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
	                                    .ColorAndOpacity(FSlateColor::UseForeground())
	                                    .TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")] +
	                            SHorizontalBox::Slot()
	                                .AutoWidth()
	                                .VAlign(VAlign_Center)
	                                .Padding(6, 0, 0, 0)[SNew(STextBlock)
	                                        .Text_Lambda(
	                                            [Count]()
	{
		return FText::AsNumber(Count.Get());
	})
	                                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
	                                        .ColorAndOpacity(FSlateColor::UseSubduedForeground())]];
}

TSharedRef<SWidget> MounteaInventoryTreeRows::MakeAssetContent(
    const FMounteaInventoryAssetRowPresentation& Presentation)
{
	struct FButtonHolder
	{
		TWeakPtr<SButton> Button;
	};
	const auto holder = MakeShared<FButtonHolder>();
	TSharedPtr<SButton> interactionButton;
	auto actionButton = [&Presentation](
	                        const FName Icon, const FText& Tooltip, const FOnClicked& Callback) -> TSharedRef<SWidget>
	{
		return SNew(SButton)
		    .ButtonStyle(FAppStyle::Get(), "SimpleButton")
		    .ContentPadding(FMargin(2))
		    .ToolTipText(Tooltip)
		    .Visibility(Presentation.ActionsVisibility)
		    .OnClicked(Callback)[SNew(SImage)
		            .Image(FAppStyle::GetBrush(Icon))
		            .ColorAndOpacity(FSlateColor::UseForeground())
		            .DesiredSizeOverride(FVector2D(16, 16))];
	};
	const TSharedRef<SWidget> content = SAssignNew(interactionButton, SButton)
	                                        .ButtonStyle(FAppStyle::Get(), "NoBorder")
	                                        .OnClicked(Presentation.Select)
	                                        .ContentPadding(0)
	                                        .IsFocusable(false)
	                                            [SNew(SBorder)
	                                                 .BorderImage_Lambda([selected = Presentation.IsSelected, holder]() -> const FSlateBrush*
	                                                                     {
		                                                                     const bool isSelected = selected.Get();
		                                                                     const auto button = holder->Button.Pin();
		                                                                     const bool isHovered = button.IsValid() && button->IsHovered();
		                                                                     const auto& style = FMounteaAdvancedInventoryEditorStyle::Get();

		                                                                     if (isSelected && isHovered)
			                                                                     return style.GetBrush("MAISStyleSet.TemplateItem.SelectedHovered");

		                                                                     if (isSelected)
			                                                                     return style.GetBrush("MAISStyleSet.TemplateItem.Selected");

		                                                                     if (isHovered)
			                                                                     return style.GetBrush("MAISStyleSet.TemplateItem.Hovered");

		                                                                     return style.GetBrush("MAISStyleSet.TemplateItem.Normal");
	                                                                     })
	                                                 .Padding(FMargin(8, 6))[SNew(SHorizontalBox) + SHorizontalBox::Slot().FillWidth(1.f)[SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(Presentation.Name).ToolTipText(Presentation.Tooltip).Font_Lambda([selected = Presentation.IsSelected]()
	                                                                                                                                                                                                                                                                                    {
		                                                                                                                                                                                                                                                                                    return FCoreStyle::GetDefaultFontStyle(selected.Get() ? "Bold" : "Regular", 10);
	                                                                                                                                                                                                                                                                                    })
	                                                                                                                                                                                                 .ColorAndOpacity(Presentation.NameColor)] +
	                                                                                                                                      SVerticalBox::Slot().AutoHeight().Padding(0, 2, 0, 0)[SNew(STextBlock).Text(FText::FromString(Presentation.AssetPath)).Font(FCoreStyle::GetDefaultFontStyle("Regular", 8)).ColorAndOpacity(FSlateColor::UseSubduedForeground())]] +
	                                                                         SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(8, 0, 0, 0)[SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth().Padding(2, 0)[actionButton("Icons.FolderOpen", LOCTEXT("Locate", "Navigate to folder in Content Browser"), Presentation.Locate)] + SHorizontalBox::Slot().AutoWidth().Padding(2, 0)[actionButton("Icons.Duplicate", LOCTEXT("Duplicate", "Duplicate this template"), Presentation.Duplicate)] + SHorizontalBox::Slot().AutoWidth().Padding(2, 0)[actionButton("Icons.Delete", LOCTEXT("Delete", "Delete this template"), Presentation.Delete)]]]];
	holder->Button = interactionButton;
	return content;
}

#undef LOCTEXT_NAMESPACE
