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

#include "Editor/MounteaInventoryEditorLayout.h"
#include "Styling/AppStyle.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Settings/EditorStyleSettings.h"
#include "Settings/MounteaAdvancedInventorySettingsEditor.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEditorLayout"

TSharedRef<SWidget> MounteaInventoryEditorLayout::MakeToolbar(const FMounteaInventoryEditorToolbarActions& Actions, TSharedRef<SWidget> Search)
{
	TSharedRef<SHorizontalBox> Bar = SNew(SHorizontalBox);
	auto AddButton = [&Bar](const FText& Label, const FText& Tooltip, const FSlateBrush* Icon, FOnClicked Click, TAttribute<bool> Enabled)
	{
		Bar->AddSlot().AutoWidth().Padding(2.f)
		[
			SNew(SButton).ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground()).ToolTipText(Tooltip).OnClicked(Click).IsEnabled(Enabled)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(4, 0)
				[ SNew(SImage).Image(Icon).ColorAndOpacity(FSlateColor::UseForeground()) ]
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 4, 0)
				[
					SNew(STextBlock).Text(Label).ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([]()
					{
						return GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText
							&& !GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Visible : EVisibility::Collapsed;
					})
				]
			]
		];
	};
	auto Separator = [&Bar]()
	{
		Bar->AddSlot().AutoWidth().Padding(8.f, 4.f)
		[ SNew(SSeparator).SeparatorImage(FAppStyle::GetBrush("Separator")).Thickness(1.f).Orientation(Orient_Vertical) ];
	};
	AddButton(LOCTEXT("New", "New Template"), LOCTEXT("NewTip", "Create new empty template"), FAppStyle::GetBrush("Icons.Plus"), Actions.Create, true);
	AddButton(LOCTEXT("Save", "Save Template"), LOCTEXT("SaveTip", "Save the current template"), FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Save"), Actions.Save, Actions.CanSave);
	AddButton(LOCTEXT("SaveAll", "Save All Templates"), LOCTEXT("SaveAllTip", "Save all unsaved templates."), FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.SaveMultiple"), Actions.SaveAll, Actions.CanSaveAll);
	Separator();
	AddButton(LOCTEXT("Import", "Import Template"), LOCTEXT("ImportTip", "Import single or multiple templates from source files."), FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Import"), Actions.Import, true);
	AddButton(LOCTEXT("Export", "Export Item"), LOCTEXT("ExportTip", "Export all selected templates to new source files."), FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Export"), Actions.Export, true);
	Separator();
	AddButton(LOCTEXT("Close", "Close Template"), LOCTEXT("CloseTip", "Close current template and return to blank canvas."), FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Close"), Actions.Close, true);
	Separator();
	AddButton(LOCTEXT("Help", "Show Tutorial"), LOCTEXT("HelpTip", "Show a quick tutorial explaining how to work with this editor."), FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Tutorial"), Actions.Help, true);
	Separator();
	Bar->AddSlot().FillWidth(1.f)[SNullWidget::NullWidget];
	Bar->AddSlot().AutoWidth().Padding(5.f)[SNew(SBox).WidthOverride(300.f)[Search]];
	return SNew(SBorder).VAlign(VAlign_Center).BorderImage(FAppStyle::GetBrush("AssetEditorToolbar.Background"))
		.Padding(4.f, 4.f, 0.f, 4.f)[Bar];
}

TSharedRef<SWidget> MounteaInventoryEditorLayout::MakeBrowserDetails(TSharedRef<SWidget> Browser, TSharedRef<SWidget> Details,
	TAttribute<float> BrowserMinimum, TAttribute<float> DetailsMinimum)
{
	return SNew(SSplitter).Orientation(Orient_Horizontal).PhysicalSplitterHandleSize(1.f)
		+ SSplitter::Slot().Value(0.3f).MinSize(BrowserMinimum)
		[ SNew(SBorder).Padding(4.f).BorderImage(FAppStyle::GetBrush("Brushes.Background"))[Browser] ]
		+ SSplitter::Slot().Value(0.7f).MinSize(DetailsMinimum)[Details];
}

TSharedRef<SWidget> MounteaInventoryEditorLayout::MakeLayout(TSharedRef<SWidget> Toolbar, TSharedRef<SWidget> Content)
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().MinHeight(36.f)[Toolbar]
		+ SVerticalBox::Slot().FillHeight(1.f)[Content];
}
#undef LOCTEXT_NAMESPACE
