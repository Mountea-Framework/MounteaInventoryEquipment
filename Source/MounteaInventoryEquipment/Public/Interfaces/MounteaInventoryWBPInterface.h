// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryWBPInterface.generated.h"

struct FInventoryNotificationData;
// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UMounteaInventoryWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryWBPInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	void ProcessWBPCommand(const FString& Command);
	virtual void ProcessWBPCommand_Implementation(const FString& Command) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;
};
