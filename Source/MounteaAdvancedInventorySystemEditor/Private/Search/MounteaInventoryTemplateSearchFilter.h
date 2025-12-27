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

class UMounteaInventoryItemTemplate;

#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(FOnSearchTextChanged, const FText&);
DECLARE_DELEGATE(FOnFiltersChanged);

struct FMounteaTemplateFilters
{
	bool bShowDirty = true;
	bool bShowClean = true;
	bool bShowTransient = true;
	TSet<FName> AllowedCategories;
	TSet<FName> AllowedRarities;
	
	void Reset()
	{
		bShowDirty = true;
		bShowClean = true;
		bShowTransient = true;
		AllowedCategories.Empty();
		AllowedRarities.Empty();
	}
	
	bool IsDefault() const
	{
		return bShowDirty && bShowClean && bShowTransient && 
			   AllowedCategories.Num() == 0 && AllowedRarities.Num() == 0;
	}
	
	bool PassesFilter(const bool bIsDirty, const bool bIsTransient, const FName Category, const FName Rarity) const
	{
		if (bIsTransient && !bShowTransient)
			return false;
		
		if (bIsDirty && !bShowDirty)
			return false;
		
		if (!bIsDirty && !bIsTransient && !bShowClean)
			return false;
		
		if (AllowedCategories.Num() > 0 && !AllowedCategories.Contains(Category))
			return false;
		
		if (AllowedRarities.Num() > 0 && !AllowedRarities.Contains(Rarity))
			return false;
		
		return true;
	}
};

class SMounteaInventoryTemplateSearchFilter : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaInventoryTemplateSearchFilter) {}
		SLATE_EVENT(FOnSearchTextChanged, OnSearchTextChanged)
		SLATE_EVENT(FOnFiltersChanged, OnFiltersChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	const FMounteaTemplateFilters& GetActiveFilters() const { return ActiveFilters; }
	FText GetSearchText() const { return CurrentSearchText; }
	void ClearSearch();

private:
	TSharedRef<SWidget> CreateFilterMenu();
	void OnResetFilters();
	void OnSearchChanged(const FText& InText);
	
	FOnSearchTextChanged OnSearchTextChangedDelegate;
	FOnFiltersChanged OnFiltersChangedDelegate;
	
	TSharedPtr<SEditableTextBox> SearchBox;
	FMounteaTemplateFilters ActiveFilters;
	FText CurrentSearchText;
};
