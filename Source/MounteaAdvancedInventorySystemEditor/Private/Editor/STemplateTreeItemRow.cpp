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

void STemplateTreeItemRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	STableRow<TSharedPtr<FTemplateTreeItem>>::Construct(
		STableRow<TSharedPtr<FTemplateTreeItem>>::FArguments()
		.Padding(InArgs._Padding)
		//.Style(FAppStyle::Get(), "TableView.NoHoverTableRow")
		.Style(FAppStyle::Get(), "MAISStyleSet.CustomTableRow")
		.ShowSelection(false)
		.Content()
		[
			InArgs._Content.Widget
		],
		InOwnerTableView
	);
	
	SetCanTick(false);
	SetVisibility(EVisibility::SelfHitTestInvisible);
}

FReply STemplateTreeItemRow::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::Cleared);
}

void STemplateTreeItemRow::OnFocusLost(const FFocusEvent& InFocusEvent)
{
}

bool STemplateTreeItemRow::SupportsKeyboardFocus() const
{
	return false;
}

FCursorReply STemplateTreeItemRow::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	return FCursorReply::Cursor(EMouseCursor::Default);
}
