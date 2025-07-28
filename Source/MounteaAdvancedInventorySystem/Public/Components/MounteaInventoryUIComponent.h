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
#include "Components/ActorComponent.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIInterface.h"
#include "MounteaInventoryUIComponent.generated.h"

/**
 * UMounteaInventoryUIComponent manages user interface aspects of inventory systems.
 * UI components handle widget creation, visibility management, item selection, category filtering,
 * notification display, and grid slot persistence for comprehensive inventory interface control.
 *
 * @see [Inventory UI System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventorySystem)
 * @see IMounteaAdvancedInventoryUIInterface
 * @see UMounteaInventoryComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, 
    AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"), HideCategories=("Cooking","Collision"), 
    meta=(BlueprintSpawnableComponent, DisplayName="Mountea Inventory UI Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryUIComponent : public UActorComponent, public IMounteaAdvancedInventoryUIInterface
{
	GENERATED_BODY()

public:

	UMounteaInventoryUIComponent();

protected:
	
	virtual void BeginPlay() override;

	// --- Interface Functions ------------------------------
	
public:
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const override;
	virtual void SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) override;
	
	virtual bool CreateMainUIWrapper_Implementation() override;
	virtual ESlateVisibility GetMainUIVisibility_Implementation() const override;
	virtual void SetMainUIVisibility_Implementation(const ESlateVisibility NewVisibility) override;	
	virtual UCommonActivatableWidget* GetMainUIWrapper_Implementation() const override { return InventoryWidget; };
	virtual void RemoveMainUIWrapper_Implementation() override;

	virtual UUserWidget* GetActiveItemWidget_Implementation() const override { return ActiveItemWidget; };
	virtual void SetActiveItemWidget_Implementation(UUserWidget* NewActiveItemWidget) override;
	
	virtual UUserWidget* GetNotificationContainer_Implementation() const override;
	virtual void SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer) override;
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) override;
	virtual void RemoveInventoryNotifications_Implementation() override;

	virtual void ProcessItemAdded_Implementation(const FInventoryItem& AddedItem) override;
	virtual void ProcessItemModified_Implementation(const FInventoryItem& ModifiedItem) override;
	virtual void ProcessItemRemoved_Implementation(const FInventoryItem& RemovedItem) override;

	virtual void CategorySelected_Implementation(const FString& SelectedCategoryId) override;
	virtual FString GetSelectedCategoryId_Implementation() const override { return ActiveCategoryId; };
	virtual void ItemSelected_Implementation(const FGuid& SelectedItem) override;
	virtual FGuid GetActiveItemGuid_Implementation() const override { return ActiveItemGuid; };

	virtual TSet<FMounteaInventoryGridSlot> GetSavedSlots_Implementation() const override {return SavedGridSlots;};
	virtual void AddSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) override;
	virtual void RemoveSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) override;
	virtual void AddSlots_Implementation(const TSet<FMounteaInventoryGridSlot>& SlotData) override;
	virtual void RemoveSlots_Implementation(const TSet<FMounteaInventoryGridSlot>& SlotData) override;
	virtual void UpdateSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) override;

	virtual FInventoryCategorySelected& GetOnCategorySelectedHandle() override
	{ return OnCategorySelected; };
	virtual FInventoryItemSelected& GetOnItemSelectedHandle() override
	{ return OnItemSelected; };

protected:

	UFUNCTION()
	void ProcessItemDurabilityChanged(const FInventoryItem& Item, const float OldDurability, const float NewDurability);
	UFUNCTION()
	void ProcessItemQuantityChanged(const FInventoryItem& Item, const int32 OldQuantity, const int32 NewQuantity);

protected:
	
	/**
	 * Delegate that is broadcast when an inventory category is selected.
	 *
	* This BlueprintAssignable delegate allows other components to handle the logic
	 * for Category selection in the inventory.
	 * It broadcasts FSTRING CategoryId.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FInventoryCategorySelected OnCategorySelected;

	/**
	 * Delegate triggered when an inventory item is selected.
	 *
	 * This BlueprintAssignable delegate allows other components to handle the logic
	 * for Item selection in the inventory.
	 * It broadcasts FGUID ItemId.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FInventoryItemSelected OnItemSelected;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	FString ActiveCategoryId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	FGuid ActiveItemGuid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	TObjectPtr<UUserWidget> ActiveItemWidget;

	/**
	 * Represents the set of saved inventory grid slots.
	 *
	 * This variable stores a collection of FMounteaInventoryGridSlot data structures,
	 * which define the slots in the inventory grid system. It is used for managing
	 * and persisting slot configurations across game sessions.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	TSet<FMounteaInventoryGridSlot> SavedGridSlots;
	
private:
	
	/**
	 * Represents the parent inventory interface.
	 *
	 * This variable serves as a reference to the parent inventory system, allowing interaction with the advanced inventory interface.
	 * It enables communication between the inventory UI component and the parent inventory for functions such as handling item modifications,
	 * notifications, and other inventory-related operations.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess), meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaAdvancedInventoryInterface> ParentInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UCommonActivatableWidget> InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryNotificationContainerWidget;
};
