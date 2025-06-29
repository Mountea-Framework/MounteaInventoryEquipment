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
#include "Decorations/MounteaInventoryItemAction.h"
#include "MounteaConsumeItemAction.generated.h"

/**
 * Action for consuming inventory items.
 * Reduces item quantity by 1 and applies configured gameplay effects.
 * Only visible for items with the Consumable flag.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Consume Item Action")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaConsumeItemAction : public UMounteaInventoryItemAction
{
	GENERATED_BODY()

public:

	UMounteaConsumeItemAction();

protected:

	virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem) override;
	virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const override;
};