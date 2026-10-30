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

#pragma once

class SMounteaInventoryWebBrowserWrapper;
enum class EWebBrowserConsoleLogSeverity;
class SWebBrowser;

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Definitions/MounteaAdvancedInventoryEditorTypes.h"

/**
 * 
 */
class SMounteaItemTemplatesEditorHelp : public SCompoundWidget
{
public:
	using FPageMap = TMap<int32, FItemTemplateEditorPageConfig>;
	SLATE_BEGIN_ARGS(SMounteaItemTemplatesEditorHelp)
	{
	}
		SLATE_ARGUMENT(TOptional<FPageMap>, Pages)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);
	
	void Reset();
	void Start();

private:
	friend class FMounteaCraftingRecipesHelpTest;
	TSharedRef<SWidget> CreateNavigationButton(const FText& Label, int32 PageId);
	void SwitchToPage(int32 PageId);
	FString GetHtmlPath(int32 PageId) const;
	void HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity);


	TMap<int32, FItemTemplateEditorPageConfig> Pages;
	TSharedPtr<SMounteaInventoryWebBrowserWrapper> WebBrowser;
	TSharedPtr<SWidget> InputBlockerOverlay;
	bool bBlockInput = false;
	int32 CurrentPageId = 0;
};
