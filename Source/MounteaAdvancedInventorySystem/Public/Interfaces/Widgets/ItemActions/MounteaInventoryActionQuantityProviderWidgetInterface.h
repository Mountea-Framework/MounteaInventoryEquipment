// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryActionQuantityProviderWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryActionQuantityProviderWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventoryActionQuantityProviderWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * Retrieves the range values for the quantity provider.
	 * This function should return a vector containing the minimum and maximum values for the quantity.
	 * 
	 * @return FVector2f containing the minimum and maximum range values.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FVector2f GetRangeValues() const;
	virtual FVector2f GetRangeValues_Implementation() const = 0;

	/**
	 * Sets the range values for the quantity provider.
	 * This function should be implemented to update the internal state of the widget with the provided range values.
	 * 
	 * @param RangeValues A vector containing the minimum and maximum range values to set.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void SetRangeValues(const FVector2f& RangeValues);
	virtual void SetRangeValues_Implementation(const FVector2f& RangeValues) = 0;
};
