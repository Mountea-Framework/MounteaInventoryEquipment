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


#include "MounteaInventoryTemplateSearchFilter.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateSearchFilter"

void SMounteaInventoryTemplateSearchFilter::Construct(const FArguments& InArgs)
{
	OnSearchTextChangedDelegate = InArgs._OnSearchTextChanged;
	OnFiltersChangedDelegate = InArgs._OnFiltersChanged;
	
	ChildSlot
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SAssignNew(SearchBox, SSearchBox)
			.HintText(LOCTEXT("SearchTemplates", "Search templates..."))
			.OnTextChanged(this, &SMounteaInventoryTemplateSearchFilter::OnSearchChanged)
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4, 0, 2, 0)
		[
			SNew(SComboButton)
			.ComboButtonStyle(FAppStyle::Get(), "SimpleComboButton")
			.ForegroundColor(FSlateColor::UseStyle())
			.ContentPadding(FMargin(1, 0))
			.OnGetMenuContent(this, &SMounteaInventoryTemplateSearchFilter::CreateFilterMenu)
			.HasDownArrow(true)
			.ButtonContent()
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("Icons.Filter"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(4, 0, 0, 0)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						return ActiveFilters.IsDefault() 
							? LOCTEXT("Filters", "Filters") 
							: LOCTEXT("FiltersActive", "Filters (Active)");
					})
					.ColorAndOpacity_Lambda([this]()
					{
						return ActiveFilters.IsDefault() 
							? FSlateColor::UseForeground() 
							: FSlateColor(FLinearColor(0.4f, 0.7f, 1.0f));
					})
				]
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateSearchFilter::CreateFilterMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	MenuBuilder.BeginSection("Manage", LOCTEXT("ManageSection", "MANAGE"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ResetFilters", "Reset Filters"),
			LOCTEXT("ResetFiltersTooltip", "Reset all filters to default"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateSP(this, &SMounteaInventoryTemplateSearchFilter::OnResetFilters))
		);
	}
	MenuBuilder.EndSection();
	
	MenuBuilder.BeginSection("CommonFilters", LOCTEXT("CommonFiltersSection", "COMMON FILTERS"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowDirty", "Show Dirty"),
			LOCTEXT("ShowDirtyTooltip", "Show templates with unsaved changes"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					ActiveFilters.bShowDirty = !ActiveFilters.bShowDirty;
					OnFiltersChangedDelegate.ExecuteIfBound();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return ActiveFilters.bShowDirty; })
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowClean", "Show Clean"),
			LOCTEXT("ShowCleanTooltip", "Show templates without unsaved changes"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					ActiveFilters.bShowClean = !ActiveFilters.bShowClean;
					OnFiltersChangedDelegate.ExecuteIfBound();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return ActiveFilters.bShowClean; })
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowTransient", "Show Transient"),
			LOCTEXT("ShowTransientTooltip", "Show unsaved new templates"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					ActiveFilters.bShowTransient = !ActiveFilters.bShowTransient;
					OnFiltersChangedDelegate.ExecuteIfBound();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return ActiveFilters.bShowTransient; })
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
	}
	MenuBuilder.EndSection();
	
	return MenuBuilder.MakeWidget();
}

void SMounteaInventoryTemplateSearchFilter::OnResetFilters()
{
	ActiveFilters.Reset();
	OnFiltersChangedDelegate.ExecuteIfBound();
}

void SMounteaInventoryTemplateSearchFilter::OnSearchChanged(const FText& InText)
{
	CurrentSearchText = InText;
	OnSearchTextChangedDelegate.ExecuteIfBound(InText);
}

void SMounteaInventoryTemplateSearchFilter::ClearSearch()
{
	if (SearchBox.IsValid())
	{
		//SearchBox->SetText(FText::GetEmpty());
	}
	CurrentSearchText = FText::GetEmpty();
	OnSearchTextChangedDelegate.ExecuteIfBound(FText::GetEmpty());
}

bool SMounteaInventoryTemplateSearchFilter::DoesTemplateMatchSearch(const TWeakObjectPtr<UMounteaInventoryItemTemplate> Template) const
{
	if (!Template.IsValid())
		return false;
	
	if (CurrentSearchText.IsEmptyOrWhitespace())
		return true;
	
	const FString searchString = CurrentSearchText.ToString().ToLower();
	const UMounteaInventoryItemTemplate* itemTemplatePtr = Template.Get();
	
	if (itemTemplatePtr->DisplayName.ToString().ToLower().Contains(searchString))
		return true;
	
	if (itemTemplatePtr->GetPathName().ToLower().Contains(searchString))
		return true;
	
	if (itemTemplatePtr->Guid.ToString().ToLower().Contains(searchString))
		return true;
	
	return false;
}

#undef LOCTEXT_NAMESPACE
