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

#include "Customizations/MounteaJsonObjectDefinitionCustomization.h"

#include "DetailWidgetRow.h"
#include "EdGraphSchema_K2.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "SPinTypeSelector.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaJsonObjectDefinitionCustomization"

namespace MounteaJsonObjectDefinitionCustomization
{
	// ─── status ───────────────────────────────────────────────────────────────

	enum class EDefinitionStatus : uint8 { Ok, Warning, Error };

	static EDefinitionStatus GetStatus(const TSharedRef<IPropertyHandle>& StructHandle)
	{
		TArray<void*> rawData;
		StructHandle->AccessRawData(rawData);
		if (rawData.Num() != 1 || !rawData[0])
			return EDefinitionStatus::Ok;

		const FMounteaJsonObjectDefinition* def = static_cast<const FMounteaJsonObjectDefinition*>(rawData[0]);
		if (!def)
			return EDefinitionStatus::Ok;

		bool hasWarning = false;
		for (const FMounteaJsonObjectDefinitionInclude& inc : def->IncludedDefinitions)
		{
			if (inc.DefinitionKey.IsEmpty())
			{
				hasWarning = true;
				break;
			}
		}

		TSet<FName> fieldNames;
		for (const FMounteaJsonObjectDefinitionField& field : def->Fields)
		{
			if (field.FieldName.IsNone())
			{
				hasWarning = true;
				continue;
			}
			if (fieldNames.Contains(field.FieldName))
				return EDefinitionStatus::Error;

			fieldNames.Add(field.FieldName);
		}

		return hasWarning ? EDefinitionStatus::Warning : EDefinitionStatus::Ok;
	}

	static const FSlateBrush* GetStatusBrush(EDefinitionStatus Status)
	{
		switch (Status)
		{
			case EDefinitionStatus::Error:   return FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Error");
			case EDefinitionStatus::Warning: return FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Warning");
			default:                         return FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Success");
		}
	}

	static FText GetStatusTooltip(EDefinitionStatus Status)
	{
		switch (Status)
		{
			case EDefinitionStatus::Error:   return LOCTEXT("StatusError",   "Error: duplicate field name detected.");
			case EDefinitionStatus::Warning: return LOCTEXT("StatusWarning", "Warning: empty field name or include key.");
			default:                         return LOCTEXT("StatusOk",      "Definition is valid.");
		}
	}

	static FText GetSummaryText(const TSharedRef<IPropertyHandle>& StructHandle)
	{
		TArray<void*> rawData;
		StructHandle->AccessRawData(rawData);
		if (rawData.Num() != 1 || !rawData[0])
			return FText::GetEmpty();

		const FMounteaJsonObjectDefinition* def = static_cast<const FMounteaJsonObjectDefinition*>(rawData[0]);
		if (!def)
			return FText::GetEmpty();

		return FText::Format(
			LOCTEXT("DefinitionSummary", "{0} include(s), {1} field(s)"),
			FText::AsNumber(def->IncludedDefinitions.Num()),
			FText::AsNumber(def->Fields.Num())
		);
	}

	// ─── global config ────────────────────────────────────────────────────────

	static UMounteaAdvancedInventoryGlobalConfig* ResolveGlobalConfig(const TSharedRef<IPropertyHandle>& Handle)
	{
		TArray<UObject*> outerObjects;
		Handle->GetOuterObjects(outerObjects);
		for (UObject* obj : outerObjects)
		{
			if (UMounteaAdvancedInventoryGlobalConfig* cfg = Cast<UMounteaAdvancedInventoryGlobalConfig>(obj))
				return cfg;
		}
		return nullptr;
	}

	static void RebuildDefinitionOptions(const TSharedRef<IPropertyHandle>& Handle, const TSharedRef<TArray<TSharedPtr<FString>>>& Options)
	{
		Options->Reset();
		if (const UMounteaAdvancedInventoryGlobalConfig* cfg = ResolveGlobalConfig(Handle))
		{
			for (const TPair<FString, FMounteaJsonObjectDefinition>& pair : cfg->JsonObjectDefinitions)
				Options->Add(MakeShared<FString>(pair.Key));
		}
		Options->Sort([](const TSharedPtr<FString>& A, const TSharedPtr<FString>& B)
		{
			return A.IsValid() && B.IsValid() && *A < *B;
		});
	}

	// ─── pin type helpers (mirror of FieldCustomization internals) ────────────

	static bool IsSupportedPinType(const FEdGraphPinType& PinType)
	{
		if (PinType.ContainerType != EPinContainerType::None)
			return false;

		return PinType.PinCategory == UEdGraphSchema_K2::PC_Int      ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Int64    ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Real     ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean  ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_String   ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Name     ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Text     ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Byte     ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Object   ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_SoftObject ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Class    ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_SoftClass ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Struct;
	}

static FEdGraphPinType GetElemFieldPinType(const TSharedRef<IPropertyHandle>& ElemHandle)
	{
		TArray<void*> rawData;
		ElemHandle->AccessRawData(rawData);
		if (rawData.Num() == 1 && rawData[0])
		{
			const FMounteaJsonObjectDefinitionField* field = static_cast<const FMounteaJsonObjectDefinitionField*>(rawData[0]);
			if (field)
			{
				if (!field->FieldValueType.PinCategory.IsNone())
					return field->FieldValueType;

				FEdGraphPinType fallback;
				fallback.ContainerType = EPinContainerType::None;
				fallback.PinCategory = UEdGraphSchema_K2::PC_String;
				return fallback;
			}
		}
		FEdGraphPinType def;
		def.PinCategory = UEdGraphSchema_K2::PC_String;
		return def;
	}

	static void SetElemFieldPinType(const TSharedRef<IPropertyHandle>& ElemHandle, const FEdGraphPinType& PinType, const TSharedPtr<IPropertyUtilities>& Utils)
	{
		if (!IsSupportedPinType(PinType))
			return;

		ElemHandle->NotifyPreChange();
		TArray<void*> rawData;
		ElemHandle->AccessRawData(rawData);
		for (void* data : rawData)
		{
			if (FMounteaJsonObjectDefinitionField* field = static_cast<FMounteaJsonObjectDefinitionField*>(data))
			{
				field->FieldValueType = PinType;
				field->FieldValueType.ContainerType = EPinContainerType::None;
			}
		}
		ElemHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		if (Utils.IsValid())
			Utils->RequestRefresh();
	}

	// ─── name validation (mirrors StepGraph's IsNameTextValid) ────────────────

	static bool IsNameValid(const FText& Text)
	{
		if (Text.IsEmpty())
			return false;
		if (!FText::TrimPrecedingAndTrailing(Text).EqualTo(Text))
			return false;
		const FString disallowed = TEXT(". []");
		for (int32 i = 0; i < disallowed.Len(); ++i)
		{
			const TCHAR ch = disallowed[i];
			if (Text.ToString().Contains(&ch))
				return false;
		}
		return true;
	}

	// ─── pin type filter (same whitelist as FieldCustomization) ───────────────

	class FDefinitionPinTypeFilter final : public IPinTypeSelectorFilter
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

	// ─── include preview text ─────────────────────────────────────────────────

	static FText GetIncludePreviewText(const TSharedPtr<IPropertyHandle>& KeyHandle, const TSharedRef<IPropertyHandle>& StructHandle)
	{
		if (!KeyHandle.IsValid())
			return FText::GetEmpty();

		FString key;
		KeyHandle->GetValue(key);
		if (key.IsEmpty())
			return FText::GetEmpty();

		const UMounteaAdvancedInventoryGlobalConfig* cfg = ResolveGlobalConfig(StructHandle);
		if (!cfg)
			return LOCTEXT("NoConfigPreview", "(no config)");

		const FMounteaJsonObjectDefinition* def = cfg->JsonObjectDefinitions.Find(key);
		if (!def)
			return FText::Format(LOCTEXT("NotFoundPreview", "\"{0}\" not found"), FText::FromString(key));

		if (def->Fields.IsEmpty())
			return LOCTEXT("NoFieldsPreview", "(no direct fields)");

		FString preview;
		for (const FMounteaJsonObjectDefinitionField& f : def->Fields)
			preview += FString::Printf(TEXT("• %s\n"), *f.FieldName.ToString());
		preview.TrimEndInline();
		return FText::FromString(preview);
	}

	// ─── status icon widget (reused in header and control row) ────────────────

	// SBox pins the SImage to 16×16 so VAlign_Center works correctly in all
	// detail-panel row types (value-content and whole-row-content).
	static TSharedRef<SWidget> MakeStatusIcon(TSharedRef<IPropertyHandle> StructHandle)
	{
		return SNew(SBox)
			.WidthOverride(16.f)
			.HeightOverride(16.f)
			[
				SNew(SImage)
				.Image_Lambda([StructHandle]()
				{
					return GetStatusBrush(GetStatus(StructHandle));
				})
				.ToolTipText_Lambda([StructHandle]()
				{
					return GetStatusTooltip(GetStatus(StructHandle));
				})
			];
	}
}

// ─── MakeInstance ─────────────────────────────────────────────────────────────

TSharedRef<IPropertyTypeCustomization> FMounteaJsonObjectDefinitionCustomization::MakeInstance()
{
	return MakeShared<FMounteaJsonObjectDefinitionCustomization>();
}

// ─── CustomizeHeader ──────────────────────────────────────────────────────────

void FMounteaJsonObjectDefinitionCustomization::CustomizeHeader(
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
	.MinDesiredWidth(260.f)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.f, 0.f, 6.f, 0.f)
		[
			MounteaJsonObjectDefinitionCustomization::MakeStatusIcon(StructPropertyHandle)
		]

		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text_Lambda([StructPropertyHandle]()
			{
				return MounteaJsonObjectDefinitionCustomization::GetSummaryText(StructPropertyHandle);
			})
		]
	];
}

// ─── CustomizeChildren ────────────────────────────────────────────────────────

void FMounteaJsonObjectDefinitionCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const TSharedPtr<IPropertyHandle> includesHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinition, IncludedDefinitions));
	const TSharedPtr<IPropertyHandle> fieldsHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinition, Fields));

	if (!includesHandle.IsValid() || !fieldsHandle.IsValid())
		return;

	const TSharedPtr<IPropertyHandleArray> includesArray = includesHandle->AsArray();
	const TSharedPtr<IPropertyHandleArray> fieldsArray   = fieldsHandle->AsArray();

	if (!includesArray.IsValid() || !fieldsArray.IsValid())
		return;

	const TSharedPtr<IPropertyUtilities> utils = StructCustomizationUtils.GetPropertyUtilities();

	if (!NewFieldPinType.IsValid())
	{
		NewFieldPinType = MakeShared<FEdGraphPinType>();
		NewFieldPinType->PinCategory = UEdGraphSchema_K2::PC_String;
	}

	TArray<TSharedPtr<IPinTypeSelectorFilter>> pinFilters;
	pinFilters.Add(MakeShared<MounteaJsonObjectDefinitionCustomization::FDefinitionPinTypeFilter>());

	// ── Control row ──────────────────────────────────────────────────────────

	StructBuilder.AddCustomRow(LOCTEXT("ControlRow", "Controls"))
	.WholeRowContent()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.f, 0.f, 6.f, 0.f)
		[
			MounteaJsonObjectDefinitionCustomization::MakeStatusIcon(StructPropertyHandle)
		]

		// Add Field button
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 4.f, 0.f)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.Text(LOCTEXT("AddField", "Add Field"))
			.OnClicked_Lambda([StructPropertyHandle, utils, this]()
			{
				// Raw mutation: IPropertyHandleArray::AddItem() silently fails for
				// arrays nested inside TMap value structs (FindComplexParent returns null).
				StructPropertyHandle->NotifyPreChange();
				TArray<void*> rawData;
				StructPropertyHandle->AccessRawData(rawData);
				for (void* data : rawData)
				{
					if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
					{
						FMounteaJsonObjectDefinitionField newField;
						newField.FieldValueType = *NewFieldPinType;
						newField.FieldValueType.ContainerType = EPinContainerType::None;
						def->Fields.Add(newField);
					}
				}
				StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ArrayAdd);
				if (utils.IsValid())
					utils->RequestRefresh();
				return FReply::Handled();
			})
		]

		// Initial pin type selector for new fields
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 12.f, 0.f)
		[
			SNew(SPinTypeSelector, FGetPinTypeTree::CreateUObject(GetDefault<UEdGraphSchema_K2>(), &UEdGraphSchema_K2::GetVariableTypeTree))
			.TargetPinType_Lambda([this]() { return *NewFieldPinType; })
			.OnPinTypeChanged_Lambda([this](const FEdGraphPinType& NewType)
			{
				if (MounteaJsonObjectDefinitionCustomization::IsSupportedPinType(NewType))
				{
					*NewFieldPinType = NewType;
					NewFieldPinType->ContainerType = EPinContainerType::None;
				}
			})
			.Schema(GetDefault<UEdGraphSchema_K2>())
			.bAllowArrays(false)
			.CustomFilters(pinFilters)
			.SelectorType(SPinTypeSelector::ESelectorType::Partial)
			.TypeTreeFilter(ETypeTreeFilter::None)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]

		// Add Include button
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.Text(LOCTEXT("AddInclude", "Add Include"))
			.OnClicked_Lambda([StructPropertyHandle, utils]()
			{
				// Raw mutation: same reason as Add Field above.
				StructPropertyHandle->NotifyPreChange();
				TArray<void*> rawData;
				StructPropertyHandle->AccessRawData(rawData);
				for (void* data : rawData)
				{
					if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
						def->IncludedDefinitions.Add(FMounteaJsonObjectDefinitionInclude());
				}
				StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ArrayAdd);
				if (utils.IsValid())
					utils->RequestRefresh();
				return FReply::Handled();
			})
		]
	];

	// ── Include rows ─────────────────────────────────────────────────────────

	uint32 numIncludes = 0;
	includesArray->GetNumElements(numIncludes);

	for (uint32 i = 0; i < numIncludes; ++i)
	{
		TSharedRef<IPropertyHandle> incElemHandle = includesArray->GetElement(i);
		TSharedPtr<IPropertyHandle> incKeyHandle  = incElemHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionInclude, DefinitionKey));

		TSharedRef<TArray<TSharedPtr<FString>>> incOptions = MakeShared<TArray<TSharedPtr<FString>>>();
		MounteaJsonObjectDefinitionCustomization::RebuildDefinitionOptions(StructPropertyHandle, incOptions);

		const uint32 capturedI = i;

		StructBuilder.AddCustomRow(FText::Format(LOCTEXT("IncludeRow", "Include {0}"), FText::AsNumber(i)))
		.NameContent()
		[
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(FText::Format(LOCTEXT("IncludeLabel", "Include {0}"), FText::AsNumber(i)))
		]
		.ValueContent()
		.MinDesiredWidth(420.f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&incOptions.Get())
					.OnComboBoxOpening_Lambda([StructPropertyHandle, incOptions]()
					{
						MounteaJsonObjectDefinitionCustomization::RebuildDefinitionOptions(StructPropertyHandle, incOptions);
					})
					.OnGenerateWidget_Lambda([](const TSharedPtr<FString>& Item)
					{
						return SNew(STextBlock)
							.Font(IDetailLayoutBuilder::GetDetailFont())
							.Text(Item.IsValid() ? FText::FromString(*Item) : FText::GetEmpty());
					})
					.OnSelectionChanged_Lambda([incKeyHandle](const TSharedPtr<FString>& Selection, ESelectInfo::Type)
					{
						if (incKeyHandle.IsValid() && Selection.IsValid())
							incKeyHandle->SetValue(*Selection);
					})
					[
						SNew(STextBlock)
						.Font(IDetailLayoutBuilder::GetDetailFont())
						.Text_Lambda([incKeyHandle]()
						{
							FString key;
							if (incKeyHandle.IsValid())
								incKeyHandle->GetValue(key);
							return key.IsEmpty()
								? LOCTEXT("NoDefinitionSelected", "Select Definition")
								: FText::FromString(key);
						})
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.f, 0.f, 0.f, 0.f)
				[
					PropertyCustomizationHelpers::MakeClearButton(
						FSimpleDelegate::CreateLambda([StructPropertyHandle, capturedI, utils]()
						{
							StructPropertyHandle->NotifyPreChange();
							TArray<void*> rawData;
							StructPropertyHandle->AccessRawData(rawData);
							for (void* data : rawData)
							{
								if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
									if (def->IncludedDefinitions.IsValidIndex(capturedI))
										def->IncludedDefinitions.RemoveAt(capturedI);
							}
							StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ArrayRemove);
							if (utils.IsValid())
								utils->RequestRefresh();
						}),
						LOCTEXT("RemoveInclude", "Remove Include")
					)
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 2.f, 0.f, 0.f)
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				.AutoWrapText(false)
				.Text_Lambda([incKeyHandle, StructPropertyHandle]()
				{
					return MounteaJsonObjectDefinitionCustomization::GetIncludePreviewText(incKeyHandle, StructPropertyHandle);
				})
				.Visibility_Lambda([incKeyHandle]()
				{
					FString key;
					if (incKeyHandle.IsValid())
						incKeyHandle->GetValue(key);
					return key.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
				})
			]
		];
	}

	// ── Field rows ────────────────────────────────────────────────────────────

	uint32 numFields = 0;
	fieldsArray->GetNumElements(numFields);

	for (uint32 j = 0; j < numFields; ++j)
	{
		TSharedRef<IPropertyHandle> fieldElemHandle = fieldsArray->GetElement(j);

		TSharedPtr<IPropertyHandle> fieldNameHandle = fieldElemHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionField, FieldName));
		TSharedPtr<IPropertyHandle> requiredHandle = fieldElemHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FMounteaJsonObjectDefinitionField, bRequired));

		const uint32 capturedJ = j;
		const uint32 capturedNumFields = numFields;

		StructBuilder.AddCustomRow(FText::Format(LOCTEXT("FieldRow", "Field {0}"), FText::AsNumber(j)))
		.WholeRowContent()
		.MinDesiredWidth(640.f)
		[
			SNew(SHorizontalBox)

			// ── Field name ────────────────────────────────────────────────────
			+ SHorizontalBox::Slot()
			.FillWidth(0.38f)
			.Padding(0.f, 0.f, 4.f, 0.f)
			.VAlign(VAlign_Center)
			[
				SNew(SEditableTextBox)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text_Lambda([fieldNameHandle]()
				{
					FName value;
					if (fieldNameHandle.IsValid())
						fieldNameHandle->GetValue(value);
					return FText::FromName(value);
				})
				.OnTextCommitted_Lambda([fieldNameHandle](const FText& NewText, ETextCommit::Type)
				{
					if (!fieldNameHandle.IsValid())
						return;
					if (!MounteaJsonObjectDefinitionCustomization::IsNameValid(NewText))
						return;
					fieldNameHandle->SetValue(FName(*NewText.ToString().TrimStartAndEnd()));
				})
				.HintText(LOCTEXT("FieldNameHint", "Field name"))
			]

			// ── Type selector ─────────────────────────────────────────────────
			+ SHorizontalBox::Slot()
			.FillWidth(0.37f)
			.Padding(0.f, 0.f, 4.f, 0.f)
			.VAlign(VAlign_Center)
			[
				SNew(SPinTypeSelector, FGetPinTypeTree::CreateUObject(GetDefault<UEdGraphSchema_K2>(), &UEdGraphSchema_K2::GetVariableTypeTree))
				.TargetPinType_Lambda([fieldElemHandle]()
				{
					return MounteaJsonObjectDefinitionCustomization::GetElemFieldPinType(fieldElemHandle);
				})
				.OnPinTypeChanged_Lambda([fieldElemHandle, utils](const FEdGraphPinType& NewType)
				{
					MounteaJsonObjectDefinitionCustomization::SetElemFieldPinType(fieldElemHandle, NewType, utils);
				})
				.Schema(GetDefault<UEdGraphSchema_K2>())
				.bAllowArrays(false)
				.SelectorType(SPinTypeSelector::ESelectorType::Partial)
				.TypeTreeFilter(ETypeTreeFilter::None)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]

			// ── Required checkbox ─────────────────────────────────────────────
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.f, 0.f, 6.f, 0.f)
			[
				SNew(SCheckBox)
				.IsChecked_Lambda([requiredHandle]()
				{
					bool val = true;
					if (requiredHandle.IsValid())
						requiredHandle->GetValue(val);
					return val ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
				.OnCheckStateChanged_Lambda([requiredHandle](ECheckBoxState NewState)
				{
					if (requiredHandle.IsValid())
						requiredHandle->SetValue(NewState == ECheckBoxState::Checked);
				})
				.ToolTipText(LOCTEXT("RequiredTooltip", "Required"))
			]

			// ── Move up ───────────────────────────────────────────────────────
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ContentPadding(2.f)
				.IsEnabled(capturedJ > 0)
				.OnClicked_Lambda([StructPropertyHandle, capturedJ, utils]()
				{
					// IPropertyHandleArray::MoveElementTo crashes for arrays nested
					// inside TMap value structs (FindComplexParent returns null).
					StructPropertyHandle->NotifyPreChange();
					TArray<void*> rawData;
					StructPropertyHandle->AccessRawData(rawData);
					for (void* data : rawData)
					{
						if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
							if (def->Fields.IsValidIndex(capturedJ) && def->Fields.IsValidIndex(capturedJ - 1))
								def->Fields.Swap(capturedJ, capturedJ - 1);
					}
					StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ArrayMove);
					if (utils.IsValid())
						utils->RequestRefresh();
					return FReply::Handled();
				})
				[
					SNew(SImage).Image(FAppStyle::GetBrush("Icons.ChevronUp"))
				]
			]

			// ── Move down ─────────────────────────────────────────────────────
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ContentPadding(2.f)
				.IsEnabled(capturedJ < capturedNumFields - 1)
				.OnClicked_Lambda([StructPropertyHandle, capturedJ, utils]()
				{
					StructPropertyHandle->NotifyPreChange();
					TArray<void*> rawData;
					StructPropertyHandle->AccessRawData(rawData);
					for (void* data : rawData)
					{
						if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
							if (def->Fields.IsValidIndex(capturedJ) && def->Fields.IsValidIndex(capturedJ + 1))
								def->Fields.Swap(capturedJ, capturedJ + 1);
					}
					StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ArrayMove);
					if (utils.IsValid())
						utils->RequestRefresh();
					return FReply::Handled();
				})
				[
					SNew(SImage).Image(FAppStyle::GetBrush("Icons.ChevronDown"))
				]
			]

			// ── Remove ────────────────────────────────────────────────────────
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(4.f, 0.f, 0.f, 0.f)
			[
				PropertyCustomizationHelpers::MakeClearButton(
					FSimpleDelegate::CreateLambda([StructPropertyHandle, capturedJ, utils]()
					{
						StructPropertyHandle->NotifyPreChange();
						TArray<void*> rawData;
						StructPropertyHandle->AccessRawData(rawData);
						for (void* data : rawData)
						{
							if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
								if (def->Fields.IsValidIndex(capturedJ))
									def->Fields.RemoveAt(capturedJ);
						}
						StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ArrayRemove);
						if (utils.IsValid())
							utils->RequestRefresh();
					}),
					LOCTEXT("RemoveField", "Remove Field")
				)
			]
		];
	}
}

#undef LOCTEXT_NAMESPACE
