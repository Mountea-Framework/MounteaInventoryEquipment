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

#pragma once

#include "CoreMinimal.h"

class SGraphNode;
class UK2Node;

namespace MounteaAdvancedInventoryK2NodeWidget
{
	/**
	 * Creates the widget the engine would use for this node and makes it resolve its brushes (corner icon)
	 * from the Mountea developer style instead of the global FAppStyle.
	 * Call from UEdGraphNode::CreateVisualWidget.
	 */
	TSharedPtr<SGraphNode> Create(UK2Node* Node);
}
