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
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIManagerInterface.h"
#include "MounteaInventoryUIComponent.generated.h"

/**
 * UMounteaInventoryUIComponent manages user interface aspects of inventory systems.
 * UI components handle widget creation, visibility management, item selection, category filtering,
 * notification display, and grid slot persistence for comprehensive inventory interface control.
 *
 * @see [Inventory UI System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventorySystem)
 * @see IMounteaAdvancedInventoryUIManagerInterface
 * @see UMounteaInventoryComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, 
    AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"), HideCategories=("Cooking","Collision"), 
    meta=(BlueprintSpawnableComponent, DisplayName="Mountea Inventory UI Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryUIComponent : public UActorComponent, public IMounteaAdvancedInventoryUIManagerInterface
{
	GENERATED_BODY()

public:

	UMounteaInventoryUIComponent();

protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// --- Interface Functions ------------------------------
	
public:
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const override;
	virtual void SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) override;
	
	virtual bool CreateWrapperWidget_Implementation() override;
	virtual UUserWidget* GetWrapperWidget_Implementation() const override { return WrapperWidget; };
	virtual void RemoveWrapperWidget_Implementation() override;
	
	virtual bool CreateInventoryWidget_Implementation() override;
	virtual UUserWidget* GetInventoryWidget_Implementation() const override { return InventoryWidget; };
	virtual void RemoveInventoryWidget_Implementation() override;
	virtual bool SetInventoryWidget_Implementation(UUserWidget* NewInventoryWidget) override;

	virtual UWidget* GetActiveItemWidget_Implementation() const override { return ActiveItemWidget; };
	virtual void SetActiveItemWidget_Implementation(UWidget* NewActiveItemWidget) override;
	
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
	
	virtual TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap_Implementation() const override { return CustomItemsMap; };
	virtual void AddCustomItemToMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) override;
	virtual void AppendCustomItemsMap_Implementation(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems) override { CustomItemsMap.Append(OtherItems); };
	virtual void ClearCustomItemsMap_Implementation() override { CustomItemsMap.Reset(); };
	virtual bool RemoveCustomItemFromMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) override;
	virtual bool IsItemStoredInCustomMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) override;
	
	virtual void ExecuteWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload) override;
	
	virtual TArray<UMounteaSelectableInventoryItemAction*> GetItemActionsQueue_Implementation() const override;
	virtual bool EnqueueItemAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload) override;
	virtual void EmptyItemActionsQueue_Implementation() override;
	virtual void CompleteQueuedAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload) override;
	virtual void CancelQueuedAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction) override;

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
	 * It broadcasts CategoryId.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FInventoryCategorySelected OnCategorySelected;

	/**
	 * Delegate triggered when an inventory item is selected.
	 *
	 * This BlueprintAssignable delegate allows other components to handle the logic
	 * for Item selection in the inventory.
	 * It broadcasts ItemId.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FInventoryItemSelected OnItemSelected;

protected:
	
	// Currently active category in UI.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	FString ActiveCategoryId;

	// Currently active item in UI.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	FGuid ActiveItemGuid;

	// Currently active item Widget.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	TObjectPtr<UWidget> ActiveItemWidget;
	
	// Custom stored map, can be used to store unique Items, like Coins, Favourites etc.
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	TMap<FGameplayTag, FInventoryUICustomData> CustomItemsMap;

	FActionsQueue ActionsQueue;
	
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
	TObjectPtr<UUserWidget> WrapperWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryNotificationContainerWidget;
};
