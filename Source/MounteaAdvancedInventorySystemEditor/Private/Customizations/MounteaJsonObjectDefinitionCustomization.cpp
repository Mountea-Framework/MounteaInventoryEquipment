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

	static const FMounteaJsonObjectDefinition* GetDefinition(const TSharedRef<IPropertyHandle>& StructHandle)
	{
		TArray<void*> rawData;
		StructHandle->AccessRawData(rawData);
		if (rawData.Num() != 1 || !rawData[0])
			return nullptr;

		return static_cast<const FMounteaJsonObjectDefinition*>(rawData[0]);
	}

	static int32 GetIncludeCount(const TSharedRef<IPropertyHandle>& StructHandle)
	{
		const FMounteaJsonObjectDefinition* def = GetDefinition(StructHandle);
		return def ? def->IncludedDefinitions.Num() : 0;
	}

	static int32 GetFieldCount(const TSharedRef<IPropertyHandle>& StructHandle)
	{
		const FMounteaJsonObjectDefinition* def = GetDefinition(StructHandle);
		return def ? def->Fields.Num() : 0;
	}

	static FString GetIncludeKey(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index)
	{
		const FMounteaJsonObjectDefinition* def = GetDefinition(StructHandle);
		return def && def->IncludedDefinitions.IsValidIndex(Index)
			? def->IncludedDefinitions[Index].DefinitionKey
			: FString();
	}

	static void SetIncludeKey(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index, const FString& Key, const TSharedPtr<IPropertyUtilities>& Utils)
	{
		StructHandle->NotifyPreChange();
		TArray<void*> rawData;
		StructHandle->AccessRawData(rawData);
		for (void* data : rawData)
		{
			if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
				if (def->IncludedDefinitions.IsValidIndex(Index))
					def->IncludedDefinitions[Index].DefinitionKey = Key;
		}
		StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		if (Utils.IsValid())
			Utils->RequestRefresh();
	}

	static FName GetFieldName(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index)
	{
		const FMounteaJsonObjectDefinition* def = GetDefinition(StructHandle);
		return def && def->Fields.IsValidIndex(Index)
			? def->Fields[Index].FieldName
			: NAME_None;
	}

	static void SetFieldName(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index, const FName Name, const TSharedPtr<IPropertyUtilities>& Utils)
	{
		StructHandle->NotifyPreChange();
		TArray<void*> rawData;
		StructHandle->AccessRawData(rawData);
		for (void* data : rawData)
		{
			if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
				if (def->Fields.IsValidIndex(Index))
					def->Fields[Index].FieldName = Name;
		}
		StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		if (Utils.IsValid())
			Utils->RequestRefresh();
	}

	static FEdGraphPinType MakeDefaultFieldPinType()
	{
		FEdGraphPinType pinType;
		pinType.PinCategory = UEdGraphSchema_K2::PC_String;
		pinType.ContainerType = EPinContainerType::None;
		return pinType;
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

	static FEdGraphPinType GetFieldPinType(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index)
	{
		const FMounteaJsonObjectDefinition* def = GetDefinition(StructHandle);
		if (def && def->Fields.IsValidIndex(Index))
		{
			const FMounteaJsonObjectDefinitionField& field = def->Fields[Index];
			if (!field.FieldValueType.PinCategory.IsNone())
				return field.FieldValueType;
		}

		return MakeDefaultFieldPinType();
	}

	static void SetFieldPinType(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index, const FEdGraphPinType& PinType, const TSharedPtr<IPropertyUtilities>& Utils)
	{
		if (!IsSupportedPinType(PinType))
			return;

		StructHandle->NotifyPreChange();
		TArray<void*> rawData;
		StructHandle->AccessRawData(rawData);
		for (void* data : rawData)
		{
			if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
			{
				if (def->Fields.IsValidIndex(Index))
				{
					def->Fields[Index].FieldValueType = PinType;
					def->Fields[Index].FieldValueType.ContainerType = EPinContainerType::None;
				}
			}
		}
		StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		if (Utils.IsValid())
			Utils->RequestRefresh();
	}

	static bool GetFieldRequired(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index)
	{
		const FMounteaJsonObjectDefinition* def = GetDefinition(StructHandle);
		return def && def->Fields.IsValidIndex(Index)
			? def->Fields[Index].bRequired
			: true;
	}

	static void SetFieldRequired(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index, bool bRequired, const TSharedPtr<IPropertyUtilities>& Utils)
	{
		StructHandle->NotifyPreChange();
		TArray<void*> rawData;
		StructHandle->AccessRawData(rawData);
		for (void* data : rawData)
		{
			if (FMounteaJsonObjectDefinition* def = static_cast<FMounteaJsonObjectDefinition*>(data))
				if (def->Fields.IsValidIndex(Index))
					def->Fields[Index].bRequired = bRequired;
		}
		StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
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
			int32 foundIndex = INDEX_NONE;
			if (Text.ToString().FindChar(ch, foundIndex))
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

	static FText GetIncludePreviewText(const TSharedRef<IPropertyHandle>& StructHandle, int32 Index)
	{
		const FString key = GetIncludeKey(StructHandle, Index);
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
	const TSharedPtr<IPropertyUtilities> utils = StructCustomizationUtils.GetPropertyUtilities();

	if (!NewFieldPinType.IsValid())
	{
		NewFieldPinType = MakeShared<FEdGraphPinType>(
			MounteaJsonObjectDefinitionCustomization::MakeDefaultFieldPinType());
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

	const int32 numIncludes = MounteaJsonObjectDefinitionCustomization::GetIncludeCount(StructPropertyHandle);

	for (int32 i = 0; i < numIncludes; ++i)
	{
		TSharedRef<TArray<TSharedPtr<FString>>> incOptions = MakeShared<TArray<TSharedPtr<FString>>>();
		MounteaJsonObjectDefinitionCustomization::RebuildDefinitionOptions(StructPropertyHandle, incOptions);

		const int32 capturedI = i;

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
					.OnSelectionChanged_Lambda([StructPropertyHandle, capturedI, utils](const TSharedPtr<FString>& Selection, ESelectInfo::Type)
					{
						if (Selection.IsValid())
							MounteaJsonObjectDefinitionCustomization::SetIncludeKey(StructPropertyHandle, capturedI, *Selection, utils);
					})
					[
						SNew(STextBlock)
						.Font(IDetailLayoutBuilder::GetDetailFont())
						.Text_Lambda([StructPropertyHandle, capturedI]()
						{
							const FString key = MounteaJsonObjectDefinitionCustomization::GetIncludeKey(StructPropertyHandle, capturedI);
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
				.Text_Lambda([StructPropertyHandle, capturedI]()
				{
					return MounteaJsonObjectDefinitionCustomization::GetIncludePreviewText(StructPropertyHandle, capturedI);
				})
				.Visibility_Lambda([StructPropertyHandle, capturedI]()
				{
					const FString key = MounteaJsonObjectDefinitionCustomization::GetIncludeKey(StructPropertyHandle, capturedI);
					return key.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
				})
			]
		];
	}

	// ── Field rows ────────────────────────────────────────────────────────────

	const int32 numFields = MounteaJsonObjectDefinitionCustomization::GetFieldCount(StructPropertyHandle);

	for (int32 j = 0; j < numFields; ++j)
	{
		const int32 capturedJ = j;
		const int32 capturedNumFields = numFields;

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
				.Text_Lambda([StructPropertyHandle, capturedJ]()
				{
					const FName value = MounteaJsonObjectDefinitionCustomization::GetFieldName(StructPropertyHandle, capturedJ);
					return FText::FromName(value);
				})
				.OnTextCommitted_Lambda([StructPropertyHandle, capturedJ, utils](const FText& NewText, ETextCommit::Type)
				{
					if (!MounteaJsonObjectDefinitionCustomization::IsNameValid(NewText))
						return;
					MounteaJsonObjectDefinitionCustomization::SetFieldName(
						StructPropertyHandle,
						capturedJ,
						FName(*NewText.ToString().TrimStartAndEnd()),
						utils);
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
				.TargetPinType_Lambda([StructPropertyHandle, capturedJ]()
				{
					return MounteaJsonObjectDefinitionCustomization::GetFieldPinType(StructPropertyHandle, capturedJ);
				})
				.OnPinTypeChanged_Lambda([StructPropertyHandle, capturedJ, utils](const FEdGraphPinType& NewType)
				{
					MounteaJsonObjectDefinitionCustomization::SetFieldPinType(StructPropertyHandle, capturedJ, NewType, utils);
				})
				.Schema(GetDefault<UEdGraphSchema_K2>())
				.bAllowArrays(false)
				.SelectorType(SPinTypeSelector::ESelectorType::Partial)
				.TypeTreeFilter(ETypeTreeFilter::None)
				.CustomFilters(pinFilters)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]

			// ── Required checkbox ─────────────────────────────────────────────
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.f, 0.f, 6.f, 0.f)
			[
				SNew(SCheckBox)
				.IsChecked_Lambda([StructPropertyHandle, capturedJ]()
				{
					const bool val = MounteaJsonObjectDefinitionCustomization::GetFieldRequired(StructPropertyHandle, capturedJ);
					return val ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
				.OnCheckStateChanged_Lambda([StructPropertyHandle, capturedJ, utils](ECheckBoxState NewState)
				{
					MounteaJsonObjectDefinitionCustomization::SetFieldRequired(
						StructPropertyHandle,
						capturedJ,
						NewState == ECheckBoxState::Checked,
						utils);
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
