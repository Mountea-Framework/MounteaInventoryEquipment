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
#include "EdGraphSchema_K2.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "PropertyHandle.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "SPinTypeSelector.h"
#include "Statics/MounteaJsonDefinitionPinTypeStatics.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaJsonObjectDefinitionFieldCustomization"

namespace MounteaJsonObjectDefinitionFieldCustomization
{
	static FEdGraphPinType GetFieldPinType(const TSharedPtr<IPropertyHandle>& StructPropertyHandle)
	{
		if (!StructPropertyHandle.IsValid())
			return MounteaJsonDefinitionPinTypeStatics::MakeDefaultFieldPinType();

		TArray<void*> rawData;
		StructPropertyHandle->AccessRawData(rawData);
		if (rawData.Num() == 1)
		{
			const FMounteaJsonObjectDefinitionField* field = static_cast<const FMounteaJsonObjectDefinitionField*>(rawData[0]);
			if (field && !field->FieldValueType.PinCategory.IsNone())
				return field->FieldValueType;
		}

		return MounteaJsonDefinitionPinTypeStatics::MakeDefaultFieldPinType();
	}

	static void SetFieldPinType(const TSharedPtr<IPropertyHandle> StructPropertyHandle, const TSharedPtr<IPropertyUtilities> PropertyUtilities, const FEdGraphPinType& PinType)
	{
		if (!StructPropertyHandle.IsValid() || !MounteaJsonDefinitionPinTypeStatics::IsSupportedPinType(PinType))
			return;

		StructPropertyHandle->NotifyPreChange();

		TArray<void*> rawData;
		StructPropertyHandle->AccessRawData(rawData);
		for (void* data : rawData)
		{
			if (FMounteaJsonObjectDefinitionField* field = static_cast<FMounteaJsonObjectDefinitionField*>(data))
			{
				field->FieldValueType = PinType;
				MounteaJsonDefinitionPinTypeStatics::NormalizeFieldPinType(field->FieldValueType);
			}
		}

		StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		if (PropertyUtilities.IsValid())
			PropertyUtilities->RequestRefresh();
	}

	class FJsonDefinitionPinTypeFilter final : public IPinTypeSelectorFilter
	{
	public:

		virtual bool ShouldShowPinTypeTreeItem(FPinTypeTreeItem InItem) const override
		{
			return MounteaJsonDefinitionPinTypeStatics::IsSupportedPinTypeTreeItem(InItem);
		}
	};
}

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
	const TSharedPtr<IPropertyHandle> requiredHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionField, bRequired));
	const TSharedPtr<IPropertyUtilities> propertyUtilities = StructCustomizationUtils.GetPropertyUtilities();

	TArray<TSharedPtr<IPinTypeSelectorFilter>> customFilters;
	customFilters.Add(MakeShared<MounteaJsonObjectDefinitionFieldCustomization::FJsonDefinitionPinTypeFilter>());
	
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
				SNew(SPinTypeSelector, FGetPinTypeTree::CreateUObject(GetDefault<UEdGraphSchema_K2>(), &UEdGraphSchema_K2::GetVariableTypeTree))
				.TargetPinType_Lambda([StructPropertyHandle]()
				{
					return MounteaJsonObjectDefinitionFieldCustomization::GetFieldPinType(StructPropertyHandle);
				})
				.OnPinTypeChanged_Lambda([StructPropertyHandle, propertyUtilities](const FEdGraphPinType& PinType)
				{
					MounteaJsonObjectDefinitionFieldCustomization::SetFieldPinType(StructPropertyHandle, propertyUtilities, PinType);
				})
				.Schema(GetDefault<UEdGraphSchema_K2>())
				.bAllowArrays(false)
				.CustomFilters(customFilters)
				.SelectorType(SPinTypeSelector::ESelectorType::Full)
				.TypeTreeFilter(ETypeTreeFilter::None)
				.Font(IDetailLayoutBuilder::GetDetailFont())
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
