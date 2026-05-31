// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Statics/MounteaInventoryModalStatics.h"

#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalContentWidgetInterface.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalWidgetInterface.h"

void UMounteaInventoryModalStatics::ConstructModalsContent(
	UObject* Target,
	UDataTable* DataTable,
	UObject* OptionalPayload,
	const FString& Key)
{
	if (!IsValid(Target))
		return;

	if (Target->Implements<UMounteaAdvancedInventoryModalWidgetInterface>())
		IMounteaAdvancedInventoryModalWidgetInterface::Execute_ConstructModalsContent(Target, DataTable, OptionalPayload, Key);
}

void UMounteaInventoryModalStatics::ConstructModalContent(
	UObject* Target,
	UDataTable* DataTable,
	UObject* OptionalPayload,
	const FString& Key)
{
	if (!IsValid(Target))
		return;

	if (Target->Implements<UMounteaAdvancedInventoryModalContentWidgetInterface>())
		IMounteaAdvancedInventoryModalContentWidgetInterface::Execute_ConstructContent(Target, DataTable, OptionalPayload, Key);
}
