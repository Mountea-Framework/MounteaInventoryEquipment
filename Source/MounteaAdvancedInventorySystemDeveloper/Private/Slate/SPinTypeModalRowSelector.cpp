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

#include "Slate/SPinTypeModalRowSelector.h"

#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Engine/DataTable.h"
#include "K2Node_CallFunction.h"
#include "ScopedTransaction.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Styling/AppStyle.h"
#include "Widgets/Text/STextBlock.h"

void SPinTypeModalRowSelector::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);

	RefreshModalRowOptions();

	if (ModalRowOptions.Num() > 0 && GraphPinObj && GraphPinObj->GetDefaultAsString().IsEmpty() && GraphPinObj->LinkedTo.Num() == 0)
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *ModalRowOptions[0]);
}

TSharedRef<SWidget> SPinTypeModalRowSelector::GetDefaultValueWidget()
{
	return SNew(SBox)
		.Visibility(this, &SPinTypeModalRowSelector::GetDefaultValueVisibility)
		[
			SAssignNew(ModalRowComboBox, SComboBox<TSharedPtr<FString>>)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.OptionsSource(&ModalRowOptions)
			.OnComboBoxOpening(this, &SPinTypeModalRowSelector::RefreshModalRowOptions)
			.OnGenerateWidget(this, &SPinTypeModalRowSelector::GenerateModalRowWidget)
			.OnSelectionChanged(this, &SPinTypeModalRowSelector::OnModalRowSelected)
			[
				SNew(STextBlock)
				.Text(this, &SPinTypeModalRowSelector::GetSelectedModalRowText)
				.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			]
		];
}

EVisibility SPinTypeModalRowSelector::GetDefaultValueVisibility() const
{
	return (GraphPinObj && GraphPinObj->LinkedTo.Num() > 0) ? EVisibility::Collapsed : EVisibility::Visible;
}

void SPinTypeModalRowSelector::RefreshModalRowOptions()
{
	TArray<FString> modalRows;
	BuildModalRowOptions(modalRows);

	ModalRowOptions.Empty(modalRows.Num());
	for (const FString& modalRow : modalRows)
	{
		ModalRowOptions.Add(MakeShared<FString>(modalRow));
	}

	if (ModalRowComboBox.IsValid())
		ModalRowComboBox->RefreshOptions();
}

void SPinTypeModalRowSelector::OnModalRowSelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid() || !GraphPinObj)
		return;

	const FString selectedModalRow = *Selection;
	const FString currentDefault = GraphPinObj->GetDefaultAsString().TrimQuotes();
	if (currentDefault == selectedModalRow)
		return;

	const FScopedTransaction transaction(NSLOCTEXT("ModalRowPin", "ChangeModalRow", "Change Modal Row"));
	GraphPinObj->Modify();

	const UEdGraphSchema* graphSchema = GraphPinObj->GetSchema();
	if (!graphSchema)
		return;

	graphSchema->TrySetDefaultValue(*GraphPinObj, selectedModalRow);
}

FText SPinTypeModalRowSelector::GetSelectedModalRowText() const
{
	if (!GraphPinObj)
		return FText::GetEmpty();

	const FString currentDefault = GraphPinObj->GetDefaultAsString().TrimQuotes();
	return currentDefault.IsEmpty() ? FText::FromString(TEXT("none")) : FText::FromString(currentDefault);
}

TSharedRef<SWidget> SPinTypeModalRowSelector::GenerateModalRowWidget(TSharedPtr<FString> Item)
{
	return SNew(STextBlock)
		.Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty())
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

void SPinTypeModalRowSelector::BuildModalRowOptions(TArray<FString>& OutModalRows) const
{
	OutModalRows.Reset();
	OutModalRows.Add(TEXT("none"));

	const UMounteaAdvancedInventoryGlobalUIConfig* globalUIConfig = ResolveGlobalUIConfig();
	if (!globalUIConfig)
		return;

	const FString modalType = ResolveModalType();
	const bool bHasModalTypeFilter = !modalType.IsEmpty() && !modalType.Equals(TEXT("none"), ESearchCase::IgnoreCase);
	if (bHasModalTypeFilter && !globalUIConfig->ModalTypes.Contains(modalType) && !globalUIConfig->Modals.Contains(modalType))
		return;

	TArray<FString> rowNames;
	for (const TSoftObjectPtr<UDataTable>& modalDataTable : globalUIConfig->ModalsData)
	{
		const UDataTable* dataTable = modalDataTable.LoadSynchronous();
		if (!IsValid(dataTable))
			continue;

		for (const FName rowName : dataTable->GetRowNames())
		{
			if (!rowName.IsNone())
				rowNames.AddUnique(rowName.ToString());
		}
	}

	rowNames.Sort();
	for (const FString& rowName : rowNames)
	{
		OutModalRows.Add(rowName);
	}
}

FString SPinTypeModalRowSelector::ResolveModalType() const
{
	if (!GraphPinObj)
		return FString();

	const UEdGraphNode* owningNode = GraphPinObj->GetOwningNode();
	const UK2Node_CallFunction* callFunctionNode = owningNode ? Cast<UK2Node_CallFunction>(owningNode) : nullptr;
	const UFunction* targetFunction = callFunctionNode ? callFunctionNode->GetTargetFunction() : nullptr;

	FName modalTypePinName(TEXT("ModalType"));
	if (targetFunction && targetFunction->HasMetaData(TEXT("MounteaModalTypePin")))
		modalTypePinName = FName(*targetFunction->GetMetaData(TEXT("MounteaModalTypePin")));

	if (!owningNode)
		return FString();

	for (const UEdGraphPin* pin : owningNode->Pins)
	{
		if (!pin || pin->PinName != modalTypePinName)
			continue;

		return pin->GetDefaultAsString().TrimQuotes();
	}

	return FString();
}

UMounteaAdvancedInventoryGlobalUIConfig* SPinTypeModalRowSelector::ResolveGlobalUIConfig() const
{
	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings)
		return nullptr;

	return settings->GlobalUIConfig.LoadSynchronous();
}

#endif
