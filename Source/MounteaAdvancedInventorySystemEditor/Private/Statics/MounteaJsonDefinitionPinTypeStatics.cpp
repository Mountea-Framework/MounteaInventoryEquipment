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
		if (PinType.ContainerType != EPinContainerType::None)
			return false;

		return PinType.PinCategory == UEdGraphSchema_K2::PC_Int ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Int64 ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Real ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_String ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Name ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Text ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Byte ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Object ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_SoftObject ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Class ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_SoftClass ||
			PinType.PinCategory == UEdGraphSchema_K2::PC_Struct;
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
