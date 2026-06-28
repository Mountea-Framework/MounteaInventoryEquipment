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
#include "K2Node_ConstructJsonObjectFromDefinition.generated.h"

struct FMounteaJsonObjectDefinition;
struct FMounteaJsonObjectDefinitionField;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMDEVELOPER_API UK2Node_ConstructJsonObjectFromDefinition : public UK2Node
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
	virtual FText GetVisualWarningTooltipText() const override;
	virtual FText GetToolTipHeading() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FName GetCornerIcon() const override;
	virtual bool ShowVisualWarning() const override;
	virtual FText GetMenuCategory() const override;
	virtual FString GetDocumentationLink() const override { return TEXT("https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/K2Nodes/"); };
#endif

protected:
	UPROPERTY()
	FString CachedDefinitionKey;

	bool bIsRefreshingGeneratedPins = false;

	void RefreshGeneratedPins();
	FString GetSelectedDefinitionKey() const;
	FString BuildDefinitionIssueText() const;
	bool ResolveSelectedDefinition(FMounteaJsonObjectDefinition& OutDefinition, TArray<FString>& Errors) const;
	void CreateFieldPins();

	static bool IsSupportedFieldPinType(const FEdGraphPinType& PinType);
	static UFunction* GetSetterFunctionForPinType(const FEdGraphPinType& PinType);
	static FName GetFieldPinName(const FName FieldName);
	static FName GetFieldNameFromPinName(const FName PinName);
	static bool IsGeneratedFieldPin(const UEdGraphPin* Pin);
	static bool ShouldSkipUnconnectedFieldPin(const UEdGraphPin* Pin);
	static void CopyPinDefaults(const UEdGraphPin* SourcePin, UEdGraphPin* TargetPin);

	static const FName TargetPinName;
	static const FName DefinitionKeyPinName;
	static const FName JsonObjectPinName;
	static const FName FailedPinName;
	static const FString FieldPinPrefix;
};
