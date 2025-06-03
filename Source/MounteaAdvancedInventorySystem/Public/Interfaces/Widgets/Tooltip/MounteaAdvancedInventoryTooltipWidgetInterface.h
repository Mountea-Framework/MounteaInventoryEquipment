// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryTooltipWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryTooltipWidgetInterface : public UInterface
{
	GENERATED_BODY()
};


class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryTooltipWidgetInterface
{
	GENERATED_BODY()
public:
	/**
	 * Blueprint implementable event allowing customization of a tooltip to display information
	 * about a specific inventory item.
	 *
	 * This function is designed to be implemented in Blueprints to update the tooltip's
	 * content based on the provided Item ID. The implementation can be tailored to show
	 * relevant information about the item tied to the given ID.
	 *
	 * @param ItemId A unique identifier (GUID) representing the inventory item to display in the tooltip.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Tooltip")
	void SetTooltipItem(const FGuid& ItemId);
	virtual void SetTooltipItem_Implementation(const FGuid& ItemId) = 0;
};
