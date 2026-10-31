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
#include "UObject/Object.h"

#include "Decorations/MounteaInventoryScriptableAction.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Definitions/MounteaInventoryItem.h"

#include "MounteaInventoryItemAction.generated.h"

/**
 * UMounteaInventoryItemAction provides lightweight inventory actions for local UI operations.
 * Simple actions execute directly without Gameplay Ability System overhead, making them ideal
 * for non-gameplay inventory operations like splitting stacks, moving items, or UI-only modifications
 * with immediate execution and minimal performance impact for inventory management tasks.
 *
 * @see [Simple Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemActions)
 * @see UMounteaInventoryItemAction
 */
UCLASS(ClassGroup=(Mountea), Abstract, NotBlueprintable, BlueprintType, DefaultToInstanced, EditInlineNew,
	AutoExpandCategories=("Mountea","Inventory Action","Mountea|Inventory Action"),
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Mountea Inventory Action"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemAction : public UMounteaInventoryScriptableAction
{
	GENERATED_BODY()

public:
	
	/**
	 * Validates the payload. The owning inventory and the item it references must both resolve.
	 *
	 * @param Payload JSON object with the `ItemAction` definition: owningInventory and itemGuid.
	 *
	 * @return True if the action can be initialized from this payload.
	 */
	virtual bool InitializeInventoryAction_Implementation(const UMounteaJsonObject* Payload) override;

	/**
	 * Gets the inventory interface that owns the target item.
	 * 
	 * @return Script interface to the inventory containing the target item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory_Implementation() const;

	/**
	 * Validates that the target item can be modified by this action.
	 * 
	 * @return True if the item is in a valid state for modification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool CanModifyTargetItem() const;
	virtual bool CanModifyTargetItem_Implementation() const { return true; };
	
protected:
	
	static bool ResolveItemFromPayload(const UMounteaJsonObject* Payload, UObject*& OutInventory, FMounteaInventoryItem& OutItem);
	
public:
	
	virtual bool IsAllowed_Implementation(const UMounteaJsonObject* Payload) const override;
	virtual FText GetDisallowedReason_Implementation(const UMounteaJsonObject* Payload) const override;
	virtual bool ExecuteInventoryAction_Implementation(const UMounteaJsonObject* Payload) override;
};
