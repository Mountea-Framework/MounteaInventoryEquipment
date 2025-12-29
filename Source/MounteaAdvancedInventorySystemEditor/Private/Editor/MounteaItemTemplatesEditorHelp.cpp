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

#include "Interfaces/IPluginManager.h"
#include "Runtime/WebBrowser/Public/SWebBrowser.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventorySystemEditorHelp"

void SMounteaItemTemplatesEditorHelp::Construct(const FArguments& InArgs)
{
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
				.Padding(10)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							CreateNavigationButton(LOCTEXT("Overview", "Overview"), 0)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							CreateNavigationButton(LOCTEXT("Filters", "Filters"), 1)
						]
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
	return SNew(SButton)
		.Text(Label)
		.OnClicked_Lambda([this, PageId]()
		{
			SwitchToPage(PageId);
			return FReply::Handled();
		});
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

FString SMounteaItemTemplatesEditorHelp::GetHtmlPath(const int32 pageId)
{
	const FString pluginDir = IPluginManager::Get().FindPlugin("MounteaAdvancedInventorySystem")->GetBaseDir();
	const FString relativePath = FPaths::Combine(pluginDir, TEXT("Resources"), TEXT("Help"), FString::Printf(TEXT("page_%d.html"), pageId));
	return FPaths::ConvertRelativePathToFull(relativePath);
}

#undef LOCTEXT_NAMESPACE