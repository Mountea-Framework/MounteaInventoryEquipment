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
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_RetVal(FString, FOnGetSourceFilePath);

/**
 * Read-only display of an asset's import source-file path, with a button to reveal
 * the containing folder in the OS file explorer. Used by MounteaSourceFilePathDetailsCustomization.
 */
class SMounteaSourceFilePathWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaSourceFilePathWidget)
		: _OnGetFilePath()
		, _Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
	{}
	SLATE_EVENT(FOnGetSourceFilePath, OnGetFilePath)
	SLATE_ATTRIBUTE(FSlateFontInfo, Font)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	bool IsFilePathValid() const;
	FReply OnOpenFolderClicked() const;

private:
	FOnGetSourceFilePath OnGetFilePath;
	TAttribute<FSlateFontInfo> Font;
};
