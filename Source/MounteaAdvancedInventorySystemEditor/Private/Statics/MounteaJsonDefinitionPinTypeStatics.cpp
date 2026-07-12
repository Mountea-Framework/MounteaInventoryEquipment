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

#include "Statics/MounteaJsonDefinitionPinTypeStatics.h"

#include "EdGraphSchema_K2.h"
#include "Statics/MounteaAdvancedInventoryJsonStatics.h"

namespace MounteaJsonDefinitionPinTypeStatics
{
	FEdGraphPinType MakeDefaultFieldPinType()
	{
		FEdGraphPinType pinType;
		pinType.PinCategory = UEdGraphSchema_K2::PC_String;
		pinType.ContainerType = EPinContainerType::None;
		return pinType;
	}

	bool IsSupportedPinType(const FEdGraphPinType& PinType)
	{
		return UMounteaAdvancedInventoryJsonStatics::IsSupportedJsonDefinitionFieldPinType(PinType);
	}

	bool IsSupportedPinTypeTreeItem(const FPinTypeTreeItem& InItem)
	{
		if (!InItem.IsValid())
			return false;

		if (InItem->bReadOnly)
			return true;

		const FEdGraphPinType& unresolvedPinType = InItem->GetPinTypeNoResolve();
		if (unresolvedPinType.PinCategory == UEdGraphSchema_K2::AllObjectTypes)
			return true;

		return IsSupportedPinType(unresolvedPinType);
	}

	void NormalizeFieldPinType(FEdGraphPinType& PinType)
	{
		PinType.ContainerType = EPinContainerType::None;
	}
}
