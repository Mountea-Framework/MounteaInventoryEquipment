// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryItemWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void SetOwningInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI) = 0;
	
	/**
	 * 
	 * @param ItemGuid 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void SetInventoryItemId(const FGuid& ItemGuid);
	virtual void SetInventoryItemId_Implementation(const FGuid& ItemGuid) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	FGuid GetInventoryItemId() const;
	virtual FGuid GetInventoryItemId_Implementation() const = 0;

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void RefreshItemWidget(const int32 Quantity);
	virtual void RefreshItemWidget_Implementation(const int32 Quantity) = 0;

	/**
	 *
	 * @param ParentSlot The parent slot to be assigned to the inventory item widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void SetParentSlot(UUserWidget* ParentSlot);
	virtual void SetParentSlot_Implementation(UUserWidget* ParentSlot) = 0;

	/**
	 * Highlights or un-highlights an inventory item within the UI by interacting with the provided widget.
	 *
	 * @param bIsSelected A boolean indicating whether to highlight (true) or un-highlight (false) the item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void HighlightItem(const bool bIsSelected = false);
	virtual void HighlightItem_Implementation(const bool bIsSelected = false) = 0;
};
