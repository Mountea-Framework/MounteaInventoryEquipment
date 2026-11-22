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

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

namespace
{
	FName ExpectedPickerForProperty(const FName PropertyName)
	{
		static const TMap<FName, FName> pickerByProperty =
		{
			{TEXT("Ink"), TEXT("PalettePicker")},
			{TEXT("DefaultInk"), TEXT("PalettePicker")},
			{TEXT("Foreground"), TEXT("PaintPicker")},
			{TEXT("ShadowColour"), TEXT("PaintPicker")},
			{TEXT("Tint"), TEXT("PaintPicker")},
			{TEXT("OutlineTint"), TEXT("PaintPicker")},
			{TEXT("IconTint"), TEXT("PaintPicker")},
			{TEXT("ForegroundPaint"), TEXT("PaintPicker")},
			{TEXT("Background"), TEXT("BrushPicker")},
			{TEXT("FocusDecoration"), TEXT("BrushPicker")},
			{TEXT("NormalBar"), TEXT("BrushPicker")},
			{TEXT("HoveredBar"), TEXT("BrushPicker")},
			{TEXT("DisabledBar"), TEXT("BrushPicker")},
			{TEXT("NormalThumb"), TEXT("BrushPicker")},
			{TEXT("HoveredThumb"), TEXT("BrushPicker")},
			{TEXT("DisabledThumb"), TEXT("BrushPicker")},
			{TEXT("Fill"), TEXT("BrushPicker")},
			{TEXT("Marquee"), TEXT("BrushPicker")},
			{TEXT("HoveredBackground"), TEXT("BrushPicker")},
			{TEXT("FocusedBackground"), TEXT("BrushPicker")},
			{TEXT("ReadOnlyBackground"), TEXT("BrushPicker")},
			{TEXT("TypeRole"), TEXT("TypographyPicker")},
			{TEXT("BrushRole"), TEXT("BrushPicker")},
			{TEXT("TintPaint"), TEXT("PaintPicker")},
			{TEXT("ControlRole"), TEXT("ControlPicker")},
			{TEXT("GroupOpacityRole"), TEXT("GroupOpacityPicker")}
		};

		if (const FName* picker = pickerByProperty.Find(PropertyName))
			return *picker;

		return NAME_None;
	}

	bool IsThemeModuleStruct(const UStruct* Struct)
	{
		return Struct && Struct->GetOutermost()->GetName() == TEXT("/Script/MounteaAdvancedInventorySystem");
	}

	void CheckPickerMetadata(FAutomationTestBase& Test, const FProperty* Property, const FString& PropertyPath)
	{
		if (const FNameProperty* nameProperty = CastField<FNameProperty>(Property))
		{
			const FName expectedPicker = ExpectedPickerForProperty(nameProperty->GetFName());
			Test.TestTrue(FString::Printf(TEXT("%s has an expected picker mapping"), *PropertyPath), !expectedPicker.IsNone());

			if (expectedPicker.IsNone())
				return;

			Test.TestTrue(FString::Printf(TEXT("%s has %s metadata"), *PropertyPath, *expectedPicker.ToString()),
				nameProperty->HasMetaData(expectedPicker));
			return;
		}

		if (const FStructProperty* structProperty = CastField<FStructProperty>(Property))
		{
			if (!IsThemeModuleStruct(structProperty->Struct))
				return;

			for (TFieldIterator<FProperty> iterator(structProperty->Struct); iterator; ++iterator)
			{
				CheckPickerMetadata(Test, *iterator, PropertyPath + TEXT(".") + iterator->GetName());
			}
			return;
		}

		if (const FMapProperty* mapProperty = CastField<FMapProperty>(Property))
		{
			CheckPickerMetadata(Test, mapProperty->ValueProp,
				PropertyPath + TEXT(".<value>"));
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaThemePickerMetadataTest,
	"Mountea.InventorySystemEditor.Theme.AllReferenceFieldsHavePickers",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaThemePickerMetadataTest::RunTest(const FString& Parameters)
{
	for (TFieldIterator<FProperty> iterator(UMounteaAdvancedInventoryThemeConfig::StaticClass(), EFieldIteratorFlags::ExcludeSuper); iterator; ++iterator)
	{
		CheckPickerMetadata(*this, *iterator, iterator->GetName());
	}

	for (TFieldIterator<FProperty> iterator(FMounteaWidgetThemeStyleAssignment::StaticStruct()); iterator; ++iterator)
	{
		CheckPickerMetadata(*this, *iterator, TEXT("PrefixStyle.") + iterator->GetName());
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
