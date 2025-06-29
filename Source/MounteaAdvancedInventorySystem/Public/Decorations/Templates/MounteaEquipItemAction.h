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
#include "MounteaEquipItemAction.generated.h"

class IMounteaAdvancedEquipmentInterface;

/**
 * Action for equipping inventory items.
 * Spawns the item's actor and attaches it to the equipment container.
 * Only visible for items without the Consumable flag.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Equip Item Action")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaEquipItemAction : public UMounteaInventoryItemAction
{
	GENERATED_BODY()

public:

	UMounteaEquipItemAction();

protected:

	virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem) override;
	virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const override;

private:

	class TScriptInterface<IMounteaAdvancedEquipmentInterface> GetEquipmentInterface(UObject* ActionInitiator) const;
};