// All rights reserved Dominik Morse 2024


#include "Decorations/MounteaInventoryItemAction.h"

#include "Definitions/MounteaInventoryItem.h"

bool UMounteaInventoryItemAction::CanExecuteAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem) const
{
	if (!IsValid(ActionInitiator))
		return false;

	return TargetItem.IsItemValid();
}

void UMounteaInventoryItemAction::ExecuteAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem)
{
	Initiator = ActionInitiator;
	ItemGuid = TargetItem.GetGuid();
}

void UMounteaInventoryItemAction::FinishAction_Implementation()
{
	Initiator = nullptr;
	ItemGuid.Invalidate();
}
