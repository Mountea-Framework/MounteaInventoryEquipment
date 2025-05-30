// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedBaseInventoryWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedBaseInventoryWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedBaseInventoryWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Sets the reference to the owning inventory UI interface.
	 * This function allows the widget to be associated with a specific inventory UI.
	 *
	 * @param NewOwningInventoryUI A TScriptInterface reference to the inventory UI interface that owns this widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Base")
	void SetOwningInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI);
	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI) = 0;
};
