// All rights reserved Dominik Pavlicek 2022.


#include "Definitions/InventoryKeyAction.h"


UInventoryKeyAction::UInventoryKeyAction()
{
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("Win32")), FKey(TEXT("E")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("Win64")), FKey(TEXT("E")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("Linux")), FKey(TEXT("E")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("Mac")), FKey(TEXT("E")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("PS4")), FKey(TEXT("Gamepad Face Button Down")), nullptr));
	PlatformBasedMapping.Add(FInventoryKeyActionData(FText::FromString(TEXT("XboxOne")), FKey(TEXT("Gamepad Face Button Down")), nullptr));
}
