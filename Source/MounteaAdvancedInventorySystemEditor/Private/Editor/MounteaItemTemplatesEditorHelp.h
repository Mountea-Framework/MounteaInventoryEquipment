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

#include "CoreMinimal.h"

/**
 * 
 */
class SMounteaItemTemplatesEditorHelp : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaItemTemplatesEditorHelp) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> CreateNavigationButton(const FText& Label, int32 PageId);
	void SwitchToPage(int32 PageId);
	static FString GetHtmlPath(int32 PageId);

	TSharedPtr<class SWebBrowser> WebBrowser;
	int32 CurrentPageId = 0;
};
