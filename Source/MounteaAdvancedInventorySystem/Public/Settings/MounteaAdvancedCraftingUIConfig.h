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
#include "Engine/DataAsset.h"
#include "MounteaAdvancedCraftingUIConfig.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Crafting UI Settings Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining crafting UI."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedCraftingUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "User Interface",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingUserInterfaceClass;
};
