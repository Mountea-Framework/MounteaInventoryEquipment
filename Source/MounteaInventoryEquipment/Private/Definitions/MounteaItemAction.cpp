// All rights reserved Dominik Morse (Pavlicek) 2024


#include "Definitions/MounteaItemAction.h"

#include "Definitions/MounteaInventoryItem.h"

void UMounteaInventoryItemAction::InitializeAction_Implementation(UMounteaInventoryItem* ItemInFocus, FMounteaInventoryCommandContext Context)
{
	OwningItem = ItemInFocus;

	if (OwningItem && World == nullptr)
	{
		SetWorld(OwningItem->GetWorld());
	}

	ActionContext = Context;
}

bool UMounteaInventoryItemAction::DisplayAction_Implementation(UMounteaInventoryItem* ItemInFocus) const
{
	return IsValid(GetWorld()) && IsValid(OwningItem);
}

void UMounteaInventoryItemAction::SetWorldFromLevel(ULevel* FromLevel)
{
	if (FromLevel == nullptr)
	{
		return;
	}
	
	if (FromLevel->GetWorld() != World)
	{
		SetWorld(FromLevel->GetWorld());
	}
}

void UMounteaInventoryItemAction::SetWorld(UWorld* NewWorld)
{
	World = NewWorld;
}

