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

#include "K2Nodes/K2Node_MounteaAdvancedInventoryCallFunction.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "GameplayTagContainer.h"
#include "Styling/MounteaAdvancedInventoryDeveloperStyle.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryCallFunction"

bool UK2Node_MounteaAdvancedInventoryCallFunction::HasMounteaMetadata(const UFunction* Function)
{
	return Function
		&& (Function->HasMetaData(TEXT("MounteaBinding"))
			|| Function->HasMetaData(TEXT("MounteaGetter"))
			|| Function->HasMetaData(TEXT("MounteaSetter"))
			|| Function->HasMetaData(TEXT("MounteaValidate"))
			|| Function->HasMetaData(TEXT("MounteaCommand")));
}

void UK2Node_MounteaAdvancedInventoryCallFunction::GetMenuActions(FBlueprintActionDatabaseRegistrar& actionRegistrar) const
{
	Super::GetMenuActions(actionRegistrar);

	UClass* nodeClass = GetClass();
	
	auto customizeNodeLambda = [](UEdGraphNode* newNode, bool bIsTemplateNode, UFunction* relevantFunction, UClass* relevantClass)
	{
		UK2Node_MounteaAdvancedInventoryCallFunction* inputNode = CastChecked<UK2Node_MounteaAdvancedInventoryCallFunction>(newNode);
		inputNode->Initialize(relevantFunction, relevantClass);
	};
	
	if (actionRegistrar.IsOpenForRegistration(nodeClass))
	{
		const TSet<UClass*>& relevantClasses = MounteaAdvancedInventoryHelpers::GetRelevantClasses();
		TSet<UFunction*> registeredFunctions;

		for (UClass* relevantClass : relevantClasses)
		{
			TArray<UFunction*> classFunctions;
			for (TFieldIterator<UFunction> FuncIt(relevantClass, EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
			{
				UFunction* function = *FuncIt;
				
				if (!HasMounteaMetadata(function))
					continue;
				
				if (!function->HasAnyFunctionFlags(FUNC_Private))
				{
					if (!registeredFunctions.Contains(function))
					{
						classFunctions.Add(function);
						registeredFunctions.Add(function);
					}
				}
			}

			for (UFunction* itrFunction : classFunctions)
			{
				if (!itrFunction->HasAnyFunctionFlags(FUNC_BlueprintEvent))
					itrFunction->SetMetaData(TEXT("BlueprintInternalUseOnly"), TEXT("true"));

				UBlueprintNodeSpawner* nodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
				nodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(customizeNodeLambda, itrFunction, relevantClass);

				actionRegistrar.AddBlueprintAction(nodeClass, nodeSpawner);
			}
		}
	}
}

FText UK2Node_MounteaAdvancedInventoryCallFunction::GetKeywords() const
{
	FString keywordsString = Super::GetKeywords().ToString();

	auto AppendKeyword = [&keywordsString](const FString& keyword)
	{
		const FString trimmedKeyword = keyword.TrimStartAndEnd();
		if (trimmedKeyword.IsEmpty()) return;

		if (!keywordsString.IsEmpty())
			keywordsString += TEXT(" ");

		keywordsString += trimmedKeyword;
	};

	if (const UFunction* targetFunction = GetTargetFunction())
	{
		if (targetFunction->HasMetaData(FBlueprintMetadata::MD_FunctionKeywords))
			AppendKeyword(targetFunction->GetMetaData(FBlueprintMetadata::MD_FunctionKeywords));
		else if (targetFunction->HasMetaData(TEXT("Keywords")))
			AppendKeyword(targetFunction->GetMetaData(TEXT("Keywords")));

		if (targetFunction->HasMetaData(FBlueprintMetadata::MD_DisplayName))
			AppendKeyword(targetFunction->GetMetaData(FBlueprintMetadata::MD_DisplayName));

		AppendKeyword(targetFunction->GetName());

		if (targetFunction->HasMetaData(TEXT("MounteaBinding")))
			AppendKeyword(TEXT("MounteaBinding Binding Bind Delegate Event"));
		if (targetFunction->HasMetaData(TEXT("MounteaGetter")))
			AppendKeyword(TEXT("MounteaGetter Getter Get"));
		if (targetFunction->HasMetaData(TEXT("MounteaSetter")))
			AppendKeyword(TEXT("MounteaSetter Setter Set"));
		if (targetFunction->HasMetaData(TEXT("MounteaValidate")))
			AppendKeyword(TEXT("MounteaValidate Validate Validation"));
		if (targetFunction->HasMetaData(TEXT("MounteaCommand")))
			AppendKeyword(TEXT("MounteaCommand Command"));
	}

	return FText::FromString(keywordsString);
}

bool UK2Node_MounteaAdvancedInventoryCallFunction::ShouldUseCommandSelector(UEdGraphPin* Pin) const
{
	if (!Pin || Pin->PinName != TEXT("Command")) return false;
    
	const UFunction* targetFunction = GetTargetFunction();
	if (!targetFunction) return false;

	return targetFunction->HasMetaData(TEXT("MounteaCommand"));
}

bool UK2Node_MounteaAdvancedInventoryCallFunction::ShouldUseEquipmentSlotSelector(UEdGraphPin* Pin) const
{
	if (!Pin)
		return false;

	if (Pin->PinName != TEXT("SlotName") && Pin->PinName != TEXT("SlotId") && Pin->PinName != TEXT("TargetSlotId"))
		return false;

	const UFunction* targetFunction = GetTargetFunction();
	if (!targetFunction)
		return false;

	return targetFunction->HasMetaData(TEXT("MounteaEquipmentSlot"));
}

bool UK2Node_MounteaAdvancedInventoryCallFunction::ShouldUseInventoryCategorySelector(UEdGraphPin* Pin) const
{
	if (!Pin)
		return false;

	if (Pin->Direction != EGPD_Input)
		return false;

	if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
		return false;

	if (Pin->PinType.PinSubCategoryObject != FGameplayTag::StaticStruct())
		return false;

	const UFunction* targetFunction = GetTargetFunction();
	if (!targetFunction)
		return false;

	if (!targetFunction->HasMetaData(TEXT("MounteaInventoryCategoryPin")))
		return false;

	const FString categoryPinName = targetFunction->GetMetaData(TEXT("MounteaInventoryCategoryPin"));
	return Pin->PinName == FName(*categoryPinName);
}

bool UK2Node_MounteaAdvancedInventoryCallFunction::ShouldUseModalTypeSelector(UEdGraphPin* Pin) const
{
	if (!Pin)
		return false;

	if (Pin->Direction != EGPD_Input)
		return false;

	if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_String)
		return false;

	const UFunction* targetFunction = GetTargetFunction();
	if (!targetFunction)
		return false;

	if (!targetFunction->HasMetaData(TEXT("MounteaModalType")))
		return false;

	const FString modalTypePinName = targetFunction->HasMetaData(TEXT("MounteaModalTypePin"))
		? targetFunction->GetMetaData(TEXT("MounteaModalTypePin"))
		: FString(TEXT("ModalType"));

	return Pin->PinName == FName(*modalTypePinName);
}

bool UK2Node_MounteaAdvancedInventoryCallFunction::ShouldUseModalRowSelector(UEdGraphPin* Pin) const
{
	if (!Pin)
		return false;

	if (Pin->Direction != EGPD_Input)
		return false;

	if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_String)
		return false;

	const UFunction* targetFunction = GetTargetFunction();
	if (!targetFunction)
		return false;

	if (!targetFunction->HasMetaData(TEXT("MounteaModalRow")))
		return false;

	const FString modalRowPinName = targetFunction->HasMetaData(TEXT("MounteaModalRowPin"))
		? targetFunction->GetMetaData(TEXT("MounteaModalRowPin"))
		: FString(TEXT("Key"));

	return Pin->PinName == FName(*modalRowPinName);
}

bool UK2Node_MounteaAdvancedInventoryCallFunction::IsNodePure() const
{
	if (const UFunction* localFunction = GetTargetFunction())
	{
		if (localFunction->HasMetaData(TEXT("MounteaBinding")))
			return false;
		if (localFunction->HasMetaData(TEXT("MounteaSetter")))
			return false;
		if (localFunction->HasMetaData(TEXT("MounteaGetter")))
			return true;
	}
	return Super::IsNodePure();
}

FText UK2Node_MounteaAdvancedInventoryCallFunction::GetToolTipHeading() const
{
	return LOCTEXT("MounteaAdvancedInventoryCallFunctionFunctions", "Mountea Advanced Inventory Function");
}

EFunctionRole UK2Node_MounteaAdvancedInventoryCallFunction::GetFunctionRole() const
{
	if (const UFunction* localFunction = GetTargetFunction())
	{
		if (localFunction->HasMetaData(TEXT("MounteaBinding")))
			return EFunctionRole::Bind;
		if (localFunction->HasMetaData(TEXT("MounteaGetter")))
			return EFunctionRole::Get;
		if (localFunction->HasMetaData(TEXT("MounteaSetter")))
			return EFunctionRole::Set;
		if (localFunction->HasMetaData(TEXT("MounteaValidate")))
			return EFunctionRole::Validate;
	}
	return EFunctionRole::Unknown;
}

FText UK2Node_MounteaAdvancedInventoryCallFunction::GetTooltipText() const
{
	FText defaultText = Super::GetTooltipText();
	
	switch (GetFunctionRole())
	{
		case EFunctionRole::Bind:
			return FText::Format(
				INVTEXT("{0}\n\nBinding: Binding functions subscribe or unsubscribe Blueprint events to native Mountea delegates.\nThe binding is possible even without direct class access which is required by regular events."),
				defaultText);

		case EFunctionRole::Set:
			return FText::Format(
				INVTEXT("{0}\n\n📥 Setter: A setter function is responsible for updating or modifying a particular value or data in an object.\nIt directly affects the state of the target by assigning a new value to one of its properties.\nSetters may include internal validation or checks to ensure that the value being assigned meets certain criteria before applying it."), 
				defaultText);

		case EFunctionRole::Validate:
			return FText::Format(
				INVTEXT("{0}\n\n❔ Validator: Validator functions are used to verify the correctness or validity of data.\nThese functions perform checks to ensure that the data adheres to certain rules or constraints, such as format, range, or consistency.\nA successful validation confirms that the data is reliable and suitable for use."), 
				defaultText);

		case EFunctionRole::Get:
			return FText::Format(
				INVTEXT("{0}\n\n📤 Getter: A getter function is designed to retrieve and return a specific value or property from an object without modifying it.\nGetters are essential for accessing data in a controlled manner, ensuring that external code can view data but not alter it directly."), 
				defaultText);

		default:
			return defaultText;
	}
}

FLinearColor UK2Node_MounteaAdvancedInventoryCallFunction::GetNodeTitleColor() const
{
	switch (GetFunctionRole())
	{
		case EFunctionRole::Bind:
			return FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("1b4d3e")));
		case EFunctionRole::Validate:
			return FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("99621e")));
		case EFunctionRole::Set:
			return FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("420039")));
		case EFunctionRole::Get:
			return FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("05204a")));
		default:
			return Super::GetNodeTitleColor();
	}
}

FName UK2Node_MounteaAdvancedInventoryCallFunction::GetCornerIcon() const
{
	auto superName = Super::GetCornerIcon();

	if (superName == NAME_None)
		return TEXT("MAISStyleSet.MounteaLogo");

	return superName;
}

FSlateIcon UK2Node_MounteaAdvancedInventoryCallFunction::GetIconAndTint(FLinearColor& outColor) const
{
	outColor = FLinearColor(.823f, .823f, .823f);

	switch (GetFunctionRole())
	{
		case EFunctionRole::Bind:
			return FSlateIcon(FMounteaAdvancedInventoryDeveloperStyle::GetAppStyleSetName(), "MAISStyleSet.K2Node_Binding.small");
		case EFunctionRole::Validate:
			return FSlateIcon(FMounteaAdvancedInventoryDeveloperStyle::GetAppStyleSetName(), "MAISStyleSet.K2Node_ValidateIcon.small");
		case EFunctionRole::Set:
			return FSlateIcon(FMounteaAdvancedInventoryDeveloperStyle::GetAppStyleSetName(), "MAISStyleSet.K2Node_SetterIcon.small");
		case EFunctionRole::Get:
			return FSlateIcon(FMounteaAdvancedInventoryDeveloperStyle::GetAppStyleSetName(), "MAISStyleSet.K2Node_GetterIcon.small");
		default:
			return Super::GetIconAndTint(outColor);
	}
}

FText UK2Node_MounteaAdvancedInventoryCallFunction::GetFunctionContextString() const
{
	FText ContextString = LOCTEXT("MounteaAdvancedInventoryCallFunctionContext", "Source is Mountea Advanced Inventory System");

	const UFunction* Function = GetTargetFunction();
	UClass* CurrentSelfClass = (Function != nullptr) ? Function->GetOwnerClass() : nullptr;
	UClass const* TrueSelfClass = CurrentSelfClass;

	if (CurrentSelfClass && CurrentSelfClass->ClassGeneratedBy)
		TrueSelfClass = CurrentSelfClass->GetAuthoritativeClass();

	if (TrueSelfClass != nullptr)
	{
		const FText TargetText = TrueSelfClass->GetDisplayNameText();
		FFormatNamedArguments Args;
		Args.Add(TEXT("TargetName"), TargetText);
		ContextString = FText::Format(LOCTEXT("CallFunctionOnDifferentContext", "Source is {TargetName}"), Args);
	}
	return ContextString;
}

void UK2Node_MounteaAdvancedInventoryCallFunction::Initialize(const UFunction* relevantFunction, UClass* relevantClass)
{
	FunctionReference.SetExternalMember(relevantFunction->GetFName(), relevantClass);
}

#undef LOCTEXT_NAMESPACE
