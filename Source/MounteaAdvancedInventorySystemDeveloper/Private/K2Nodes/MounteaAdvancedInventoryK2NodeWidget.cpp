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

#include "K2Nodes/MounteaAdvancedInventoryK2NodeWidget.h"

#include "K2Node.h"
#include "KismetNodes/SGraphNodeK2Base.h"
#include "NodeFactory.h"
#include "Styling/MounteaAdvancedInventoryDeveloperStyle.h"

namespace
{
	bool bIsCreatingWidget = false;

	// SGraphNodeK2Base::Style is protected and some engine node widgets (e.g. the switch one) are private,
	// so the style is assigned through a member pointer formed in a derived scope
	struct FK2NodeStyleAccess : public SGraphNodeK2Base
	{
		static TSharedPtr<ISlateStyle> SGraphNodeK2Base::* StyleMember()
		{
			return &FK2NodeStyleAccess::Style;
		}
	};
}

TSharedPtr<SGraphNode> MounteaAdvancedInventoryK2NodeWidget::Create(UK2Node* Node)
{
	// Re-entered from FNodeFactory below - return nothing so the factory picks its default widget
	if (bIsCreatingWidget)
		return nullptr;

	TSharedPtr<SGraphNode> widget;
	{
		TGuardValue<bool> creatingGuard(bIsCreatingWidget, true);
		widget = FNodeFactory::CreateNodeWidget(Node);
	}

	// Only widgets known to derive from SGraphNodeK2Base
	static const FName defaultWidgetType(TEXT("SGraphNodeK2Default"));
	static const FName switchWidgetType(TEXT("SGraphNodeSwitchStatement"));
	if (widget.IsValid() && (widget->GetType() == defaultWidgetType || widget->GetType() == switchWidgetType))
		static_cast<SGraphNodeK2Base*>(widget.Get())->*FK2NodeStyleAccess::StyleMember() = FMounteaAdvancedInventoryDeveloperStyle::GetStylePtr();

	return widget;
}
