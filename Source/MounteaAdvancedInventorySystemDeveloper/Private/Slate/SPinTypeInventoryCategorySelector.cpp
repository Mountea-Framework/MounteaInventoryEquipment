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

#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "ScopedTransaction.h"
#include "Statics/MounteaCraftingStatics.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

void SPinTypeInventoryCategorySelector::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);

	TArray<FGameplayTag> categoryTags;
	GetCategoriesFromSettings(categoryTags);

	CategoryOptions.Empty(categoryTags.Num());
	for (const FGameplayTag& categoryTag : categoryTags)
	{
		CategoryOptions.Add(MakeShared<FGameplayTag>(categoryTag));
	}

	if (CategoryOptions.Num() > 0 && GraphPinObj && GraphPinObj->GetDefaultAsString().IsEmpty() && GraphPinObj->LinkedTo.Num() == 0)
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, CategoryOptions[0]->ToString());
}

TSharedRef<SWidget> SPinTypeInventoryCategorySelector::GetDefaultValueWidget()
{
	return SNew(SBox)
		.Visibility(this, &SPinTypeInventoryCategorySelector::GetDefaultValueVisibility)
		[
			SNew(SComboBox<TSharedPtr<FGameplayTag>>)
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

void SPinTypeInventoryCategorySelector::OnCategorySelected(TSharedPtr<FGameplayTag> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid() || !Selection->IsValid() || !GraphPinObj)
		return;

	const FString selectedCategory = Selection->ToString();
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

TSharedRef<SWidget> SPinTypeInventoryCategorySelector::GenerateCategoryWidget(TSharedPtr<FGameplayTag> Item)
{
	return SNew(STextBlock)
		.Text(Item.IsValid() ? FText::FromString(Item->ToString()) : FText::GetEmpty())
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

void SPinTypeInventoryCategorySelector::GetCategoriesFromSettings(TArray<FGameplayTag>& OutCategories)
{
	UMounteaCraftingStatics::GetAllowedInventoryCategoryTags(OutCategories);
}

#endif
