// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryUIStatics.h"

#include "GameFramework/PlayerState.h"
#include "Interfaces/Widgets/MounteaInventoryBaseWidgetInterface.h"

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

void UMounteaInventoryUIStatics::RemoveInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryUI(Target.GetObject());
}

void UMounteaInventoryUIStatics::SetInventoryUIVisibility(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const bool bShowInventory)
{
	if (Target.GetObject())
		Target->Execute_SetInventoryUIVisibility(Target.GetObject(), bShowInventory);
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

void UMounteaInventoryUIStatics::RemoveInventoryWidget(
	const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryWidget(Target.GetObject());
}

bool UMounteaInventoryUIStatics::SetSourceInventory(
	const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target,
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory)
{
	return Target.GetObject() ? Target->Execute_SetSourceInventory(Target.GetObject(), ParentInventory) : false;
}
