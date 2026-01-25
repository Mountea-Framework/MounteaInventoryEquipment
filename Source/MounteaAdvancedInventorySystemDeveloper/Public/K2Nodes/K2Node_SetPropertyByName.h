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
#include "K2Node.h"
#include "K2Node_SetPropertyByName.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMDEVELOPER_API UK2Node_SetPropertyByName : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:
	virtual void AllocateDefaultPins() override;
	
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PostReconstructNode() override;
	
#if WITH_EDITOR
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;	
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	
	virtual FText GetVisualWarningTooltipText() const override;
	virtual FText GetToolTipHeading() const override;
	virtual FName GetCornerIcon() const override;
	virtual bool ShowVisualWarning() const override { return true; };
#endif

protected:
	void RefreshPinTypes() const;
	static UFunction* GetTargetFunction(const UEdGraphPin* ValuePin);
	
	static const FName TargetPinName;
	static const FName PropertyNamePinName;
	static const FName ValuePinName;
	static const FName ReturnValuePinName;
	
	static const FEdGraphPinType& GetEffectiveType(const UEdGraphPin* Pin)
	{
		return (Pin && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard && Pin->LinkedTo.Num() > 0 && Pin->LinkedTo[0]) ?
			Pin->LinkedTo[0]->PinType : Pin->PinType;
	}
};
