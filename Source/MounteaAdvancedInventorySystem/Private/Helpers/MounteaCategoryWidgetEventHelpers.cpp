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

#include "Helpers/MounteaCategoryWidgetEventHelpers.h"

bool FMounteaCategoryWidgetEventHelpers::BindCategoryWidgetSelected(FCategoryWidgetEventRegistry& Registry, UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding)
{
	if (!IsValid(CategoryWidget) || !Binding.IsBound())
		return false;

	PruneStaleEntries(Registry);
	Registry.FindOrAdd(CategoryWidget).AddUnique(Binding);
	return true;
}

bool FMounteaCategoryWidgetEventHelpers::UnbindCategoryWidgetSelected(FCategoryWidgetEventRegistry& Registry, UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding)
{
	if (!IsValid(CategoryWidget) || !Binding.IsBound())
		return false;

	PruneStaleEntries(Registry);

	FOnCategoryWidgetSelected* delegateForWidget = Registry.Find(CategoryWidget);
	if (!delegateForWidget)
		return false;

	delegateForWidget->Remove(Binding);
	if (!delegateForWidget->IsBound())
		Registry.Remove(CategoryWidget);
	return true;
}

bool FMounteaCategoryWidgetEventHelpers::TriggerCategoryWidgetSelected(FCategoryWidgetEventRegistry& Registry, UObject* CategoryWidget, const FString& CategoryId)
{
	if (!IsValid(CategoryWidget))
		return false;

	PruneStaleEntries(Registry);

	FOnCategoryWidgetSelected* delegateForWidget = Registry.Find(CategoryWidget);
	if (!delegateForWidget || !delegateForWidget->IsBound())
		return false;

	delegateForWidget->Broadcast(CategoryId);
	return true;
}

int32 FMounteaCategoryWidgetEventHelpers::PruneStaleEntries(FCategoryWidgetEventRegistry& Registry)
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
