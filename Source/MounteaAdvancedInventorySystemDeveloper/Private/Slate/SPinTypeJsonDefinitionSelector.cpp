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

#include "Slate/SPinTypeJsonDefinitionSelector.h"

#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphSchema.h"
#include "ScopedTransaction.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Widgets/Text/STextBlock.h"

void SPinTypeJsonDefinitionSelector::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);

	RefreshDefinitionOptions();

	if (DefinitionOptions.Num() > 0 && GraphPinObj && GraphPinObj->GetDefaultAsString().IsEmpty() && GraphPinObj->LinkedTo.Num() == 0)
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *DefinitionOptions[0]);
}

TSharedRef<SWidget> SPinTypeJsonDefinitionSelector::GetDefaultValueWidget()
{
	return SNew(SBox)
		.Visibility(this, &SPinTypeJsonDefinitionSelector::GetDefaultValueVisibility)
		[
			SAssignNew(DefinitionComboBox, SComboBox<TSharedPtr<FString>>)
			.ContentPadding(FMargin(2.f, 2.f))
			.OptionsSource(&DefinitionOptions)
			.OnComboBoxOpening(this, &SPinTypeJsonDefinitionSelector::RefreshDefinitionOptions)
			.OnGenerateWidget(this, &SPinTypeJsonDefinitionSelector::GenerateDefinitionWidget)
			.OnSelectionChanged(this, &SPinTypeJsonDefinitionSelector::OnDefinitionSelected)
			[
				SNew(STextBlock)
				.Text(this, &SPinTypeJsonDefinitionSelector::GetSelectedDefinitionText)
				.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			]
		];
}

EVisibility SPinTypeJsonDefinitionSelector::GetDefaultValueVisibility() const
{
	return (GraphPinObj && GraphPinObj->LinkedTo.Num() > 0) ? EVisibility::Collapsed : EVisibility::Visible;
}

void SPinTypeJsonDefinitionSelector::RefreshDefinitionOptions()
{
	TArray<FString> definitions;
	BuildDefinitionOptions(definitions);

	DefinitionOptions.Empty(definitions.Num());
	for (const FString& definition : definitions)
		DefinitionOptions.Add(MakeShared<FString>(definition));

	if (DefinitionComboBox.IsValid())
		DefinitionComboBox->RefreshOptions();
}

void SPinTypeJsonDefinitionSelector::OnDefinitionSelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid() || !GraphPinObj)
		return;

	const FString selectedDefinition = *Selection;
	const FString currentDefault = GraphPinObj->GetDefaultAsString().TrimQuotes();
	if (currentDefault == selectedDefinition)
		return;

	const FScopedTransaction transaction(NSLOCTEXT("JsonDefinitionPin", "ChangeJsonDefinition", "Change JSON Definition"));
	GraphPinObj->Modify();
	if (UEdGraphNode* owningNode = GraphPinObj->GetOwningNode())
		owningNode->Modify();

	if (const UEdGraphSchema* graphSchema = GraphPinObj->GetSchema())
		graphSchema->TrySetDefaultValue(*GraphPinObj, selectedDefinition);
}

FText SPinTypeJsonDefinitionSelector::GetSelectedDefinitionText() const
{
	return GraphPinObj ? FText::FromString(GraphPinObj->GetDefaultAsString()) : FText::GetEmpty();
}

TSharedRef<SWidget> SPinTypeJsonDefinitionSelector::GenerateDefinitionWidget(TSharedPtr<FString> Item)
{
	return SNew(STextBlock)
		.Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty())
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

void SPinTypeJsonDefinitionSelector::BuildDefinitionOptions(TArray<FString>& OutDefinitions) const
{
	OutDefinitions.Reset();
	OutDefinitions.Add(TEXT("none"));

	const UMounteaAdvancedInventoryGlobalConfig* globalConfig = ResolveGlobalConfig();
	if (!globalConfig)
		return;

	globalConfig->JsonObjectDefinitions.GenerateKeyArray(OutDefinitions);
	OutDefinitions.RemoveAll([](const FString& DefinitionKey)
	{
		return DefinitionKey.IsEmpty();
	});
	OutDefinitions.AddUnique(TEXT("none"));
	OutDefinitions.Sort();
	OutDefinitions.Remove(TEXT("none"));
	OutDefinitions.Insert(TEXT("none"), 0);
}

UMounteaAdvancedInventoryGlobalConfig* SPinTypeJsonDefinitionSelector::ResolveGlobalConfig() const
{
	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	return settings ? settings->GlobalConfig.LoadSynchronous() : nullptr;
}

#endif
