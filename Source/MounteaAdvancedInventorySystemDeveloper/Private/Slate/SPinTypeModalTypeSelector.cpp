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

#include "Slate/SPinTypeModalTypeSelector.h"

#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

void SPinTypeModalTypeSelector::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);

	TArray<FString> modalTypes;
	BuildModalTypeOptions(modalTypes);

	ModalTypeOptions.Empty(modalTypes.Num());
	for (const FString& modalType : modalTypes)
	{
		ModalTypeOptions.Add(MakeShared<FString>(modalType));
	}

	if (ModalTypeOptions.Num() > 0 && GraphPinObj && GraphPinObj->GetDefaultAsString().IsEmpty() && GraphPinObj->LinkedTo.Num() == 0)
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *ModalTypeOptions[0]);
}

TSharedRef<SWidget> SPinTypeModalTypeSelector::GetDefaultValueWidget()
{
	return SNew(SBox)
		.Visibility(this, &SPinTypeModalTypeSelector::GetDefaultValueVisibility)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.OptionsSource(&ModalTypeOptions)
			.OnGenerateWidget(this, &SPinTypeModalTypeSelector::GenerateModalTypeWidget)
			.OnSelectionChanged(this, &SPinTypeModalTypeSelector::OnModalTypeSelected)
			[
				SNew(STextBlock)
				.Text(this, &SPinTypeModalTypeSelector::GetSelectedModalTypeText)
				.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			]
		];
}

EVisibility SPinTypeModalTypeSelector::GetDefaultValueVisibility() const
{
	return (GraphPinObj && GraphPinObj->LinkedTo.Num() > 0) ? EVisibility::Collapsed : EVisibility::Visible;
}

void SPinTypeModalTypeSelector::OnModalTypeSelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid() || !GraphPinObj)
		return;

	const FString selectedModalType = *Selection;
	const FString currentDefault = GraphPinObj->GetDefaultAsString().TrimQuotes();
	if (currentDefault == selectedModalType)
		return;

	const FScopedTransaction transaction(NSLOCTEXT("ModalTypePin", "ChangeModalType", "Change Modal Type"));
	GraphPinObj->Modify();

	if (const UEdGraphSchema* graphSchema = GraphPinObj->GetSchema())
		graphSchema->TrySetDefaultValue(*GraphPinObj, selectedModalType);
}

FText SPinTypeModalTypeSelector::GetSelectedModalTypeText() const
{
	if (GraphPinObj)
		return FText::FromString(GraphPinObj->GetDefaultAsString());

	return FText::GetEmpty();
}

TSharedRef<SWidget> SPinTypeModalTypeSelector::GenerateModalTypeWidget(TSharedPtr<FString> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*Item))
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

void SPinTypeModalTypeSelector::BuildModalTypeOptions(TArray<FString>& OutModalTypes) const
{
	OutModalTypes.Reset();
	OutModalTypes.Add(TEXT("none"));

	const UMounteaAdvancedInventoryGlobalUIConfig* globalUIConfig = ResolveGlobalUIConfig();
	if (!globalUIConfig)
		return;

	TArray<FString> modalTypes = globalUIConfig->ModalTypes.Array();
	modalTypes.Sort();

	for (const FString& modalType : modalTypes)
	{
		if (!modalType.IsEmpty())
			OutModalTypes.AddUnique(modalType);
	}
}

UMounteaAdvancedInventoryGlobalUIConfig* SPinTypeModalTypeSelector::ResolveGlobalUIConfig() const
{
	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings)
		return nullptr;

	return settings->GlobalUIConfig.LoadSynchronous();
}

#endif
