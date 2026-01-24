// REPLACE COPYRIGHT

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
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetVisualWarningTooltipText() const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

#if WITH_EDITOR
	virtual FText GetToolTipHeading() const override;
	virtual FName GetCornerIcon() const override;
#endif

protected:
	void RefreshPinTypes() const;
	static UFunction* GetTargetFunction(const UEdGraphPin* ValuePin);
	
	static const FName TargetPinName;
	static const FName PropertyNamePinName;
	static const FName ValuePinName;
	
	static const FEdGraphPinType& GetEffectiveType(const UEdGraphPin* Pin)
	{
		if (Pin && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard && Pin->LinkedTo.Num() > 0 && Pin->LinkedTo[0])
		{
			return Pin->LinkedTo[0]->PinType;
		}
		return Pin->PinType;
	}
};
