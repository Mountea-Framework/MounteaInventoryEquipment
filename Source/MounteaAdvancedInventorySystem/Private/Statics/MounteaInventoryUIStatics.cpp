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


#include "Statics/MounteaInventoryUIStatics.h"

#include "InputMappingContext.h"
#include "Algo/Copy.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

#include "Interfaces/Widgets/MounteaInventorySystemBaseWidgetInterface.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/ItemActions/MounteaAdvancedInventoryItemActionInterface.h"
#include "Interfaces/Widgets/BaseWidget/MounteaAdvancedBaseInventoryWidgetInterface.h"
#include "Interfaces/Widgets/Inventory/MounteaAdvancedInventoryWidgetInterface.h"
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoriesWrapperWidgetInterface.h"
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoryWidgetInterface.h"
#include "Interfaces/Widgets/ItemActions/MounteaAdvancedInventoryItemActionsContainerWidgetInterface.h"
#include "Interfaces/Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemsGridWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface.h"
#include "Interfaces/Widgets/Tooltip/MounteaAdvancedInventoryTooltipWidgetInterface.h"

#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"
#include "Statics/MounteaInventoryStatics.h"

#include "Statics/MounteaInventorySystemStatics.h"
#include "Widgets/ItemPreview/MounteaAdvancedInventoryInteractableObjectWidget.h"

APlayerController* UMounteaInventoryUIStatics::FindPlayerController(AActor* Actor, int SearchDepth)
{
	SearchDepth++;
	if (SearchDepth >= 8)
		return nullptr;
	
	if (APlayerController* playerController = Cast<APlayerController>(Actor))
		return playerController;

	if (APlayerState* playerState = Cast<APlayerState>(Actor))
		return playerState->GetPlayerController();

	if (APawn* actorPawn = Cast<APawn>(Actor))
		return Cast<APlayerController>(actorPawn->GetController());
	
	if (AActor* ownerActor = Actor->GetOwner())
		return FindPlayerController(ownerActor, SearchDepth);
	
	return nullptr;
}

void UMounteaInventoryUIStatics::SetOwningInventoryUIInternal(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI)
{
	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("[SetOwningInventoryUI] Failed to set Owning Inventory UI. Reason: Target is null."));
		return;
	}

	if (!Target->Implements<UMounteaAdvancedBaseInventoryWidgetInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("[SetOwningInventoryUI] Failed to set Owning Inventory UI for %s. Reason: Target does not implement IMounteaAdvancedBaseInventoryWidgetInterface."),
			*Target->GetName());
		return;
	}

	IMounteaAdvancedBaseInventoryWidgetInterface::Execute_SetOwningInventoryUI(Target, NewOwningInventoryUI);
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryUIStatics::GetParentInventory(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetParentInventory(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::SetParentInventory(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	if (Target.GetObject())
		Target->Execute_SetParentInventory(Target.GetObject(), NewParentInventory);
}

bool UMounteaInventoryUIStatics::CreateMainUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_CreateMainUIWrapper(Target.GetObject()) : false;
}

UUserWidget* UMounteaInventoryUIStatics::GetMainUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetMainUIWrapper(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::RemoveMainUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveMainUIWrapper(Target.GetObject());
}

UUserWidget* UMounteaInventoryUIStatics::GetNotificationContainer(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetNotificationContainer(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::SetNotificationContainer(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, UUserWidget* NewNotificationContainer)
{
	if (Target.GetObject())
		Target->Execute_SetNotificationContainer(Target.GetObject(), NewNotificationContainer);
}

void UMounteaInventoryUIStatics::CreateInventoryNotification(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target,
	const FInventoryNotificationData& NotificationData)
{
	if (Target.GetObject())
		Target->Execute_CreateInventoryNotification(Target.GetObject(), NotificationData);
}

void UMounteaInventoryUIStatics::RemoveInventoryNotifications(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryNotifications(Target.GetObject());
}

void UMounteaInventoryUIStatics::CategorySelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target,
	const FString& SelectedCategoryId)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIInterface::Execute_CategorySelected(Target.GetObject(), SelectedCategoryId);
}

FString UMounteaInventoryUIStatics::GetSelectedCategoryId(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetSelectedCategoryId(Target.GetObject()) : TEXT("none");
}

void UMounteaInventoryUIStatics::ItemSelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target,
											  const FGuid& ItemGuid)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIInterface::Execute_ItemSelected(Target.GetObject(), ItemGuid);
}

FGuid UMounteaInventoryUIStatics::GetSelectedItemGuid(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetActiveItemGuid(Target.GetObject()) : FGuid();
}

void UMounteaInventoryUIStatics::SetInventoryOwningInventoryUI(UWidget* Target,
															   const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetCategoriesWrapperOwningInventoryUI(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetActiveCategoryId(UWidget* Target, const FString& ActiveCategoryId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface>())
		IMounteaAdvancedInventoryCategoriesWrapperWidgetInterface::Execute_SetActiveCategoryId(Target, ActiveCategoryId);
}

FString UMounteaInventoryUIStatics::GetActiveCategoryId(UWidget* Target)
{
	return (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface>())
		? IMounteaAdvancedInventoryCategoriesWrapperWidgetInterface::Execute_GetActiveCategoryId(Target) : TEXT("none");
}

bool UMounteaInventoryUIStatics::IsMainUIOpen(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	if (!IsValid(Target.GetObject())) return false;
	switch (Target->Execute_GetMainUIVisibility(Target.GetObject()))
	{
		case ESlateVisibility::Visible:
		case ESlateVisibility::HitTestInvisible:
		case ESlateVisibility::SelfHitTestInvisible:
			return true;
			break;
		case ESlateVisibility::Collapsed:
		case ESlateVisibility::Hidden:
			return false;
			break;
	}
	return false;
}

void UMounteaInventoryUIStatics::ApplyTheme(UWidget* Target)
{
	if (!IsValid(Target)) return;
	IMounteaInventoryGenericWidgetInterface::Execute_ApplyTheme(Target);
}

UMounteaAdvancedInventoryThemeConfig* UMounteaInventoryUIStatics::GetThemeConfig()
{
	const auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings))
	{
		LOG_ERROR(TEXT("[GetThemeConfig] Settings not found!"))
		return nullptr;
	}
	const auto config = settings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(config))
	{
		LOG_ERROR(TEXT("[GetThemeConfig] Config not found!"))
		return nullptr;
	}
	return config->BaseTheme.LoadSynchronous();
}

void UMounteaInventoryUIStatics::SetGridSlotData(FMounteaInventoryGridSlot& SourceData,
												 const FMounteaInventoryGridSlot& TargetData)
{
	SourceData = TargetData;
}

void UMounteaInventoryUIStatics::StoreItem(FMounteaInventoryGridSlot& SourceData,
										   const FGuid& ItemId)
{
	SourceData.OccupiedItemId = ItemId;
}

void UMounteaInventoryUIStatics::ResetItem(FMounteaInventoryGridSlot& SourceData)
{
	SourceData.ResetSlot();
}

FString UMounteaInventoryUIStatics::GridSlot_ToString(const FMounteaInventoryGridSlot& SourceData)
{
	return SourceData.ToString();
}

bool UMounteaInventoryUIStatics::IsInputAllowed(const FKey& InputKey, const FName& InputName)
{
	if (InputName.IsNone())
		return false;
    
	const auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings))
		return false;

	const auto inputMapping = settings->AdvancedInventoryEquipmentInputMapping.LoadSynchronous();
	if (!IsValid(inputMapping))
		return false;

	const auto inputActions = inputMapping->GetMappings();
	if (inputActions.IsEmpty())
		return false;
	
	const FText inputText = FText::FromName(InputName);

	const auto* matchingAction = Algo::FindByPredicate(inputActions, [&](const auto& inputAction)
	{
		return inputAction.Action->GetFName().ToString() == InputName.ToString() && inputAction.Key == InputKey;
	});

	return matchingAction != nullptr;
}

FInventoryItem UMounteaInventoryUIStatics::FindItem(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target,
	const FInventoryItemSearchParams& SearchParams)
{
	if (!IsValid(Target.GetObject()))
		return {};

	const auto& inventory = Target->Execute_GetParentInventory(Target.GetObject());
	if (!IsValid(inventory.GetObject()))
		return {};

	return inventory->Execute_FindItem(inventory.GetObject(), SearchParams);
}

FVector2D UMounteaInventoryUIStatics::GetActionsListSpawnLocation(UWidget* ParentWidget)
{
	if (!IsValid(ParentWidget))
	{
		LOG_ERROR(TEXT("[GetActionsListSpawnLocation] Invalid Parent Widget!"));
		return FVector2D::ZeroVector;
	}

	if (!IsValid(ParentWidget->GetWorld()))
	{
		LOG_ERROR(TEXT("[GetActionsListSpawnLocation] Invalid World!"));
		return FVector2D::ZeroVector;
	}
	const auto Geometry = ParentWidget->GetCachedGeometry();
	FVector2D localCoordinates = FVector2D(Geometry.GetLocalSize().X, 0.f);
	FVector2D pixelPosition, viewportPosition;
	USlateBlueprintLibrary::LocalToViewport(ParentWidget->GetWorld(), Geometry, localCoordinates, pixelPosition, viewportPosition);

	return pixelPosition;
}

bool UMounteaInventoryUIStatics::MouseEvent_IsInputAllowed(const FPointerEvent& MouseEvent, const FName& InputName)
{
	return IsInputAllowed(MouseEvent.GetEffectingButton(), InputName);
}

bool UMounteaInventoryUIStatics::KeyEvent_IsInputAllowed(const FKeyEvent& InKeyEvent, const FName& InputName)
{
	return IsInputAllowed(InKeyEvent.GetKey(), InputName);
}

FButtonStyle UMounteaInventoryUIStatics::MakeButtonStyle(
	const FButtonStyle& BaseBrush,
	const EMounteaThemeLevel Level)
{
	FButtonStyle returnValue;
	returnValue.Normal = MakeSlateBrush(BaseBrush.Normal, Level, EMounteaThemeState::Normal, EMounteaThemeType::Background);
	returnValue.Hovered = MakeSlateBrush(BaseBrush.Hovered, Level, EMounteaThemeState::Hovered, EMounteaThemeType::Background);
	returnValue.Pressed = MakeSlateBrush(BaseBrush.Pressed, Level, EMounteaThemeState::Active, EMounteaThemeType::Background);
	returnValue.Disabled = MakeSlateBrush(BaseBrush.Disabled, Level, EMounteaThemeState::Disabled, EMounteaThemeType::Background);

	FSlateBrushOutlineSettings OutlineSettings;
	OutlineSettings = MakeSlateBrushOutline(OutlineSettings, Level, EMounteaThemeState::Hovered, true, true, true, true);
	returnValue.Hovered.OutlineSettings = OutlineSettings;

	// Optional: Customize padding or text alignment
	returnValue.PressedPadding = BaseBrush.PressedPadding;
	returnValue.NormalPadding = BaseBrush.NormalPadding;
	returnValue.PressedSlateSound = FSlateSound();
	returnValue.HoveredSlateSound = FSlateSound();

	return returnValue;
}

FScrollBarStyle UMounteaInventoryUIStatics::MakeScrollBarStyle(const FScrollBarStyle& SourceStyle,
	const EMounteaThemeLevel Level,
	const EMounteaThemeState State)
{
	auto returnValue = SourceStyle;
	switch (State)
	{
		case EMounteaThemeState::Normal:
			returnValue.NormalThumbImage = MakeSlateBrush(SourceStyle.NormalThumbImage, Level, State, EMounteaThemeType::Background);
			break;
		case EMounteaThemeState::Hovered:
			returnValue.HoveredThumbImage = MakeSlateBrush(SourceStyle.NormalThumbImage, Level, State, EMounteaThemeType::Background);
			break;
		case EMounteaThemeState::Active:
			returnValue.DraggedThumbImage = MakeSlateBrush(SourceStyle.NormalThumbImage, Level, State, EMounteaThemeType::Background);
			break;
		case EMounteaThemeState::Disabled:
			break;
	}
	return returnValue;
}

FScrollBarStyle UMounteaInventoryUIStatics::ApplyScrollBarStyle(const FScrollBarStyle& SourceStyle,
	const EMounteaThemeLevel Level)
{
	auto returnValue = SourceStyle;
	returnValue.NormalThumbImage = ApplySlateBrush(SourceStyle.NormalThumbImage, Level, EMounteaThemeState::Normal, EMounteaThemeType::Background);
	returnValue.HoveredThumbImage = ApplySlateBrush(SourceStyle.NormalThumbImage, Level, EMounteaThemeState::Hovered, EMounteaThemeType::Background);
	returnValue.DraggedThumbImage = ApplySlateBrush(SourceStyle.NormalThumbImage, Level, EMounteaThemeState::Active, EMounteaThemeType::Background);
	return returnValue;
}

FButtonStyle UMounteaInventoryUIStatics::ApplyButtonStyle(const FButtonStyle& BaseBrush, EMounteaThemeLevel Level,
														  const bool bApplyCorner1, const bool bApplyCorner2, const bool bApplyCorner3, const bool bApplyCorner4)
{
	FButtonStyle returnValue;
	returnValue.Normal = ApplySlateBrush(BaseBrush.Normal, Level, EMounteaThemeState::Normal, EMounteaThemeType::Background, bApplyCorner1, bApplyCorner2, bApplyCorner3, bApplyCorner4);
	returnValue.Hovered = ApplySlateBrush(BaseBrush.Hovered, Level, EMounteaThemeState::Hovered, EMounteaThemeType::Background, bApplyCorner1, bApplyCorner2, bApplyCorner3, bApplyCorner4);
	returnValue.Pressed = ApplySlateBrush(BaseBrush.Pressed, Level, EMounteaThemeState::Active, EMounteaThemeType::Background, bApplyCorner1, bApplyCorner2, bApplyCorner3, bApplyCorner4);
	returnValue.Disabled = ApplySlateBrush(BaseBrush.Disabled, Level, EMounteaThemeState::Disabled, EMounteaThemeType::Background, bApplyCorner1, bApplyCorner2, bApplyCorner3, bApplyCorner4);

	// Optional: Customize padding or text alignment
	returnValue.PressedPadding = BaseBrush.PressedPadding;
	returnValue.NormalPadding = BaseBrush.NormalPadding;
	returnValue.PressedSlateSound = FSlateSound();
	returnValue.HoveredSlateSound = FSlateSound();

	return returnValue;
}

FSlateBrush UMounteaInventoryUIStatics::MakeSlateBrush(const FSlateBrush& SourceBrush, const EMounteaThemeLevel Level,
													   const EMounteaThemeState State, const EMounteaThemeType Type)
{
	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings)) return SourceBrush;

	auto config = settings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(config)) return SourceBrush;

	auto theme = config->BaseTheme.LoadSynchronous();
	if (!IsValid(theme)) return SourceBrush;

	FSlateBrush returnValue = SourceBrush;

	switch (Type)
	{
	case EMounteaThemeType::Text:
		switch (Level)
		{
		case EMounteaThemeLevel::Primary:
			returnValue.TintColor = theme->PrimaryText;
			break;
		case EMounteaThemeLevel::Secondary:
			returnValue.TintColor = theme->SecondaryText;
			break;
		case EMounteaThemeLevel::Tertiary:
			returnValue.TintColor = theme->TertiaryText;
			break;
		}
		break;

	case EMounteaThemeType::Background:
		switch (Level)
		{
		case EMounteaThemeLevel::Primary:
			switch (State)
			{
			case EMounteaThemeState::Normal:
				returnValue.TintColor = theme->BackgroundPrimary;
				break;
			case EMounteaThemeState::Hovered:
				returnValue.TintColor = theme->PrimaryHovered;
				break;
			case EMounteaThemeState::Active:
				returnValue.TintColor = theme->PrimaryActive;
				break;
			case EMounteaThemeState::Disabled:
				returnValue.TintColor = theme->BackgroundDisabled;
				break;
			}
			break;

		case EMounteaThemeLevel::Secondary:
			switch (State)
			{
			case EMounteaThemeState::Normal:
				returnValue.TintColor = theme->BackgroundSecondary;
				break;
			case EMounteaThemeState::Hovered:
				returnValue.TintColor = theme->SecondaryHovered;
				break;
			case EMounteaThemeState::Active:
				returnValue.TintColor = theme->SecondaryActive;
				break;
			case EMounteaThemeState::Disabled:
				returnValue.TintColor = theme->BackgroundDisabled;
				break;
			}
			break;

		case EMounteaThemeLevel::Tertiary:
			switch (State)
			{
			case EMounteaThemeState::Normal:
				returnValue.TintColor = theme->BackgroundTertiary;
				break;
			case EMounteaThemeState::Hovered:
				returnValue.TintColor = theme->TertiaryHovered;
				break;
			case EMounteaThemeState::Active:
				returnValue.TintColor = theme->TertiaryActive;
				break;
			case EMounteaThemeState::Disabled:
				returnValue.TintColor = theme->BackgroundDisabled;
				break;
			}
			break;
		}
		break;

	case EMounteaThemeType::Default:
		switch (Level)
		{
		case EMounteaThemeLevel::Primary:
			switch (State)
			{
			case EMounteaThemeState::Normal:
				returnValue.TintColor = theme->PrimaryNormal;
				break;
			case EMounteaThemeState::Hovered:
				returnValue.TintColor = theme->PrimaryHovered;
				break;
			case EMounteaThemeState::Active:
				returnValue.TintColor = theme->PrimaryActive;
				break;
			case EMounteaThemeState::Disabled:
				returnValue.TintColor = theme->PrimaryDisabled;
				break;
			}
			break;

		case EMounteaThemeLevel::Secondary:
			switch (State)
			{
			case EMounteaThemeState::Normal:
				returnValue.TintColor = theme->SecondaryNormal;
				break;
			case EMounteaThemeState::Hovered:
				returnValue.TintColor = theme->SecondaryHovered;
				break;
			case EMounteaThemeState::Active:
				returnValue.TintColor = theme->SecondaryActive;
				break;
			case EMounteaThemeState::Disabled:
				returnValue.TintColor = theme->SecondaryDisabled;
				break;
			}
			break;

		case EMounteaThemeLevel::Tertiary:
			switch (State)
			{
			case EMounteaThemeState::Normal:
				returnValue.TintColor = theme->TertiaryNormal;
				break;
			case EMounteaThemeState::Hovered:
				returnValue.TintColor = theme->TertiaryHovered;
				break;
			case EMounteaThemeState::Active:
				returnValue.TintColor = theme->TertiaryActive;
				break;
			case EMounteaThemeState::Disabled:
				returnValue.TintColor = theme->TertiaryDisabled;
				break;
			}
			break;
		}
		break;
	}

	return returnValue;
}

FSlateBrush UMounteaInventoryUIStatics::ApplySlateBrush(const FSlateBrush& SourceBrush, const EMounteaThemeLevel Level,
	const EMounteaThemeState State, const EMounteaThemeType Type, const bool bApplyCorner1, const bool bApplyCorner2,
	const bool bApplyCorner3, const bool bApplyCorner4)
{
	FSlateBrush returnValue = MakeSlateBrush(SourceBrush, Level, State, Type);
	returnValue.OutlineSettings = MakeSlateBrushOutline(returnValue.OutlineSettings, Level, State, bApplyCorner1,bApplyCorner2, bApplyCorner3, bApplyCorner4);
	return returnValue;
}

FSlateBrushOutlineSettings UMounteaInventoryUIStatics::MakeSlateBrushOutline(
	const FSlateBrushOutlineSettings& SourceOutline, const EMounteaThemeLevel Level, const EMounteaThemeState State,
	const bool bApplyCorner1, const bool bApplyCorner2,
	const bool bApplyCorner3, const bool bApplyCorner4)
{
	FSlateBrushOutlineSettings returnValue = SourceOutline;

	const auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings)) return returnValue;

	const auto config = settings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(config)) return returnValue;

	const auto theme = config->BaseTheme.LoadSynchronous();
	if (!IsValid(theme)) return returnValue;

	switch (Level)
	{
		case EMounteaThemeLevel::Primary:
			switch (State)
			{
				case EMounteaThemeState::Normal:
					returnValue.Color = theme->PrimaryNormal;
					break;
				case EMounteaThemeState::Hovered:
					returnValue.Color = theme->PrimaryHoverBorder;
					break;
				case EMounteaThemeState::Active:
					returnValue.Color = theme->PrimaryFocus;
					break;
				case EMounteaThemeState::Disabled:
					returnValue.Color = theme->PrimaryDisabled;
					break;
			}
			break;

		case EMounteaThemeLevel::Secondary:
			switch (State)
			{
				case EMounteaThemeState::Normal:
					returnValue.Color = theme->SecondaryNormal;
					break;
				case EMounteaThemeState::Hovered:
					returnValue.Color = theme->SecondaryHoverBorder;
					break;
				case EMounteaThemeState::Active:
					returnValue.Color = theme->SecondaryFocus;
					break;
				case EMounteaThemeState::Disabled:
					returnValue.Color = theme->SecondaryDisabled;
					break;
			}
			break;

		case EMounteaThemeLevel::Tertiary:
			switch (State)
			{
				case EMounteaThemeState::Normal:
					returnValue.Color = theme->TertiaryNormal;
					break;
				case EMounteaThemeState::Hovered:
					returnValue.Color = theme->TertiaryHoverBorder;
					break;
				case EMounteaThemeState::Active:
					returnValue.Color = theme->TertiaryFocus;
					break;
				case EMounteaThemeState::Disabled:
					returnValue.Color = theme->TertiaryDisabled;
					break;
			}
			break;
	}

	FVector4 Radius = returnValue.CornerRadii;

	if (bApplyCorner1) Radius.X = config->BaseBorderRadius.X;
	if (bApplyCorner2) Radius.Y = config->BaseBorderRadius.Y;
	if (bApplyCorner3) Radius.Z = config->BaseBorderRadius.Z;
	if (bApplyCorner4) Radius.W = config->BaseBorderRadius.W;

	returnValue.CornerRadii = Radius;
	returnValue.Width = config->BaseBorderWidth;

	return returnValue;
}

FSlateBrush UMounteaInventoryUIStatics::ApplySlateBrushOutline(const FSlateBrush& SourceBrush,
	EMounteaThemeLevel Level, EMounteaThemeState State, const bool bApplyCorner1, const bool bApplyCorner2,
	const bool bApplyCorner3, const bool bApplyCorner4)
{
	FSlateBrush returnValue = SourceBrush;
	returnValue.OutlineSettings = MakeSlateBrushOutline(SourceBrush.OutlineSettings, Level, State, bApplyCorner1,
														bApplyCorner2, bApplyCorner3, bApplyCorner4);
	return returnValue;
}

FSlateFontInfo UMounteaInventoryUIStatics::ApplySlateFontInfo(const FSlateFontInfo& SourceFont,
	const EMounteaThemeLevel Level, const EMounteaThemeState State)
{
	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings)) return SourceFont;

	auto config = settings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(config)) return SourceFont;

	auto theme = config->BaseTheme.LoadSynchronous();
	if (!IsValid(theme)) return SourceFont;

	FSlateFontInfo returnValue = SourceFont;

	float sizeMultiplier = 1.0f;
	switch (Level)
	{
		case EMounteaThemeLevel::Primary:
			sizeMultiplier = 1.0f;
			break;
		case EMounteaThemeLevel::Secondary:
			sizeMultiplier = 0.9f;
			break;
		case EMounteaThemeLevel::Tertiary:
			sizeMultiplier = 0.8f;
			break;
	}

	switch (State)
	{
		case EMounteaThemeState::Normal:
			break;
		case EMounteaThemeState::Hovered:
			sizeMultiplier *= 1.05f;
			break;
		case EMounteaThemeState::Active:
			sizeMultiplier *= 1.1f;
			break;
		case EMounteaThemeState::Disabled:
			sizeMultiplier *= 0.95f;
			break;
	}

	returnValue.Size = FMath::RoundToInt(SourceFont.Size * sizeMultiplier);

	if (State == EMounteaThemeState::Disabled)
	{
		returnValue.OutlineSettings.OutlineSize = 0;
	}

	return returnValue;
}

FTextBlockStyle UMounteaInventoryUIStatics::ApplyTextBlockStyle(const FTextBlockStyle& SourceStyle,
	const EMounteaThemeLevel Level, const EMounteaThemeState State)
{
	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings)) return SourceStyle;

	auto config = settings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(config)) return SourceStyle;

	auto theme = config->BaseTheme.LoadSynchronous();
	if (!IsValid(theme)) return SourceStyle;

	FTextBlockStyle returnValue = SourceStyle;

	FLinearColor textColor;
	switch (Level)
	{
		case EMounteaThemeLevel::Primary:
			textColor = theme->PrimaryText;
			returnValue.Font.Size = theme->PrimaryTextSize;
			break;
		case EMounteaThemeLevel::Secondary:
			textColor = theme->SecondaryText;
			returnValue.Font.Size = theme->SecondaryTextSize;
			break;
		case EMounteaThemeLevel::Tertiary:
			textColor = theme->TertiaryText;
			returnValue.Font.Size = theme->TertiaryTextSize;
			break;
		default:
			textColor = theme->PrimaryText;
			break;
	}

	switch (State)
	{
		case EMounteaThemeState::Normal:
			break;
		case EMounteaThemeState::Hovered:
			textColor = FLinearColor(
				FMath::Min(textColor.R * 1.1f, 1.0f),
				FMath::Min(textColor.G * 1.1f, 1.0f),
				FMath::Min(textColor.B * 1.1f, 1.0f),
				textColor.A
			);
			break;
		case EMounteaThemeState::Active:
			textColor = theme->Accent;
			break;
		case EMounteaThemeState::Disabled:
			textColor = FLinearColor(textColor.R, textColor.G, textColor.B, textColor.A * 0.5f);
			break;
	}

	returnValue.ColorAndOpacity = textColor;

	returnValue.ShadowOffset = FVector2D(1.0f, 1.0f);
	returnValue.ShadowColorAndOpacity = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);

	return returnValue;
}

void UMounteaInventoryUIStatics::ApplyTextBlockTheme(UTextBlock* TextBlock, const EMounteaThemeLevel Level,
	const EMounteaThemeState State)
{
	if (!IsValid(TextBlock))
	{
		LOG_WARNING(TEXT("[ApplyTextBlockTheme] Invalid TextBlock provided!"))
		return;
	}

	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings)) return;

	auto config = settings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(config)) return;

	auto theme = config->BaseTheme.LoadSynchronous();
	if (!IsValid(theme)) return;
	
	FSlateFontInfo currentFont = TextBlock->GetFont();
	FSlateFontInfo themedFont = ApplySlateFontInfo(currentFont, Level, State);
	switch (Level)
	{
		case EMounteaThemeLevel::Primary:
			themedFont.Size = theme->PrimaryTextSize;
			break;
		case EMounteaThemeLevel::Secondary:
			themedFont.Size = theme->SecondaryTextSize;
			break;
		case EMounteaThemeLevel::Tertiary:
			themedFont.Size = theme->TertiaryTextSize;
			break;
	}
	TextBlock->SetFont(themedFont);

	FTextBlockStyle currentStyle;
	currentStyle.ColorAndOpacity = TextBlock->GetColorAndOpacity();
	currentStyle.ShadowOffset = TextBlock->GetShadowOffset();
	currentStyle.ShadowColorAndOpacity = TextBlock->GetShadowColorAndOpacity();
	
	FTextBlockStyle themedStyle = ApplyTextBlockStyle(currentStyle, Level, State);
	TextBlock->SetColorAndOpacity(themedStyle.ColorAndOpacity);
	TextBlock->SetShadowOffset(themedStyle.ShadowOffset);
	TextBlock->SetShadowColorAndOpacity(themedStyle.ShadowColorAndOpacity);
}

void UMounteaInventoryUIStatics::RefreshWidget(UWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(Target);
}

void UMounteaInventoryUIStatics::SetOwningInventoryUI(UWidget* Target,
													  const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, NewOwningInventoryUI);
}

void UMounteaInventoryUIStatics::InitializeMainUIWidget(
	const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent)
{
	if (Target.GetObject() && Parent.GetObject())
		Target->Execute_InitializeMainUI(Target.GetObject(), Parent);
}

void UMounteaInventoryUIStatics::RemoveInventoryWidgetWrapper(
	const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveMainUI(Target.GetObject());
}

bool UMounteaInventoryUIStatics::SetSourceInventory(
	const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory)
{
	return Target.GetObject() ? Target->Execute_SetSourceInventory(Target.GetObject(), ParentInventory) : false;
}

ESlateVisibility UMounteaInventoryUIStatics::GetMainUIVisibility(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetMainUIVisibility(Target.GetObject()) : ESlateVisibility::Hidden;
}

void UMounteaInventoryUIStatics::SetMainUIVisibility(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const ESlateVisibility Visibility)
{
	if (Target.GetObject())
		Target->Execute_SetMainUIVisibility(Target.GetObject(), Visibility);
}

void UMounteaInventoryUIStatics::SetInventoryCategoryKey(UWidget* Target, const FString& CategoryId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SetInventoryCategoryKey(Target, CategoryId);
}

FString UMounteaInventoryUIStatics::GetInventoryCategoryKey(UWidget* Target)
{
	return (
		IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>()
		) ? IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_GetInventoryCategoryKey(Target) : TEXT("none");
}

void UMounteaInventoryUIStatics::SetCategoryOwningInventoryUI(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetActiveState(UWidget* Target, const bool bIsActive)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SetActiveState(Target, bIsActive);
}

UUserWidget* UMounteaInventoryUIStatics::GetActiveItemWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return (
		IsValid(Target.GetObject()) )
	? IMounteaAdvancedInventoryUIInterface::Execute_GetActiveItemWidget(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::SetActiveItemWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, UUserWidget* NewActiveItemWidget)
{
	if (!IsValid(Target.GetObject())) return;

	Target->Execute_SetActiveItemWidget(Target.GetObject(), NewActiveItemWidget);
}

void UMounteaInventoryUIStatics::SetInventoryItemId(UWidget* Target, const FGuid& ItemGuid)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SetInventoryItemId(Target, ItemGuid);
}

FGuid UMounteaInventoryUIStatics::GetInventoryItemId(UWidget* Target)
{
	return (
		IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>()
		) ? IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetInventoryItemId(Target) : FGuid();
}

void UMounteaInventoryUIStatics::SetItemOwningInventoryUI(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::Item_RefreshWidget(UWidget* Target, const int32 Quantity)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_RefreshItemWidget(Target, Quantity);
}

void UMounteaInventoryUIStatics::Item_SetParentSlot(UWidget* Target, UUserWidget* ParentSlot)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SetParentSlot(Target, ParentSlot);
}

void UMounteaInventoryUIStatics::Item_HighlightItem(UWidget* Target, const bool bIsSelected)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_HighlightItem(Target, bIsSelected);
}

void UMounteaInventoryUIStatics::ItemAction_InitializeItemAction(UUserWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI,
	const TSoftClassPtr<UObject>& ItemActionClass, UWidget* ParentWidget)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>())
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_InitializeItemAction(
				Target, ParentUI, ItemActionClass, ParentWidget);
	else
		LOG_ERROR(TEXT("[InitializeItemAction] Target does not implement IMounteaAdvancedInventoryItemActionWidgetInterface!"));
}

bool UMounteaInventoryUIStatics::ItemAction_IsActionEnabled(UUserWidget* Target)
{
	return IsValid(Target) &&
		Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>() && 
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_IsActionEnabled(Target);
}

bool UMounteaInventoryUIStatics::ItemAction_IsActionValid(UUserWidget* Target)
{
	return IsValid(Target) && 
		Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>() && 
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_IsActionValid(Target);
}

void UMounteaInventoryUIStatics::ItemAction_ExecuteItemAction(UUserWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>())
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_ExecuteItemAction(Target);
	else
		LOG_ERROR(TEXT("[ExecuteItemAction] Target does not implement IMounteaAdvancedInventoryItemActionWidgetInterface!"));
}

TSoftClassPtr<UObject> UMounteaInventoryUIStatics::ItemAction_GetItemAction(UUserWidget* Target)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[GetItemAction] Target is not valid!"));
		return nullptr;
	}

	if (!Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>())
	{
		LOG_ERROR(TEXT("[GetItemAction] Target does not implement IMounteaAdvancedInventoryItemActionWidgetInterface!"));
		return nullptr;
	}

	return IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_GetItemAction(Target);
}

FMounteaItemActionData UMounteaInventoryUIStatics::ItemAction_GetActionData(UWidget* Target)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[GetActionData] Invalid Target!"));
		return FMounteaItemActionData();
	}

	if (!Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>())
	{
		LOG_ERROR(TEXT("[GetActionData] Target does not implement IMounteaAdvancedInventoryItemActionWidgetInterface!"));
		return FMounteaItemActionData();
	}

	return IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_GetItemActionData(Target);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_SetParentItemWidget(UWidget* Target, UWidget* ParentItemWidget)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[SetParentItemWidget] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_SetParentItemWidget(Target, ParentItemWidget);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_ConstructFromActionsList(UUserWidget* Target,
																			   const TArray<TSoftClassPtr<UObject>>& ItemActionsList)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[ConstructFromActionsList] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_ConstructFromActionsList(Target, ItemActionsList);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_AddItemActionToContainer(UUserWidget* Target,
	UUserWidget* ItemActionWidget)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[AddItemActionToContainer] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_AddItemActionToContainer(Target, ItemActionWidget);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_RemoveItemActionFromContainer(UUserWidget* Target,
	UUserWidget* ItemActionWidget)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[RemoveItemActionFromContainer] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	if (!IsValid(ItemActionWidget))
	{
		LOG_WARNING(TEXT("[RemoveItemActionFromContainer] ItemActionWidget is invalid!"));
		return;
	}

	IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_RemoveItemActionFromContainer(Target, ItemActionWidget);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_ClearItemActionsContainer(UUserWidget* Target)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[ClearItemActionsContainer] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_ClearItemActionsContainer(Target);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_SelectItemAction(UUserWidget* Target,
	UUserWidget* ItemActionWidget)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[SelectItemAction] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	if (!IsValid(ItemActionWidget))
	{
		LOG_WARNING(TEXT("[SelectItemAction] ItemActionWidget is invalid!"));
		return;
	}

	IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_SelectItemAction(Target, ItemActionWidget);
}

TArray<UUserWidget*> UMounteaInventoryUIStatics::ItemActionsContainer_GetItemActionsInContainer(UUserWidget* Target)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[GetItemActionsInContainer] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return {};
	}

	return IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_GetItemActionsInContainer(Target);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_SelectItemActionByIndex(UUserWidget* Target,
	const int32 ItemActionIndex)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[SelectItemActionByIndex] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	const auto itemActions = ItemActionsContainer_GetItemActionsInContainer(Target);
	if (itemActions.Num() == 0 || ItemActionIndex < 0 || ItemActionIndex >= itemActions.Num() || !itemActions.IsValidIndex(ItemActionIndex))
	{
		LOG_WARNING(TEXT("[SelectItemActionByIndex] Invalid ItemActionIndex or no actions in container!"));
		return;
	}

	ItemActionsContainer_SelectItemAction(Target, itemActions[ItemActionIndex]);
}

void UMounteaInventoryUIStatics::ItemTooltip_SetTooltipItem(UWidget* Target, const FInventorySlot& SourceSlot)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryTooltipWidgetInterface>())
		IMounteaAdvancedInventoryTooltipWidgetInterface::Execute_SetTooltipItem(Target, SourceSlot);
}

FInventorySlot UMounteaInventoryUIStatics::MakeInventoryGridSlotData(const FInventorySlot& SlotData)
{
	return FMounteaInventoryGridSlot(SlotData);
}

FInventorySlot UMounteaInventoryUIStatics::MakeInventorySlotData(const FMounteaInventoryGridSlot& GridSlotData)
{
	return FInventorySlot(GridSlotData);
}

FInventorySlot UMounteaInventoryUIStatics::MakeInventorySlot(UUserWidget* UserWidget, const FGuid& ItemId,
															 const int32 Quantity)
{
	FInventorySlot returnValue;
	{
		returnValue.OccupiedItemId = ItemId;
		returnValue.SlotQuantity = Quantity;
		returnValue.SlotWidget = UserWidget;
	}
	return returnValue;
}

FString UMounteaInventoryUIStatics::ItemSlot_GetSlotTooltip(UUserWidget* Target)
{
	return IsValid(Target) ? IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_GetSlotTooltip(Target) : TEXT("none");
}

FString UMounteaInventoryUIStatics::ItemSlot_GenerateSlotTooltip(UWidget* Target)
{
	if (!IsValid(Target)) return TEXT("none");
	if (!Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>()) return TEXT("none");
	if (!Target->Implements<UMounteaAdvancedBaseInventoryWidgetInterface>()) return TEXT("none");
	
	const auto slotInventory = IMounteaAdvancedBaseInventoryWidgetInterface::Execute_GetOwningInventoryUI(Target);
	if (!IsValid(slotInventory.GetObject())) return TEXT("none");

	const auto ownerInventory = slotInventory->Execute_GetParentInventory(slotInventory.GetObject());
	if (!IsValid(ownerInventory.GetObject())) return TEXT("none");
	
	const auto slotData = IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetSlotData(Target);
	if (!slotData.IsValid()) return TEXT("none");
	const int slotQuantity =slotData.SlotQuantity;
	const FGuid itemGuid = slotData.OccupiedItemId;

	const auto slotItem = ownerInventory->Execute_FindItem(ownerInventory.GetObject(), FInventoryItemSearchParams(itemGuid));
	if (!slotItem.IsItemValid()) return TEXT("none");

	const auto itemTemplate = slotItem.GetTemplate();
	if (!IsValid(itemTemplate)) return TEXT("none");

	return FString::Printf(TEXT("${quantityText}: %d | ${rarityText}: %s"), slotQuantity, *itemTemplate->ItemRarity);
}

void UMounteaInventoryUIStatics::SetItemSlotOwningInventoryUI(UWidget* Target,
															  const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::ItemSlot_AddItemToSlot(UWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_AddItemToSlot(Target, ItemId);
}

void UMounteaInventoryUIStatics::ItemSlot_RemoveItemFromSlot(UWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_RemoveItemFromSlot(Target, ItemId);
}

void UMounteaInventoryUIStatics::StoreGridSlotData(UWidget* Target, const FMounteaInventoryGridSlot& SlotData)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(Target, SlotData);
}

FMounteaInventoryGridSlot UMounteaInventoryUIStatics::GetGridSlotData(UWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		return IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_GetGridSlotData(Target);
	return FMounteaInventoryGridSlot();
}

UUserWidget* UMounteaInventoryUIStatics::ItemSlot_GetItemWidgetInSlot(UWidget* Target)
{
	return (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
	? IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_GetItemWidgetInSlot(Target) : nullptr;
}

void UMounteaInventoryUIStatics::ItemSlot_SelectItemInSlot(UWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_SelectItemInSlot(Target);
}

void UMounteaInventoryUIStatics::ItemSlot_SetParentSlotsWrapper(UWidget* Target, UUserWidget* ParentSlotsWrapper)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_SetParentSlotsWrapper(Target, ParentSlotsWrapper);
}

void UMounteaInventoryUIStatics::SetItemSlotsWrapperOwningInventoryUI(UWidget* Target,
																	  const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SlotsWrapper_AddItem(UWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_AddItem(Target, ItemId);
}

void UMounteaInventoryUIStatics::SlotsWrapper_UpdateItem(UWidget* Target, const FGuid& ItemId, const int32 OptionalItemSlot)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_UpdateItem(Target, ItemId, OptionalItemSlot);
}

void UMounteaInventoryUIStatics::SlotsWrapper_RemoveItem(UWidget* Target, const FGuid& ItemId, const int32 Quantity)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_RemoveItem(Target, ItemId, Quantity);
}

UUserWidget* UMounteaInventoryUIStatics::SlotsWrapper_GetSelectedItemWidget(UWidget* Target)
{
	return (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
	? IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_GetSelectedItemWidget(Target) : nullptr;
}

void UMounteaInventoryUIStatics::SlotsWrapper_SetSelectedItemWidget(UWidget* Target,
	UUserWidget* NewSelectedItemWidget)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_SetSelectedItemWidget(
			Target, NewSelectedItemWidget);
}

bool UMounteaInventoryUIStatics::ItemsGrid_AddItemToEmptySlot(UWidget* Target, const FGuid& ItemId)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToEmptySlot(Target, ItemId) : false;
}

bool UMounteaInventoryUIStatics::ItemsGrid_AddItemToSlot(UWidget* Target, const FGuid& ItemId,
	const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToSlot(Target, ItemId, SlotIndex) : false;
}

bool UMounteaInventoryUIStatics::ItemsGrid_RemoveItemFromSlot(UWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_RemoveItemFromSlot(Target, SlotIndex) : false;
}

bool UMounteaInventoryUIStatics::ItemsGrid_RemoveItem(UWidget* Target, const FGuid& ItemId, const int32 Quantity)
{
	return (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>())
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_RemoveItemFromGrid(Target, ItemId, Quantity) : false;
}

FGuid UMounteaInventoryUIStatics::ItemsGrid_GetItemInSlot(UWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetItemInSlot(Target, SlotIndex) : FGuid();
}

bool UMounteaInventoryUIStatics::ItemsGrid_SwapItemsBetweenSlots(UWidget* Target, const int32 SlotIndex1,
	const int32 SlotIndex2)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_SwapItemsBetweenSlots(Target, SlotIndex1, SlotIndex2) : false;
}

void UMounteaInventoryUIStatics::ItemsGrid_ClearAllSlots(UWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>())
		IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_ClearAllSlots(Target);
}

int32 UMounteaInventoryUIStatics::ItemsGrid_GetTotalSlots(UWidget* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetTotalSlots(Target) : INDEX_NONE;
}

bool UMounteaInventoryUIStatics::ItemsGrid_IsSlotEmpty(UWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_IsSlotEmpty(Target, SlotIndex) : false;
}

int32 UMounteaInventoryUIStatics::ItemsGrid_GetSlotIndexByItem(UWidget* Target, const FGuid& ItemId)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetSlotIndexByItem(Target, ItemId) : INDEX_NONE;
}

int32 UMounteaInventoryUIStatics::ItemsGrid_GetGridSlotIndexByCoords(UWidget* Target, const FIntPoint& SlotCoords)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotIndexByCoords(Target, SlotCoords) : INDEX_NONE;
}

bool UMounteaInventoryUIStatics::ItemsGrid_IsItemInGrid(UWidget* Target, const FGuid& ItemId)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_IsItemInGrid(Target, ItemId) : false;
}

FMounteaInventoryGridSlot UMounteaInventoryUIStatics::ItemsGrid_GetGridSlotData(UWidget* Target,
	const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(Target, SlotIndex) : FMounteaInventoryGridSlot();
}

TSet<FMounteaInventoryGridSlot> UMounteaInventoryUIStatics::ItemsGrid_GetGridSlotsData(UWidget* Target)
{
	return IsValid(Target) ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotsData(Target) : TSet<FMounteaInventoryGridSlot>();
}

UUserWidget* UMounteaInventoryUIStatics::ItemsGrid_GetItemWidgetInSlot(UWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetItemWidgetInSlot(Target, SlotIndex) : nullptr;
}

UUserWidget* UMounteaInventoryUIStatics::ItemsGrid_FindEmptyWidgetSlot(UWidget* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_FindEmptyWidgetSlot(Target) : nullptr;
}

int32 UMounteaInventoryUIStatics::ItemsGrid_FindEmptySlotIndex(UWidget* Target, const FGuid& ItemId)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_FindEmptySlotIndex(Target, ItemId) : INDEX_NONE;
}

void UMounteaInventoryUIStatics::ItemsGrid_AddSlot(UWidget* Target, const FMounteaInventoryGridSlot& SlotData)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>())
		IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddSlot(Target, SlotData);
}

bool UMounteaInventoryUIStatics::ItemsGrid_UpdateItemInSlot(UWidget* Target, const FGuid& ItemId,
	const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()
	? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_UpdateItemInSlot(Target, ItemId, SlotIndex) : false;
}

int32 UMounteaInventoryUIStatics::Helper_FindEmptyGridSlotIndex(const UWidget* Target, const FGuid& ItemId, UObject* ParentInventory)
{
	if (!IsValid(Target)) return INDEX_NONE;
	if (!Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()) return INDEX_NONE;
	if (!ItemId.IsValid()) return INDEX_NONE;
	if (!IsValid(ParentInventory)) return INDEX_NONE;

	const bool bHasUIInterface = ParentInventory->Implements<UMounteaAdvancedInventoryUIInterface>();
	const bool bHasInventoryInterface = ParentInventory->Implements<UMounteaAdvancedInventoryInterface>();
	if (!bHasUIInterface && !bHasInventoryInterface) return INDEX_NONE;

	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings)) return INDEX_NONE;

	auto config = settings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(config)) return INDEX_NONE;
	
	UObject* inventoryObject = nullptr;
	TScriptInterface<IMounteaAdvancedInventoryInterface> InventoryInterface = nullptr;
	if (bHasUIInterface)
	{
		auto inventoryRef = IMounteaAdvancedInventoryUIInterface::Execute_GetParentInventory(ParentInventory);
		inventoryObject = inventoryRef.GetObject();
		InventoryInterface = inventoryRef;
	}
	else
	{
		inventoryObject = ParentInventory;
		InventoryInterface = inventoryObject;
	}
	
	if (!IsValid(inventoryObject)) return INDEX_NONE;

	const auto inventoryItem = IMounteaAdvancedInventoryInterface::Execute_FindItem(inventoryObject, FInventoryItemSearchParams(ItemId));
	if (!inventoryItem.IsItemValid() || !inventoryItem.Template) return INDEX_NONE;

	const auto gridSlots = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotsData(Target).Array();
	if (gridSlots.Num() == 0) return INDEX_NONE;
	
	const bool bIsStackable = UMounteaInventorySystemStatics::HasFlag(inventoryItem.Template->ItemFlags, EInventoryItemFlags::EIIF_Stackable);
	const bool bAlwaysStack = config->bAlwaysStackStackableItems;
	
	return FindEmptyGridSlotRecursive(InventoryInterface, inventoryItem, gridSlots, bIsStackable, bAlwaysStack);
}

int32 UMounteaInventoryUIStatics::FindEmptyGridSlotRecursive(TScriptInterface<IMounteaAdvancedInventoryInterface>& InventoryInterface, const FInventoryItem& InventoryItem, const TArray<FMounteaInventoryGridSlot>& GridSlots, const bool bIsStackable, const bool bAlwaysStackItems)
{
	// For non-stackable items, just find first empty slot
	if (!bIsStackable || !bAlwaysStackItems)
	{
		for (int i = 0; i < GridSlots.Num(); i++)
		{
			const auto& gridSlot = GridSlots[i];
			if (!IsValid(gridSlot.SlotWidget)) continue;
			if (!gridSlot.SlotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) continue;
			
			if (IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_IsSlotEmpty(gridSlot.SlotWidget))
				return i;
		}
		return INDEX_NONE;
	}
	
	// For stackable items
	int32 firstEmptySlotIndex = INDEX_NONE;
	int32 leastFilledStackIndex = INDEX_NONE;
	int32 leastFilledStackAmount = MAX_int32;
	const int32 maxStack = InventoryItem.Template->MaxStackSize;
	
	TSet<FMounteaInventoryGridSlot> itemSlots;
	Algo::CopyIf(GridSlots, itemSlots, [&InventoryItem](const FMounteaInventoryGridSlot& gridSlot) {
		return gridSlot.OccupiedItemId == InventoryItem.GetGuid();
	});
	
	// Find slot with this item that has the most available space
	if (bAlwaysStackItems && !itemSlots.IsEmpty())
	{
		for (int i = 0; i < itemSlots.Num(); i++)
		{
			const auto& gridSlot = itemSlots.Array()[i];
			if (gridSlot.OccupiedItemId != InventoryItem.GetGuid()) continue;
			
			const int32 currentStack = gridSlot.SlotQuantity;
			if (currentStack >= maxStack) continue;
			
			if (currentStack < leastFilledStackAmount)
			{
				leastFilledStackAmount = currentStack;
				leastFilledStackIndex = i;
			}
		}
	}
	
	// Find first empty slot
	for (int i = 0; i < GridSlots.Num(); i++)
	{
		const auto& gridSlot = GridSlots[i];
		if (!IsValid(gridSlot.SlotWidget)) continue;
		if (!gridSlot.SlotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) continue;
		
		if (IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_IsSlotEmpty(gridSlot.SlotWidget))
		{
			firstEmptySlotIndex = i;
			break;
		}
	}
	
	// Return the appropriate slot index
	if (bAlwaysStackItems && leastFilledStackIndex != INDEX_NONE)
		return leastFilledStackIndex;
	return firstEmptySlotIndex;
}

bool UMounteaInventoryUIStatics::Helper_ItemsGrid_UpdateItemInSlot(
	UUserWidget* GridWidget, 
	const FGuid& ItemId, 
	int32 SlotIndex,
	TScriptInterface<IMounteaAdvancedInventoryUIInterface> ParentUIComponent)
{
	if (!ItemId.IsValid() || !IsValid(GridWidget)) return false;
	if (!GridWidget->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>()) return false;
	if (!IsValid(ParentUIComponent.GetObject())) return false;
	
	const auto parentInventory = ParentUIComponent->Execute_GetParentInventory(ParentUIComponent.GetObject());
	if (!IsValid(parentInventory.GetObject())) return false;

	const auto item = parentInventory->Execute_FindItem(parentInventory.GetObject(), FInventoryItemSearchParams(ItemId));
	if (!item.IsItemValid()) return false;

	const auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	const auto config = settings ? settings->InventorySettingsConfig.LoadSynchronous() : nullptr;
		
	const bool bIsStackable = UMounteaInventoryStatics::HasInventoryFlags(
		item.Template->ItemFlags, static_cast<int32>(EInventoryItemFlags::EIIF_Stackable)
	) && config ? config->bAlwaysStackStackableItems : true;
	
	auto itemSlots = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotsDataForItem(GridWidget, ItemId);
	const int32 currentTotalQuantity = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetStacksSizeForItem(GridWidget, ItemId);
	const int32 targetTotalQuantity = item.GetQuantity();
	const int32 maxStackSize = item.Template->MaxStackSize;
	
	if (currentTotalQuantity == targetTotalQuantity && itemSlots.Num() > 0)
		return false;

	bool bAnySlotUpdated = false;

	// Helper function to update a slot's quantity and refresh UI
	auto updateSlotQuantity = [&](FMounteaInventoryGridSlot& slot, const int32 newQuantity) -> void {
		slot.SlotQuantity = newQuantity;
		IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddSlot(GridWidget, slot);
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(slot.SlotWidget, slot);
		ParentUIComponent->Execute_UpdateSlot(ParentUIComponent.GetObject(), slot);
		
		if (IsValid(slot.SlotWidget))
			IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(slot.SlotWidget);
			
		bAnySlotUpdated = true;
	};

	itemSlots.Sort([](const FMounteaInventoryGridSlot& a, const FMounteaInventoryGridSlot& b) {
			if (a.SlotPosition.Y != b.SlotPosition.Y)
				return a.SlotPosition.Y > b.SlotPosition.Y; // Sort by Y descending first
			return a.SlotPosition.X > b.SlotPosition.X; // Then by X descending
		});

	if (SlotIndex == INDEX_NONE)
		SlotIndex = 0;
	
	// Case 1: We need to add items (SUM < item Quantity)
	if (targetTotalQuantity > currentTotalQuantity)
	{
		int32 quantityToAdd = targetTotalQuantity - currentTotalQuantity;
		
		// Handle non-stackable items differently
		if (!bIsStackable)
		{
			bool bSpecifiedSlotUsed = false;
			
			while (quantityToAdd > 0)
			{
				int32 emptySlotIndex = INDEX_NONE;
				
				// Use specified slot first if available and not used yet
				if (SlotIndex != INDEX_NONE && !bSpecifiedSlotUsed && 
					IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_IsSlotEmpty(GridWidget, SlotIndex))
				{
					emptySlotIndex = SlotIndex;
					bSpecifiedSlotUsed = true;
				}
				else
					emptySlotIndex = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_FindEmptySlotIndex(GridWidget, ItemId);
				
				if (emptySlotIndex == INDEX_NONE) break; // No more empty slots
				
				if (IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToSlot(GridWidget, ItemId, emptySlotIndex))
				{
					FMounteaInventoryGridSlot slotData = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(GridWidget, emptySlotIndex);
					updateSlotQuantity(slotData, 1); // Each non-stackable item gets quantity of 1
					quantityToAdd--;
				}
				else
				{
					break; // Failed to add to slot
				}
			}
		}
		// Handle stackable items
		else
		{
			// If no slots exist for this item, add it to an empty slot
			if (itemSlots.Num() == 0)
			{
				int32 emptySlotIndex = SlotIndex != INDEX_NONE ? SlotIndex : 
					IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_FindEmptySlotIndex(GridWidget, ItemId);
				
				if (emptySlotIndex != INDEX_NONE && 
					IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_IsSlotEmpty(GridWidget, emptySlotIndex))
				{
					int32 quantityToAddToSlot = FMath::Min(maxStackSize, quantityToAdd);
					if (IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToSlot(GridWidget, ItemId, emptySlotIndex))
					{
						FMounteaInventoryGridSlot slotData = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(GridWidget, emptySlotIndex);
						updateSlotQuantity(slotData, quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}
			}
			// Otherwise, first try to add to the specified slot if provided
			else if (SlotIndex != INDEX_NONE && 
					IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotsData(GridWidget).Array().IsValidIndex(SlotIndex))
			{
				FMounteaInventoryGridSlot slotData = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(GridWidget, SlotIndex);
				
				if (slotData.IsEmpty())
				{
					int32 quantityToAddToSlot = FMath::Min(maxStackSize, quantityToAdd);
					if (IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToSlot(GridWidget, ItemId, SlotIndex))
					{
						slotData = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(GridWidget, SlotIndex);
						updateSlotQuantity(slotData, quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}
				else if (slotData.OccupiedItemId == ItemId)
				{
					int32 availableSpace = maxStackSize - slotData.SlotQuantity;
					int32 quantityToAddToSlot = FMath::Min(availableSpace, quantityToAdd);
					
					if (quantityToAddToSlot > 0)
					{
						updateSlotQuantity(slotData, slotData.SlotQuantity + quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}

				itemSlots.Remove(slotData);
			}
			
			// Fill up existing item slots
			if (quantityToAdd > 0)
			{
				for (auto& slot : itemSlots)
				{
					if (quantityToAdd <= 0) break;
					
					const int32 availableSpace = maxStackSize - slot.SlotQuantity;
					const int32 quantityToAddToSlot = FMath::Min(availableSpace, quantityToAdd);
					
					if (quantityToAddToSlot > 0)
					{
						updateSlotQuantity(slot, slot.SlotQuantity + quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}
			}
			
			// Create new slots if needed
			while (quantityToAdd > 0)
			{
				int32 emptySlotIndex = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_FindEmptySlotIndex(GridWidget, ItemId);
				if (emptySlotIndex == INDEX_NONE) break;
				
				int32 quantityToAddToSlot = FMath::Min(maxStackSize, quantityToAdd);

				if (IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToSlot(GridWidget, ItemId, emptySlotIndex))
				{
					FMounteaInventoryGridSlot slotData = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(GridWidget, emptySlotIndex);
					updateSlotQuantity(slotData, quantityToAddToSlot);
					quantityToAdd -= quantityToAddToSlot;
				}
				else
				{
					break;
				}
			}
		}
	}
	
	// Case 2: We need to remove items (SUM > item Quantity)
	else if (targetTotalQuantity < currentTotalQuantity)
	{
		int32 quantityToRemove = currentTotalQuantity - targetTotalQuantity;
	
		if (SlotIndex != INDEX_NONE && 
			IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotsData(GridWidget).Array().IsValidIndex(SlotIndex))
		{
			FMounteaInventoryGridSlot slotData = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(GridWidget, SlotIndex);
			if (slotData.OccupiedItemId == ItemId)
			{
				const int32 quantityToRemoveFromSlot = FMath::Min(slotData.SlotQuantity, quantityToRemove);
			
				if (quantityToRemoveFromSlot >= slotData.SlotQuantity)
				{
					IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_RemoveItemFromSlot(GridWidget, SlotIndex);
					bAnySlotUpdated = true;
				}
				else
					updateSlotQuantity(slotData, slotData.SlotQuantity - quantityToRemoveFromSlot);
			
				quantityToRemove -= quantityToRemoveFromSlot;
				itemSlots.Remove(slotData);
			}
		}
		
		if (quantityToRemove > 0)
		{
			auto itemSlotsArray = itemSlots.Array();
			for (int32 i = 0; i < itemSlots.Num(); i++)
			{
				if (!itemSlotsArray.IsValidIndex(i)) continue;
				
				auto& slot =itemSlotsArray[i];
				if (quantityToRemove <= 0) break;
			
				const int32 slotIndex = IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotIndexByCoords(GridWidget, slot.SlotPosition);
				if (slotIndex == INDEX_NONE) continue;
			
				const int32 quantityToRemoveFromSlot = FMath::Min(slot.SlotQuantity, quantityToRemove);
			
				if (quantityToRemoveFromSlot >= slot.SlotQuantity)
				{
					IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_RemoveItemFromSlot(GridWidget, slotIndex);
					bAnySlotUpdated = true;
				}
				else
					updateSlotQuantity(slot, slot.SlotQuantity - quantityToRemoveFromSlot);
			
				quantityToRemove -= quantityToRemoveFromSlot;
			}
		}
	}

	return bAnySlotUpdated;
}

bool UMounteaInventoryUIStatics::ItemPreview_InitializeInteractableWidget(UMounteaAdvancedInventoryInteractableObjectWidget* Target)
{
	return IsValid(Target) ? Target->InitializeInteractableWidget() : false;
}

void UMounteaInventoryUIStatics::ItemPreview_SetPreviewMesh(UMounteaAdvancedInventoryInteractableObjectWidget* Target,
	UStaticMesh* StaticMesh)
{
	if (IsValid(Target))
		Target->SetPreviewMesh(StaticMesh);
}

void UMounteaInventoryUIStatics::ItemPreview_SetPreviewSkeletalMesh(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, USkeletalMesh* SkeletalMesh)
{
	if (IsValid(Target))
		Target->SetPreviewSkeletalMesh(SkeletalMesh);
}

void UMounteaInventoryUIStatics::ItemPreview_ClearPreview(UMounteaAdvancedInventoryInteractableObjectWidget* Target)
{
	if (IsValid(Target))
		Target->ClearPreview();
}

void UMounteaInventoryUIStatics::ItemPreview_ResetPreview(UMounteaAdvancedInventoryInteractableObjectWidget* Target)
{
	if (IsValid(Target))
		Target->ResetCameraToDefaults();
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraRotation(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2D& MouseDelta)
{
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraHeight(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2D& MouseDelta)
{
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraZoom(UMounteaAdvancedInventoryInteractableObjectWidget* Target,
	const float WheelDelta)
{
	if (IsValid(Target))
		Target->UpdateCameraZoom(WheelDelta);
}

void UMounteaInventoryUIStatics::ItemPreview_SetCameraRotationAbsolute(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float YawNormalized, const float PitchNormalized)
{
	if (IsValid(Target))
		Target->SetCameraRotationAbsolute(YawNormalized, PitchNormalized);
}

void UMounteaInventoryUIStatics::ItemPreview_SetCameraHeightAbsolute(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float HeightNormalized)
{
	if (IsValid(Target))
		Target->SetCameraHeightAbsolute(HeightNormalized);
}

void UMounteaInventoryUIStatics::ItemPreview_SetCameraZoomAbsolute(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float ZoomNormalized)
{
	if (IsValid(Target))
		Target->SetCameraZoomAbsolute(ZoomNormalized);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraRotationAnalog(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2D& AnalogInput, const float DeltaTime)
{
	if (IsValid(Target))
		Target->UpdateCameraRotationAnalog(AnalogInput, DeltaTime);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraHeightAnalog(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float AnalogInput, const float DeltaTime)
{
	if (IsValid(Target))
		Target->UpdateCameraHeightAnalog(AnalogInput, DeltaTime);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraZoomAnalog(
	UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float AnalogInput, const float DeltaTime)
{
	if (IsValid(Target))
		Target->UpdateCameraZoomAnalog(AnalogInput, DeltaTime);
}
