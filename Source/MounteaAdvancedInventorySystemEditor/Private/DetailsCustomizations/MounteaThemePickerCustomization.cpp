// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "MounteaThemePickerCustomization.h"

#include "DetailWidgetRow.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

namespace
{
	const FName PalettePicker(TEXT("PalettePicker"));
	const FName PaintPicker(TEXT("PaintPicker"));
	const FName BrushPicker(TEXT("BrushPicker"));
	const FName TypographyPicker(TEXT("TypographyPicker"));

	FName GetPicker(const IPropertyHandle& PropertyHandle)
	{
		for (const FName picker : { PalettePicker, PaintPicker, BrushPicker, TypographyPicker })
		{
			if (PropertyHandle.HasMetaData(picker))
				return picker;
		}

		return NAME_None;
	}

	void GetThemeKeys(const UMounteaAdvancedInventoryThemeConfig& Theme, const FName Picker, TArray<FName>& Keys)
	{
		if (Picker == PalettePicker)
			Theme.Palette.GenerateKeyArray(Keys);
		else if (Picker == PaintPicker)
			Theme.Paints.GenerateKeyArray(Keys);
		else if (Picker == BrushPicker)
			Theme.Brushes.GenerateKeyArray(Keys);
		else if (Picker == TypographyPicker)
			Theme.Typography.GenerateKeyArray(Keys);
	}

	void GetPickerOptions(TArray<TSharedPtr<FString>>& Options, TArray<TSharedPtr<SToolTip>>& Tooltips,
		TArray<bool>& RestrictedItems, TSharedRef<IPropertyHandle> PropertyHandle)
	{
		TArray<UObject*> owners;
		PropertyHandle->GetOuterObjects(owners);
		const FName picker = GetPicker(*PropertyHandle);
		TArray<FName> keys;

		for (int32 index = 0; index < owners.Num(); ++index)
		{
			const auto* theme = Cast<UMounteaAdvancedInventoryThemeConfig>(owners[index]);

			if (!theme)
			{
				keys.Reset();
				break;
			}

			if (index == 0)
				GetThemeKeys(*theme, picker, keys);
			else
			{
				TArray<FName> otherKeys;
				GetThemeKeys(*theme, picker, otherKeys);
				const TSet<FName> sharedCandidates(otherKeys);
				keys.RemoveAll([&sharedCandidates](const FName Key)
				{
					return !sharedCandidates.Contains(Key);
				});
			}
		}

		keys.Remove(NAME_None);
		keys.Sort(FNameLexicalLess());
		Options.Add(MakeShared<FString>(FName(NAME_None).ToString()));

		for (const FName key : keys)
		{
			Options.Add(MakeShared<FString>(key.ToString()));
		}
	}
}

bool FMounteaThemePickerIdentifier::IsPropertyTypeCustomized(const IPropertyHandle& PropertyHandle) const
{
	if (GetPicker(PropertyHandle).IsNone())
		return false;

	TArray<UObject*> owners;
	PropertyHandle.GetOuterObjects(owners);

	for (const UObject* owner : owners)
	{
		if (!IsValid(owner) || !owner->IsA<UMounteaAdvancedInventoryThemeConfig>())
			return false;
	}

	return !owners.IsEmpty();
}

TSharedRef<IPropertyTypeCustomization> FMounteaThemePickerCustomization::MakeInstance()
{
	return MakeShared<FMounteaThemePickerCustomization>();
}

void FMounteaThemePickerCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(160.f)
	[
		PropertyCustomizationHelpers::MakePropertyComboBox(PropertyHandle,
			FOnGetPropertyComboBoxStrings::CreateStatic(&GetPickerOptions, PropertyHandle))
	];
}

void FMounteaThemePickerCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}
