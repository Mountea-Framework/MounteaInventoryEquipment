// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryBaseWidgetInterface.generated.h"

class IMounteaAdvancedInventoryInterface;
class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryBaseWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventoryBaseWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 * @param Parent 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory")
	void InitializeInventoryWidget(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent);
	virtual void InitializeInventoryWidget_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent) = 0;

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory")
	void RemoveInventoryWidget();
	virtual void RemoveInventoryWidget_Implementation() = 0;

	// TODO: Do I need this?
	/**
	 * 
	 * @param ParentInventory 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory")
	bool SetSourceInventory(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory);
	virtual bool SetSourceInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory) = 0;
	
	// TODO: Apply sorting
	// TODO: Apply filtering
};
