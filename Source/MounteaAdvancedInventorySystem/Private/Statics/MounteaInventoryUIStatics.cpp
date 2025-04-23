// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryUIStatics.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/Widgets/MounteaInventoryBaseWidgetInterface.h"
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoriesWrapperWidgetInterface.h"
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoryWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotWidgetInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "Interfaces/Widgets/Inventory/MounteaAdvancedInventoryWidgetInterface.h"

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

void UMounteaInventoryUIStatics::SetItemSlotOwningInventoryUI(UUserWidget* Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI)
{
	if (IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_SetOwningInventoryUI(Target, OwningInventoryUI);
}
