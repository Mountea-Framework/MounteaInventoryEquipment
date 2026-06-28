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

#include "Customizations/MounteaModalDefinitionCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyHandle.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaModalDefinitionCustomization"

namespace MounteaModalDefinitionCustomization
{
	static FString GetCurrentValue(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		FString returnValue;
		if (PropertyHandle.IsValid())
			PropertyHandle->GetValue(returnValue);

		return returnValue;
	}

	static UMounteaAdvancedInventoryGlobalConfig* ResolveGlobalConfig()
	{
		const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
		return settings ? settings->GlobalConfig.LoadSynchronous() : nullptr;
	}

	static void RebuildDefinitionOptions(const TSharedRef<TArray<TSharedPtr<FString>>> Options)
	{
		Options->Reset();
		Options->Add(MakeShared<FString>(TEXT("none")));

		if (const UMounteaAdvancedInventoryGlobalConfig* globalConfig = ResolveGlobalConfig())
		{
			TArray<FString> definitionKeys;
			globalConfig->JsonObjectDefinitions.GenerateKeyArray(definitionKeys);
			definitionKeys.RemoveAll([](const FString& DefinitionKey)
			{
				return DefinitionKey.IsEmpty();
			});
			definitionKeys.Sort();

			for (const FString& definitionKey : definitionKeys)
				Options->Add(MakeShared<FString>(definitionKey));
		}
	}

	static void AddDefinitionKeyRow(
		IDetailChildrenBuilder& StructBuilder,
		const TSharedPtr<IPropertyHandle>& DefinitionKeyHandle,
		const FText& SearchText)
	{
		if (!DefinitionKeyHandle.IsValid())
			return;

		const TSharedRef<TArray<TSharedPtr<FString>>> options = MakeShared<TArray<TSharedPtr<FString>>>();
		RebuildDefinitionOptions(options);

		StructBuilder.AddCustomRow(SearchText)
		.NameContent()
		[
			DefinitionKeyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(300.f)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.ContentPadding(FMargin(2.f, 2.f))
			.OptionsSource(&options.Get())
			.OnComboBoxOpening_Lambda([options]()
			{
				RebuildDefinitionOptions(options);
			})
			.OnGenerateWidget_Lambda([](const TSharedPtr<FString>& Item)
			{
				return SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty());
			})
			.OnSelectionChanged_Lambda([DefinitionKeyHandle](const TSharedPtr<FString>& Selection, ESelectInfo::Type)
			{
				if (DefinitionKeyHandle.IsValid() && Selection.IsValid())
					DefinitionKeyHandle->SetValue(*Selection);
			})
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text_Lambda([DefinitionKeyHandle]()
				{
					const FString currentValue = GetCurrentValue(DefinitionKeyHandle);
					return currentValue.IsEmpty() ? LOCTEXT("NoDefinitionSelected", "none") : FText::FromString(currentValue);
				})
			]
		];
	}
}

TSharedRef<IPropertyTypeCustomization> FMounteaModalDefinitionCustomization::MakeInstance()
{
	return MakeShared<FMounteaModalDefinitionCustomization>();
}

void FMounteaModalDefinitionCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		StructPropertyHandle->CreatePropertyValueWidget()
	];
}

void FMounteaModalDefinitionCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const TSharedPtr<IPropertyHandle> widgetClassHandle = StructPropertyHandle->GetChildHandle(TEXT("WidgetClass"));
	if (widgetClassHandle.IsValid())
		StructBuilder.AddProperty(widgetClassHandle.ToSharedRef());

	MounteaModalDefinitionCustomization::AddDefinitionKeyRow(
		StructBuilder,
		StructPropertyHandle->GetChildHandle(TEXT("InPayloadDefinitionKey")),
		LOCTEXT("InPayloadDefinitionKeySearch", "In Payload Definition Key"));

	MounteaModalDefinitionCustomization::AddDefinitionKeyRow(
		StructBuilder,
		StructPropertyHandle->GetChildHandle(TEXT("OutPayloadDefinitionKey")),
		LOCTEXT("OutPayloadDefinitionKeySearch", "Out Payload Definition Key"));
}

#undef LOCTEXT_NAMESPACE
