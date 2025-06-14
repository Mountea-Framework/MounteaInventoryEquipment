// All rights reserved Dominik Morse 2024

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