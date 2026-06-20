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
#include "K2Node_BreakJsonObjectByDefinition.generated.h"

struct FMounteaJsonObjectDefinition;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMDEVELOPER_API UK2Node_BreakJsonObjectByDefinition : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsNodePure() const override { return false; }
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PostReconstructNode() override;

	bool ShouldUseJsonDefinitionSelector(UEdGraphPin* Pin) const;

#if WITH_EDITOR
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetToolTipHeading() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FName GetCornerIcon() const override;
	virtual FText GetMenuCategory() const override;
	virtual FString GetDocumentationLink() const override { return TEXT("https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/K2Nodes/"); };
#endif

protected:
	UPROPERTY()
	FString CachedDefinitionKey;

	bool bIsRefreshingGeneratedPins = false;

	void RefreshGeneratedPins();
	FString GetSelectedDefinitionKey() const;
	bool ResolveSelectedDefinition(FMounteaJsonObjectDefinition& OutDefinition, TArray<FString>& Errors) const;
	void CreateFieldPins(const FMounteaJsonObjectDefinition& Definition);

	static bool IsSupportedFieldPinType(const FEdGraphPinType& PinType);
	static UFunction* GetGetterFunctionForPinType(const FEdGraphPinType& PinType);
	static FName GetFieldPinName(const FName FieldName);
	static bool IsGeneratedFieldPin(const UEdGraphPin* Pin);
	static void CopyPinDefaults(const UEdGraphPin* SourcePin, UEdGraphPin* TargetPin);
	static void CopyPinLinksAndDefaults(FKismetCompilerContext& CompilerContext, UEdGraphPin* SourcePin, UEdGraphPin* TargetPin);

	static const FName JsonObjectPinName;
	static const FName DefinitionKeyPinName;
	static const FName ErrorsPinName;
	static const FName FailedPinName;
	static const FString FieldPinPrefix;
};
