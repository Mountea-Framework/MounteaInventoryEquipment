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
#include "Widgets/Views/STableRow.h"

struct FTemplateTreeItem;

class STemplateTreeItemRow : public STableRow<TSharedPtr<FTemplateTreeItem>>
{
public:
	SLATE_BEGIN_ARGS(STemplateTreeItemRow)
		: _Padding(FMargin(0.0f))
		{}    
	SLATE_ARGUMENT(FMargin, Padding)
	SLATE_DEFAULT_SLOT(FArguments, Content)

SLATE_END_ARGS()


void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

protected:
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual bool SupportsKeyboardFocus() const override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
};