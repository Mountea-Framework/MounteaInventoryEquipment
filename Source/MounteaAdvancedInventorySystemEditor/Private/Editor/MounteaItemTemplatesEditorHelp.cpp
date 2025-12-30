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

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Definitions/MounteaAdvancedInventoryEditorTypes.h"
#include "Runtime/WebBrowser/Public/SWebBrowser.h"
#include "Settings/MounteaAdvancedInventorySettingsEditor.h"
#include "Statics/MounteaAdvancedInventorySystemEditorStatics.h"
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
				.OnConsoleMessage_Raw(this, &SMounteaItemTemplatesEditorHelp::HandleConsoleMessage)
				.InitialURL(TEXT("about:blank"))
			]
		]		
	];
}

void SMounteaItemTemplatesEditorHelp::Reset()
{
	CurrentPageId = 0;
	WebBrowser->LoadString(TEXT(""), TEXT("about:blank"));
}

void SMounteaItemTemplatesEditorHelp::Start()
{
	CurrentPageId = 0;
	SwitchToPage(CurrentPageId);
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
				.Font_Lambda([this, PageId]()
				{
					return CurrentPageId == PageId ? FCoreStyle::GetDefaultFontStyle("Bold", 12) : FCoreStyle::GetDefaultFontStyle("Regular", 10);
				})
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
	{
		const FString htmlWithCss = InjectSharedAssets(htmlContent);
		
		const FString baseUrl = FString::Printf(TEXT("file:///%s/"), *FPaths::GetPath(filePath).Replace(TEXT("\\"), TEXT("/")));
		
		WebBrowser->LoadString(htmlWithCss, *baseUrl);
		
		RegisterActiveTimer(0.1f, FWidgetActiveTimerDelegate::CreateLambda(
	[this](double, float) -> EActiveTimerReturnType
			{
				if (WebBrowser.IsValid())
				{
					WebBrowser->ExecuteJavascript(TEXT(
						"(function() {"
						"  var start = window.pageYOffset || document.documentElement.scrollTop;"
						"  var startTime = null;"
						"  var duration = 200;"
						"  function animation(currentTime) {"
						"    if (startTime === null) startTime = currentTime;"
						"    var timeElapsed = currentTime - startTime;"
						"    var progress = Math.min(timeElapsed / duration, 1);"
						"    var ease = 1 - Math.pow(1 - progress, 3);"
						"    window.scrollTo(0, start * (1 - ease));"
						"    if (timeElapsed < duration) {"
						"      requestAnimationFrame(animation);"
						"    }"
						"  }"
						"  requestAnimationFrame(animation);"
						"})();"
					));
				}
				return EActiveTimerReturnType::Stop;
			}
		));	
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Failed to load: %s"), *filePath);
}

FString SMounteaItemTemplatesEditorHelp::InjectSharedAssets(const FString& HtmlContent)
{
	const UMounteaAdvancedInventorySettingsEditor* editorSettings = GetDefault<UMounteaAdvancedInventorySettingsEditor>();
	if (!editorSettings)
		return HtmlContent;
	
	FString result = HtmlContent;
	
	// Inject CSS
	if (!editorSettings->SharedStylesheetPath.FilePath.IsEmpty())
	{
		FString cssContent;
		const FString cssPath = FPaths::ConvertRelativePathToFull(editorSettings->SharedStylesheetPath.FilePath);
		
		if (FFileHelper::LoadFileToString(cssContent, *cssPath))
		{
			const FString styleTag = FString::Printf(TEXT("<style>%s</style>"), *cssContent);
			
			if (!result.Contains(styleTag))
			{
				const int32 headEndPos = result.Find(TEXT("</head>"), ESearchCase::IgnoreCase);
				
				if (headEndPos != INDEX_NONE)
					result.InsertAt(headEndPos, styleTag);
				else
					result = styleTag + result;
			}
		}
	}
	
	// Inject Script
	if (!editorSettings->SharedScriptPath.FilePath.IsEmpty())
	{
		FString scriptContent;
		const FString scriptPath = FPaths::ConvertRelativePathToFull(editorSettings->SharedScriptPath.FilePath);
		
		if (FFileHelper::LoadFileToString(scriptContent, *scriptPath))
		{
			const FString scriptTag = FString::Printf(TEXT("<script>%s</script>"), *scriptContent);
			
			if (!result.Contains(scriptTag))
			{
				const int32 bodyEndPos = result.Find(TEXT("</body>"), ESearchCase::IgnoreCase);
				
				if (bodyEndPos != INDEX_NONE)
					result.InsertAt(bodyEndPos, scriptTag);
				else
					result += scriptTag;
			}
		}
	}
	
	return result;
}

FString SMounteaItemTemplatesEditorHelp::GetHtmlPath(const int32 PageId)
{
	if (const UMounteaAdvancedInventorySettingsEditor* editorSettings = GetDefault<UMounteaAdvancedInventorySettingsEditor>())
		if (const FItemTemplateEditorPageConfig* pageConfig = editorSettings->EditorTemplatePages.Find(PageId))
			return FPaths::ConvertRelativePathToFull(pageConfig->PageFile.FilePath);
	
	return FString();
}

void SMounteaItemTemplatesEditorHelp::HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity)
{
	if (Message.StartsWith(TEXT("MIAE_LINK:")))
	{
		const FString data = Message.RightChop(10);
		
		FString dataType, url;
		if (data.Split(TEXT(":"), &dataType, &url))
		{
			if (dataType.Equals(TEXT("content")))
			{
				FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
				contentBrowserModule.Get().SyncBrowserToAssets(TArray<FAssetData>());
			}
			else if (dataType.Equals(TEXT("page")))
			{
				const int32 pageId = FCString::Atoi(*url);
				SwitchToPage(pageId);
			}
			else if (dataType.Equals(TEXT("external")))
			{
				FPlatformProcess::LaunchURL(*url, nullptr, nullptr);
			}
			else if (dataType.Equals(TEXT("settings")))
			{
				UMounteaAdvancedInventorySystemEditorStatics::OpenSettings(
					"Project", TEXT("Mountea Framework"), TEXT("Mountea Inventory System"));
			}
			else if (dataType.Equals(TEXT("editor-settings")))
			{
				UMounteaAdvancedInventorySystemEditorStatics::OpenSettings(
					"Project",  TEXT("Mountea Framework"), TEXT("Mountea Inventory System (Editor)"));
			}
			else if (dataType.Equals(TEXT("inventory-configuration")))
			{
				UMounteaAdvancedInventorySystemEditorStatics::OpenInventoryConfig();
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
