// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryUIStatics.h"

#include "GameFramework/PlayerState.h"

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

bool UMounteaInventoryUIStatics::CreateInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_CreateInventoryUI(Target.GetObject()) : false;
}

UUserWidget* UMounteaInventoryUIStatics::GetInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetInventoryUI(Target.GetObject()) : nullptr;
}

void UMounteaInventoryUIStatics::RemoveInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryUI(Target.GetObject());
}

void UMounteaInventoryUIStatics::SetInventoryUIVisibility(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const ESlateVisibility NewVisibility)
{
	if (Target.GetObject())
		Target->Execute_SetInventoryUIVisibility(Target.GetObject(), NewVisibility);
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
