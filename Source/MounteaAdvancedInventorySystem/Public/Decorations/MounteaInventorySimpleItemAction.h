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
#include "Definitions/MounteaInventoryItem.h"
#include "Interfaces/ItemActions/MounteaAdvancedInventoryItemActionInterface.h"
#include "UObject/Object.h"
#include "MounteaInventorySimpleItemAction.generated.h"

class UUserWidget;

/**
 * UMounteaInventorySimpleItemAction provides lightweight inventory actions for local UI operations.
 * Simple actions execute directly without Gameplay Ability System overhead, making them ideal
 * for non-gameplay inventory operations like splitting stacks, moving items, or UI-only modifications
 * with immediate execution and minimal performance impact for inventory management tasks.
 *
 * @see [Simple Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/SimpleActions)
 * @see IMounteaAdvancedInventoryItemActionInterface
 * @see UMounteaInventoryItemAction
 */
UCLASS(ClassGroup=(Mountea), Abstract, BlueprintType, Blueprintable,
	AutoExpandCategories=("Mountea","Inventory Action","Mountea|Inventory Action"),
	HideCategories=("Cooking","Collision"),
	meta=(DisplayName="Mountea Simple Inventory Action"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventorySimpleItemAction : public UObject, public IMounteaAdvancedInventoryItemActionInterface
{
	GENERATED_BODY()

#pragma region Inventory Specific
protected:

	/** Data of the Inventory Action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action",
		meta=(ShowOnlyInnerProperties))
	FMounteaItemActionData ItemActionData;

#pragma endregion

#pragma region IMounteaAdvancedInventoryItemActionInterface
public:
	
	virtual bool InitializeItemAction_Implementation(const FInventoryItem& NewTargetItem,
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewOwningInventory,
		UObject* ContextPayload = nullptr) override;
	virtual FInventoryItem GetTargetItem_Implementation() const override
	{ return CurrentTargetItem; };
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory_Implementation() const override;
	virtual FMounteaItemActionData GetActionData_Implementation() const override
	{ return ItemActionData; };
	virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const override;
	virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem) override;
	virtual FGameplayTag GetInventoryItemTag_Implementation() const override
	{ return ItemActionData.ItemActionTag; };
	virtual bool ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem) override;
	virtual bool CanModifyTargetItem_Implementation() const override
	{ return true; };
	virtual void AddActionFlag_Implementation(const EInventoryItemActionCallback FlagToAdd) override;
	virtual void RemoveActionFlag_Implementation(const EInventoryItemActionCallback FlagToRemove) override;
	virtual void ClearAllActionFlags_Implementation() override;
	virtual EInventoryItemActionCallback GetInventoryItemActionCallback_Implementation() const override;

#pragma endregion

#pragma region Private Data
private:

	/**
	 * The parent widget that owns this item action.
	 * Used for UI context and interaction.
	 */
	UPROPERTY(Transient, BlueprintReadOnly, meta=(ExposeOnSpawn, AllowPrivateAccess))
	TObjectPtr<UUserWidget> ParentItemWidget;
	
	/**
	 * The inventory item currently being processed by this action.
	 */
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FInventoryItem CurrentTargetItem;
	
#pragma endregion
};
