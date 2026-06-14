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

#include "Customizations/MounteaJsonObjectDefinitionFieldCustomization.h"

#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyHandle.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaJsonObjectDefinitionFieldCustomization"

TSharedRef<IPropertyTypeCustomization> FMounteaJsonObjectDefinitionFieldCustomization::MakeInstance()
{
	return MakeShared<FMounteaJsonObjectDefinitionFieldCustomization>();
}

void FMounteaJsonObjectDefinitionFieldCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils
)
{
	const TSharedPtr<IPropertyHandle> fieldNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionField, FieldName));
	const TSharedPtr<IPropertyHandle> fieldTypeHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionField, FieldType));
	const TSharedPtr<IPropertyHandle> requiredHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionField, bRequired));

	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(620.f)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.FillWidth(0.45f)
		.Padding(0.f, 0.f, 6.f, 0.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("FieldNameLabel", "Name"))
				.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				fieldNameHandle.IsValid() ? fieldNameHandle->CreatePropertyValueWidget() : SNullWidget::NullWidget
			]
		]

		+ SHorizontalBox::Slot()
		.FillWidth(0.35f)
		.Padding(0.f, 0.f, 6.f, 0.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("FieldTypeLabel", "Type"))
				.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				fieldTypeHandle.IsValid() ? fieldTypeHandle->CreatePropertyValueWidget() : SNullWidget::NullWidget
			]
		]

		+ SHorizontalBox::Slot()
		.FillWidth(0.2f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("RequiredLabel", "Required"))
				.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				requiredHandle.IsValid() ? requiredHandle->CreatePropertyValueWidget() : SNullWidget::NullWidget
			]
		]
	];
}

void FMounteaJsonObjectDefinitionFieldCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils
)
{
}

#undef LOCTEXT_NAMESPACE
