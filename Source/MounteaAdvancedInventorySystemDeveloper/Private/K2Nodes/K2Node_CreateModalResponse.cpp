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

#include "K2Nodes/K2Node_CreateModalResponse.h"

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

#define LOCTEXT_NAMESPACE "K2Node_CreateModalResponse"

const FName UK2Node_CreateModalResponse::TargetPinName(TEXT("Target"));
const FName UK2Node_CreateModalResponse::ModalTypePinName(TEXT("ModalType"));
const FName UK2Node_CreateModalResponse::ConfirmationTypePinName(TEXT("bConfirmationType"));
const FName UK2Node_CreateModalResponse::ResponsePinName(TEXT("Response"));
const FName UK2Node_CreateModalResponse::FailedPinName(TEXT("Failed"));

UK2Node_CreateModalResponse::UK2Node_CreateModalResponse(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UK2Node_CreateModalResponse::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* nodeSchema = GetDefault<UEdGraphSchema_K2>();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FailedPinName);

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), TargetPinName);

	UEdGraphPin* modalTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, ModalTypePinName);
	nodeSchema->TrySetDefaultValue(*modalTypePin, TEXT("none"));

	UEdGraphPin* confirmationTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, ConfirmationTypePinName);
	nodeSchema->TrySetDefaultValue(*confirmationTypePin, TEXT("false"));

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, ResolveResponsePinClass(), ResponsePinName);
}

void UK2Node_CreateModalResponse::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* actionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(actionKey))
	{
		UBlueprintNodeSpawner* nodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(nodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(actionKey, nodeSpawner);
	}
}

void UK2Node_CreateModalResponse::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin && Pin->PinName == ModalTypePinName)
		RefreshResponsePinType();
}

void UK2Node_CreateModalResponse::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	if (Pin && Pin->PinName == ModalTypePinName)
		RefreshResponsePinType();
}

void UK2Node_CreateModalResponse::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* schema = CompilerContext.GetSchema();
	if (!schema)
	{
		BreakAllNodeLinks();
		return;
	}

	UFunction* createResponseFunction = UMounteaInventoryModalStatics::StaticClass()->FindFunctionByName(
		GET_FUNCTION_NAME_CHECKED(UMounteaInventoryModalStatics, CreateModalResponse));
	if (!createResponseFunction)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("MissingCreateResponseFunction", "@@ could not find CreateModalResponse.").ToString(), this);
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* execPin = GetExecPin();
	UEdGraphPin* thenPin = FindPinChecked(UEdGraphSchema_K2::PN_Then, EGPD_Output);
	UEdGraphPin* failedPin = FindPinChecked(FailedPinName, EGPD_Output);
	UEdGraphPin* targetPin = FindPinChecked(TargetPinName, EGPD_Input);
	UEdGraphPin* modalTypePin = FindPinChecked(ModalTypePinName, EGPD_Input);
	UEdGraphPin* confirmationTypePin = FindPinChecked(ConfirmationTypePinName, EGPD_Input);
	UEdGraphPin* responsePin = FindPinChecked(ResponsePinName, EGPD_Output);

	UK2Node_CallFunction* callFunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	callFunctionNode->SetFromFunction(createResponseFunction);
	callFunctionNode->AllocateDefaultPins();

	UEdGraphPin* callExecPin = callFunctionNode->GetExecPin();
	UEdGraphPin* callThenPin = callFunctionNode->GetThenPin();
	UEdGraphPin* callTargetPin = callFunctionNode->FindPinChecked(TargetPinName);
	UEdGraphPin* callModalTypePin = callFunctionNode->FindPinChecked(ModalTypePinName);
	UEdGraphPin* callConfirmationTypePin = callFunctionNode->FindPinChecked(ConfirmationTypePinName);
	UEdGraphPin* callReturnPin = callFunctionNode->GetReturnValuePin();

	CompilerContext.MovePinLinksToIntermediate(*execPin, *callExecPin);
	CompilerContext.MovePinLinksToIntermediate(*targetPin, *callTargetPin);
	CompilerContext.MovePinLinksToIntermediate(*modalTypePin, *callModalTypePin);
	CompilerContext.MovePinLinksToIntermediate(*confirmationTypePin, *callConfirmationTypePin);

	if (modalTypePin->LinkedTo.Num() == 0)
		callModalTypePin->DefaultValue = modalTypePin->DefaultValue;

	if (confirmationTypePin->LinkedTo.Num() == 0)
		callConfirmationTypePin->DefaultValue = confirmationTypePin->DefaultValue;

	UK2Node_DynamicCast* castNode = CompilerContext.SpawnIntermediateNode<UK2Node_DynamicCast>(this, SourceGraph);
	castNode->TargetType = ResolveResponsePinClass();
	castNode->SetPurity(false);
	castNode->AllocateDefaultPins();

	schema->TryCreateConnection(callThenPin, castNode->GetExecPin());
	schema->TryCreateConnection(callReturnPin, castNode->GetCastSourcePin());
	CompilerContext.MovePinLinksToIntermediate(*thenPin, *castNode->GetValidCastPin());
	CompilerContext.MovePinLinksToIntermediate(*failedPin, *castNode->GetInvalidCastPin());
	CompilerContext.MovePinLinksToIntermediate(*responsePin, *castNode->GetCastResultPin());

	BreakAllNodeLinks();
}

void UK2Node_CreateModalResponse::PostReconstructNode()
{
	Super::PostReconstructNode();
	RefreshResponsePinType();
}

bool UK2Node_CreateModalResponse::ShouldUseModalTypeSelector(UEdGraphPin* Pin) const
{
	return Pin
		&& Pin->Direction == EGPD_Input
		&& Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_String
		&& Pin->PinName == ModalTypePinName;
}

void UK2Node_CreateModalResponse::RefreshResponsePinType() const
{
	UEdGraphPin* responsePin = FindPin(ResponsePinName, EGPD_Output);
	if (!responsePin)
		return;

	UClass* responseClass = ResolveResponsePinClass();
	responsePin->PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
	responsePin->PinType.PinSubCategory = NAME_None;
	responsePin->PinType.PinSubCategoryObject = responseClass;
	responsePin->PinType.ContainerType = EPinContainerType::None;
	responsePin->PinType.bIsReference = false;
	responsePin->PinType.bIsConst = false;

	if (UEdGraph* graph = GetGraph())
		graph->NotifyGraphChanged();
}

UClass* UK2Node_CreateModalResponse::ResolveResponsePinClass() const
{
	UEdGraphPin* modalTypePin = FindPin(ModalTypePinName, EGPD_Input);
	if (!modalTypePin || modalTypePin->LinkedTo.Num() > 0)
		return UMounteaModalResponsePayload::StaticClass();

	const FString modalType = modalTypePin->GetDefaultAsString().TrimQuotes();
	return ResolveResponsePinClassFromModalType(modalType);
}

UClass* UK2Node_CreateModalResponse::ResolveResponsePinClassFromModalType(const FString& ModalType) const
{
	if (ModalType.IsEmpty() || ModalType.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return UMounteaModalResponsePayload::StaticClass();

	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	const UMounteaAdvancedInventoryGlobalUIConfig* globalUIConfig = settings ? settings->GlobalUIConfig.LoadSynchronous() : nullptr;
	if (!globalUIConfig)
		return UMounteaModalResponsePayload::StaticClass();

	if (const FMounteaModalDefinition* modalDefinition = globalUIConfig->Modals.Find(ModalType))
	{
		if (!modalDefinition->PayloadResponseClass.IsNull())
		{
			if (UClass* responseClass = modalDefinition->PayloadResponseClass.LoadSynchronous())
			{
				if (responseClass->IsChildOf(UMounteaModalResponsePayload::StaticClass()))
					return responseClass;
			}
		}
	}

	return UMounteaModalResponsePayload::StaticClass();
}

#if WITH_EDITOR

FText UK2Node_CreateModalResponse::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText functionName = LOCTEXT("ConstructModalResponse_Title", "Modal - Construct Dynamic Modal Response");
	
	if(TitleType == ENodeTitleType::FullTitle)
	{
		FText contextString = LOCTEXT("ConstructModalResponse_Subtitle", "Source is Advanced Mountea Inventory & Equipment System");
		
		FFormatNamedArguments namedArgs;
		namedArgs.Add(TEXT("FunctionName"), functionName);
		namedArgs.Add(TEXT("ContextString"), contextString);
		
		return FText::Format(LOCTEXT("ConstructModalResponse_Title_WithContext", "{FunctionName}\n{ContextString}"), namedArgs);
	}
	return functionName;
}

FText UK2Node_CreateModalResponse::GetTooltipText() const
{
	return LOCTEXT("CreateModalResponse_Tooltip", "Creates a modal response payload and types the Response output from the selected modal type configuration.");
}

FLinearColor UK2Node_CreateModalResponse::GetNodeTitleColor() const
{
	return FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("420039")));
}

FSlateIcon UK2Node_CreateModalResponse::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = FLinearColor(.823f, .823f, .823f);
	return FSlateIcon(FMounteaAdvancedInventoryDeveloperStyle::GetAppStyleSetName(), "MAISStyleSet.K2Node_SetterIcon.small");
}

FText UK2Node_CreateModalResponse::GetToolTipHeading() const
{
	return LOCTEXT("CreateModalResponse_ToolTipHeading", "Mountea Advanced Inventory Function");
}

FName UK2Node_CreateModalResponse::GetCornerIcon() const
{
	const FName superName = Super::GetCornerIcon();

	if (superName == NAME_None)
		return TEXT("MAISStyleSet.MounteaLogo");

	return superName;
}

FText UK2Node_CreateModalResponse::GetMenuCategory() const
{
	return LOCTEXT("CreateModalResponse_Category", "Mountea|Inventory & Equipment|UI|Modal");
}

#endif

#undef LOCTEXT_NAMESPACE
