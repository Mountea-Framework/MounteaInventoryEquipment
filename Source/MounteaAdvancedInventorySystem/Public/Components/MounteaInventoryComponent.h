// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "MounteaInventoryComponent.generated.h"


enum class EInventoryFlags : uint8;
enum class EInventoryType : uint8;

/**
 *  Mountea Inventory Component
 * 
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,  AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"), HideCategories=("Cooking","Collision"), meta=(BlueprintSpawnableComponent, DisplayName="Mountea Inventory Component"))
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

protected:
	virtual AActor* GetOwningActor_Implementation() const override;
	virtual bool AddItem_Implementation(const FInventoryItem& Item) override;
	virtual bool AddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1, const float Durability = 1.f) override;
	virtual bool RemoveItem_Implementation(const FGuid& ItemGuid) override;
	virtual bool RemoveItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1) override;
	virtual bool CanAddItem_Implementation(const FInventoryItem& Item) const override;
	virtual bool CanAddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1) const override;
	virtual FInventoryItem FindItem_Implementation(const FInventoryItemSearchParams& SearchParams) const override;
	virtual TArray<FInventoryItem> FindItems_Implementation(const FInventoryItemSearchParams& SearchParams) const override;
	virtual TArray<FInventoryItem> GetAllItems_Implementation() const override;
	virtual bool IncreaseItemQuantity_Implementation(const FGuid& ItemGuid, const int32 Amount = 1) override;
	virtual bool DecreaseItemQuantity_Implementation(const FGuid& ItemGuid, const int32 Amount = 1) override;
	virtual bool ModifyItemDurability_Implementation(const FGuid& ItemGuid, const float DeltaDurability) override;
	virtual void ClearInventory_Implementation() override;
	virtual void ProcessInventoryNotification_Implementation(const FInventoryNotificationData& Notification) override;
	
	// --- Events ------------------------------

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

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	EInventoryType InventoryType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	EInventoryFlags InventoryTypeFlag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_InventoryItems, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	FInventoryItemArray InventoryItems;
};
