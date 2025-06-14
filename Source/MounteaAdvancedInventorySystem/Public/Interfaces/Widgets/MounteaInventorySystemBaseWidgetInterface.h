// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventorySystemBaseWidgetInterface.generated.h"

class IMounteaAdvancedInventoryInterface;
class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventorySystemBaseWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for managing base inventory widget functionality in the Mountea Advanced Inventory System.
 * This interface provides the necessary functions to initialize, display, and configure inventory UI elements.
 *
 * This Interface should be applied to main wrapper.
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventorySystemBaseWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 * @param Parent 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void InitializeMainUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent);
	virtual void InitializeMainUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent) = 0;

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void RemoveMainUI();
	virtual void RemoveMainUI_Implementation() = 0;
	
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
