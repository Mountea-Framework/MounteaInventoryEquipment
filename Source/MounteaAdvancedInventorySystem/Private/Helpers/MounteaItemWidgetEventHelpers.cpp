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

#include "Helpers/MounteaItemWidgetEventHelpers.h"

bool FMounteaItemWidgetEventHelpers::BindItemWidgetSelected(FItemWidgetEventRegistry& Registry, UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding)
{
	if (!IsValid(ItemWidget) || !Binding.IsBound())
		return false;

	PruneStaleEntries(Registry);
	Registry.FindOrAdd(ItemWidget).AddUnique(Binding);
	return true;
}

bool FMounteaItemWidgetEventHelpers::UnbindItemWidgetSelected(FItemWidgetEventRegistry& Registry, UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding)
{
	if (!IsValid(ItemWidget) || !Binding.IsBound())
		return false;

	PruneStaleEntries(Registry);

	FOnItemWidgetSelected* delegateForWidget = Registry.Find(ItemWidget);
	if (!delegateForWidget)
		return false;

	delegateForWidget->Remove(Binding);
	if (!delegateForWidget->IsBound())
		Registry.Remove(ItemWidget);
	return true;
}

bool FMounteaItemWidgetEventHelpers::TriggerItemWidgetSelected(FItemWidgetEventRegistry& Registry, UObject* ItemWidget, const FGuid& ItemId)
{
	if (!IsValid(ItemWidget))
		return false;

	PruneStaleEntries(Registry);

	FOnItemWidgetSelected* delegateForWidget = Registry.Find(ItemWidget);
	if (!delegateForWidget || !delegateForWidget->IsBound())
		return false;

	delegateForWidget->Broadcast(ItemId);
	return true;
}

int32 FMounteaItemWidgetEventHelpers::PruneStaleEntries(FItemWidgetEventRegistry& Registry)
{
	TArray<TWeakObjectPtr<UObject>> staleKeys;
	for (const auto& pair : Registry)
	{
		if (!pair.Key.IsValid())
			staleKeys.Add(pair.Key);
	}
	for (const auto& key : staleKeys)
		Registry.Remove(key);
	return staleKeys.Num();
}
