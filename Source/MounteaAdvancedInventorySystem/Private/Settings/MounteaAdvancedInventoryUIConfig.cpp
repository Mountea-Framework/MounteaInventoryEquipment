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


#include "Settings/MounteaAdvancedInventoryUIConfig.h"

#include "Definitions/MounteaInventoryBaseCommands.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"

UMounteaAdvancedInventoryUIConfig::UMounteaAdvancedInventoryUIConfig() : Super()
	, bAlwaysStackStackableItems(true)
	, bAllowDragAndDrop(true)
	, bAllowAutoFocus(true)
{
	SetupWidgetCommands();
}

void UMounteaAdvancedInventoryUIConfig::SetupWidgetCommands()
{
	auto coreCommands = InventoryUICommands::GetAllCommandTypes();
	for (const FString& command : InventoryUICommands::AllCommands)
	{
		if (!WidgetCommands.Contains(command))
			WidgetCommands.Add(command);
	}
}

#if WITH_EDITOR

void UMounteaAdvancedInventoryUIConfig::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventoryUIConfig, WidgetCommands))
	{
		SetupWidgetCommands();
	}
}

#endif
