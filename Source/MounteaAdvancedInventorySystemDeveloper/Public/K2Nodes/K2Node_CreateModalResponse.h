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
#include "K2Node_CreateModalResponse.generated.h"

class UMounteaModalResponsePayload;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMDEVELOPER_API UK2Node_CreateModalResponse : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsNodePure() const override { return false; }
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PostReconstructNode() override;

	bool ShouldUseModalTypeSelector(UEdGraphPin* Pin) const;

#if WITH_EDITOR
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FText GetToolTipHeading() const override;
	virtual FName GetCornerIcon() const override;
	virtual FText GetMenuCategory() const override;
	virtual FString GetDocumentationLink() const override { return TEXT("https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/K2Nodes/"); };
#endif

private:
	void RefreshResponsePinType() const;
	UClass* ResolveResponsePinClass() const;
	UClass* ResolveResponsePinClassFromModalType(const FString& ModalType) const;

	static const FName TargetPinName;
	static const FName ModalTypePinName;
	static const FName ConfirmationTypePinName;
	static const FName ResponsePinName;
	static const FName FailedPinName;
};
