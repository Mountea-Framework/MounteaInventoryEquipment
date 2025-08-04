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
#include "Definitions/MounteaInventoryItem.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "MounteaInventoryComponent.generated.h"

enum class EInventoryNotificationType : uint8;

enum class EInventoryFlags : uint8;
enum class EInventoryType : uint8;

/**
 * UMounteaInventoryComponent manages inventory item collections with network replication support.
 * Inventory components provide item storage, modification, searching, and notification systems
 * with efficient delta replication for multiplayer inventory management.
 *
 * @see [Inventory System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventorySystem)
 * @see IMounteaAdvancedInventoryInterface
 * @see FInventoryItem
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
    AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"),
    HideCategories=("Cooking","Collision"),
    meta=(BlueprintSpawnableComponent, DisplayName="Mountea Inventory Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryComponent : public UActorComponent, public IMounteaAdvancedInventoryInterface
{
	GENERATED_BODY()

public:

	UMounteaInventoryComponent();

protected:

	virtual void BeginPlay() override;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_InventoryItems();

	// --- Interface Functions ------------------------------
protected:
	virtual AActor* GetOwningActor_Implementation() const override;
	virtual bool AddItem_Implementation(const FInventoryItem& Item) override;
	virtual bool AddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1, const float Durability = 1.f) override;
	virtual bool RemoveItem_Implementation(const FGuid& ItemGuid) override;
	virtual bool RemoveItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1) override;
	virtual bool CanAddItem_Implementation(const FInventoryItem& Item) const override;
	virtual bool CanAddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1) const override;
	virtual FInventoryItem FindItem_Implementation(const FInventoryItemSearchParams& SearchParams) const override;
	virtual int32 FindItemIndex_Implementation(const FInventoryItemSearchParams& SearchParams) const override;
	virtual TArray<FInventoryItem> FindItems_Implementation(const FInventoryItemSearchParams& SearchParams) const override;
	virtual TArray<FInventoryItem> GetAllItems_Implementation() const override;
	virtual bool IncreaseItemQuantity_Implementation(const FGuid& ItemGuid, const int32 Amount = 1) override;
	virtual bool DecreaseItemQuantity_Implementation(const FGuid& ItemGuid, const int32 Amount = 1) override;
	virtual bool ModifyItemDurability_Implementation(const FGuid& ItemGuid, const float DeltaDurability) override;
	virtual void ClearInventory_Implementation() override;
	virtual bool HasItem_Implementation(const FInventoryItemSearchParams& SearchParams) const override;
	virtual void ProcessInventoryNotification_Implementation(const FInventoryNotificationData& Notification) override;

	// --- Class Functions ------------------------------
protected:
	bool IsAuthority() const;
	
	UFUNCTION(Server, Reliable)
	void AddItem_Server(const FInventoryItem& Item);
	UFUNCTION(Server, Reliable)
	void RemoveItem_Server(const FGuid& ItemGuid);
	UFUNCTION(Server, Reliable)
	void ChangeItemQuantity_Server(const FGuid& ItemGuid, const int32 DeltaAmount);

	UFUNCTION(Client, Unreliable)
	void ProcessInventoryNotification_Client(const FGuid& TargetItem, const FString& NotifType, const int32 QuantityDelta);
	UFUNCTION(Client, Unreliable)
	void PostItemAdded_Client(const FInventoryItem& Item);
	UFUNCTION(Client, Unreliable)
	void PostItemRemoved_Client(const FInventoryItem& Item);
	UFUNCTION(Client, Unreliable)
	void PostItemQuantityChanged(const FInventoryItem& Item, const int32 OldQuantity, const int32 NewQuantity);
	UFUNCTION(Client, Unreliable)
	void PostItemDurabilityChanged(const FInventoryItem& Item, const int32 OldDurability, const int32 NewDurability);

	// --- Events ------------------------------
protected:
	virtual FOnItemAdded& GetOnItemAddedEventHandle() override
	{ return OnItemAdded; };
	virtual FOnItemRemoved& GetOnItemRemovedEventHandle() override
	{ return OnItemRemoved; };
	virtual FOnItemQuantityChanged& GetOnItemQuantityChangedEventHandle() override
	{ return OnItemQuantityChanged; };
	virtual FOnItemDurabilityChanged& GetOnItemDurabilityChangedEventHandle() override
	{ return OnItemDurabilityChanged; };
	virtual FOnNotificationProcessed& GetOnNotificationProcessedEventHandle() override
	{ return OnNotificationProcessed; };

protected:
	
	/**
	* Called when item is added to inventory
	*/
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory & Equipment|Inventory|Events")
	FOnItemAdded OnItemAdded;

	/**
	* Called when item is removed from inventory
	*/
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory & Equipment|Inventory|Events")
	FOnItemRemoved OnItemRemoved;

	/**
	* Called when item quantity changes
	*/
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory & Equipment|Inventory|Events")
	FOnItemQuantityChanged OnItemQuantityChanged;

	/**
	* Called when item durability changes
	*/
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory & Equipment|Inventory|Events")
	FOnItemDurabilityChanged OnItemDurabilityChanged;

	/**
	* Called when inventory Notification is processed
	*/
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory & Equipment|Inventory|Events")
	FOnNotificationProcessed OnNotificationProcessed;

private:

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Mountea|Inventory",
		meta=(AllowPrivateAccess))
	EInventoryType InventoryType;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Mountea|Inventory",
		meta=(AllowPrivateAccess))
	EInventoryFlags InventoryTypeFlag;
	
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly,
		ReplicatedUsing=OnRep_InventoryItems, Category="Mountea|Inventory",
		meta=(AllowPrivateAccess))
	FInventoryItemArray InventoryItems;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Mountea|Inventory",
		meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryWidget;
};
