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

#include "K2Nodes/K2Node_ConstructModalPayload.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "Helpers/MounteaModalsPayload.h"
#include "K2Node_CallFunction.h"
#include "K2Node_DynamicCast.h"
#include "KismetCompiler.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaInventoryModalStatics.h"
#include "Styling/MounteaAdvancedInventoryDeveloperStyle.h"

#define LOCTEXT_NAMESPACE "K2Node_ConstructModalPayload"

const FName UK2Node_ConstructModalPayload::TargetPinName(TEXT("Target"));
const FName UK2Node_ConstructModalPayload::ModalTypePinName(TEXT("ModalType"));
const FName UK2Node_ConstructModalPayload::KeyPinName(TEXT("Key"));
const FName UK2Node_ConstructModalPayload::OptionalPayloadPinName(TEXT("OptionalPayload"));
const FName UK2Node_ConstructModalPayload::PayloadPinName(TEXT("Payload"));
const FName UK2Node_ConstructModalPayload::FailedPinName(TEXT("Failed"));

UK2Node_ConstructModalPayload::UK2Node_ConstructModalPayload(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UK2Node_ConstructModalPayload::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* nodeSchema = GetDefault<UEdGraphSchema_K2>();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FailedPinName);

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), TargetPinName);

	UEdGraphPin* modalTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, ModalTypePinName);
	nodeSchema->TrySetDefaultValue(*modalTypePin, TEXT("none"));

	UEdGraphPin* keyPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, KeyPinName);
	nodeSchema->TrySetDefaultValue(*keyPin, TEXT("none"));

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), OptionalPayloadPinName);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, ResolvePayloadPinClass(), PayloadPinName);
}

void UK2Node_ConstructModalPayload::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* actionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(actionKey))
	{
		UBlueprintNodeSpawner* nodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(nodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(actionKey, nodeSpawner);
	}
}

void UK2Node_ConstructModalPayload::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin && Pin->PinName == ModalTypePinName)
		RefreshPayloadPinType();
}

void UK2Node_ConstructModalPayload::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	if (Pin && Pin->PinName == ModalTypePinName)
		RefreshPayloadPinType();
}

void UK2Node_ConstructModalPayload::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* schema = CompilerContext.GetSchema();
	if (!schema)
	{
		BreakAllNodeLinks();
		return;
	}

	UFunction* constructPayloadFunction = UMounteaInventoryModalStatics::StaticClass()->FindFunctionByName(
		GET_FUNCTION_NAME_CHECKED(UMounteaInventoryModalStatics, ConstructModalPayload));
	if (!constructPayloadFunction)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("MissingConstructPayloadFunction", "@@ could not find ConstructModalPayload.").ToString(), this);
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* execPin = GetExecPin();
	UEdGraphPin* thenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then, EGPD_Output);
	UEdGraphPin* failedPin = FindPinChecked(FailedPinName, EGPD_Output);
	UEdGraphPin* targetPin = FindPinChecked(TargetPinName, EGPD_Input);
	UEdGraphPin* modalTypePin = FindPinChecked(ModalTypePinName, EGPD_Input);
	UEdGraphPin* keyPin = FindPinChecked(KeyPinName, EGPD_Input);
	UEdGraphPin* optionalPayloadPin = FindPinChecked(OptionalPayloadPinName, EGPD_Input);
	UEdGraphPin* payloadPin = FindPinChecked(PayloadPinName, EGPD_Output);

	UK2Node_CallFunction* callFunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	callFunctionNode->SetFromFunction(constructPayloadFunction);
	callFunctionNode->AllocateDefaultPins();

	UEdGraphPin* callExecPin = callFunctionNode->GetExecPin();
	UEdGraphPin* callThenPin = callFunctionNode->GetThenPin();
	UEdGraphPin* callTargetPin = callFunctionNode->FindPinChecked(TEXT("Target"));
	UEdGraphPin* callModalTypePin = callFunctionNode->FindPinChecked(ModalTypePinName);
	UEdGraphPin* callKeyPin = callFunctionNode->FindPinChecked(KeyPinName);
	UEdGraphPin* callOptionalPayloadPin = callFunctionNode->FindPinChecked(OptionalPayloadPinName);
	UEdGraphPin* callReturnPin = callFunctionNode->GetReturnValuePin();

	CompilerContext.MovePinLinksToIntermediate(*execPin, *callExecPin);
	CompilerContext.MovePinLinksToIntermediate(*targetPin, *callTargetPin);
	CompilerContext.MovePinLinksToIntermediate(*modalTypePin, *callModalTypePin);
	CompilerContext.MovePinLinksToIntermediate(*keyPin, *callKeyPin);
	CompilerContext.MovePinLinksToIntermediate(*optionalPayloadPin, *callOptionalPayloadPin);

	if (modalTypePin->LinkedTo.Num() == 0)
		callModalTypePin->DefaultValue = modalTypePin->DefaultValue;

	if (keyPin->LinkedTo.Num() == 0)
		callKeyPin->DefaultValue = keyPin->DefaultValue;

	UK2Node_DynamicCast* castNode = CompilerContext.SpawnIntermediateNode<UK2Node_DynamicCast>(this, SourceGraph);
	castNode->TargetType = ResolvePayloadPinClass();
	castNode->SetPurity(false);
	castNode->AllocateDefaultPins();

	schema->TryCreateConnection(callThenPin, castNode->GetExecPin());
	schema->TryCreateConnection(callReturnPin, castNode->GetCastSourcePin());

	CompilerContext.MovePinLinksToIntermediate(*thenPin, *castNode->GetValidCastPin());
	CompilerContext.MovePinLinksToIntermediate(*failedPin, *castNode->GetInvalidCastPin());
	CompilerContext.MovePinLinksToIntermediate(*payloadPin, *castNode->GetCastResultPin());

	BreakAllNodeLinks();
}

void UK2Node_ConstructModalPayload::PostReconstructNode()
{
	Super::PostReconstructNode();
	RefreshPayloadPinType();
}

bool UK2Node_ConstructModalPayload::ShouldUseModalTypeSelector(UEdGraphPin* Pin) const
{
	return Pin
		&& Pin->Direction == EGPD_Input
		&& Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_String
		&& Pin->PinName == ModalTypePinName;
}

bool UK2Node_ConstructModalPayload::ShouldUseModalRowSelector(UEdGraphPin* Pin) const
{
	return Pin
		&& Pin->Direction == EGPD_Input
		&& Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_String
		&& Pin->PinName == KeyPinName;
}

void UK2Node_ConstructModalPayload::RefreshPayloadPinType() const
{
	UEdGraphPin* payloadPin = FindPin(PayloadPinName, EGPD_Output);
	if (!payloadPin)
		return;

	UClass* payloadClass = ResolvePayloadPinClass();
	payloadPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
	payloadPin->PinType.PinSubCategory = NAME_None;
	payloadPin->PinType.PinSubCategoryObject = payloadClass;
	payloadPin->PinType.ContainerType = EPinContainerType::None;
	payloadPin->PinType.bIsReference = false;
	payloadPin->PinType.bIsConst = false;

	if (UEdGraph* graph = GetGraph())
		graph->NotifyGraphChanged();
}

UClass* UK2Node_ConstructModalPayload::ResolvePayloadPinClass() const
{
	UEdGraphPin* modalTypePin = FindPin(ModalTypePinName, EGPD_Input);
	if (!modalTypePin || modalTypePin->LinkedTo.Num() > 0)
		return UMounteaModalsPayload::StaticClass();

	const FString modalType = modalTypePin->GetDefaultAsString().TrimQuotes();
	return ResolvePayloadPinClassFromModalType(modalType);
}

UClass* UK2Node_ConstructModalPayload::ResolvePayloadPinClassFromModalType(const FString& ModalType) const
{
	if (ModalType.IsEmpty() || ModalType.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return UMounteaModalsPayload::StaticClass();

	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	const UMounteaAdvancedInventoryGlobalUIConfig* globalUIConfig = settings ? settings->GlobalUIConfig.LoadSynchronous() : nullptr;
	if (!globalUIConfig)
		return UMounteaModalsPayload::StaticClass();

	if (const FMounteaModalDefinition* modalDefinition = globalUIConfig->Modals.Find(ModalType))
	{
		if (!modalDefinition->PayloadClass.IsNull())
		{
			if (UClass* payloadClass = modalDefinition->PayloadClass.LoadSynchronous())
			{
				if (payloadClass->IsChildOf(UMounteaModalsPayload::StaticClass()))
					return payloadClass;
			}
		}
	}

	if (!globalUIConfig->ModalPayloadClass.IsNull())
	{
		if (UClass* payloadClass = globalUIConfig->ModalPayloadClass.LoadSynchronous())
		{
			if (payloadClass->IsChildOf(UMounteaModalsPayload::StaticClass()))
				return payloadClass;
		}
	}

	return UMounteaModalsPayload::StaticClass();
}

#if WITH_EDITOR

FText UK2Node_ConstructModalPayload::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText functionName = LOCTEXT("ConstructModalPayload_Title", "Modal - Construct Dynamic Modal Payload");
	
	if(TitleType == ENodeTitleType::FullTitle)
	{
		FText contextString = LOCTEXT("ConstructModalPayload_Subtitle", "Source is Advanced Mountea Inventory & Equipment System");
		
		FFormatNamedArguments namedArgs;
		namedArgs.Add(TEXT("FunctionName"), functionName);
		namedArgs.Add(TEXT("ContextString"), contextString);
		
		return FText::Format(LOCTEXT("ConstructModalPayload_Title_WithContext", "{FunctionName}\n{ContextString}"), namedArgs);
	}
	return functionName;
}

FText UK2Node_ConstructModalPayload::GetTooltipText() const
{
	return LOCTEXT("ConstructModalPayload_Tooltip", "Constructs a modal payload and types the Payload output from the selected modal type configuration.");
}

FLinearColor UK2Node_ConstructModalPayload::GetNodeTitleColor() const
{
	return FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("420039")));
}

FSlateIcon UK2Node_ConstructModalPayload::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = FLinearColor(.823f, .823f, .823f);
	return FSlateIcon(FMounteaAdvancedInventoryDeveloperStyle::GetAppStyleSetName(), "MAISStyleSet.K2Node_SetterIcon.small");
}

FText UK2Node_ConstructModalPayload::GetToolTipHeading() const
{
	return LOCTEXT("ConstructModalPayload_ToolTipHeading", "Mountea Advanced Inventory & Equipment Function");
}

FName UK2Node_ConstructModalPayload::GetCornerIcon() const
{
	const FName superName = Super::GetCornerIcon();

	if (superName == NAME_None)
		return TEXT("MAISStyleSet.MounteaLogo");

	return superName;
}

FText UK2Node_ConstructModalPayload::GetMenuCategory() const
{
	return LOCTEXT("ConstructModalPayload_Category", "Mountea|Inventory & Equipment|UI|Modal");
}

#endif

#undef LOCTEXT_NAMESPACE
