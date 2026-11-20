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

	void GetPickerOptions(TArray<TSharedPtr<FString>>& Options, TArray<TSharedPtr<SToolTip>>& Tooltips,
		TArray<bool>& RestrictedItems, TSharedRef<IPropertyHandle> PropertyHandle)
	{
		TArray<UObject*> owners;
		PropertyHandle->GetOuterObjects(owners);
		const bool usePalette = PropertyHandle->HasMetaData(PalettePicker);
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
			{
				if (usePalette)
					theme->Palette.GenerateKeyArray(keys);
				else
					theme->Paints.GenerateKeyArray(keys);
			}
			else
			{
				keys.RemoveAll([theme, usePalette](const FName Key)
				{
					return usePalette ? !theme->Palette.Contains(Key) : !theme->Paints.Contains(Key);
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
	if (!PropertyHandle.HasMetaData(PalettePicker) && !PropertyHandle.HasMetaData(PaintPicker))
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
