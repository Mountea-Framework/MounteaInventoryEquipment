// All rights reserved Dominik Pavlicek 2022.


#include "Definitions/InventoryKeyAction.h"

#include "Helpers/ActorInventoryBPFLibrary.h"

#include "Widgets/InventoryWidget.h"


UInventoryKeyAction::UInventoryKeyAction()
{
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("Windows")), FKey(TEXT("E")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("Linux")), FKey(TEXT("E")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("Mac")), FKey(TEXT("E")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("PS4")), FKey(TEXT("Gamepad Face Button Down")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("XboxOne")), FKey(TEXT("Gamepad Face Button Down")), nullptr));
}

UWorld* UInventoryKeyAction::GetWorld() const
{
	return UObject::GetWorld();
}

UActorInventoryManagerComponent* UInventoryKeyAction::GetInventoryManager(const UObject* InventoryContext)
{
	return UActorInventoryBPFLibrary::GetInventoryManager(InventoryContext);
}
