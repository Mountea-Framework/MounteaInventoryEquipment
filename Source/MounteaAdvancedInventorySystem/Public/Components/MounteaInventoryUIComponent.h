// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIInterface.h"
#include "MounteaInventoryUIComponent.generated.h"

/**
 *  Mountea Inventory UI Component
 * 
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,  AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"), HideCategories=("Cooking","Collision"), meta=(BlueprintSpawnableComponent, DisplayName="Mountea Inventory UI Component"))
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
	virtual bool CreateInventoryUIWrapper_Implementation() override;
	virtual UUserWidget* GetInventoryUIWrapper_Implementation() const override { return InventoryWidget; };
	virtual void RemoveInventoryUIWrapper_Implementation() override;
	virtual void SetInventoryUIWrapperVisibility_Implementation(const bool bShowInventory) override;
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

	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FInventoryCategorySelected OnCategorySelected;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FInventoryItemSelected OnItemSelected;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	FString ActiveCategoryId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	FGuid ActiveItemGuid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory")
	TSet<FMounteaInventoryGridSlot> SavedGridSlots;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess), meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaAdvancedInventoryInterface> ParentInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryNotificationContainerWidget;
};
