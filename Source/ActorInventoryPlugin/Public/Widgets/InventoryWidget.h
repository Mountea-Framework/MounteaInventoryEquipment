// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/InventoryHelpers.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryItem;
class IActorInventoryInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryWidgetUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryUpdated, const FGuid&, CategoryGUID);

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryWidget final : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual bool Initialize() override;

public:
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnCategoryUpdated OnCategoryUpdated;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory")
	void InitializeInventoryWidget(TScriptInterface<IActorInventoryInterface>& SourceInventory);

	/**
	 * @brief Refreshes Inventory Widget. No C++ logic is implemented yet.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Inventory")
	void RefreshInventoryWidget();

	/**
	 * @brief Save Inventory Slot to Owning Inventory. Must be called with Parent Call!
	 * @param InventorySlot Slot to be Saved.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory")
	void SaveInventorySlot(const struct FInventorySlotData& InventorySlot);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetSelectedCategory(const FGuid& CategoryGUID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void OnCategorySelected(const FGuid CategoryGUID);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Inventory")
	void OnItemInspected(class UInventoryItemSlot* InspectedItem);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	TScriptInterface<IActorInventoryInterface>& GetOwningInventory () {return OwningInventory; };

protected:

	/**
	 * @brief Owning Inventory Interface.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TScriptInterface<IActorInventoryInterface> OwningInventory = nullptr;

	/**
	 * @brief Unique Guid of Selected Category.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FGuid SelectedCategory = FGuid();
};
