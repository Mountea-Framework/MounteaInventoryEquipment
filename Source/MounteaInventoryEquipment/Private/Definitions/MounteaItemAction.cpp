// All rights reserved Dominik Pavlicek 2023.


#include "Definitions/MounteaItemAction.h"

#include "Definitions/MounteaInventoryItem.h"

void UMounteaInventoryItemAction::InitializeAction_Implementation(UMounteaInventoryItemBase* ItemInFocus, FMounteaDynamicDelegateContext Context)
{
	OwningItem = ItemInFocus;

	if (OwningItem && World == nullptr)
	{
		SetWorld(OwningItem->GetWorld());
	}

	ActionContext = Context;
}

bool UMounteaInventoryItemAction::DisplayAction_Implementation(UMounteaInventoryItemBase* ItemInFocus) const
{
	return GetWorld() != nullptr;
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

void UMounteaInventoryItemAction::CopyFromOther(const UMounteaInventoryItemAction* OtherAction)
{
	if (!OtherAction) return;
	
	ActionIcon = OtherAction->ActionIcon;
	ActionName = OtherAction->ActionName;
	ActionTag = OtherAction->ActionTag;
	ActionContext = OtherAction->ActionContext;

	World = OtherAction->GetWorld();
	OwningItem = OtherAction->GetOwningItem();
}

