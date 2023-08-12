// All rights reserved Dominik Pavlicek 2023.


#include "K2Nodes/K2Node_DistinctArray.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphUtilities.h"
#include "KismetCastingUtils.h"
#include "KismetCompiledFunctionContext.h"
#include "KismetCompilerMisc.h"
#include "ToolMenu.h"
#include "Helpers/MounteaInventoryEquipmentDeveloperBPF.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet2/CompilerResultsLog.h"

#define LOCTEXT_NAMESPACE "DistinctArray"

class FKCHandler_DistinctArray : public FNodeHandlingFunctor
{

protected:
	EKismetCompiledStatementType CompiledStatementType;
	
public:
	FKCHandler_DistinctArray(FKismetCompilerContext& InCompilerContext) : FNodeHandlingFunctor(InCompilerContext)
	{
		CompiledStatementType = KCST_CallFunction ;
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_DistinctArray* ContainerNode = CastChecked<UK2Node_DistinctArray>(Node);
		UEdGraphPin* OutputPin = ContainerNode->GetOutputPin();

		FNodeHandlingFunctor::RegisterNets(Context, Node);

		// Create a local term to drop the container into
		FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
		Term->bPassedByReference = false;
		Term->Source = Node;
		Context.NetMap.Add(OutputPin, Term);
	}
	
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
	    // 1. Retrieve all TArray Inputs
	    TArray<FBPTerminal*> InputArrays;

	    for (UEdGraphPin* Pin : Node->Pins)
	    {
	        if (Pin && Pin->Direction == EGPD_Input)
	        {
	            FBPTerminal** InputTerm = Context.NetMap.Find(FEdGraphUtilities::GetNetFromPin(Pin));
	            if (InputTerm)
	            {
	                InputArrays.Add(*InputTerm);
	            }
	        }
	    }

	    // 2. Get the empty TArray Output
	    UK2Node_DistinctArray* DistinctArrayNode = CastChecked<UK2Node_DistinctArray>(Node);
	    UEdGraphPin* OutputPin = DistinctArrayNode->GetOutputPin();
	    FBPTerminal** OutputArrayTerm = Context.NetMap.Find(OutputPin);
	    check(OutputArrayTerm);
	    FScriptArrayHelper OutputArrayHelper(CastField<FArrayProperty>((*OutputArrayTerm)->AssociatedVarProperty), (*OutputArrayTerm)->AssociatedVarProperty);
	    //OutputArrayHelper.EmptyValues(); // Make sure the output array is empty

	    // 3. Add All Intersecting Values from All Inputs to the Output
	    for (FBPTerminal* ArrayTerm : InputArrays)
	    {
			FArrayProperty* Arr = CastField<FArrayProperty>(ArrayTerm->AssociatedVarProperty);
			if (!Arr) continue;
	    	
	    	void* ArrVal = Arr->GetPropertyValuePtr_InContainer(ArrayTerm->Source);
			if (!ArrVal) continue;
	    	
	        FScriptArrayHelper InputArrayHelper(Arr, ArrVal);
	        FProperty* InnerProp = CastField<FArrayProperty>(ArrayTerm->AssociatedVarProperty)->Inner;

	        for (int32 SourceIndex = 0; SourceIndex < InputArrayHelper.Num(); ++SourceIndex)
	        {
	            void* SourceItem = InputArrayHelper.GetRawPtr(SourceIndex);

	            bool bItemExistsInAllArrays = true;

	            for (FBPTerminal* OtherArrayTerm : InputArrays)
	            {
	                if (OtherArrayTerm != ArrayTerm) // Don't compare with itself
	                {
	                    FScriptArrayHelper OtherArrayHelper(CastField<FArrayProperty>(OtherArrayTerm->AssociatedVarProperty), OtherArrayTerm->AssociatedVarProperty);
	                    bool bItemFound = false;

	                    for (int32 OtherIndex = 0; OtherIndex < OtherArrayHelper.Num(); ++OtherIndex)
	                    {
	                        void* OtherItem = OtherArrayHelper.GetRawPtr(OtherIndex);
	                        if (InnerProp->Identical(SourceItem, OtherItem))
	                        {
	                            bItemFound = true;
	                            break;
	                        }
	                    }

	                    if (!bItemFound)
	                    {
	                        bItemExistsInAllArrays = false;
	                        break;
	                    }
	                }
	            }

	            if (bItemExistsInAllArrays)
	            {
	                bool bAlreadyInOutput = false;
	                for (int32 OutIdx = 0; OutIdx < OutputArrayHelper.Num(); ++OutIdx)
	                {
	                    void* OutputItem = OutputArrayHelper.GetRawPtr(OutIdx);
	                    if (InnerProp->Identical(SourceItem, OutputItem))
	                    {
	                        bAlreadyInOutput = true;
	                        break;
	                    }
	                }

	                if (!bAlreadyInOutput)
	                {
	                    int32 NewIdx = OutputArrayHelper.AddValue();
	                    InnerProp->CopySingleValueToScriptVM(OutputArrayHelper.GetRawPtr(NewIdx), SourceItem);
	                }
	            }
	        }
	    }
	}


};

UK2Node_DistinctArray::UK2Node_DistinctArray()
{
	MinNumInputs = 2;
	NumInputs = 2;
	ContainerType = EPinContainerType::Array;
}

FText UK2Node_DistinctArray::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("DistinctArray_Title", "Distinct Array");
}

FText UK2Node_DistinctArray::GetTooltipText() const
{
	return LOCTEXT("DistinctArray_Tooltip", "Return distinct (intersection) result of any specified arrays.");
}

FLinearColor UK2Node_DistinctArray::GetNodeTitleColor() const
{
	return FLinearColor::Green;
}

FLinearColor UK2Node_DistinctArray::GetNodeBodyTintColor() const
{
	return FLinearColor::Yellow;
}

FSlateIcon UK2Node_DistinctArray::GetIconAndTint(FLinearColor& OutColor) const
{
	static const FSlateIcon Icon = FSlateIcon("MIEStyleSet", "MIEStyleSet.Icon.DistinctArray");
	return Icon;
}

void UK2Node_DistinctArray::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	UK2Node::ValidateNodeDuringCompilation(MessageLog);

	const UEdGraphSchema_K2* Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	UEdGraphPin* OutputPin = GetOutputPin();;
	if (!ensure(Schema) || !ensure(OutputPin) || Schema->IsExecPin(*OutputPin))
	{
		MessageLog.Error(*NSLOCTEXT("K2Node", "DistinctArray_OutputIsExec", "Unacceptable array type in @@").ToString(), this);
	}
}

void UK2Node_DistinctArray::PostReconstructNode()
{
	// Find a pin that has connections to use to jumpstart the wildcard process
	FEdGraphPinType OutputPinType;
	FEdGraphTerminalType OutputPinValueType;

	const bool bMapContainer = ContainerType == EPinContainerType::Map;
	bool bFoundKey = false;
	bool bFoundValue = !bMapContainer;

	UEdGraphPin* OutputPin = GetOutputPin();
	if (OutputPin->LinkedTo.Num() > 0)
	{
		OutputPinType = OutputPin->LinkedTo[0]->PinType;
		bFoundKey = true;

		if (bMapContainer)
		{
			OutputPinValueType = OutputPin->LinkedTo[0]->PinType.PinValueType;
			bFoundValue = true;
		}
	}
	else
	{
		bool bKeyPin = !bMapContainer;
		UEdGraphPin* CurrentTopParent = nullptr;

		check(Pins[0] == OutputPin);
		for (int32 PinIndex = 1; PinIndex < Pins.Num() && (!bFoundKey || !bFoundValue); ++PinIndex)
		{
			if (UEdGraphPin* CurrentPin = Pins[PinIndex])
			{
				if (CurrentPin->ParentPin == nullptr)
				{
					CurrentTopParent = CurrentPin;
					if (bMapContainer)
					{
						bKeyPin = !bKeyPin;
					}
				}

				if ((bKeyPin && !bFoundKey) || (!bKeyPin && !bFoundValue))
				{
					checkSlow((CurrentPin->ParentPin == nullptr) || (CurrentTopParent != nullptr));
					if (CurrentPin->LinkedTo.Num() > 0)
					{
						// The pin is linked, use its type as the type for the key or value type as appropriate

						// If this is a split pin, so we want to base the pin type on the parent rather than the linked to pin
						const FEdGraphPinType& PinType = (CurrentPin->ParentPin ? CurrentTopParent->PinType : CurrentPin->LinkedTo[0]->PinType);
						if (bKeyPin)
						{
							OutputPinType = PinType;
							bFoundKey = true;
						}
						else
						{
							OutputPinValueType = FEdGraphTerminalType::FromPinType(PinType);
							bFoundValue = true;
						}
					}
					else if (!Pins[PinIndex]->DoesDefaultValueMatchAutogenerated())
					{
						// The pin has user data in it, continue to use its type as the type for all pins.

						// If this is a split pin, so we want to base the pin type on the parent rather than this pin
						const FEdGraphPinType& PinType = (CurrentPin->ParentPin ? CurrentTopParent->PinType : CurrentPin->PinType);

						if (bKeyPin)
						{
							OutputPinType = PinType;
							bFoundKey = true;
						}
						else
						{
							OutputPinValueType = FEdGraphTerminalType::FromPinType(PinType);
							bFoundValue = true;
						}
					}
				}
			}
		}
	}

	if (bFoundKey)
	{
		OutputPin->PinType = MoveTemp(OutputPinType);
	}
	else
	{
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		OutputPin->PinType.PinSubCategory = NAME_None;
		OutputPin->PinType.PinSubCategoryObject = nullptr;
	}

	if (bMapContainer)
	{
		if (bFoundValue)
		{
			OutputPin->PinType.PinValueType = MoveTemp(OutputPinValueType);
		}
		else
		{
			OutputPin->PinType.PinValueType.TerminalCategory = UEdGraphSchema_K2::PC_Wildcard;
			OutputPin->PinType.PinValueType.TerminalSubCategory = NAME_None;
			OutputPin->PinType.PinValueType.TerminalSubCategoryObject = nullptr;
		}
	}

	OutputPin->PinType.ContainerType = ContainerType;
	PropagatePinType();

	UK2Node::PostReconstructNode();
}

FText UK2Node_DistinctArray::GetKeywords() const
{
	return GetClass()->GetMetaDataText(TEXT("Keywords"), TEXT("UObjectKeywords"), GetClass()->GetFullGroupName(false));
}

void UK2Node_DistinctArray::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	UK2Node::NotifyPinConnectionListChanged(Pin);

	// Array to cache the input pins we might want to find these if we are removing the last link
	TArray<UEdGraphPin*> KeyPins;
	TArray<UEdGraphPin*> ValuePins;
	GetKeyAndValuePins(KeyPins, ValuePins);

	auto CountLinkedPins = [](const TArray<UEdGraphPin*> PinsToCount)
	{
		int32 LinkedPins = 0;
		for (UEdGraphPin* CurrentPin : PinsToCount)
		{
			if (CurrentPin->LinkedTo.Num() > 0)
			{
				++LinkedPins;
			}
		}
		return LinkedPins;
	};

	// Was this the first or last connection?
	const int32 NumKeyPinsWithLinks = CountLinkedPins(KeyPins);
	const int32 NumValuePinsWithLinks = CountLinkedPins(ValuePins);

	UEdGraphPin* OutputPin = GetOutputPin();

	bool bNotifyGraphChanged = false;
	if (Pin->LinkedTo.Num() > 0)
	{
		if (Pin->ParentPin == nullptr)
		{
			if (Pin == OutputPin)
			{
				if (NumKeyPinsWithLinks == 0 && (OutputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard && Pin->LinkedTo[0]->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard))
				{
					FEdGraphTerminalType TerminalType = MoveTemp(OutputPin->PinType.PinValueType);
					OutputPin->PinType = Pin->LinkedTo[0]->PinType;
					OutputPin->PinType.PinValueType = MoveTemp(TerminalType);
					OutputPin->PinType.ContainerType = ContainerType;
					bNotifyGraphChanged = true;
				}
				if (ContainerType == EPinContainerType::Map && NumValuePinsWithLinks == 0 && (OutputPin->PinType.PinValueType.TerminalCategory == UEdGraphSchema_K2::PC_Wildcard && Pin->LinkedTo[0]->PinType.PinValueType.TerminalCategory != UEdGraphSchema_K2::PC_Wildcard))
				{
					OutputPin->PinType.PinValueType = Pin->LinkedTo[0]->PinType.PinValueType;
					bNotifyGraphChanged = true;
				}
			}
			else if (ValuePins.Contains(Pin))
			{
				// Just made a connection to a value pin, was it the first?
				if (NumValuePinsWithLinks == 1 && (OutputPin->PinType.PinValueType.TerminalCategory == UEdGraphSchema_K2::PC_Wildcard && Pin->LinkedTo[0]->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard))
				{
					// Update the types on all the pins
					OutputPin->PinType.PinValueType = FEdGraphTerminalType::FromPinType(Pin->LinkedTo[0]->PinType);
					bNotifyGraphChanged = true;
				}
			}
			else 
			{
				// Just made a connection to a key pin, was it the first?
				if (NumKeyPinsWithLinks == 1 && (OutputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard && Pin->LinkedTo[0]->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard))
				{
					FEdGraphTerminalType TerminalType = MoveTemp(OutputPin->PinType.PinValueType);
					OutputPin->PinType = Pin->LinkedTo[0]->PinType;
					OutputPin->PinType.PinValueType = MoveTemp(TerminalType);
					OutputPin->PinType.ContainerType = ContainerType;
					bNotifyGraphChanged = true;
				}
			}
		}
	}
	else if (OutputPin->LinkedTo.Num() == 0)
	{
		// Return to wildcard if theres nothing in any of the input pins
		TFunction<bool(TArray<UEdGraphPin*>&)> PinsInUse = [this, &PinsInUse](TArray<UEdGraphPin*>& PinsToConsider)
		{
			bool bPinInUse = false;
			for (UEdGraphPin* CurrentPin : PinsToConsider)
			{
				// Is there something in this pin?
				if (CurrentPin->SubPins.Num() > 0 || !CurrentPin->DoesDefaultValueMatchAutogenerated())
				{
					bPinInUse = true;
					break;
				}
			}
			return bPinInUse;
		};

		const bool bResetOutputPinPrimary = ((NumKeyPinsWithLinks == 0) && !PinsInUse(KeyPins));
		const bool bResetOutputPinSecondary = ((NumValuePinsWithLinks == 0) && !PinsInUse(ValuePins));

		if (bResetOutputPinPrimary)
		{
			OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
			OutputPin->PinType.PinSubCategory = NAME_None;
			OutputPin->PinType.PinSubCategoryObject = nullptr;

			bNotifyGraphChanged = true;
		}
		if (bResetOutputPinSecondary && ContainerType == EPinContainerType::Map)
		{
			OutputPin->PinType.PinValueType.TerminalCategory = UEdGraphSchema_K2::PC_Wildcard;
			OutputPin->PinType.PinValueType.TerminalSubCategory = NAME_None;
			OutputPin->PinType.PinValueType.TerminalSubCategoryObject = nullptr;

			bNotifyGraphChanged = true;
		}
	}

	if (bNotifyGraphChanged)
	{
		PropagatePinType();
		GetGraph()->NotifyGraphChanged();
	}
}

void UK2Node_DistinctArray::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	UK2Node::GetNodeContextMenuActions(Menu, Context);

	if (!Context->bIsDebugging)
	{
		FToolMenuSection& Section = Menu->AddSection("K2NodeMakeArray", NSLOCTEXT("K2Nodes", "DistinctArrayHeader", "DistinctArray"));

		/* TODO
		if (Context->Pin != NULL)
		{
			if (Context->Pin->Direction == EGPD_Input && Context->Pin->ParentPin == nullptr)
			{
				Section.AddMenuEntry(
					"RemovePin",
					LOCTEXT("RemovePin", "Remove array element pin"),
					LOCTEXT("RemovePinTooltip", "Remove this array element pin"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateUObject(const_cast<UK2Node_DistinctArray*>(this), &UK2Node_DistinctArray::RemoveInputPin, const_cast<UEdGraphPin*>(Context->Pin))
					)
				);
			}
		}
		else
		{
			Section.AddMenuEntry(
				"AddPin",
				LOCTEXT("AddPin", "Add array element pin"),
				LOCTEXT("AddPinTooltip", "Add another array element pin"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject(const_cast<UK2Node_DistinctArray*>(this), &UK2Node_DistinctArray::InteractiveAddInputPin)
				)
			);
		}

		Section.AddMenuEntry(
			"ResetToWildcard",
			LOCTEXT("ResetToWildcard", "Reset to wildcard"),
			LOCTEXT("ResetToWildcardTooltip", "Reset the node to have wildcard input/outputs. Requires no pins are connected."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateUObject(const_cast<UK2Node_DistinctArray*>(this), &UK2Node_DistinctArray::ClearPinTypeToWildcard),
				FCanExecuteAction::CreateUObject(this, &UK2Node_DistinctArray::CanResetToWildcard)
			)
		);

		*/
	}
}

FNodeHandlingFunctor* UK2Node_DistinctArray::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_DistinctArray(CompilerContext);
}

FText UK2Node_DistinctArray::GetMenuCategory() const
{
	return LOCTEXT("DistinctArray_MenuCategory", "Mountea Inventory & Equipment");
}

void UK2Node_DistinctArray::AllocateDefaultPins()
{
	// Create the output pin
	UEdGraphNode::FCreatePinParams PinParams;
	PinParams.ContainerType = EPinContainerType::Array;
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, GetOutputPinName(), PinParams);

	// Create the input pins to create the container from
	for (int32 i = 0; i < NumInputs; ++i)
	{
		UEdGraphNode::FCreatePinParams InputPinParams;
		InputPinParams.ContainerType = EPinContainerType::Array;
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, GetPinName(i), InputPinParams);
	}
}

void UK2Node_DistinctArray::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_DistinctArray::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	UK2Node::ReallocatePinsDuringReconstruction(OldPins);

	// This is necessary to retain type information after pasting or loading from disc
	if (UEdGraphPin* OutputPin = GetOutputPin())
	{
		// Only update the output pin if it is currently a wildcard
		if (OutputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
		{
			// Find the matching Old Pin if it exists
			for (UEdGraphPin* OldPin : OldPins)
			{
				if (OldPin->Direction == EGPD_Output)
				{
					// Update our output pin with the old type information and then propagate it to our input pins
					OutputPin->PinType = OldPin->PinType;
					PropagatePinType();
					break;
				}
			}
		}
	}
}

bool UK2Node_DistinctArray::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (!ensure(OtherPin))
	{
		return true;
	}

	// if MyPin has a ParentPin then we are dealing with a split pin and we should evaluate it with default behavior
	if (MyPin->ParentPin == nullptr && !OtherPin->PinType.IsContainer() == true && MyPin->Direction == EGPD_Input)
	{
		OutReason = NSLOCTEXT("K2Node", "DistinctArray_InputIsContainer", "Cannot make a container with an input of a container!").ToString();
		return true;
	}

	if (UEdGraphSchema_K2::IsExecPin(*OtherPin))
	{
		OutReason = NSLOCTEXT("K2Node", "DistinctArray_InputIsExec", "Cannot make a container with an execution input!").ToString();
		return true;
	}

	return false;
}

FName UK2Node_DistinctArray::GetPinName(const int32 PinIndex) const
{
	return *FString::Printf(TEXT("Array [%d]"), PinIndex);
}

FName UK2Node_DistinctArray::GetOutputPinName() const
{
	return FName("Distinct Array");
}

UEdGraphPin* UK2Node_DistinctArray::GetOutputPin() const
{
	return FindPin(GetOutputPinName());
}

void UK2Node_DistinctArray::GetKeyAndValuePins(TArray<UEdGraphPin*>& KeyPins, TArray<UEdGraphPin*>& ValuePins) const
{
	for (UEdGraphPin* CurrentPin : Pins)
	{
		if (CurrentPin->Direction == EGPD_Input && CurrentPin->ParentPin == nullptr)
		{
			KeyPins.Add(CurrentPin);
		}
	}
}

void UK2Node_DistinctArray::PropagatePinType()
{
	const UEdGraphPin* OutputPin = GetOutputPin();

	if (OutputPin)
	{
		UClass const* CallingContext = nullptr;
		if (UBlueprint const* Blueprint = GetBlueprint())
		{
			CallingContext = Blueprint->GeneratedClass;
			if (CallingContext == nullptr)
			{
				CallingContext = Blueprint->ParentClass;
			}
		}

		TArray<UEdGraphPin*> KeyPins;
		TArray<UEdGraphPin*> ValuePins;
		GetKeyAndValuePins(KeyPins, ValuePins);

		// Propagate pin type info (except for array info!) to pins with dependent types
		const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
		auto PropagateToPin = [Schema](UEdGraphPin* CurrentPin, const FEdGraphPinType& PinType)
		{
			// if we've reset to wild card or the parent pin no longer matches we need to collapse the split pin(s)
			// otherwise everything should be OK:
			if (CurrentPin->SubPins.Num() != 0 &&
				(CurrentPin->PinType.PinCategory != PinType.PinCategory ||
				 CurrentPin->PinType.PinSubCategory != PinType.PinSubCategory ||
				 CurrentPin->PinType.PinSubCategoryObject != PinType.PinSubCategoryObject)
				)
			{
				Schema->RecombinePin(CurrentPin->SubPins[0]);
			}

			CurrentPin->PinType.PinCategory = PinType.PinCategory;
			CurrentPin->PinType.PinSubCategory = PinType.PinSubCategory;
			CurrentPin->PinType.PinSubCategoryObject = PinType.PinSubCategoryObject;
		};

		for (UEdGraphPin* CurrentKeyPin : KeyPins)
		{
			PropagateToPin(CurrentKeyPin, OutputPin->PinType);
		}

		if (ValuePins.Num() > 0)
		{
			const FEdGraphPinType ValuePinType = FEdGraphPinType::GetPinTypeForTerminalType(OutputPin->PinType.PinValueType);
			for (UEdGraphPin* CurrentValuePin : ValuePins)
			{
				PropagateToPin(CurrentValuePin, ValuePinType);
			}
		}

		for (UEdGraphPin* CurrentPin : Pins)
		{
			if (CurrentPin && CurrentPin != OutputPin)
			{
				if (CurrentPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard || CurrentPin->GetDefaultAsString().IsEmpty() == true)
				{
					// Only reset default value if there isn't one set or it is a wildcard. Otherwise this deletes data!
					Schema->SetPinAutogeneratedDefaultValueBasedOnType(CurrentPin);
				}

				// Verify that all previous connections to this pin are still valid with the new type
				TArray<UEdGraphPin*> LinkedToCopy = CurrentPin->LinkedTo;
				for (UEdGraphPin* ConnectedPin : LinkedToCopy)
				{
					if (!Schema->ArePinsCompatible(CurrentPin, ConnectedPin, CallingContext))
					{
						CurrentPin->BreakLinkTo(ConnectedPin);
					}
				}
			}
		}
		// If we have a valid graph we should refresh it now to refelect any changes we made
		if (UEdGraphNode* OwningNode = OutputPin->GetOwningNode())
		{
			if (UEdGraph* Graph = OwningNode->GetGraph())
			{
				Graph->NotifyGraphChanged();
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
