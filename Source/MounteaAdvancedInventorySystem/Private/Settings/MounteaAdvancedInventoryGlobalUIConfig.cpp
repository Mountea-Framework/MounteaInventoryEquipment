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

#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"

#include "Definitions/MounteaInventoryBaseCommands.h"
#include "Statics/MounteaInventoryBaseUIStatics.h"

UMounteaAdvancedInventoryGlobalUIConfig::UMounteaAdvancedInventoryGlobalUIConfig()
{
	ValidateModalTypes();
	SetupWidgetCommands();
}

const TArray<FString>& UMounteaAdvancedInventoryGlobalUIConfig::GetDefaultModalTypes()
{
	static const TArray<FString> defaultModalTypes
	{
		TEXT("Inventory.DropItem"),
		TEXT("Inventory.UseItem"),
		TEXT("Inventory.DestroyItem"),
		TEXT("Inventory.SplitStack"),
		TEXT("Crafting.NoRecipes"),
		TEXT("Crafting.CraftItem"),
		TEXT("Crafting.MissingIngredients"),
		TEXT("Equipment.EquipItem"),
		TEXT("Equipment.UnequipItem"),
		TEXT("Equipment.ReplaceItem"),
		TEXT("Pickup.PickupItem"),
		TEXT("Loot.TakeItem"),
		TEXT("Merchant.BuyItem"),
		TEXT("Merchant.SellItem"),
		TEXT("Merchant.InsufficientFunds"),
		TEXT("Stealing.StealItem")
	};
	return defaultModalTypes;
}

void UMounteaAdvancedInventoryGlobalUIConfig::ValidateModalTypes()
{
	for (const FString& defaultModalType : GetDefaultModalTypes())
	{
		if (ModalTypes.Contains(defaultModalType)) 
			continue;
		ModalTypes.Add(defaultModalType);
	}
}

void UMounteaAdvancedInventoryGlobalUIConfig::SetupWidgetCommands()
{
	for (const FString& command : InventoryUICommands::AllCommands)
	{
		if (!WidgetCommands.Contains(command))
			WidgetCommands.Add(command);
	}
}

TArray<FString> UMounteaAdvancedInventoryGlobalUIConfig::GetModalTypes() const
{
	TArray<FString> returnValues;
	returnValues.Reserve(ModalTypes.Num() + 1);
	returnValues.Add(TEXT("none"));
	returnValues.Append(ModalTypes.Array());
	return returnValues;
}

#if WITH_EDITOR
void UMounteaAdvancedInventoryGlobalUIConfig::SetDefaultModalTypes()
{
	ValidateModalTypes();
}

void UMounteaAdvancedInventoryGlobalUIConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UMounteaInventoryBaseUIStatics::InvalidateWidgetThemePrefixCache(this);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventoryGlobalUIConfig, ModalTypes))
		ValidateModalTypes();

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventoryGlobalUIConfig, WidgetCommands))
		SetupWidgetCommands();
}
#endif

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/ProgressBar.h"
#include "Components/EditableTextBox.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

void UMounteaAdvancedInventoryGlobalUIConfig::ValidateThemeRules(const UMounteaAdvancedInventoryThemeConfig* Theme, FDataValidationContext& Context) const
{
	TMap<UClass*, TSet<FName>> aliasesByClass;

	for (const FMounteaWidgetPrefixGroup& group : this->WidgetPrefixGroups)
	{
		UClass* widgetClass = group.WidgetClass.Get();

		if (!widgetClass)
		{
			Context.AddError(FText::FromString(TEXT("Theme prefix groups require a widget class.")));
			continue;
		}

		TSet<FName>& aliases = aliasesByClass.FindOrAdd(widgetClass);

		for (const FMounteaWidgetPrefixRule& rule : group.Rules)
		{
			bool compatible = false;
			bool rolesValid = true;
			const FMounteaWidgetThemeStyleAssignment& style = rule.Style;

			switch (style.StyleType)
			{
				case EMounteaWidgetThemeStyleType::Text:
					compatible = widgetClass->IsChildOf(UTextBlock::StaticClass());
					rolesValid = !style.TypeRole.IsNone() && (!Theme || Theme->Typography.Contains(style.TypeRole))
						&& (style.ForegroundPaint.IsNone() || !Theme || Theme->Paints.Contains(style.ForegroundPaint));
					break;
				case EMounteaWidgetThemeStyleType::Brush:
					compatible = widgetClass->IsChildOf(UBorder::StaticClass()) || widgetClass->IsChildOf(UImage::StaticClass());
					rolesValid = !style.BrushRole.IsNone() && (!Theme || Theme->Brushes.Contains(style.BrushRole));
					break;
				case EMounteaWidgetThemeStyleType::ImageTint:
					compatible = widgetClass->IsChildOf(UImage::StaticClass());
					rolesValid = !style.TintPaint.IsNone() && (!Theme || Theme->Paints.Contains(style.TintPaint));
					break;
				case EMounteaWidgetThemeStyleType::Control:
					compatible = widgetClass->IsChildOf(UButton::StaticClass()) || widgetClass->IsChildOf(USlider::StaticClass())
						|| widgetClass->IsChildOf(UProgressBar::StaticClass()) || widgetClass->IsChildOf(UEditableTextBox::StaticClass());
					rolesValid = !style.ControlRole.IsNone() && (!Theme || Theme->Controls.Contains(style.ControlRole));
					break;
				case EMounteaWidgetThemeStyleType::GroupOpacity:
					compatible = widgetClass->IsChildOf(UWidget::StaticClass());
					rolesValid = !style.GroupOpacityRole.IsNone() && (!Theme || Theme->GroupOpacity.Contains(style.GroupOpacityRole));
					break;
				default:
					break;
			}

			if (!style.TintPaint.IsNone() && (style.StyleType == EMounteaWidgetThemeStyleType::Brush || style.StyleType == EMounteaWidgetThemeStyleType::Control))
			{
				rolesValid = rolesValid && (!Theme || Theme->Paints.Contains(style.TintPaint));

				if (style.StyleType == EMounteaWidgetThemeStyleType::Control && !widgetClass->IsChildOf(UProgressBar::StaticClass()))
					Context.AddError(FText::FromString(TEXT("Control TintPaint is supported only for progress fill-colour multipliers.")));
			}

			if (Theme && style.StyleType == EMounteaWidgetThemeStyleType::Control && widgetClass->IsChildOf(UButton::StaticClass()))
			{
				const FMounteaThemeControlStyle* controlStyle = Theme->Controls.Find(style.ControlRole);

				if (controlStyle)
				{
					for (const auto& state : controlStyle->States)
					{
						if (state.Key == EMounteaThemeControlState::Focused || state.Key == EMounteaThemeControlState::Selected
							|| !state.Value.IconTint.IsNone() || !state.Value.FocusDecoration.IsNone())
							Context.AddWarning(FText::FromString(FString::Printf(TEXT("Button role '%s' contains focus, selection, icon or decoration fields that require its external state owner; the prefix pass installs native button states only."), *style.ControlRole.ToString())));
					}
				}
			}

			if (!compatible)
				Context.AddError(FText::FromString(FString::Printf(TEXT("Widget class '%s' does not support its theme assignment."), *widgetClass->GetPathName())));

			if (!rolesValid)
				Context.AddError(FText::FromString(FString::Printf(TEXT("Prefix rule for '%s' has an empty or unresolved role in theme '%s'."), *widgetClass->GetName(), *GetPathNameSafe(Theme))));

			if (rule.Prefixes.IsEmpty())
				Context.AddError(FText::FromString(TEXT("Theme prefix rules require at least one alias.")));

			for (FName prefix : rule.Prefixes)
			{
				const FString prefixString = prefix.ToString();
				int32 separatorIndex = INDEX_NONE;
				prefixString.FindChar(TEXT('_'), separatorIndex);

				if (prefix.IsNone() || prefixString.Len() < 2 || separatorIndex != prefixString.Len() - 1)
					Context.AddError(FText::FromString(FString::Printf(TEXT("Theme prefix '%s' must contain one trailing underscore."), *prefixString)));

				if (aliases.Contains(prefix))
					Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicate theme prefix '%s' for class '%s'."), *prefixString, *widgetClass->GetName())));

				aliases.Add(prefix);
			}
		}
	}
}

EDataValidationResult UMounteaAdvancedInventoryGlobalUIConfig::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	const UMounteaAdvancedInventoryThemeConfig* theme = this->BaseTheme.LoadSynchronous();

	if (!this->BaseTheme.IsNull() && !IsValid(theme))
		Context.AddError(FText::FromString(FString::Printf(TEXT("Cannot resolve base theme '%s'."), *this->BaseTheme.ToSoftObjectPath().ToString())));

	this->ValidateThemeRules(theme, Context);

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
