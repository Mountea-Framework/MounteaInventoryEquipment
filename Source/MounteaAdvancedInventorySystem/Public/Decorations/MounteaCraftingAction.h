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
#include "Decorations/MounteaInventoryScriptableAction.h"

#include "MounteaCraftingAction.generated.h"

/**
 * UMounteaCraftingAction is the base of actions performed on crafting recipes.
 *
 * @see UMounteaInventoryScriptableAction
 */
UCLASS(ClassGroup=(Mountea), Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew,
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Mountea Crafting Action"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingAction : public UMounteaInventoryScriptableAction
{
	GENERATED_BODY()
};
