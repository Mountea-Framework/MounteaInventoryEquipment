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
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/STreeView.h"
#include "Framework/Application/SlateApplication.h"

struct FTemplateTreeItem;

/** Presentation shared by item templates and recipes. Keep styling in one place. */
struct FMounteaInventoryAssetRowPresentation
{
	TAttribute<FText> Name;
	TAttribute<FText> Tooltip;
	TAttribute<FSlateColor> NameColor = FSlateColor::UseForeground();
	TAttribute<bool> IsSelected = false;
	TAttribute<EVisibility> ActionsVisibility = EVisibility::Visible;
	FString AssetPath;
	FOnClicked Select, Locate, Duplicate, Delete;
};

namespace MounteaInventoryTreeRows
{
	const FTableRowStyle* GetRowStyle();
	FText CategoryLabel(const FString& Category);
	FText DisplayName(const FText& Name, bool IsModified);
	TSharedRef<SWidget> MakeCategoryContent(const FString& Category, TAttribute<int32> Count);
	TSharedRef<SWidget> MakeAssetContent(const FMounteaInventoryAssetRowPresentation& Presentation);
	FReply LocateAsset(TWeakObjectPtr<UObject> Asset);
	FString AssetPath(const UObject* Asset);

	/** Match item-template Ctrl and same-category Shift selection. */
	template<typename ItemType>
	FReply SelectItem(const TSharedPtr<STreeView<ItemType>>& Tree, ItemType Item, const TArray<ItemType>& Roots)
	{
		if (!Tree.IsValid() || !Item.IsValid())
			return FReply::Handled();

		const FModifierKeysState keys = FSlateApplication::Get().GetModifierKeys();

		if (keys.IsControlDown())
			Tree->SetItemSelection(Item, !Tree->IsItemSelected(Item), ESelectInfo::OnMouseClick);
		else if (keys.IsShiftDown())
		{
			const TArray<ItemType> selected = Tree->GetSelectedItems();

			if (selected.IsEmpty())
				Tree->SetSelection(Item, ESelectInfo::OnMouseClick);
			else
			{
				for (const ItemType& root : Roots)
				{
					const int32 lastIndex = root->Children.Find(selected.Last());
					const int32 currentIndex = root->Children.Find(Item);

					if (lastIndex == INDEX_NONE || currentIndex == INDEX_NONE)
						continue;

					Tree->ClearSelection();

					for (int32 index = FMath::Min(lastIndex, currentIndex); index <= FMath::Max(lastIndex, currentIndex); ++index)
						Tree->SetItemSelection(root->Children[index], true, ESelectInfo::OnMouseClick);

					break;
				}
			}
		}
		else
			Tree->SetSelection(Item, ESelectInfo::OnMouseClick);

		return FReply::Handled();
	}
}

template<typename ItemType>
class SMounteaInventoryTreeItemRow : public STableRow<ItemType>
{
public:
	SLATE_BEGIN_ARGS(SMounteaInventoryTreeItemRow)
		: _Padding(FMargin(0.0f))
		{
		}
		SLATE_ARGUMENT(FMargin, Padding)
		SLATE_DEFAULT_SLOT(FArguments, Content)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		STableRow<ItemType>::Construct(typename STableRow<ItemType>::FArguments()
			.Padding(InArgs._Padding)
			.Style(MounteaInventoryTreeRows::GetRowStyle())
			.ShowSelection(false)
			[InArgs._Content.Widget], InOwnerTableView);

		this->SetCanTick(false);
		this->SetVisibility(EVisibility::SelfHitTestInvisible);
	}

protected:
	virtual FReply OnFocusReceived(const FGeometry&, const FFocusEvent&) override
	{
		return FReply::Handled().SetUserFocus(this->AsShared(), EFocusCause::Cleared);
	}

	virtual void OnFocusLost(const FFocusEvent&) override
	{
	}
	virtual bool SupportsKeyboardFocus() const override
	{
		return false;
	}

	virtual FCursorReply OnCursorQuery(const FGeometry&, const FPointerEvent&) const override
	{
		return FCursorReply::Cursor(EMouseCursor::Default);
	}
};

using STemplateTreeItemRow = SMounteaInventoryTreeItemRow<TSharedPtr<FTemplateTreeItem>>;
