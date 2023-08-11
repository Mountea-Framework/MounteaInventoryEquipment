// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"

#include "K2Node.h"

#include "K2Node_DistinctArray.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, meta=(Keywords = "Distinct,Intersection,Mountea"))
class  UK2Node_DistinctArray : public UK2Node
{
	GENERATED_BODY()

public:

	UK2Node_DistinctArray();

public:
	
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FLinearColor GetNodeBodyTintColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual void PostReconstructNode() override;
	virtual FText GetKeywords() const override;
	// End of UEdGraphNode interface

public:
	
	// UK2Node interface
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual bool IncludeParentNodeContextMenu() const override { return true; }
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	virtual FText GetMenuCategory() const override;
	virtual bool IsNodePure() const override {return true;};
	virtual void AllocateDefaultPins() override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual int32 GetNodeRefreshPriority() const override { return EBaseNodeRefreshPriority::Low_UsesDependentWildcard; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	// End of UK2Node interface

public:
	
	// UK2Node_DistinctArray
	virtual FName GetPinName(int32 PinIndex) const;
	virtual FName GetOutputPinName() const ;
	virtual UEdGraphPin* GetOutputPin() const;
	virtual void GetKeyAndValuePins(TArray<UEdGraphPin*>& KeyPins, TArray<UEdGraphPin*>& ValuePins) const;
	virtual void PropagatePinType();
	// End of UK2Node_DistinctArray

protected:
	
	/** The number of input pins to generate for this node */
	UPROPERTY()
	int32 NumInputs;

	EPinContainerType ContainerType;
};
