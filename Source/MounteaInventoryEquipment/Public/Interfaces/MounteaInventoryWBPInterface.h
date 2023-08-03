// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Helpers/MounteaInventoryHelpers.h"

#include "MounteaInventoryWBPInterface.generated.h"

class IMounteaInventoryInterface;
struct FInventoryNotificationData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
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
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	bool GetInventoryVisibility();
	virtual bool GetInventoryVisibility_Implementation() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetOwningInventory();
	virtual TScriptInterface<IMounteaInventoryInterface> GetOwningInventory_Implementation() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	void SetOwningInventory(const TScriptInterface<IMounteaInventoryInterface>& OwningInventory);
	virtual void SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& OwningInventory) = 0;
};
