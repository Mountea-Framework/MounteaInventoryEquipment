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
#include "MounteaAdvancedInventoryCategoryWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryCategoryWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryCategoryWidgetInterface defines individual category widget functionality.
 * Category widget interfaces manage category identification, selection state, and activation
 * for individual category buttons and selection elements within category systems.
 *
 * @see [Category Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryCategoriesWrapperWidgetInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryCategoryWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 * @param CategoryId 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	void SetInventoryCategoryKey(const FString& CategoryId);
	virtual void SetInventoryCategoryKey_Implementation(const FString& CategoryId) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	FString GetInventoryCategoryKey() const;
	virtual FString GetInventoryCategoryKey_Implementation() const = 0;

	/**
	 * 
	 * @param bIsActive 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	void SetActiveState(const bool bIsActive);
	virtual void SetActiveState_Implementation(const bool bIsActive) = 0;
};
