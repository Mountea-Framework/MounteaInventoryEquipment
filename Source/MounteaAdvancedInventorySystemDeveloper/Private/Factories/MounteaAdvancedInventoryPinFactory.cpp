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

#if WITH_EDITOR

#include "MounteaAdvancedInventoryPinFactory.h"

#include "Slate/SPinTypeCommandSelector.h"
#include "Slate/SPinTypeEquipmentSlotSelector.h"
#include "Slate/SPinTypeInventoryCategorySelector.h"
#include "Slate/SPinTypeJsonDefinitionSelector.h"
#include "Slate/SPinTypeModalRowSelector.h"
#include "Slate/SPinTypeModalTypeSelector.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphNode.h"
#include "K2Nodes/K2Node_BreakJsonObjectByDefinition.h"
#include "K2Nodes/K2Node_ConstructJsonObjectFromDefinition.h"
#include "K2Nodes/K2Node_MounteaAdvancedInventoryCallFunction.h"

TSharedPtr<SGraphPin> FMounteaInventoryPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (!Pin) return nullptr;

	if (UK2Node_MounteaAdvancedInventoryCallFunction* callFunctionNode = Cast<UK2Node_MounteaAdvancedInventoryCallFunction>(Pin->GetOwningNode()))
	{
		if (callFunctionNode->ShouldUseEquipmentSlotSelector(Pin))
			return SNew(SPinTypeEquipmentSlotSelector, Pin);

		if (callFunctionNode->ShouldUseInventoryCategorySelector(Pin))
			return SNew(SPinTypeInventoryCategorySelector, Pin);

		if (callFunctionNode->ShouldUseJsonDefinitionSelector(Pin))
			return SNew(SPinTypeJsonDefinitionSelector, Pin);

		if (callFunctionNode->ShouldUseCommandSelector(Pin))
			return SNew(SPinTypeCommandSelector, Pin);

		if (callFunctionNode->ShouldUseModalTypeSelector(Pin))
			return SNew(SPinTypeModalTypeSelector, Pin);

		if (callFunctionNode->ShouldUseModalRowSelector(Pin))
			return SNew(SPinTypeModalRowSelector, Pin);
	}

	if (UK2Node_ConstructJsonObjectFromDefinition* constructJsonNode = Cast<UK2Node_ConstructJsonObjectFromDefinition>(Pin->GetOwningNode()))
	{
		if (constructJsonNode->ShouldUseJsonDefinitionSelector(Pin))
			return SNew(SPinTypeJsonDefinitionSelector, Pin);
	}

	if (UK2Node_BreakJsonObjectByDefinition* breakJsonNode = Cast<UK2Node_BreakJsonObjectByDefinition>(Pin->GetOwningNode()))
	{
		if (breakJsonNode->ShouldUseJsonDefinitionSelector(Pin))
			return SNew(SPinTypeJsonDefinitionSelector, Pin);
	}

	return nullptr;
}

#endif
