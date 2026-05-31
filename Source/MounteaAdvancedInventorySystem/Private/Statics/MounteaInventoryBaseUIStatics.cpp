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


#include "Statics/MounteaInventoryBaseUIStatics.h"

#include "Blueprint/UserWidget.h"
#include "Components/ActorComponent.h"
#include "Components/Widget.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/UserInterface/MounteaAdvancedInventorySharedHUDInterface.h"
#include "Interfaces/Widgets/MounteaInventorySystemWrapperWidgetInterface.h"
#include "Subsystems/MounteaAdvancedInventorySharedHUDSubsystem.h"

APlayerController* UMounteaInventoryBaseUIStatics::FindPlayerController(AActor* Actor, int SearchDepth)
{
	SearchDepth++;
	if (SearchDepth >= 8)
		return nullptr;

	if (!Actor)
		return nullptr;

	if (APlayerController* playerController = Cast<APlayerController>(Actor))
		return playerController;

	if (APawn* actorPawn = Cast<APawn>(Actor))
		return Cast<APlayerController>(actorPawn->GetController());

	if (AActor* ownerActor = Actor->GetOwner())
		return FindPlayerController(ownerActor, SearchDepth);

	return nullptr;
}

bool UMounteaInventoryBaseUIStatics::IsValidBaseUIHandler(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaInventorySystemWrapperWidgetInterface>();
}

UMounteaAdvancedInventorySharedHUDSubsystem* UMounteaInventoryBaseUIStatics::GetSharedHUDSubsystem(UObject* Context)
{
	if (!IsValid(Context))
		return nullptr;

	APlayerController* playerController = Cast<APlayerController>(Context);
	if (!IsValid(playerController))
	{
		if (AActor* actor = Cast<AActor>(Context))
			playerController = FindPlayerController(actor, 3);
		else if (const UActorComponent* actorComp = Cast<UActorComponent>(Context))
		{
			if (AActor* ownerActor = actorComp->GetOwner())
				playerController = FindPlayerController(ownerActor, 3);
		}
		else if (const UUserWidget* userWidget = Cast<UUserWidget>(Context))
			playerController = FindPlayerController(userWidget->GetOwningPlayer(), 2);
	}

	if (!IsValid(playerController))
		return nullptr;

	const ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
	return IsValid(localPlayer) ? localPlayer->GetSubsystem<UMounteaAdvancedInventorySharedHUDSubsystem>() : nullptr;
}

bool UMounteaInventoryBaseUIStatics::CreateWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_CreateWrapperWidget(Target.GetObject()) : false;
}

UUserWidget* UMounteaInventoryBaseUIStatics::GetWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetWrapperWidget(Target.GetObject()) : nullptr;
}

void UMounteaInventoryBaseUIStatics::RemoveWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveWrapperWidget(Target.GetObject());
}

void UMounteaInventoryBaseUIStatics::ExecuteWidgetCommandFromManager(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FString& Command,
	UObject* OptionalPayload)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventorySharedHUDInterface::Execute_ExecuteWidgetCommand(Target.GetObject(), Command, OptionalPayload);
}

UWidget* UMounteaInventoryBaseUIStatics::GetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetNotificationContainer(Target.GetObject()) : nullptr;
}

void UMounteaInventoryBaseUIStatics::SetNotificationContainer(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	UWidget* NewNotificationContainer)
{
	if (Target.GetObject())
		Target->Execute_SetNotificationContainer(Target.GetObject(), NewNotificationContainer);
}

void UMounteaInventoryBaseUIStatics::CreateInventoryNotification(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FInventoryNotificationData& NotificationData)
{
	if (Target.GetObject())
		Target->Execute_CreateInventoryNotification(Target.GetObject(), NotificationData);
}

void UMounteaInventoryBaseUIStatics::RemoveInventoryNotifications(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryNotifications(Target.GetObject());
}

FGameplayTagContainer UMounteaInventoryBaseUIStatics::GetManagerWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_GetWidgetStates(Target.GetObject()) : FGameplayTagContainer();
}

void UMounteaInventoryBaseUIStatics::SetManagerWidgetStates(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTagContainer& NewStates)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventorySharedHUDInterface::Execute_SetWidgetStates(Target.GetObject(), NewStates);
}

bool UMounteaInventoryBaseUIStatics::AddWidgetStateTag(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTag& Tag)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_AddWidgetStateTag(Target.GetObject(), Tag) : false;
}

bool UMounteaInventoryBaseUIStatics::RemoveWidgetStateTag(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTag& Tag)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_RemoveWidgetStateTag(Target.GetObject(), Tag) : false;
}

bool UMounteaInventoryBaseUIStatics::HasWidgetStateTag(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTag& Tag,
	const bool bExactMatch)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_HasWidgetStateTag(Target.GetObject(), Tag, bExactMatch) : false;
}

void UMounteaInventoryBaseUIStatics::ClearWidgetStateTags(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventorySharedHUDInterface::Execute_ClearWidgetStateTags(Target.GetObject());
}

bool UMounteaInventoryBaseUIStatics::AppendWidgetStateTags(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTagContainer& TagsToAppend)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_AppendWidgetStateTags(Target.GetObject(), TagsToAppend) : false;
}

bool UMounteaInventoryBaseUIStatics::HasAnyWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? GetManagerWidgetStates(Target).IsEmpty() : false;
}

void UMounteaInventoryBaseUIStatics::InitializeWrapperWidget(
	UObject* Target,
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Parent)
{
	if (IsValidBaseUIHandler(Target))
		IMounteaInventorySystemWrapperWidgetInterface::Execute_InitializeWrapperWidget(Target, Parent);
}

void UMounteaInventoryBaseUIStatics::RemoveBaseWrapperWidget(UObject* Target)
{
	if (IsValidBaseUIHandler(Target))
		IMounteaInventorySystemWrapperWidgetInterface::Execute_RemoveWrapperWidget(Target);
}

bool UMounteaInventoryBaseUIStatics::AddModalWindow(UObject* Target, UUserWidget* ModalWindow)
{
	return IsValidBaseUIHandler(Target) ? IMounteaInventorySystemWrapperWidgetInterface::Execute_AddModalWindow(Target, ModalWindow) : false;
}

bool UMounteaInventoryBaseUIStatics::CleanupModalWindow(UObject* Target)
{
	return IsValidBaseUIHandler(Target) ? IMounteaInventorySystemWrapperWidgetInterface::Execute_CleanupModalWindow(Target) : false;
}
