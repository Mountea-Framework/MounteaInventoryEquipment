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
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateSearchFilter"

TArray<FString> FMounteaTemplateFilters::GetAvailableCategories()
{
	TArray<FString> returnValue;
	
	const UMounteaAdvancedInventorySettings* inventorySettings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!inventorySettings)
		return returnValue;
	
	const UMounteaAdvancedInventorySettingsConfig* inventoryConfig = inventorySettings->AdvancedInventorySettingsConfig.LoadSynchronous();
	if (!inventoryConfig)
		return returnValue;
	
	inventoryConfig->AllowedCategories.GetKeys(returnValue);
	return returnValue;
}

TArray<FString> FMounteaTemplateFilters::GetAvailableRarities()
{
	TArray<FString> returnValue;
	
	const UMounteaAdvancedInventorySettings* inventorySettings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!inventorySettings)
		return returnValue;
	
	const UMounteaAdvancedInventorySettingsConfig* inventoryConfig = inventorySettings->AdvancedInventorySettingsConfig.LoadSynchronous();
	if (!inventoryConfig)
		return returnValue;
	
	inventoryConfig->AllowedRarities.GetKeys(returnValue);
	return returnValue;
}

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
							: LOCTEXT("FiltersActive", "Filters");
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
	
	ActiveFilters.AllowedCategories = TSet<FString>(GetAvailableCategories());
	ActiveFilters.AllowedRarities = TSet<FString>(GetAvailableRarities());
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
	
	MenuBuilder.BeginSection("Conditions", LOCTEXT("ConditionsSection", "CONDITIONS"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("FilterByName", "Filter by Name"),
			LOCTEXT("FilterByNameTooltip", "If enabled, the search criteria input will be used to filter by Item Template name."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					ActiveFilters.bFilterByName = !ActiveFilters.bFilterByName;
					OnFiltersChangedDelegate.ExecuteIfBound();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return ActiveFilters.bFilterByName; })
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("FilterByGUID", "Filter by GUID"),
			LOCTEXT("FilterByGUIDTooltip", "If enabled, the search criteria input will be used to filter by Item Template guid."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					ActiveFilters.bFilterByGuid = !ActiveFilters.bFilterByGuid;
					OnFiltersChangedDelegate.ExecuteIfBound();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return ActiveFilters.bFilterByGuid; })
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("FilterByCategory", "Filter by Category"),
			LOCTEXT("FilterByCategoryTooltip", "If enabled, the search criteria input will be used to filter by Item Template Category."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					ActiveFilters.bFilterByCategory = !ActiveFilters.bFilterByCategory;
					OnFiltersChangedDelegate.ExecuteIfBound();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return ActiveFilters.bFilterByCategory; })
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("FilterByRarity", "Filter by Rarity"),
			LOCTEXT("FilterByRarityTooltip", "If enabled, the search criteria input will be used to filter by Item Template Rarity."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					ActiveFilters.bFilterByRarity = !ActiveFilters.bFilterByRarity;
					OnFiltersChangedDelegate.ExecuteIfBound();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return ActiveFilters.bFilterByRarity; })
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
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
	}
	MenuBuilder.EndSection();
	
	MenuBuilder.BeginSection("Display Conditions", LOCTEXT("DisplayConditions", "DISPLAY CONDITIONS"));
	{
		// Categories
		MenuBuilder.AddSubMenu(
	LOCTEXT("CategoriesFilter", "Categories Filter"),
	LOCTEXT("CategoriesFilterTooltip", "Show templates only with matching categories."),
	FNewMenuDelegate::CreateLambda([this](FMenuBuilder& SubMenuBuilder)
			{
				const TArray<FString> availableCategories = GetAvailableCategories();
				
				if (availableCategories.Num() == 0)
				{
					SubMenuBuilder.AddMenuEntry(
						LOCTEXT("NoCategories", "No categories available"),
						FText::GetEmpty(),
						FSlateIcon(),
						FUIAction(),
						NAME_None,
						EUserInterfaceActionType::None
					);
				}
				else
				{
					SubMenuBuilder.AddMenuEntry(
						LOCTEXT("ClearAllCategories", "Clear All"),
						LOCTEXT("ClearAllCategoriesTooltip", "Clear all category filters"),
						FSlateIcon(),
						FUIAction(
							FExecuteAction::CreateLambda([this]()
							{
								ActiveFilters.AllowedCategories.Empty();
								OnFiltersChangedDelegate.ExecuteIfBound();
							})
						)
					);
					
					SubMenuBuilder.AddSeparator();
					
					for (const FString& itemCategory : availableCategories)
					{
						SubMenuBuilder.AddMenuEntry(
							FText::FromString(itemCategory),
							FText::Format(LOCTEXT("ToggleCategoryTooltip", "Toggle {0} category filter"), FText::FromString(itemCategory)),
							FSlateIcon(),
							FUIAction(
								FExecuteAction::CreateLambda([this, itemCategory]()
								{
									ToggleCategory(itemCategory);
								}),
								FCanExecuteAction(),
								FIsActionChecked::CreateLambda([this, itemCategory]() 
								{ 
									return ActiveFilters.AllowedCategories.Contains(itemCategory); 
								})
							),
							NAME_None,
							EUserInterfaceActionType::ToggleButton
						);
					}
				}
			}),
			false,
			FSlateIcon()
		);
		
		// Rarities
		MenuBuilder.AddSubMenu(
	LOCTEXT("RaritiesFilter", "Rarities Filter"),
	LOCTEXT("RaritiesFilterTooltip", "Show templates only with matching rarities."),
	FNewMenuDelegate::CreateLambda([this](FMenuBuilder& SubMenuBuilder)
			{
				const TArray<FString> availableRarities = GetAvailableRarities();
				
				if (availableRarities.Num() == 0)
				{
					SubMenuBuilder.AddMenuEntry(
						LOCTEXT("NoRarities", "No rarities available"),
						FText::GetEmpty(),
						FSlateIcon(),
						FUIAction(),
						NAME_None,
						EUserInterfaceActionType::None
					);
				}
				else
				{
					SubMenuBuilder.AddMenuEntry(
						LOCTEXT("ClearAllRarities", "Clear All"),
						LOCTEXT("ClearAllRaritiesTooltip", "Clear all rarity filters"),
						FSlateIcon(),
						FUIAction(
							FExecuteAction::CreateLambda([this]()
							{
								ActiveFilters.AllowedRarities.Empty();
								OnFiltersChangedDelegate.ExecuteIfBound();
							})
						)
					);
					
					SubMenuBuilder.AddSeparator();
					
					for (const FString& itemRarity : availableRarities)
					{
						SubMenuBuilder.AddMenuEntry(
							FText::FromString(itemRarity),
							FText::Format(LOCTEXT("ToggleRariryTooltip", "Toggle {0} rarity filter"), FText::FromString(itemRarity)),
							FSlateIcon(),
							FUIAction(
								FExecuteAction::CreateLambda([this, itemRarity]()
								{
									ToggleRarity(itemRarity);
								}),
								FCanExecuteAction(),
								FIsActionChecked::CreateLambda([this, itemRarity]() 
								{ 
									return ActiveFilters.AllowedRarities.Contains(itemRarity); 
								})
							),
							NAME_None,
							EUserInterfaceActionType::ToggleButton
						);
					}
				}
			}),
			false,
			FSlateIcon()
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

void SMounteaInventoryTemplateSearchFilter::ToggleCategory(const FString& Category)
{
	if (ActiveFilters.AllowedCategories.Contains(Category))
		ActiveFilters.AllowedCategories.Remove(Category);
	else
		ActiveFilters.AllowedCategories.Add(Category);
	
	OnFiltersChangedDelegate.ExecuteIfBound();
}

TArray<FString> SMounteaInventoryTemplateSearchFilter::GetAvailableCategories() const
{
	return ActiveFilters.GetAvailableCategories();
}

void SMounteaInventoryTemplateSearchFilter::ToggleRarity(const FString& Rarity)
{
	if (ActiveFilters.AllowedRarities.Contains(Rarity))
		ActiveFilters.AllowedRarities.Remove(Rarity);
	else
		ActiveFilters.AllowedRarities.Add(Rarity);
	
	OnFiltersChangedDelegate.ExecuteIfBound();
}

TArray<FString> SMounteaInventoryTemplateSearchFilter::GetAvailableRarities() const
{
	return ActiveFilters.GetAvailableRarities();
}

void SMounteaInventoryTemplateSearchFilter::ClearSearch()
{
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
	
	if (ActiveFilters.bFilterByCategory && ActiveFilters.AllowedCategories.Contains(itemTemplatePtr->ItemCategory))
		return true;
	
	if (ActiveFilters.bFilterByRarity && ActiveFilters.AllowedRarities.Contains(itemTemplatePtr->ItemRarity))
		return true;
	
	if (ActiveFilters.bFilterByName && itemTemplatePtr->DisplayName.ToString().ToLower().Contains(searchString))
		return true;
	
	if (ActiveFilters.bFilterByName && itemTemplatePtr->GetPathName().ToLower().Contains(searchString))
		return true;
	
	if (ActiveFilters.bFilterByGuid && itemTemplatePtr->Guid.ToString().ToLower().Contains(searchString))
		return true;
	
	if (ActiveFilters.bFilterByCategory && itemTemplatePtr->ItemCategory.ToLower().Contains(searchString))
		return true;
	
	if (ActiveFilters.bFilterByRarity && itemTemplatePtr->ItemRarity.ToLower().Contains(searchString))
		return true;
	
	return false;
}

#undef LOCTEXT_NAMESPACE
