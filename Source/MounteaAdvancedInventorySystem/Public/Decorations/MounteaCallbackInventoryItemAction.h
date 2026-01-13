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
#include "MounteaInventoryItemAction.h"
#include "MounteaCallbackInventoryItemAction.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCallbackInventoryItemAction : public UMounteaInventoryItemAction
{
	GENERATED_BODY()
	
protected:
	
	/**
	 * Gameplay effects to apply when this action is successfully executed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		DisplayName="Action Effects (GAS)")
	TArray<TSoftClassPtr<UGameplayEffect>> ActionEffects;
};
