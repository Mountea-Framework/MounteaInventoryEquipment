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
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
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

	TArray<FName> GetPickerKeys(const TSharedRef<IPropertyHandle>& PropertyHandle)
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
		keys.Insert(NAME_None, 0);
		return keys;
	}

	TSharedRef<SWidget> MakeBrushPreview(TWeakObjectPtr<UMounteaAdvancedInventoryThemeConfig> Theme, TAttribute<FName> Key)
	{
		const auto preview = MakeShared<FSlateBrush>();

		return SNew(SBox)
			.WidthOverride(26.f)
			.HeightOverride(14.f)
			.Visibility_Lambda([Theme, Key]()
			{
				return Theme.IsValid() && !Key.Get().IsNone() && Theme->Brushes.Contains(Key.Get())
					? EVisibility::HitTestInvisible : EVisibility::Collapsed;
			})
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SColorBlock)
					.Color(FLinearColor::Transparent)
					.ShowBackgroundForAlpha(true)
				]
				+ SOverlay::Slot()
				[
					SNew(SImage)
					.Image_Lambda([Theme, Key, preview]() -> const FSlateBrush*
					{
						if (!Theme.IsValid())
							return nullptr;

						// Tint-only roles acquire their geometry from the widget at runtime.
						*preview = FSlateRoundedBoxBrush(FLinearColor::Transparent, 3.f, FLinearColor::Transparent, 1.5f);
						Theme->PatchBrush(Key.Get(), *preview);
						return &preview.Get();
					})
				]
			];
	}

	TSharedRef<SWidget> MakeTypePreview(TWeakObjectPtr<UMounteaAdvancedInventoryThemeConfig> Theme, TAttribute<FName> Key)
	{
		return SNew(STextBlock)
			.Text(INVTEXT("Aa"))
			.Visibility_Lambda([Theme, Key]()
			{
				return Theme.IsValid() && !Key.Get().IsNone() && Theme->Typography.Contains(Key.Get())
					? EVisibility::HitTestInvisible : EVisibility::Collapsed;
			})
			.Font_Lambda([Theme, Key]()
			{
				const auto* type = Theme.IsValid() ? Theme->Typography.Find(Key.Get()) : nullptr;
				FSlateFontInfo font = type ? type->Font : FAppStyle::GetFontStyle("PropertyWindow.NormalFont");
				font.Size = 11.f;
				return font;
			});
	}

	TSharedRef<SWidget> MakePickerLabel(const TSharedRef<IPropertyHandle>& PropertyHandle, TOptional<FName> MenuKey)
	{
		const FName picker = GetPicker(*PropertyHandle);
		const auto key = TAttribute<FName>::CreateLambda([PropertyHandle, MenuKey]()
		{
			if (MenuKey.IsSet())
				return MenuKey.GetValue();

			FName selected;
			return PropertyHandle->GetValue(selected) == FPropertyAccess::Success ? selected : FName(NAME_None);
		});

		auto label = SNew(SHorizontalBox);
		TArray<UObject*> owners;
		PropertyHandle->GetOuterObjects(owners);

		for (UObject* owner : owners)
		{
			const TWeakObjectPtr<UMounteaAdvancedInventoryThemeConfig> theme = Cast<UMounteaAdvancedInventoryThemeConfig>(owner);

			if (picker == BrushPicker || picker == TypographyPicker)
			{
				label->AddSlot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					picker == BrushPicker ? MakeBrushPreview(theme, key) : MakeTypePreview(theme, key)
				];
				continue;
			}

			const bool isPaint = picker == PaintPicker;
			const auto findColour = [theme, key, isPaint]() -> TOptional<FLinearColor>
			{
				const FName role = key.Get();

				if (!theme.IsValid() || role.IsNone())
					return {};

				if (isPaint)
				{
					FLinearColor colour;

					if (theme->ResolvePaint(role, colour))
						return colour;
				}
				else if (const FLinearColor* ink = theme->Palette.Find(role))
					return *ink;

				return {};
			};

			label->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SColorBlock)
				.Size(FVector2D(20.f, 12.f))
				.CornerRadius(FVector4(3.f))
				.AlphaDisplayMode(isPaint ? EColorBlockAlphaDisplayMode::Combined : EColorBlockAlphaDisplayMode::Ignore)
				.ShowBackgroundForAlpha(isPaint)
				.Visibility_Lambda([findColour]()
				{
					return findColour().IsSet() ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
				})
				.Color_Lambda([findColour]()
				{
					return findColour().Get(FLinearColor::Transparent);
				})
			];
		}

		label->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(4.f, 0.f)
		[
			SNew(STextBlock)
			.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			.Text_Lambda([PropertyHandle, MenuKey]()
			{
				if (MenuKey.IsSet())
					return FText::FromName(MenuKey.GetValue());

				FText value;
				PropertyHandle->GetValueAsDisplayText(value);
				return value;
			})
		];

		return label;
	}

	TSharedRef<SWidget> MakeThemePicker(const TSharedRef<IPropertyHandle>& PropertyHandle)
	{
		return SNew(SComboButton)
			.ContentPadding(FMargin(4.f, 2.f))
			.OnGetMenuContent_Lambda([PropertyHandle]()
			{
				FMenuBuilder menu(true, nullptr);

				for (const FName key : GetPickerKeys(PropertyHandle))
				{
					const FUIAction select(FExecuteAction::CreateLambda([PropertyHandle, key]()
					{
						PropertyHandle->SetValue(key);
					}));

					menu.AddMenuEntry(select, MakePickerLabel(PropertyHandle, key));
				}

				return menu.MakeWidget(nullptr, 400);
			})
			.ButtonContent()
			[
				MakePickerLabel(PropertyHandle, {})
			];
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
		MakeThemePicker(PropertyHandle)
	];
}

void FMounteaThemePickerCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}
