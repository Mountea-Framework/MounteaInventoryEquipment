// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void SetOwningInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI) = 0;

	/**
	 * 
	 * @param ItemId 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void AddItem(const FGuid& ItemId);
	virtual void AddItem_Implementation(const FGuid& ItemId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void UpdateItem(const FGuid& ItemId, const int32 OptionalItemSlot = INDEX_NONE);
	virtual void UpdateItem_Implementation(const FGuid& ItemId, const int32 OptionalItemSlot = INDEX_NONE) = 0;

	/**
	 *
	 * @param ItemId Unique identifier of the item to be removed.
	 * @param Quantity The number of items to remove. If set to -1, all items with the given ID will be removed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void RemoveItem(const FGuid& ItemId, const int32 Quantity = -1);
	virtual void RemoveItem_Implementation(const FGuid& ItemId, const int32 Quantity = -1) = 0;
};
