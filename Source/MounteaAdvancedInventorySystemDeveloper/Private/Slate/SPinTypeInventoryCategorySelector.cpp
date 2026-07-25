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

#if WITH_EDITOR

#include "Slate/SPinTypeInventoryCategorySelector.h"

#include "Algo/Transform.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "GameplayTagContainer.h"
#include "ScopedTransaction.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaCraftingStatics.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

void SPinTypeInventoryCategorySelector::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);

	TArray<FString> categories;
	GetCategoriesFromSettings(categories);

	CategoryOptions.Empty(categories.Num());
	for (const FString& category : categories)
	{
		CategoryOptions.Add(MakeShared<FString>(category));
	}

	if (CategoryOptions.Num() > 0 && GraphPinObj && GraphPinObj->GetDefaultAsString().IsEmpty() && GraphPinObj->LinkedTo.Num() == 0)
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *CategoryOptions[0]);
}

TSharedRef<SWidget> SPinTypeInventoryCategorySelector::GetDefaultValueWidget()
{
	return SNew(SBox)
		.Visibility(this, &SPinTypeInventoryCategorySelector::GetDefaultValueVisibility)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.OptionsSource(&CategoryOptions)
			.OnGenerateWidget(this, &SPinTypeInventoryCategorySelector::GenerateCategoryWidget)
			.OnSelectionChanged(this, &SPinTypeInventoryCategorySelector::OnCategorySelected)
			[
				SNew(STextBlock)
				.Text(this, &SPinTypeInventoryCategorySelector::GetSelectedCategoryText)
				.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			]
		];
}

EVisibility SPinTypeInventoryCategorySelector::GetDefaultValueVisibility() const
{
	return (GraphPinObj && GraphPinObj->LinkedTo.Num() > 0) ? EVisibility::Collapsed : EVisibility::Visible;
}

void SPinTypeInventoryCategorySelector::OnCategorySelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid() || Selection->IsEmpty() || !GraphPinObj)
		return;

	const FString selectedCategory = *Selection;
	const FString currentDefault = GraphPinObj->GetDefaultAsString().TrimQuotes();
	if (currentDefault == selectedCategory)
		return;

	const FScopedTransaction transaction(NSLOCTEXT("InventoryCategoryPin", "ChangeCategory", "Change Inventory Category"));
	GraphPinObj->Modify();

	const UEdGraphSchema* graphSchema = GraphPinObj->GetSchema();
	if (!graphSchema)
		return;

	graphSchema->TrySetDefaultValue(*GraphPinObj, selectedCategory);
}

FText SPinTypeInventoryCategorySelector::GetSelectedCategoryText() const
{
	if (!GraphPinObj)
		return FText::GetEmpty();

	const FString currentDefault = GraphPinObj->GetDefaultAsString().TrimQuotes();
	return currentDefault.IsEmpty() ? FText::FromString(TEXT("None")) : FText::FromString(currentDefault);
}

TSharedRef<SWidget> SPinTypeInventoryCategorySelector::GenerateCategoryWidget(TSharedPtr<FString> Item)
{
	return SNew(STextBlock)
		.Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty())
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

void SPinTypeInventoryCategorySelector::GetCategoriesFromSettings(TArray<FString>& OutCategories) const
{
	OutCategories.Reset();

	if (GraphPinObj && GraphPinObj->PinType.PinCategory == UEdGraphSchema_K2::PC_String)
	{
		const auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
		if (!IsValid(inventorySettings))
		{
			OutCategories.Add(TEXT("All"));
			return;
		}

		inventorySettings->GetAllowedCategories().GetKeys(OutCategories);
		return;
	}

	TArray<FGameplayTag> categoryTags;
	UMounteaCraftingStatics::GetAllowedInventoryCategoryTags(categoryTags);
	Algo::Transform(
		categoryTags,
		OutCategories,
		[](const FGameplayTag& CategoryTag)
		{
			return CategoryTag.ToString();
		});
}

#endif
