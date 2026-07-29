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
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryCraftingRecipeIngredientsWrapperWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryCraftingRecipeIngredientsWrapperWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Marker interface for crafting recipe ingredients wrapper widgets.
 * Ingredients wrappers contain every ingredient widget for one selected recipe ingredient option/list.
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryCraftingRecipeIngredientsWrapperWidgetInterface
{
	GENERATED_BODY()

public:
};
