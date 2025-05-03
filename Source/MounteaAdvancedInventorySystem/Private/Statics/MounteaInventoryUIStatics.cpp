// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryUIStatics.h"

#include "Blueprint/UserWidget.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/Widgets/MounteaInventoryBaseWidgetInterface.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoriesWrapperWidgetInterface.h"
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoryWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "Interfaces/Widgets/Inventory/MounteaAdvancedInventoryWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemsGridWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

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

bool UMounteaInventoryUIStatics::CreateInventoryUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_CreateInventoryUIWrapper(Target.GetObject()) : false;
}

UUserWidget* UMounteaInventoryUIStatics::GetInventoryUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetInventoryUIWrapper(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::RemoveInventoryUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryUIWrapper(Target.GetObject());
}

void UMounteaInventoryUIStatics::SetInventoryUIWrapperVisibility(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const bool bShowInventory)
{
	if (Target.GetObject())
		Target->Execute_SetInventoryUIWrapperVisibility(Target.GetObject(), bShowInventory);
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

void UMounteaInventoryUIStatics::SetInventoryOwningInventoryUI(UUserWidget* Target,
															   const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryWidgetInterface>())
		IMounteaAdvancedInventoryWidgetInterface::Execute_SetOwningInventoryUI(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetCategoriesWrapperOwningInventoryUI(UUserWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface>())
		IMounteaAdvancedInventoryCategoriesWrapperWidgetInterface::Execute_SetOwningInventoryUI(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetActiveCategoryId(UUserWidget* Target, const FString& ActiveCategoryId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface>())
		IMounteaAdvancedInventoryCategoriesWrapperWidgetInterface::Execute_SetActiveCategoryId(Target, ActiveCategoryId);
}

FString UMounteaInventoryUIStatics::GetActiveCategoryId(UUserWidget* Target)
{
	return (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface>())
		? IMounteaAdvancedInventoryCategoriesWrapperWidgetInterface::Execute_GetActiveCategoryId(Target) : TEXT("none");
}

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

void UMounteaInventoryUIStatics::ApplyTheme(UUserWidget* Target)
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
					returnValue.Color = theme->PrimaryActive;
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
					returnValue.Color = theme->SecondaryActive;
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
					returnValue.Color = theme->TertiaryHovered;
					break;
				case EMounteaThemeState::Active:
					returnValue.Color = theme->TertiaryActive;
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

void UMounteaInventoryUIStatics::InitializeInventoryWidget(
	const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent)
{
	if (Target.GetObject() && Parent.GetObject())
		Target->Execute_InitializeInventoryWidget(Target.GetObject(), Parent);
}

void UMounteaInventoryUIStatics::RemoveInventoryWidgetWrapper(
	const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryWidgetWrapper(Target.GetObject());
}

bool UMounteaInventoryUIStatics::SetSourceInventory(
	const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory)
{
	return Target.GetObject() ? Target->Execute_SetSourceInventory(Target.GetObject(), ParentInventory) : false;
}

void UMounteaInventoryUIStatics::SetInventoryCategoryKey(UUserWidget* Target, const FString& CategoryId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SetInventoryCategoryKey(Target, CategoryId);
}

FString UMounteaInventoryUIStatics::GetInventoryCategoryKey(UUserWidget* Target)
{
	return (
		IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>()
		) ? IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_GetInventoryCategoryKey(Target) : TEXT("none");
}

void UMounteaInventoryUIStatics::SetCategoryOwningInventoryUI(UUserWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SetOwningInventoryUI(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetActiveState(UUserWidget* Target, const bool bIsActive)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SetActiveState(Target, bIsActive);
}

void UMounteaInventoryUIStatics::SetInventoryItemId(UUserWidget* Target, const FGuid& ItemGuid)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SetInventoryItemId(Target, ItemGuid);
}

FGuid UMounteaInventoryUIStatics::GetInventoryItemId(UUserWidget* Target)
{
	return (
		IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>()
		) ? IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetInventoryItemId(Target) : FGuid();
}

void UMounteaInventoryUIStatics::SetItemOwningInventoryUI(UUserWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SetOwningInventoryUI(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::Item_RefreshWidget(UUserWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_RefreshWidget(Target);
}

void UMounteaInventoryUIStatics::SetItemSlotOwningInventoryUI(UUserWidget* Target,
															  const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_SetOwningInventoryUI(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::ItemSlot_AddItemToSlot(UUserWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_AddItemToSlot(Target, ItemId);
}

void UMounteaInventoryUIStatics::ItemSlot_RemoveItemFromSlot(UUserWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_RemoveItemFromSlot(Target, ItemId);
}

void UMounteaInventoryUIStatics::StoreGridSlotData(UUserWidget* Target, const FMounteaInventoryGridSlot& SlotData)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(Target, SlotData);
}

FMounteaInventoryGridSlot UMounteaInventoryUIStatics::GetGridSlotData(UUserWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		return IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_GetSlotData(Target);
	return FMounteaInventoryGridSlot();
}

void UMounteaInventoryUIStatics::SetItemSlotsWrapperOwningInventoryUI(UUserWidget* Target,
																	  const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_SetOwningInventoryUI(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SlotsWrapper_AddItem(UUserWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_AddItem(Target, ItemId);
}

void UMounteaInventoryUIStatics::SlotsWrapper_UpdateItem(UUserWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_UpdateItem(Target, ItemId);
}

void UMounteaInventoryUIStatics::SlotsWrapper_RemoveItem(UUserWidget* Target, const FGuid& ItemId)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_RemoveItem(Target, ItemId);
}

bool UMounteaInventoryUIStatics::ItemsGrid_AddItemToEmptySlot(UUserWidget* Target, const FGuid& ItemId)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToEmptySlot(Target, ItemId) : false;
}

bool UMounteaInventoryUIStatics::ItemsGrid_AddItemToSlot(UUserWidget* Target, const FGuid& ItemId,
	const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_AddItemToSlot(Target, ItemId, SlotIndex) : false;
}

bool UMounteaInventoryUIStatics::ItemsGrid_RemoveItemFromSlot(UUserWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_RemoveItemFromSlot(Target, SlotIndex) : false;
}

FGuid UMounteaInventoryUIStatics::ItemsGrid_GetItemInSlot(UUserWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetItemInSlot(Target, SlotIndex) : FGuid();
}

bool UMounteaInventoryUIStatics::ItemsGrid_SwapItemsBetweenSlots(UUserWidget* Target, const int32 SlotIndex1,
	const int32 SlotIndex2)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_SwapItemsBetweenSlots(Target, SlotIndex1, SlotIndex2) : false;
}

void UMounteaInventoryUIStatics::ItemsGrid_ClearAllSlots(UUserWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>())
		IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_ClearAllSlots(Target);
}

int32 UMounteaInventoryUIStatics::ItemsGrid_GetTotalSlots(UUserWidget* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetTotalSlots(Target) : INDEX_NONE;
}

bool UMounteaInventoryUIStatics::ItemsGrid_IsSlotEmpty(UUserWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_IsSlotEmpty(Target, SlotIndex) : false;
}

int32 UMounteaInventoryUIStatics::ItemsGrid_GetSlotIndexByItem(UUserWidget* Target, const FGuid& ItemId)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetSlotIndexByItem(Target, ItemId) : INDEX_NONE;
}

bool UMounteaInventoryUIStatics::ItemsGrid_IsItemInGrid(UUserWidget* Target, const FGuid& ItemId)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_IsItemInGrid(Target, ItemId) : false;
}

FMounteaInventoryGridSlot UMounteaInventoryUIStatics::ItemsGrid_GetGridSlotData(UUserWidget* Target,
	const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotData(Target, SlotIndex) : FMounteaInventoryGridSlot();
}

TSet<FMounteaInventoryGridSlot> UMounteaInventoryUIStatics::ItemsGrid_GetGridSlotsData(UUserWidget* Target)
{
	return IsValid(Target) ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetGridSlotsData(Target) : TSet<FMounteaInventoryGridSlot>();
}

UUserWidget* UMounteaInventoryUIStatics::ItemsGrid_GetItemWidgetInSlot(UUserWidget* Target, const int32 SlotIndex)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemsGridWidgetInterface>() ? IMounteaAdvancedInventoryItemsGridWidgetInterface::Execute_GetItemWidgetInSlot(Target, SlotIndex) : nullptr;
}
