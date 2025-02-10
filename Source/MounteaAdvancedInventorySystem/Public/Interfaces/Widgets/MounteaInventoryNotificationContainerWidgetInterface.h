// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryNotificationContainerWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryNotificationContainerWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventoryNotificationContainerWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void AddNotification();
};
