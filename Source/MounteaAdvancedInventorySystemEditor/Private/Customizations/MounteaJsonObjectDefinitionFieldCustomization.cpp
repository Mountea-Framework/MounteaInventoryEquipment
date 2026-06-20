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
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaJsonObjectDefinitionFieldCustomization"

namespace MounteaJsonObjectDefinitionFieldCustomization
{
	static bool IsSupportedPinType(const FEdGraphPinType& PinType)
	{
		if (PinType.ContainerType != EPinContainerType::None)
			return false;

		return PinType.PinCategory == UEdGraphSchema_K2::PC_Int ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Int64 ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Real ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_String ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Name ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Text ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Byte ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Object ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_SoftObject ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Class ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_SoftClass ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Struct;
	}

	static FEdGraphPinType GetFieldPinType(const TSharedPtr<IPropertyHandle>& StructPropertyHandle)
	{
		FEdGraphPinType defaultType;
		defaultType.PinCategory = UEdGraphSchema_K2::PC_String;

		if (!StructPropertyHandle.IsValid())
			return defaultType;

		TArray<void*> rawData;
		StructPropertyHandle->AccessRawData(rawData);
		if (rawData.Num() == 1)
		{
			const FMounteaJsonObjectDefinitionField* field = static_cast<const FMounteaJsonObjectDefinitionField*>(rawData[0]);
			if (field && !field->FieldValueType.PinCategory.IsNone())
				return field->FieldValueType;
		}

		return defaultType;
	}

	static void SetFieldPinType(const TSharedPtr<IPropertyHandle> StructPropertyHandle, const TSharedPtr<IPropertyUtilities> PropertyUtilities, const FEdGraphPinType& PinType)
	{
		if (!StructPropertyHandle.IsValid() || !IsSupportedPinType(PinType))
			return;

		StructPropertyHandle->NotifyPreChange();

		TArray<void*> rawData;
		StructPropertyHandle->AccessRawData(rawData);
		for (void* data : rawData)
		{
			if (FMounteaJsonObjectDefinitionField* field = static_cast<FMounteaJsonObjectDefinitionField*>(data))
			{
				field->FieldValueType = PinType;
				field->FieldValueType.ContainerType = EPinContainerType::None;
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
			if (!InItem.IsValid())
				return false;

			if (InItem->bReadOnly)
				return true;

			return IsSupportedPinType(InItem->GetPinTypeNoResolve());
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
