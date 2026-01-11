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
#include "K2Node_Switch.h"
#include "K2Node_SwitchWidgetCommand.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMDEVELOPER_API UK2Node_SwitchWidgetCommand : public UK2Node_Switch
{
	GENERATED_UCLASS_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, Category="PinOptions", 
		meta=(DisplayName="Widget Commands"))
	TMap<FName, bool> PinNames;

	UPROPERTY(EditAnywhere, Category="PinOptions")
	uint32 bIsCaseSensitive : 1;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FText GetToolTipHeading() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FName GetCornerIcon() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
#endif

	virtual void AddPinToSwitchNode() override;
	virtual FName GetUniquePinName() override;
	virtual FEdGraphPinType GetPinType() const override;
	virtual FName GetPinNameGivenIndex(int32 Index) const override;

protected:
	virtual void CreateSelectionPin() override;
	virtual void CreateCasePins() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;

	void SetupCaseSensitivityFunction();
	void GetWidgetCommandsFromSettings(TArray<FName>& OutCommands) const;
	bool IsPredefinedPin(const FName& PinName) const;
	void GetVisiblePinNames(TArray<FName>& OutVisiblePins) const;
};