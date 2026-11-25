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

#include "MounteaThemeCategoryIconsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "Engine/Texture2D.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "IPropertyUtilities.h"
#include "UObject/UObjectGlobals.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "ScopedTransaction.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"
#include "Widgets/Text/STextBlock.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"

namespace
{
	TOptional<TSoftObjectPtr<UTexture2D>> GetCommonIcon(const TArray<TWeakObjectPtr<UObject>>& Objects,
		const FString& CategoryId, const FString& SubCategoryId, bool ResolveInheritance)
	{
		TOptional<TSoftObjectPtr<UTexture2D>> commonIcon;

		for (const auto& object : Objects)
		{
			const auto* theme = Cast<UMounteaAdvancedInventoryThemeConfig>(object.Get());

			if (!theme)
				continue;

			TSoftObjectPtr<UTexture2D> icon;
			const auto* icons = theme->CategoryIcons.Find(CategoryId);

			if (icons)
			{
				const auto* childIcon = icons->SubCategories.Find(SubCategoryId);

				if (SubCategoryId.IsEmpty() || (ResolveInheritance && (!childIcon || childIcon->IsNull())))
					icon = icons->Icon;
				else if (childIcon)
					icon = *childIcon;
			}

			if (commonIcon.IsSet() && commonIcon.GetValue() != icon)
				return {};

			commonIcon = icon;
		}

		return commonIcon;
	}

	struct FCategoryIconPreview
	{
		FSoftObjectPath Path;
		TStrongObjectPtr<UTexture2D> Texture;
		FSlateBrush Brush;
	};

	void AddCategoryIconRow(FDetailWidgetRow& Row, const FText& Label, const FString& CategoryId,
		const FString& SubCategoryId, const TArray<TWeakObjectPtr<UObject>>& Objects,
		const TSharedRef<IPropertyHandle>& PropertyHandle)
	{
		const auto preview = MakeShared<FCategoryIconPreview>();
		Row.FilterString(FText::FromString(Label.ToString() + TEXT(" ") + CategoryId + TEXT(" ") + SubCategoryId));
		Row.NameContent()
		[
			SNew(STextBlock)
			.Text(Label)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.ToolTipText(FText::FromString(SubCategoryId.IsEmpty() ? CategoryId : CategoryId + TEXT(" / ") + SubCategoryId))
		];

		Row.ValueContent()
		.MinDesiredWidth(300.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.f, 2.f, 8.f, 2.f)
			[
				SNew(SBox)
				.WidthOverride(48.f)
				.HeightOverride(48.f)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ToolTipText_Lambda([Objects, CategoryId, SubCategoryId]()
				{
					const auto icon = GetCommonIcon(Objects, CategoryId, SubCategoryId, true);

					if (!icon.IsSet())
						return NSLOCTEXT("MounteaTheme", "MixedPreview", "The selected themes resolve to different textures.");

					if (icon->IsNull())
						return NSLOCTEXT("MounteaTheme", "EmptyPreview", "No resolved texture.");

					return FText::FromString(icon->ToSoftObjectPath().ToString());
				})
				[
					SNew(SImage)
					.ForceVolatile(true)
					.DesiredSizeOverride_Lambda([preview]() -> TOptional<FVector2D>
					{
						if (!preview->Texture.IsValid())
							return FVector2D::ZeroVector;

						const FVector2D size(preview->Texture->GetSizeX(), preview->Texture->GetSizeY());
						return size * (48.0 / FMath::Max(1.0, size.GetMax()));
					})
					.Image_Lambda([preview, Objects, CategoryId, SubCategoryId]() -> const FSlateBrush*
					{
						const auto icon = GetCommonIcon(Objects, CategoryId, SubCategoryId, true);
						const FSoftObjectPath path = icon.IsSet() ? icon->ToSoftObjectPath() : FSoftObjectPath();

						if (preview->Path != path)
						{
							preview->Path = path;
							preview->Texture.Reset(icon.IsSet() ? icon->LoadSynchronous() : nullptr);
							preview->Brush.SetResourceObject(preview->Texture.Get());
						}

						preview->Brush.DrawAs = preview->Texture.IsValid() ? ESlateBrushDrawType::Image : ESlateBrushDrawType::NoDrawType;
						return &preview->Brush;
					})
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UTexture2D::StaticClass())
					.AllowClear(true)
					.DisplayThumbnail(false)
					.ToolTipText(SubCategoryId.IsEmpty()
						? NSLOCTEXT("MounteaTheme", "ParentIcon", "Texture for this category and fallback for its children.")
						: NSLOCTEXT("MounteaTheme", "ChildIcon", "Optional child texture. Clear to inherit the parent category texture."))
					.ObjectPath_Lambda([Objects, CategoryId, SubCategoryId]()
					{
						const auto icon = GetCommonIcon(Objects, CategoryId, SubCategoryId, false);
						return icon.IsSet() ? icon->ToSoftObjectPath().ToString() : FString();
					})
					.OnObjectChanged_Lambda([Objects, CategoryId, SubCategoryId, PropertyHandle](const FAssetData& Asset)
					{
						const FScopedTransaction transaction(NSLOCTEXT("MounteaTheme", "SetCategoryIcon", "Set Category Icon"));

						for (const auto& object : Objects)
						{
							if (auto* theme = Cast<UMounteaAdvancedInventoryThemeConfig>(object.Get()))
								theme->Modify();
						}

						PropertyHandle->NotifyPreChange();

						for (const auto& object : Objects)
						{
							auto* theme = Cast<UMounteaAdvancedInventoryThemeConfig>(object.Get());

							if (!theme)
								continue;

							auto& icons = theme->CategoryIcons.FindOrAdd(CategoryId);
							const TSoftObjectPtr<UTexture2D> icon(Asset.GetSoftObjectPath());

							if (SubCategoryId.IsEmpty())
								icons.Icon = icon;
							else
								icons.SubCategories.FindOrAdd(SubCategoryId) = icon;
						}

						PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
						PropertyHandle->NotifyFinishedChangingProperties();
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text_Lambda([Objects, CategoryId, SubCategoryId]()
					{
						const auto icon = GetCommonIcon(Objects, CategoryId, SubCategoryId, false);

						if (!icon.IsSet())
							return NSLOCTEXT("MounteaTheme", "MixedIcons", "Multiple values");

						if (icon->IsNull())
						{
							return SubCategoryId.IsEmpty()
								? NSLOCTEXT("MounteaTheme", "NoIcon", "No texture")
								: NSLOCTEXT("MounteaTheme", "InheritedIcon", "Inherits category texture");
						}

						return FText::GetEmpty();
					})
				]
			]
		];
	}
}

FMounteaThemeCategoryIconsCustomization::~FMounteaThemeCategoryIconsCustomization()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(this->SettingsChangedHandle);
}

TSharedRef<IDetailCustomization> FMounteaThemeCategoryIconsCustomization::MakeInstance()
{
	return MakeShared<FMounteaThemeCategoryIconsCustomization>();
}

void FMounteaThemeCategoryIconsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(this->SettingsChangedHandle);
	this->SettingsChangedHandle.Reset();

	TArray<TWeakObjectPtr<UObject>> objects;
	DetailBuilder.GetObjectsBeingCustomized(objects);
	objects.RemoveAll([](const TWeakObjectPtr<UObject>& Object)
	{
		return !Object.IsValid() || !Object->IsA<UMounteaAdvancedInventoryThemeConfig>();
	});

	if (objects.IsEmpty())
		return;

	const auto property = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventoryThemeConfig, CategoryIcons));

	if (!property->IsValidHandle())
		return;

	DetailBuilder.HideProperty(property);

	const TWeakPtr<IPropertyUtilities> weakUtilities = DetailBuilder.GetPropertyUtilities();
	this->SettingsChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda(
		[weakUtilities](UObject* Object, FPropertyChangedEvent& Event)
		{
			const auto* settings = GetDefault<UMounteaAdvancedInventorySettings>();
			const FName memberName = Event.GetMemberPropertyName();
			const bool settingsChanged = Object == settings
				&& (memberName.IsNone() || memberName == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventorySettings, AdvancedInventorySettingsConfig));
			const bool categoriesChanged = Object && Object == settings->AdvancedInventorySettingsConfig.Get()
				&& (memberName.IsNone() || memberName == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventorySettingsConfig, AllowedCategories));

			if (!settingsChanged && !categoriesChanged)
				return;

			if (const auto utilities = weakUtilities.Pin())
				utilities->RequestForceRefresh();
		});

	const auto categories = GetDefault<UMounteaAdvancedInventorySettings>()->GetAllowedCategories();
	auto& category = DetailBuilder.EditCategory(TEXT("Theme|Category Icons"));

	category.InitiallyCollapsed(false);

	for (const auto& entry : categories)
	{
		const FText categoryLabel = entry.Value.CategoryData.CategoryDisplayName.IsEmpty()
			|| entry.Value.CategoryData.CategoryDisplayName.ToString() == entry.Key
			? FText::FromString(entry.Key)
			: FText::Format(NSLOCTEXT("MounteaTheme", "CategoryLabel", "{0} [{1}]"),
				entry.Value.CategoryData.CategoryDisplayName, FText::FromString(entry.Key));
		auto& group = category.AddGroup(FName(*entry.Key), categoryLabel, false, true);
		AddCategoryIconRow(group.HeaderRow(), categoryLabel, entry.Key, FString(), objects, property);

		for (const auto& child : entry.Value.SubCategories)
		{
			const FText childLabel = child.Value.CategoryDisplayName.IsEmpty()
				|| child.Value.CategoryDisplayName.ToString() == child.Key
				? FText::FromString(child.Key)
				: FText::Format(NSLOCTEXT("MounteaTheme", "SubCategoryLabel", "{0} [{1}]"),
					child.Value.CategoryDisplayName, FText::FromString(child.Key));
			AddCategoryIconRow(group.AddWidgetRow(), childLabel, entry.Key, child.Key, objects, property);
		}
	}
}
