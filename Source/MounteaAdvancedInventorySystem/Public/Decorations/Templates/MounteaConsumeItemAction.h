// All rights reserved Dominik Morse 2024

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