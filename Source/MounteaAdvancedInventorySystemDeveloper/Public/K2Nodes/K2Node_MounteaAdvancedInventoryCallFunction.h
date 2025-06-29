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
#include "K2Node_CallFunction.h"
#include "Helpers/FMounteaAdvancedInventoryK2NodesHelpers.h"
#include "K2Node_MounteaAdvancedInventoryCallFunction.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMDEVELOPER_API UK2Node_MounteaAdvancedInventoryCallFunction : public UK2Node_CallFunction
{
	GENERATED_BODY()
	
public:
	
	EFunctionCallType GetFunctionType() const;
	EFunctionRole GetFunctionRole() const;

	void Initialize(const UFunction* relevantFunction, UClass* relevantClass);
	
	// UK2Node
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& actionRegistrar) const override;

	// UK2Node_CallFunction
	virtual FText GetToolTipHeading() const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FName GetCornerIcon() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& outColor) const override;
	virtual FText GetFunctionContextString() const override;
};
