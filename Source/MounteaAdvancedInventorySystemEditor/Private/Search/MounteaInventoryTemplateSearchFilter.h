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
	
	bool bFilterByName = true;
	bool bFilterByGuid = true;
	bool bFilterByCategory = false;
	bool bFilterByRarity = false;
		
	TSet<FString> AllowedCategories;
	TSet<FString> AllowedRarities;
	
	void Reset()
	{
		bShowDirty = true;
		bShowClean = true;
		bFilterByName = true;
		bFilterByGuid = true;
		bFilterByCategory = false;
		bFilterByRarity = false;
		AllowedCategories.Empty();
		AllowedRarities.Empty();
	}
	
	bool IsDefault() const
	{
		return bShowDirty && bShowClean && bFilterByName && bFilterByGuid && !bFilterByCategory && !bFilterByRarity &&
			   AllowedCategories.Num() == 0 && AllowedRarities.Num() == 0;
	}
	
	bool PassesFilter(const bool bIsDirty) const
	{		
		if (bIsDirty && !bShowDirty)
			return false;
		
		if (!bIsDirty && !bShowClean)
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
	bool HasSearchText() const { return !CurrentSearchText.IsEmptyOrWhitespace(); }
	void ClearSearch();
	
	bool DoesTemplateMatchSearch(TWeakObjectPtr<class UMounteaInventoryItemTemplate> Template) const;

private:
	TSharedRef<SWidget> CreateFilterMenu();
	void OnResetFilters();
	void OnSearchChanged(const FText& InText);
	
	FOnSearchTextChanged OnSearchTextChangedDelegate;
	FOnFiltersChanged OnFiltersChangedDelegate;
	
	TSharedPtr<SSearchBox> SearchBox;
	FMounteaTemplateFilters ActiveFilters;
	FText CurrentSearchText;
};
