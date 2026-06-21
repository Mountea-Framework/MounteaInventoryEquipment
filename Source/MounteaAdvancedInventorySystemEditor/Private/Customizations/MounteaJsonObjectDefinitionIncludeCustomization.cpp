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

#include "Customizations/MounteaJsonObjectDefinitionIncludeCustomization.h"

#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyHandle.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaJsonObjectDefinitionIncludeCustomization"

namespace MounteaJsonObjectDefinitionIncludeCustomization
{
	static FString GetCurrentKey(const TSharedPtr<IPropertyHandle>& DefinitionKeyHandle)
	{
		FString returnValue;
		if (DefinitionKeyHandle.IsValid())
			DefinitionKeyHandle->GetValue(returnValue);

		return returnValue;
	}

	static UMounteaAdvancedInventoryGlobalConfig* ResolveGlobalConfig(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
	{
		TArray<UObject*> outerObjects;
		StructPropertyHandle->GetOuterObjects(outerObjects);

		for (UObject* outerObject : outerObjects)
		{
			if (UMounteaAdvancedInventoryGlobalConfig* globalConfig = Cast<UMounteaAdvancedInventoryGlobalConfig>(outerObject))
				return globalConfig;
		}

		return nullptr;
	}

	static void RebuildOptions(const TSharedRef<IPropertyHandle> StructPropertyHandle, const TSharedRef<TArray<TSharedPtr<FString>>> Options)
	{
		Options->Reset();

		if (const UMounteaAdvancedInventoryGlobalConfig* globalConfig = ResolveGlobalConfig(StructPropertyHandle))
		{
			for (const TPair<FString, FMounteaJsonObjectDefinition>& definition : globalConfig->JsonObjectDefinitions)
			{
				Options->Add(MakeShared<FString>(definition.Key));
			}
		}

		Options->Sort([](const TSharedPtr<FString>& A, const TSharedPtr<FString>& B)
		{
			return A.IsValid() && B.IsValid() && *A < *B;
		});
	}
}

TSharedRef<IPropertyTypeCustomization> FMounteaJsonObjectDefinitionIncludeCustomization::MakeInstance()
{
	return MakeShared<FMounteaJsonObjectDefinitionIncludeCustomization>();
}

void FMounteaJsonObjectDefinitionIncludeCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils
)
{
	const TSharedPtr<IPropertyHandle> definitionKeyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionInclude, DefinitionKey));
	const TSharedRef<TArray<TSharedPtr<FString>>> options = MakeShared<TArray<TSharedPtr<FString>>>();
	MounteaJsonObjectDefinitionIncludeCustomization::RebuildOptions(StructPropertyHandle, options);

	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(420.f)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&options.Get())
			.OnComboBoxOpening_Lambda([StructPropertyHandle, options]()
			{
				MounteaJsonObjectDefinitionIncludeCustomization::RebuildOptions(StructPropertyHandle, options);
			})
			.OnGenerateWidget_Lambda([](const TSharedPtr<FString>& Item)
			{
				return SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty());
			})
			.OnSelectionChanged_Lambda([definitionKeyHandle](const TSharedPtr<FString>& Selection, ESelectInfo::Type)
			{
				if (definitionKeyHandle.IsValid() && Selection.IsValid())
					definitionKeyHandle->SetValue(*Selection);
			})
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text_Lambda([definitionKeyHandle]()
				{
					const FString currentKey = MounteaJsonObjectDefinitionIncludeCustomization::GetCurrentKey(definitionKeyHandle);
					return currentKey.IsEmpty() ? LOCTEXT("NoDefinitionSelected", "Select Definition") : FText::FromString(currentKey);
				})
			]
		]
	];
}

void FMounteaJsonObjectDefinitionIncludeCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils
)
{
	const TSharedPtr<IPropertyHandle> definitionKeyHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionInclude, DefinitionKey));

	if (!definitionKeyHandle.IsValid())
		return;

	// Inline field preview — mirrors StepGraphPropertyCustomization showing selected graph's property values
	StructBuilder.AddCustomRow(LOCTEXT("IncludedFieldsPreview", "Fields Preview"))
	.WholeRowContent()
	[
		SNew(STextBlock)
		.Font(IDetailLayoutBuilder::GetDetailFont())
		.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		.AutoWrapText(true)
		.Text_Lambda([definitionKeyHandle, StructPropertyHandle]() -> FText
		{
			FString key;
			definitionKeyHandle->GetValue(key);
			if (key.IsEmpty())
				return LOCTEXT("SelectDefinitionPreview", "Select a definition to preview its fields.");

			const UMounteaAdvancedInventoryGlobalConfig* globalConfig =
				MounteaJsonObjectDefinitionIncludeCustomization::ResolveGlobalConfig(StructPropertyHandle);
			if (!globalConfig)
				return LOCTEXT("NoConfigPreview", "No global config available.");

			const FMounteaJsonObjectDefinition* def = globalConfig->JsonObjectDefinitions.Find(key);
			if (!def)
				return FText::Format(LOCTEXT("NotFoundPreview", "\"{0}\" not found in definitions."), FText::FromString(key));

			if (def->Fields.IsEmpty() && def->IncludedDefinitions.IsEmpty())
				return LOCTEXT("EmptyDefinitionPreview", "(empty definition)");

			FString preview;
			if (!def->IncludedDefinitions.IsEmpty())
				preview += FString::Printf(TEXT("+%d include(s)\n"), def->IncludedDefinitions.Num());

			for (const FMounteaJsonObjectDefinitionField& field : def->Fields)
				preview += FString::Printf(TEXT("• %s\n"), *field.FieldName.ToString());

			preview.TrimEndInline();
			return FText::FromString(preview);
		})
	];
}

#undef LOCTEXT_NAMESPACE
