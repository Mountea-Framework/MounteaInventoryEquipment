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
#include "Statics/MounteaInventorySystemStatics.h"
#include "Statics/MounteaInventoryStatics.h"

#include "Algo/Copy.h"
#include "Algo/MaxElement.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

#include "CommonInputSubsystem.h"

#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

#include "GameFramework/PlayerState.h"

#include "Components/PanelWidget.h"

#include "Interfaces/Widgets/MounteaInventorySystemWrapperWidgetInterface.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/Widgets/BaseWidget/MounteaAdvancedBaseInventoryWidgetInterface.h"
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
#include "Settings/MounteaAdvancedInventoryUIConfig.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

#include "Slate/MounteaInventoryScrollBox.h"

#include "Subsystems/MounteaAdvancedInventoryUISubsystem.h"
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
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& NewOwningInventoryUI)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[SetOwningInventoryUI] Failed to set Owning Inventory UI. Reason: Target is null."));
		return;
	}

	if (!Target->Implements<UMounteaAdvancedBaseInventoryWidgetInterface>())
	{
		LOG_ERROR(TEXT("[SetOwningInventoryUI] Failed to set Owning Inventory UI for %s. Reason: Target does not implement IMounteaAdvancedBaseInventoryWidgetInterface."),
			*Target->GetName());
		return;
	}

	IMounteaAdvancedBaseInventoryWidgetInterface::Execute_SetOwningInventoryUI(Target, NewOwningInventoryUI);
}

bool UMounteaInventoryUIStatics::IsItemWidgetDetachedFromValidItems(const TArray<FInventoryItem>& ValidItems,
	const FInventoryItemData& InventoryItemData)
{
	if (ValidItems.IsEmpty())
		return true;
	if (!InventoryItemData.ContainingItem.IsItemValid())
		return true;
	
	return !ValidItems.ContainsByPredicate([&InventoryItemData](const FInventoryItem& Item)
	{
		return Item.Guid == InventoryItemData.ContainingItem.Guid;
	});
}

bool UMounteaInventoryUIStatics::InInputTypeActive(const UUserWidget* ContextObject, const ECommonInputType& InputType)
{
	if (!IsValid(ContextObject))
		return false;

	const ULocalPlayer* localPlayer = ContextObject->GetOwningLocalPlayer();
	if (!localPlayer)
		return false;

	const UCommonInputSubsystem* inputSubsystem = UCommonInputSubsystem::Get(localPlayer);
	if (!inputSubsystem)
		return false;

	return InputType == inputSubsystem->GetCurrentInputType();
}

void UMounteaInventoryUIStatics::CenterListItemAtIndex(UPanelWidget* ListWidget, const int32 SelectedIndex)
{
	if (!ListWidget || SelectedIndex < 0)
		return;
    
	if (ListWidget->GetChildrenCount() == 0)
		return;
    
	UWidget* FirstChild = ListWidget->GetChildAt(0);
	if (!FirstChild)
		return;
    
	const float ItemHeight = FirstChild->GetDesiredSize().Y;
	const float ViewportHeight = ListWidget->GetCachedGeometry().GetLocalSize().Y;
    
	if (ViewportHeight <= 0.0f || ItemHeight <= 0.0f)
		return;
    
	const float ViewportCenter = ViewportHeight * 0.5f;
	const float ItemCenter = (SelectedIndex * ItemHeight) + (ItemHeight * 0.5f);
	const float TranslationY = ViewportCenter - ItemCenter;
    
	ListWidget->SetRenderTranslation(FVector2D(0.0f, TranslationY));
}

FVector2D UMounteaInventoryUIStatics::CalculateCenteredListTranslation(UPanelWidget* ListWidget, const int32 SelectedIndex)
{
	if (!ListWidget || SelectedIndex < 0 || ListWidget->GetChildrenCount() == 0)
		return FVector2D::ZeroVector;

	const UWidget* firstChild = ListWidget->GetChildAt(0);
	if (!firstChild)
		return FVector2D::ZeroVector;
    
	const float itemHeight = firstChild->GetDesiredSize().Y;
	const float viewportHeight = ListWidget->GetCachedGeometry().GetLocalSize().Y;
    
	if (viewportHeight <= 0.0f || itemHeight <= 0.0f)
		return FVector2D::ZeroVector;
    
	const float viewportCenter = viewportHeight * 0.5f;
	const float itemCenter = (SelectedIndex * itemHeight) + (itemHeight * 0.5f);
	const float translationY = (viewportCenter - itemCenter);
    
	return FVector2D(0.0f, translationY);
}

FVector2D UMounteaInventoryUIStatics::CalculateCenteredListTranslation(const TSharedPtr<SWidget>& ListWidget, 
	const FGeometry& ListGeometry, const int32 SelectedIndex)
{
	TSharedPtr<SPanel> panel = StaticCastSharedPtr<SPanel>(ListWidget);
	
	if (!panel.IsValid() || SelectedIndex < 0)
		return FVector2D::ZeroVector;

	FChildren* children = panel->GetChildren();
	if (!children || children->Num() == 0 || SelectedIndex >= children->Num())
		return FVector2D::ZeroVector;

	const TSharedRef<SWidget> firstChild = children->GetChildAt(0);
	const float itemHeight = firstChild->GetDesiredSize().Y;
	const float viewportHeight = ListGeometry.GetLocalSize().Y;
    
	if (viewportHeight <= 0.0f || itemHeight <= 0.0f)
		return FVector2D::ZeroVector;
    
	const float viewportCenter = viewportHeight * 0.5f;
	const float itemCenter = (SelectedIndex * itemHeight) + (itemHeight * 0.5f);
	const float translationY = viewportCenter - itemCenter;
    
	return FVector2D(0.0f, translationY);
}

FSlateFontInfo UMounteaInventoryUIStatics::SetFontSize(const FSlateFontInfo& Font, const int32 NewSize)
{
	auto returnValue = Font;
	returnValue.Size = NewSize;
	return returnValue;
}

FSlateFontInfo UMounteaInventoryUIStatics::SetFontTypeface(const FSlateFontInfo& Font, const FName& NewTypeface)
{
	auto returnValue = Font;
	returnValue.TypefaceFontName = NewTypeface;
	return returnValue;
}

UMounteaAdvancedInventoryUIConfig* UMounteaInventoryUIStatics::GetInventoryUISettingsConfig()
{
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return nullptr;
	return settings->InventoryUISettingsConfig.LoadSynchronous();
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryUIStatics::GetParentInventory(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetParentInventory(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	if (Target.GetObject())
		Target->Execute_SetParentInventory(Target.GetObject(), NewParentInventory);
}

bool UMounteaInventoryUIStatics::CreateWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_CreateWrapperWidget(Target.GetObject()) : false;
}

UUserWidget* UMounteaInventoryUIStatics::GetWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetWrapperWidget(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::RemoveWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveWrapperWidget(Target.GetObject());
}

bool UMounteaInventoryUIStatics::CreateInventoryWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return (Target.GetObject() ? Target->Execute_CreateInventoryWidget(Target.GetObject()) : false);
}

UUserWidget* UMounteaInventoryUIStatics::GetInventoryWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetInventoryWidget(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::RemoveInventoryWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryWidget(Target.GetObject());
}

bool UMounteaInventoryUIStatics::SetInventoryWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, UUserWidget* NewInventoryWidget)
{
	return (Target.GetObject() ? Target->Execute_SetInventoryWidget(Target.GetObject(), NewInventoryWidget) : false);
}

UUserWidget* UMounteaInventoryUIStatics::GetNotificationContainer(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetNotificationContainer(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::SetNotificationContainer(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, UUserWidget* NewNotificationContainer)
{
	if (Target.GetObject())
		Target->Execute_SetNotificationContainer(Target.GetObject(), NewNotificationContainer);
}

void UMounteaInventoryUIStatics::CreateInventoryNotification(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	const FInventoryNotificationData& NotificationData)
{
	if (Target.GetObject())
		Target->Execute_CreateInventoryNotification(Target.GetObject(), NotificationData);
}

void UMounteaInventoryUIStatics::RemoveInventoryNotifications(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryNotifications(Target.GetObject());
}

void UMounteaInventoryUIStatics::CategorySelected(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	const FString& SelectedCategoryId)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_CategorySelected(Target.GetObject(), SelectedCategoryId);
}

FString UMounteaInventoryUIStatics::GetSelectedCategoryId(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetSelectedCategoryId(Target.GetObject()) : TEXT("none");
}

void UMounteaInventoryUIStatics::ItemSelected(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, 
	const FGuid& ItemGuid)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_ItemSelected(Target.GetObject(), ItemGuid);
}

FGuid UMounteaInventoryUIStatics::GetSelectedItemGuid(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetActiveItemGuid(Target.GetObject()) : FGuid();
}

void UMounteaInventoryUIStatics::SetInventoryOwningInventoryUI(UWidget* Target, 
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetCategoriesWrapperOwningInventoryUI(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI)
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

void UMounteaInventoryUIStatics::StoreItem(FMounteaInventoryGridSlot& SourceData, const FGuid& ItemId)
{
	//SourceData.OccupiedItemId = ItemId;
}

void UMounteaInventoryUIStatics::ResetItem(FMounteaInventoryGridSlot& SourceData)
{
	SourceData.ResetSlot();
}

FString UMounteaInventoryUIStatics::GridSlot_ToString(const FMounteaInventoryGridSlot& SourceData)
{
	return SourceData.ToString();
}

#pragma region UI_Input

bool UMounteaInventoryUIStatics::FindUIActionMappingForItemAction(const FGameplayTagContainer& PreferredInputActions,
	const FGameplayTagContainer& WidgetStates, FMounteaWidgetInputActionMapping& OutMapping)
{
	OutMapping = FMounteaWidgetInputActionMapping();

	const auto uiConfig = GetInventoryUISettingsConfig();
	if (!IsValid(uiConfig))
		return false;

	const auto& definedMappings = uiConfig->UIActionMappings;
	if (definedMappings.Num() == 0)
		return false;

	if (PreferredInputActions.Num() == 0)
		return false;

	const bool bUseStateFiltering = (WidgetStates.Num() > 0);

	const auto foundMapping = Algo::FindByPredicate(definedMappings,
		[&PreferredInputActions, &WidgetStates, bUseStateFiltering](const FMounteaWidgetInputActionMapping& Mapping)
		{
			if (!PreferredInputActions.HasTagExact(Mapping.ActionTag))
				return false;

			if (!bUseStateFiltering)
				return true;

			return !Mapping.BlacklistedWidgetStates.HasAnyExact(WidgetStates);
		});

	if (foundMapping == nullptr)
		return false;

	OutMapping = *foundMapping;
	return true;
}

bool UMounteaInventoryUIStatics::FindUIActionMappingFromKeyEvent(const FKeyEvent& KeyEvent, const FGameplayTagContainer& WidgetStates,
	const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping)
{
	return FindUIActionMappingByKey(KeyEvent.GetKey(), KeyEvent.GetModifierKeys(), WidgetStates, Mappings, OutMapping);
}

bool UMounteaInventoryUIStatics::FindUIActionMappingFromAnalogEvent(const FAnalogInputEvent& AnalogEvent, const FGameplayTagContainer& WidgetStates,
	const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping)
{
	return FindUIActionMappingByKey(AnalogEvent.GetKey(), AnalogEvent.GetModifierKeys(), WidgetStates, Mappings, OutMapping);
}

bool UMounteaInventoryUIStatics::FindUIActionMappingFromMouseEvent(const FPointerEvent& MouseEvent, const FGameplayTagContainer& WidgetStates,
	const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping)
{
	// Wheel events (NativeOnMouseWheel) don't have an "effecting button".
	// Convention: bind wheel mappings using EKeys::MouseWheelAxis.
	const float WheelDelta = MouseEvent.GetWheelDelta();
	const FKey PressedKey = !FMath::IsNearlyZero(WheelDelta)
		? EKeys::MouseWheelAxis
		: MouseEvent.GetEffectingButton();

	return FindUIActionMappingByKey(PressedKey, MouseEvent.GetModifierKeys(), WidgetStates, Mappings, OutMapping);
}

bool UMounteaInventoryUIStatics::FindUIActionMappingsFromKeyEvent(const FKeyEvent& KeyEvent, const FGameplayTagContainer& WidgetStates,
	const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings)
{
	return FindUIActionMappingsByKey(KeyEvent.GetKey(), KeyEvent.GetModifierKeys(), WidgetStates, Mappings, OutMappings);
}

bool UMounteaInventoryUIStatics::FindUIActionMappingsFromAnalogEvent(const FAnalogInputEvent& AnalogEvent, const FGameplayTagContainer& WidgetStates,
	const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings)
{
	return FindUIActionMappingsByKey(AnalogEvent.GetKey(), AnalogEvent.GetModifierKeys(), WidgetStates, Mappings, OutMappings);
}

bool UMounteaInventoryUIStatics::FindUIActionMappingsFromMouseEvent(const FPointerEvent& MouseEvent, const FGameplayTagContainer& WidgetStates,
	const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings)
{
	const float WheelDelta = MouseEvent.GetWheelDelta();
	const FKey PressedKey = !FMath::IsNearlyZero(WheelDelta)
		? EKeys::MouseWheelAxis
		: MouseEvent.GetEffectingButton();

	return FindUIActionMappingsByKey(PressedKey, MouseEvent.GetModifierKeys(), WidgetStates, Mappings, OutMappings);
}

bool UMounteaInventoryUIStatics::FindUIActionMappingByKey(const FKey& PressedKey, const FModifierKeysState& Modifiers,
	const FGameplayTagContainer& WidgetStates, const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping)
{
	if (!PressedKey.IsValid())
		return false;
	
	if (Mappings.Num() <= 0)
		return false;

	const bool bUseStateFiltering = (WidgetStates.Num() > 0);

	const auto matchingMapping = [&PressedKey, &WidgetStates, bUseStateFiltering](const FMounteaWidgetInputActionMapping& M)
	{
		const bool bKeyFound = M.Keys.Contains(PressedKey);
		return bUseStateFiltering ? bKeyFound && !M.BlacklistedWidgetStates.HasAnyExact(WidgetStates) : bKeyFound;
	};

	TArray<const FMounteaWidgetInputActionMapping*> candidateMappings;
	candidateMappings.Reserve(Mappings.Num());

	Algo::TransformIf(
		Mappings,
		candidateMappings,
		matchingMapping,
		[](const FMounteaWidgetInputActionMapping& M) { return &M; }
	);

	const FMounteaWidgetInputActionMapping* const* bestMapping =
		Algo::MaxElementBy(candidateMappings, [](const auto* M) { return M->InputPriority; });

	if (!bestMapping)
		return false;

	OutMapping = **bestMapping;
	return true;
}

bool UMounteaInventoryUIStatics::FindUIActionMappingsByKey(const FKey& PressedKey, const FModifierKeysState& Modifiers,
	const FGameplayTagContainer& WidgetStates, const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings)
{
	OutMappings.Reset();

	if (!PressedKey.IsValid())
		return false;

	if (Mappings.Num() <= 0)
		return false;
	
	const bool bUseStateFiltering = (WidgetStates.Num() > 0);

	const auto matchingMapping = [&PressedKey, &WidgetStates, bUseStateFiltering](const FMounteaWidgetInputActionMapping& M)
	{
		const bool bKeyFound = M.Keys.Contains(PressedKey);
		return bUseStateFiltering ? bKeyFound && !M.BlacklistedWidgetStates.HasAnyExact(WidgetStates) : bKeyFound;
	};

	TArray<const FMounteaWidgetInputActionMapping*> candidateMappings;
	candidateMappings.Reserve(Mappings.Num());

	Algo::TransformIf(
		Mappings,
		candidateMappings,
		matchingMapping,
		[](const FMounteaWidgetInputActionMapping& M) { return &M; }
	);

	if (candidateMappings.IsEmpty())
		return false;

	Algo::Sort(candidateMappings, [](const auto* A, const auto* B)
	{
		return A->InputPriority > B->InputPriority;
	});

	OutMappings.Reserve(candidateMappings.Num());

	Algo::Transform(
		candidateMappings,
		OutMappings,
		[](const FMounteaWidgetInputActionMapping* M) { return *M; }
	);

	return true;
}

#pragma endregion 

FInventoryItem UMounteaInventoryUIStatics::FindItem(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	const FInventoryItemSearchParams& SearchParams)
{
	if (!IsValid(Target.GetObject()))
		return {};

	const auto& inventory = Target->Execute_GetParentInventory(Target.GetObject());
	if (!IsValid(inventory.GetObject()))
		return {};

	return inventory->Execute_FindItem(inventory.GetObject(), SearchParams);
}

TMap<FGameplayTag, FInventoryUICustomData> UMounteaInventoryUIStatics::GetCustomItemsMap(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? 
		IMounteaAdvancedInventoryUIManagerInterface::Execute_GetCustomItemsMap(Target.GetObject()) : TMap<FGameplayTag, FInventoryUICustomData>();
}

void UMounteaInventoryUIStatics::AddCustomItem(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_AddCustomItemToMap(Target.GetObject(), ItemTag, ItemId);
}

void UMounteaInventoryUIStatics::AppendCustomItems(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_AppendCustomItemsMap(Target.GetObject(), OtherItems);
}

void UMounteaInventoryUIStatics::ClearCustomItems(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_ClearCustomItemsMap(Target.GetObject());
}

bool UMounteaInventoryUIStatics::RemoveCustomItem(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventoryUIManagerInterface::Execute_RemoveCustomItemFromMap(Target.GetObject(), ItemTag, ItemId);
	return false;
}

bool UMounteaInventoryUIStatics::IsItemStoredInCustomMap(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, 
	const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventoryUIManagerInterface::Execute_IsItemStoredInCustomMap(Target.GetObject(), ItemTag, ItemId);
	return false;
}

void UMounteaInventoryUIStatics::ExecuteWidgetCommandFromManager(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, 
	const FString& Command, UObject* OptionalPayload)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventoryUIManagerInterface::Execute_ExecuteWidgetCommand(Target.GetObject(), Command, OptionalPayload);
}

TArray<UMounteaSelectableInventoryItemAction*> UMounteaInventoryUIStatics::GetItemActionsQueue(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? IMounteaAdvancedInventoryUIManagerInterface::Execute_GetItemActionsQueue(Target.GetObject()) : TArray<UMounteaSelectableInventoryItemAction*>();
}

bool UMounteaInventoryUIStatics::EnqueueItemAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload)
{
	return Target.GetObject() ? IMounteaAdvancedInventoryUIManagerInterface::Execute_EnqueueItemAction(Target.GetObject(), ItemAction, Payload) : false;
}

bool UMounteaInventoryUIStatics::EnqueueItemActions_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
	TArray<UMounteaSelectableInventoryItemAction*>& ItemActions, UObject* Payload)
{
	return Target.GetObject() ? IMounteaAdvancedInventoryUIManagerInterface::Execute_EnqueueItemActions(Target.GetObject(), ItemActions, Payload) : false;
}

void UMounteaInventoryUIStatics::EmptyItemActionsQueue_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_EmptyItemActionsQueue(Target.GetObject());
}

void UMounteaInventoryUIStatics::PauseItemActionsQueue_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_PauseItemActionsQueue(Target.GetObject());
}

bool UMounteaInventoryUIStatics::ResumeItemActionsQueue_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return Target.GetObject() ? IMounteaAdvancedInventoryUIManagerInterface::Execute_ResumeItemActionsQueue(Target.GetObject()) : false;
}

void UMounteaInventoryUIStatics::StartItemActionsQueue_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	if (Target.GetObject())
		IMounteaAdvancedInventoryUIManagerInterface::Execute_StartItemActionsQueue(Target.GetObject());
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

UObject* UMounteaInventoryUIStatics::GetInventoryUIManager(AActor* FromActor)
{
	if (!FromActor) return nullptr;

	APlayerController* playerController = FindPlayerController(FromActor, 3);
	if (!playerController) return nullptr;

	const UMounteaAdvancedInventoryUISubsystem* localSubsystem = GetInventoryUISubsystem(playerController);
	return localSubsystem ? localSubsystem->GetInventoryUIManager() : nullptr;
}

UMounteaAdvancedInventoryUISubsystem* UMounteaInventoryUIStatics::GetInventoryUISubsystem(APlayerController* FromPlayerController)
{
	if (!FromPlayerController) return nullptr;

	UMounteaAdvancedInventoryUISubsystem* localSubsystem = FromPlayerController->GetLocalPlayer()->GetSubsystem<
		UMounteaAdvancedInventoryUISubsystem>();
	return localSubsystem;
}

UMounteaAdvancedInventoryUISubsystem* UMounteaInventoryUIStatics::GetInventoryUISubsystem_Generic(UObject* Context)
{
	if (!IsValid(Context))
		return nullptr;

	APlayerController* playerController = nullptr;

	if (AActor* actor = Cast<AActor>(Context))
		playerController = FindPlayerController(actor, 3);
	else if (const UActorComponent* actorComp = Cast<UActorComponent>(Context))
	{
		if (AActor* ownerActor = actorComp->GetOwner())
			playerController = FindPlayerController(ownerActor, 3);
	}
	else if (const UUserWidget* userWidget = Cast<UUserWidget>(Context))
		playerController = FindPlayerController(userWidget->GetOwningPlayer(), 2);

	return IsValid(playerController) ? GetInventoryUISubsystem(playerController) : nullptr;
}

int32 UMounteaInventoryUIStatics::MounteaInventoryScrollBox_GetChildrenCount(
	const UMounteaInventoryScrollBox* ScrollBox)
{
	return ScrollBox ? ScrollBox->GetChildrenCount() : INDEX_NONE;
}

int32 UMounteaInventoryUIStatics::MounteaInventoryScrollBox_GetActiveIndex(const UMounteaInventoryScrollBox* ScrollBox)
{
	return ScrollBox ? ScrollBox->GetActiveIndex() : INDEX_NONE;
}

void UMounteaInventoryUIStatics::MounteaInventoryScrollBox_SetActiveIndex(UMounteaInventoryScrollBox* ScrollBox,
	const int32 NewActiveIndex)
{
	if (ScrollBox)
		ScrollBox->SetActiveIndex(NewActiveIndex);
}

void UMounteaInventoryUIStatics::MounteaInventoryScrollBox_AddChild(UMounteaInventoryScrollBox* ScrollBox, UWidget* Content)
{
	if (!ScrollBox || !Content)
		return;
	ScrollBox->AddChild(Content);
}

void UMounteaInventoryUIStatics::MounteaInventoryScrollBox_ResetChildren(UMounteaInventoryScrollBox* ScrollBox)
{
	if (!ScrollBox)
		return;
	ScrollBox->ResetChildren();
}

TArray<UWidget*> UMounteaInventoryUIStatics::MounteaInventoryScrollBox_GetAllChildren(UMounteaInventoryScrollBox* ScrollBox)
{
	return ScrollBox ? ScrollBox->GetChildWidgets() : TArray<UWidget*>();
}

void UMounteaInventoryUIStatics::MounteaInventoryScrollBox_RemoveChild(UMounteaInventoryScrollBox* ScrollBox, UWidget* Content)
{
	if (ScrollBox)
		return ScrollBox->RemoveChild(Content);
}

void UMounteaInventoryUIStatics::MounteaInventoryScrollBox_RemoveChildAt(UMounteaInventoryScrollBox* ScrollBox, const int32 Index)
{
	if (ScrollBox)
		return ScrollBox->RemoveChildAt(Index);
}

void UMounteaInventoryUIStatics::SetOwningInventoryUI(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& NewOwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, NewOwningInventoryUI);
}

void UMounteaInventoryUIStatics::InitializeWrapperWidget(UObject* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Parent)
{
	if (Target && Target->Implements<UMounteaInventorySystemWrapperWidgetInterface>())
		IMounteaInventorySystemWrapperWidgetInterface::Execute_InitializeWrapperWidget(Target, Parent);
}

void UMounteaInventoryUIStatics::SetSourceInventory(
	const TScriptInterface<IMounteaAdvancedBaseInventoryWidgetInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& ParentInventory)
{
	if (Target.GetObject())
		Target->Execute_SetOwningInventoryUI(Target.GetObject(), ParentInventory);
}

#pragma region Wrapper

bool UMounteaInventoryUIStatics::IsValidWrapperWidget(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaInventorySystemWrapperWidgetInterface>();
}

FGameplayTagContainer UMounteaInventoryUIStatics::GetWrapperWidgetStates(UObject* Target)
{
	return IsValidWrapperWidget(Target) ? 
		IMounteaInventorySystemWrapperWidgetInterface::Execute_GetWrapperWidgetStates(Target) : FGameplayTagContainer();
}

void UMounteaInventoryUIStatics::SetWrapperWidgetStates(UObject* Target, const FGameplayTagContainer& NewStates)
{
	if (IsValidWrapperWidget(Target))
		return IMounteaInventorySystemWrapperWidgetInterface::Execute_SetWrapperWidgetStates(Target, NewStates);
}

bool UMounteaInventoryUIStatics::AddWidgetStateTag(UObject* Target, const FGameplayTag& Tag)
{
	return IsValidWrapperWidget(Target) ? 
		IMounteaInventorySystemWrapperWidgetInterface::Execute_AddWidgetStateTag(Target, Tag) : false;
}

bool UMounteaInventoryUIStatics::RemoveWidgetStateTag(UObject* Target, const FGameplayTag& Tag)
{
	return IsValidWrapperWidget(Target) ? 
		IMounteaInventorySystemWrapperWidgetInterface::Execute_RemoveWidgetStateTag(Target, Tag) : false;
}

bool UMounteaInventoryUIStatics::HasWidgetStateTag(UObject* Target, const FGameplayTag& Tag, const bool bExactMatch)
{
	return IsValidWrapperWidget(Target) ? 
		IMounteaInventorySystemWrapperWidgetInterface::Execute_HasWidgetStateTag(Target, Tag, bExactMatch) : false;
}

void UMounteaInventoryUIStatics::ClearWidgetStateTags(UObject* Target)
{
	if (IsValidWrapperWidget(Target))
		return IMounteaInventorySystemWrapperWidgetInterface::Execute_ClearWidgetStateTags(Target);
}

bool UMounteaInventoryUIStatics::AppendWidgetStateTags(UObject* Target, const FGameplayTagContainer& TagsToAppend)
{
	return IsValidWrapperWidget(Target) ? 
		IMounteaInventorySystemWrapperWidgetInterface::Execute_AppendWidgetStateTags(Target, TagsToAppend) : false;
}

#pragma endregion 

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
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::SetActiveState(UWidget* Target, const bool bIsActive)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SetActiveState(Target, bIsActive);
}

FInventoryItemData UMounteaInventoryUIStatics::MakeInventoryItemWidgetData(const FInventoryItem& Item, const int32 Quantity)
{
	return FInventoryItemData(Quantity, Item);
}

UWidget* UMounteaInventoryUIStatics::GetActiveItemWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target)
{
	return (
		IsValid(Target.GetObject()) )
	? IMounteaAdvancedInventoryUIManagerInterface::Execute_GetActiveItemWidget(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::SetActiveItemWidget(
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, UWidget* NewActiveItemWidget)
{
	if (!IsValid(Target.GetObject())) return;

	Target->Execute_SetActiveItemWidget(Target.GetObject(), NewActiveItemWidget);
}

void UMounteaInventoryUIStatics::ItemWidget_SelectItem(UWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SelectItem(Target);
}

FGuid UMounteaInventoryUIStatics::ItemWidget_GetInventoryItemId(UWidget* Target)
{
	return (
		IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>()
		) ? IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetInventoryItemId(Target) : FGuid();
}

void UMounteaInventoryUIStatics::SetItemOwningInventoryUI(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI)
{
	SetOwningInventoryUIInternal(Target, OwningInventoryUI);
}

void UMounteaInventoryUIStatics::ItemWidget_InitializeItemWidget(UWidget* Target, const FInventoryItem& Item,
	const int32 Quantity)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_InitializeItemWidget(Target, Item, Quantity);
}

FInventoryItemData UMounteaInventoryUIStatics::ItemWidget_GetInventoryData(UWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		return IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetInventoryData(Target);
	return FInventoryItemData();
}

void UMounteaInventoryUIStatics::ItemWidget_SetInventoryData(UWidget* Target, const FInventoryItemData& InventoryItemData)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SetInventoryData(Target, InventoryItemData);
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

void UMounteaInventoryUIStatics::ItemAction_InitializeItemAction(UWidget* Target,
                                                                 const UMounteaSelectableInventoryItemAction* ItemAction, const FGuid& SelectedItem)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>())
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_InitializeItemAction(
				Target, ItemAction, SelectedItem);
	else
		LOG_ERROR(TEXT("[InitializeItemAction] Target does not implement IMounteaAdvancedInventoryItemActionWidgetInterface!"));
}

bool UMounteaInventoryUIStatics::ItemAction_IsActionEnabled(UWidget* Target)
{
	return IsValid(Target) &&
		Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>() && 
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_IsActionEnabled(Target);
}

bool UMounteaInventoryUIStatics::ItemAction_IsActionValid(UWidget* Target)
{
	return IsValid(Target) && 
		Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>() && 
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_IsActionValid(Target);
}

void UMounteaInventoryUIStatics::ItemAction_ExecuteItemAction(UWidget* Target)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemActionWidgetInterface>())
		IMounteaAdvancedInventoryItemActionWidgetInterface::Execute_ExecuteItemAction(Target);
	else
		LOG_ERROR(TEXT("[ExecuteItemAction] Target does not implement IMounteaAdvancedInventoryItemActionWidgetInterface!"));
}

TSoftClassPtr<UMounteaSelectableInventoryItemAction> UMounteaInventoryUIStatics::ItemAction_GetItemAction(UWidget* Target)
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

#pragma region ItemActionsContainer

bool UMounteaInventoryUIStatics::IsValidActionsContainerWidget(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>();
}

void UMounteaInventoryUIStatics::ItemActionsContainer_ConstructFromActionsList(UWidget* Target, 
	const TArray<UMounteaSelectableInventoryItemAction*>& ItemActionsList)
{
	if (IsValidActionsContainerWidget(Target))
		IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_ConstructFromActionsList(Target, ItemActionsList);
}

UWidget* UMounteaInventoryUIStatics::ItemActionsContainer_AddItemActionToContainer(UWidget* Target, UMounteaSelectableInventoryItemAction* ItemAction, 
	const FGuid& ItemId)
{
	return IsValidActionsContainerWidget(Target) ? 
		IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_AddItemActionToContainer(Target, ItemAction, ItemId) :
		nullptr;
}

void UMounteaInventoryUIStatics::ItemActionsContainer_RemoveItemActionFromContainer(UWidget* Target, UWidget* ItemActionWidget)
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

void UMounteaInventoryUIStatics::ItemActionsContainer_ClearItemActionsContainer(UWidget* Target)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[ClearItemActionsContainer] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return;
	}

	IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_ClearItemActionsContainer(Target);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_SelectItemAction(UWidget* Target, UWidget* ItemActionWidget)
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

TArray<UWidget*> UMounteaInventoryUIStatics::ItemActionsContainer_GetItemActionsInContainer(UWidget* Target)
{
	if (!IsValid(Target) || !Target->Implements<UMounteaAdvancedInventoryItemActionsContainerWidgetInterface>())
	{
		LOG_ERROR(TEXT("[GetItemActionsInContainer] Target does not implement IMounteaAdvancedInventoryItemActionsContainerWidgetInterface or is invalid!"));
		return {};
	}

	return IMounteaAdvancedInventoryItemActionsContainerWidgetInterface::Execute_GetItemActionsInContainer(Target);
}

void UMounteaInventoryUIStatics::ItemActionsContainer_SelectItemActionByIndex(UWidget* Target, const int32 ItemActionIndex)
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

#pragma endregion

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

FInventorySlot UMounteaInventoryUIStatics::MakeInventorySlot(UUserWidget* UserWidget, const FGuid& ItemId)
{
	FInventorySlot returnValue;
	{
		returnValue.OverrideGuid(ItemId);
		returnValue.ItemWidget = UserWidget;
	}
	return returnValue;
}

void UMounteaInventoryUIStatics::ItemSlot_SetItemSlotOwningInventoryUI(UWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI)
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

void UMounteaInventoryUIStatics::ItemSlot_StoreGridSlotData(UWidget* Target, const FMounteaInventoryGridSlot& SlotData)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(Target, SlotData);
}

FMounteaInventoryGridSlot UMounteaInventoryUIStatics::ItemSlot_GetGridSlotData(UWidget* Target)
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
	const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI)
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

UWidget* UMounteaInventoryUIStatics::SlotsWrapper_GetSelectedItemWidget(UWidget* Target)
{
	return (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface>())
	? IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface::Execute_GetSelectedItemWidget(Target) : nullptr;
}

void UMounteaInventoryUIStatics::SlotsWrapper_SetSelectedItemWidget(UWidget* Target,
	UWidget* NewSelectedItemWidget)
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

	const bool bHasUIInterface = ParentInventory->Implements<UMounteaAdvancedInventoryUIManagerInterface>();
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
		auto inventoryRef = IMounteaAdvancedInventoryUIManagerInterface::Execute_GetParentInventory(ParentInventory);
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
	const bool bAlwaysStack = true; //config->bAlwaysStackStackableItems; This is now in UI config! And it should be in specific UI config class!
	
	return FindEmptyGridSlotRecursive(InventoryInterface, inventoryItem, gridSlots, bIsStackable, bAlwaysStack);
}

int32 UMounteaInventoryUIStatics::FindEmptyGridSlotRecursive(TScriptInterface<IMounteaAdvancedInventoryInterface>& InventoryInterface, 
	const FInventoryItem& InventoryItem, const TArray<FMounteaInventoryGridSlot>& GridSlots, const bool bIsStackable, 
	const bool bAlwaysStackItems)
{
	return INDEX_NONE;
	/*
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
	
	// Storing itemSlots in a temp array first to avoid a dangling reference in line 1475
	TArray<FMounteaInventoryGridSlot> slotsArray = itemSlots.Array();
	
	// Find slot with this item that has the most available space
	if (bAlwaysStackItems && !slotsArray.IsEmpty())
	{
		for (int i = 0; i < slotsArray.Num(); i++)
		{
			const auto& gridSlot = slotsArray[i];
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
	*/
}

bool UMounteaInventoryUIStatics::Helper_ItemsGrid_UpdateItemInSlot(UUserWidget* GridWidget, const FGuid& ItemId, 
	int32 SlotIndex, TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface> ParentUIComponent)
{
	return false;
	/*
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
	*/
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

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraRotation(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const FVector2f& MouseDelta)
{
	if (IsValid(Target))
		Target->ProcessRotationInput(MouseDelta);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraHeight(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const FVector2f& MouseDelta)
{
	if (IsValid(Target))
		Target->ProcessHeightInput(MouseDelta);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraZoom(UMounteaAdvancedInventoryInteractableObjectWidget* Target,
	const float WheelDelta)
{
	if (IsValid(Target))
		ItemPreview_UpdateCameraZoom(Target, WheelDelta);
}

void UMounteaInventoryUIStatics::ItemPreview_SetCameraRotationAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const float YawNormalized, const float PitchNormalized)
{
	if (IsValid(Target))
		Target->SetCameraRotationAbsolute(YawNormalized, PitchNormalized);
}

void UMounteaInventoryUIStatics::ItemPreview_SetCameraHeightAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const float HeightNormalized)
{
	if (IsValid(Target))
		Target->SetCameraHeightAbsolute(HeightNormalized);
}

void UMounteaInventoryUIStatics::ItemPreview_SetCameraZoomAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const float ZoomNormalized)
{
	if (IsValid(Target))
		Target->SetCameraZoomAbsolute(ZoomNormalized);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraRotationAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const FVector2f& AnalogInput, const float DeltaTime)
{
	if (IsValid(Target))
		Target->ProcessAnalogRotation(AnalogInput, DeltaTime);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraHeightAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const float AnalogInput, const float DeltaTime)
{
	if (IsValid(Target))
		Target->ProcessAnalogHeight(AnalogInput, DeltaTime);
}

void UMounteaInventoryUIStatics::ItemPreview_UpdateCameraZoomAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
	const float AnalogInput, const float DeltaTime)
{
	if (IsValid(Target))
		Target->ProcessAnalogZoom(AnalogInput, DeltaTime);
}
