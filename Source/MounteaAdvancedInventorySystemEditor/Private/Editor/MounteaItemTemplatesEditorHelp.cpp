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


#include "MounteaItemTemplatesEditorHelp.h"

#include "Definitions/MounteaAdvancedInventoryEditorTypes.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/WebBrowser/Public/SWebBrowser.h"
#include "Settings/MounteaAdvancedInventorySettingsEditor.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventorySystemEditorHelp"

void SMounteaItemTemplatesEditorHelp::Construct(const FArguments& InArgs)
{
	TSharedRef<SVerticalBox> navigationBox = SNew(SVerticalBox);

	if (const UMounteaAdvancedInventorySettingsEditor* editorSettings = GetDefault<UMounteaAdvancedInventorySettingsEditor>())
	{
		TArray<int32> pageIds;
		editorSettings->EditorTemplatePages.GetKeys(pageIds);
		pageIds.Sort();
		
		for (const int32 pageId : pageIds)
		{
			if (const FItemTemplateEditorPageConfig* pageConfig = editorSettings->EditorTemplatePages.Find(pageId))
			{
				navigationBox->AddSlot()
				.AutoHeight()
				[
					CreateNavigationButton(pageConfig->PageTitle, pageId)
				];
			}
		}
	}
	
	ChildSlot
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor::Transparent)
		.Padding(0)
		[
			SNew(SHorizontalBox)
		
			+ SHorizontalBox::Slot()
			.FillWidth(0.25f)
			.Padding(0)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Brushes.Background"))
				.Padding(25)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						navigationBox
					]
				]
			]
			
			+ SHorizontalBox::Slot()
			.FillWidth(0.75f)
			[
				SAssignNew(WebBrowser, SWebBrowser)
				.ShowControls(false)
				.ShowAddressBar(false)
				.ShowInitialThrobber(true)
				.SupportsTransparency(true)
			]
		]		
	];
	
	SwitchToPage(0);
}

TSharedRef<SWidget> SMounteaItemTemplatesEditorHelp::CreateNavigationButton(const FText& Label, int32 PageId)
{
	TSharedPtr<SBorder> borderWidget;
	
	TSharedRef<SButton> button = SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "NoBorder")
		.OnClicked_Lambda([this, PageId]()
		{
			SwitchToPage(PageId);
			return FReply::Handled();
		})
		.ContentPadding(4)
		[
			SAssignNew(borderWidget, SBorder)
			.Padding(FMargin(10, 8))
			[
				SNew(STextBlock)
				.Text(Label)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
			]
		];
	
	borderWidget->SetBorderImage(TAttribute<const FSlateBrush*>::CreateLambda([this, PageId, WeakButton = TWeakPtr<SButton>(button)]() -> const FSlateBrush*
	{
		const bool bSelected = (CurrentPageId == PageId);
		const bool bHovered = WeakButton.IsValid() && WeakButton.Pin()->IsHovered();
		
		if (bSelected && bHovered)
			return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.SelectedHovered");
		if (bSelected)
			return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.Selected");
		if (bHovered)
			return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.Hovered");
		
		return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.Normal");
	}));
	
	return button;
}

void SMounteaItemTemplatesEditorHelp::SwitchToPage(const int32 PageId)
{
	if (!WebBrowser.IsValid())
		return;
	
	CurrentPageId = PageId;
	
	FString htmlContent;
	const FString filePath = GetHtmlPath(PageId);
	
	if (FFileHelper::LoadFileToString(htmlContent, *filePath))
		WebBrowser->LoadString(htmlContent, TEXT("main"));
	else
		UE_LOG(LogTemp, Error, TEXT("Failed to load: %s"), *filePath);
}

FString SMounteaItemTemplatesEditorHelp::GetHtmlPath(const int32 PageId)
{
	if (const UMounteaAdvancedInventorySettingsEditor* editorSettings = GetDefault<UMounteaAdvancedInventorySettingsEditor>())
		if (const FItemTemplateEditorPageConfig* pageConfig = editorSettings->EditorTemplatePages.Find(PageId))
			return FPaths::ConvertRelativePathToFull(pageConfig->PageFile.FilePath);
	
	return FString();
}

#undef LOCTEXT_NAMESPACE