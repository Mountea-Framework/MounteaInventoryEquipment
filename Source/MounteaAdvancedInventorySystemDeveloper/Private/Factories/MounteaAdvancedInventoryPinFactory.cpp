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
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphNode.h"
#include "K2Nodes/K2Node_MounteaAdvancedInventoryCallFunction.h"

TSharedPtr<SGraphPin> FMounteaInventoryPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (!Pin) return nullptr;
    
	if (UK2Node_MounteaAdvancedInventoryCallFunction* callFunctionNode = Cast<UK2Node_MounteaAdvancedInventoryCallFunction>(Pin->GetOwningNode()))
	{
		if (callFunctionNode->ShouldUseCommandSelector(Pin))
		{
			return SNew(SPinTypeCommandSelector, Pin);
		}
	}
    
	return nullptr;
}

#endif