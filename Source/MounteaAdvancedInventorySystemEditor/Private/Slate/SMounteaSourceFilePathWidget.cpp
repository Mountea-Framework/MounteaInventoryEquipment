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

#include "SMounteaSourceFilePathWidget.h"

#include "Misc/Paths.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SMounteaSourceFilePathWidget"

void SMounteaSourceFilePathWidget::Construct(const FArguments& InArgs)
{
	OnGetFilePath = InArgs._OnGetFilePath;
	Font = InArgs._Font;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
			{
				return FText::FromString(OnGetFilePath.IsBound() ? OnGetFilePath.Execute() : TEXT(""));
			})
			.Font(Font)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ContentPadding(FMargin(1, 0))
			.OnClicked(this, &SMounteaSourceFilePathWidget::OnOpenFolderClicked)
			.IsEnabled(this, &SMounteaSourceFilePathWidget::IsFilePathValid)
			.ToolTipText(LOCTEXT("SMounteaSourceFilePathWidget_OpenFolderTooltip", "Open the folder containing the source file"))
			[
				SNew(SImage)
				.Image(FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.FolderOpen"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]
	];
}

bool SMounteaSourceFilePathWidget::IsFilePathValid() const
{
	const FString currentPath = OnGetFilePath.IsBound() ? OnGetFilePath.Execute() : TEXT("");
	return !currentPath.IsEmpty() && FPaths::FileExists(currentPath);
}

FReply SMounteaSourceFilePathWidget::OnOpenFolderClicked() const
{
	const FString currentPath = OnGetFilePath.IsBound() ? OnGetFilePath.Execute() : TEXT("");
	if (!currentPath.IsEmpty())
		FPlatformProcess::ExploreFolder(*FPaths::GetPath(currentPath));

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
