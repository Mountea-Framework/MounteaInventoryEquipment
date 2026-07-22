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

#include "Statics/MounteaItemWidgetStatics.h"

#include "Interfaces/Widgets/Items/MounteaItemWidgetEventRegistrarInterface.h"

UObject* UMounteaItemWidgetStatics::ResolveRegistrar(const UObject* ItemWidget)
{
	if (!IsValid(ItemWidget) || !ItemWidget->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		return nullptr;

	UObject* registrar = IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetItemWidgetEventRegistrar(ItemWidget);
	return (IsValid(registrar) && registrar->Implements<UMounteaItemWidgetEventRegistrarInterface>()) ? registrar : nullptr;
}

void UMounteaItemWidgetStatics::SetItemWidgetEventRegistrar(UObject* ItemWidget, UObject* Registrar)
{
	if (IsValid(ItemWidget) && ItemWidget->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SetItemWidgetEventRegistrar(ItemWidget, Registrar);
}

UObject* UMounteaItemWidgetStatics::GetItemWidgetEventRegistrar(UObject* ItemWidget)
{
	if (!IsValid(ItemWidget) || !ItemWidget->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		return nullptr;

	return IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetItemWidgetEventRegistrar(ItemWidget);
}

bool UMounteaItemWidgetStatics::BindToOnItemWidgetSelected(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding)
{
	UObject* registrar = ResolveRegistrar(ItemWidget);
	return registrar ? IMounteaItemWidgetEventRegistrarInterface::Execute_BindItemWidgetSelected(registrar, ItemWidget, Binding) : false;
}

bool UMounteaItemWidgetStatics::UnbindFromOnItemWidgetSelected(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding)
{
	UObject* registrar = ResolveRegistrar(ItemWidget);
	return registrar ? IMounteaItemWidgetEventRegistrarInterface::Execute_UnbindItemWidgetSelected(registrar, ItemWidget, Binding) : false;
}

bool UMounteaItemWidgetStatics::TriggerOnItemWidgetSelected(UObject* ItemWidget)
{
	UObject* registrar = ResolveRegistrar(ItemWidget);
	if (!registrar)
		return false;

	const FGuid itemId = IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetInventoryItemId(ItemWidget);
	return IMounteaItemWidgetEventRegistrarInterface::Execute_TriggerItemWidgetSelected(registrar, ItemWidget, itemId);
}

void UMounteaItemWidgetStatics::SelectItem(UObject* ItemWidget)
{
	if (IsValid(ItemWidget) && ItemWidget->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		IMounteaAdvancedInventoryItemWidgetInterface::Execute_SelectItem(ItemWidget);
}
