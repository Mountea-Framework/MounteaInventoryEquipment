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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void InitializeInventoryWidget(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent);
	virtual void InitializeInventoryWidget_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent) = 0;

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void RemoveInventoryWidgetWrapper();
	virtual void RemoveInventoryWidgetWrapper_Implementation() = 0;

	// TODO: Do I need this?
	/**
	 * 
	 * @param ParentInventory 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	bool SetSourceInventory(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory);
	virtual bool SetSourceInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory) = 0;

	/**
	 * Retrieves the visibility state of the main inventory UI.
	 *
	 * This function determines the current visibility status of the main UI
	 * and returns an appropriate ESlateVisibility value.
	 *
	 * @return The visibility state of the main UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	ESlateVisibility GetMainUIVisibility() const;
	virtual ESlateVisibility GetMainUIVisibility_Implementation() const = 0;

	/**
	 * Sets MainMenuUI Visibility.
	 * 
	 * @param Visibility The desired visibility state to apply to the main inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void SetMainUIVisibility(const ESlateVisibility Visibility);
	virtual void SetMainUIVisibility_Implementation(const ESlateVisibility Visibility) = 0;
	
	// TODO: Apply sorting
	// TODO: Apply filtering
};
