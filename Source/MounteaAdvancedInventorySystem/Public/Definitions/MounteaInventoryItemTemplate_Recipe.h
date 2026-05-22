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
#include "MounteaInventoryItemTemplate.h"
#include "MounteaInventoryItemTemplate_Recipe.generated.h"

class UMounteaRecipeTemplate;
/**
 * Represents an inventory item template specifically designed for recipes.
 *
 * This class serves as a specialized template for inventory items that function as recipes
 * within the Mountea Advanced Inventory System. It inherits all base functionality from
 * UMounteaInventoryItemTemplate and is used to define specific properties or logic associated
 * with recipe-type items.
 * 
 * @see [Item Templates](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemTemplates)
 * @see FInventoryItem
 * @see UMounteaInventoryManagerComponent
 * @see UMounteaCraftingParticipantComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, 
	meta=(DisplayName = "Inventory Item Template (Recipe)"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemTemplate_Recipe : public UMounteaInventoryItemTemplate
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Specifies a recipe granted by this inventory item template.
	 *
	 * Represents a soft object reference to a UMounteaRecipeTemplate that defines a recipe
	 * associated with this inventory item. This property allows linking a specific recipe
	 * to the inventory item, enabling integration of crafting or recipe-based systems.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting",
		meta=(NoResetToDefault))
	TSoftObjectPtr<UMounteaRecipeTemplate> GrantedRecipe;

	/**
	 * Indicates whether the associated recipe is granted temporarily.
	 * The grant shall be bound to existence of the Item in Inventory, therefore
	 * dropping/removing the ItemTemplate from inventory would remove the recipe
	 * from know recipes, too.
	 * 
	 * This is very niche feature so by default is false.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting",
		meta=(NoResetToDefault))
	uint8 bTemporaryGrant : 1;
};
