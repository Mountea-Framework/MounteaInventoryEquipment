// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaBaseWidgetInterface.generated.h"

class IMounteaInventoryWBPInterface;

// This class does not need to be modified.
UINTERFACE()
class UMounteaBaseWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaBaseWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Base")
	TScriptInterface<IMounteaInventoryWBPInterface> GetOwningInventoryWidget() const;
	virtual TScriptInterface<IMounteaInventoryWBPInterface> GetOwningInventoryWidget_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Base")
	void SetOwningInventoryWidget(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory);
	virtual void SetOwningInventoryWidget_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory) = 0;
};
