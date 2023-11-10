// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "MounteaInventoryInstancedItemInterface.generated.h"


// This class does not need to be modified.
UINTERFACE()
class UMounteaInventoryInstancedItemInterface : public UInterface
{
	GENERATED_BODY()
};

class IMounteaInventoryInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemGenericEvent, const FString&, Message);

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryInstancedItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void SetOwningInventory(TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory) = 0;
	virtual TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const = 0;

	virtual FGameplayTagContainer GetItemFlags() const = 0;

	virtual FItemGenericEvent& GetEventHandle_OnItemAdded() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemRemoved() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemInitialized() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemModified() = 0;
};
