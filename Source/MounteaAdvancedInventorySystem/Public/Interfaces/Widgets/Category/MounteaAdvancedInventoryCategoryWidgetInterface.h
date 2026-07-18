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

class IMounteaAdvancedInventoryUIManagerInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryWidgetSelected, const FString&, CategoryId);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMounteaCategoryWidgetSelectedBinding, const FString&, CategoryId);

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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Categories")
	void SetInventoryCategoryKey(const FString& CategoryId);
	virtual void SetInventoryCategoryKey_Implementation(const FString& CategoryId) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Categories")
	FString GetInventoryCategoryKey() const;
	virtual FString GetInventoryCategoryKey_Implementation() const = 0;

	/**
	 * 
	 * @param bIsActive 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Categories")
	void SetActiveState(const bool bIsActive);
	virtual void SetActiveState_Implementation(const bool bIsActive) = 0;

	/**
	 * Sets the object this widget will route its OnCategorySelected event through once it fires.
	 * Set once, at widget-creation time, by whoever spawns this widget. The widget itself never
	 * needs to know what kind of object the registrar is (Inventory UI, Crafting UI, or anything else).
	 *
	 * @param Registrar Object implementing IMounteaCategoryWidgetEventRegistrarInterface.
	 * @see IMounteaCategoryWidgetEventRegistrarInterface
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Categories")
	void SetCategoryWidgetEventRegistrar(UObject* Registrar);
	virtual void SetCategoryWidgetEventRegistrar_Implementation(UObject* Registrar) = 0;

	/**
	 * Returns the object previously set via SetCategoryWidgetEventRegistrar.
	 * @return Object implementing IMounteaCategoryWidgetEventRegistrarInterface, or nullptr if unset.
	 * @see IMounteaCategoryWidgetEventRegistrarInterface
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Categories")
	UObject* GetCategoryWidgetEventRegistrar() const;
	virtual UObject* GetCategoryWidgetEventRegistrar_Implementation() const = 0;
};
