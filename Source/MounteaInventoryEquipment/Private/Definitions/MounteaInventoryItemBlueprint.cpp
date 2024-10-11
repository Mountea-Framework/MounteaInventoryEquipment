// All rights reserved Dominik Morse (Pavlicek) 2024

#include "Definitions/MounteaInventoryItemBlueprint.h"

#include "Definitions/MounteaItemAction.h"

UMounteaInventoryItemBlueprint::UMounteaInventoryItemBlueprint() : BlueprintGuid(FGuid::NewGuid())
{
}

void UMounteaInventoryItemBlueprint::SetItemActions(const TArray<TSubclassOf<UMounteaInventoryItemAction>>& NewActions)
{
	for (const auto& newAction : NewActions)
	{
		if (newAction && !ItemActions.Contains(newAction))
			ItemActions.Add(newAction);
	}
}