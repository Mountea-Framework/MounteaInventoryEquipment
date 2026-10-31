// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Decorations/MounteaInventoryScriptableAction.h"

bool UMounteaInventoryScriptableAction::InitializeInventoryAction_Implementation(const UMounteaJsonObject* Payload)
{
	return false;
}

bool UMounteaInventoryScriptableAction::IsAllowed_Implementation(const UMounteaJsonObject* Payload) const
{
	return false;
}

FText UMounteaInventoryScriptableAction::GetDisallowedReason_Implementation(const UMounteaJsonObject* Payload) const
{
	return FText::FromString(TEXT("Reason not implemented"));
}

bool UMounteaInventoryScriptableAction::ExecuteInventoryAction_Implementation(const UMounteaJsonObject* Payload)
{
	return false;
}

void UMounteaInventoryScriptableAction::CancelInventoryAction_Implementation()
{
	CleanupInventoryAction();
}
