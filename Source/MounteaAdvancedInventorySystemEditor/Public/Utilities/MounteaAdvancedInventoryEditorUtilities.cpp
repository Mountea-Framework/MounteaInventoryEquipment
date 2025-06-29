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

#include "MounteaAdvancedInventoryEditorUtilities.h"

#include "ClassViewerModule.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Event.h"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"

bool FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class)
{
	// Create filter
	TSharedPtr<FMounteaInventoryClassViewerFilter> Filter = MakeShareable(new FMounteaInventoryClassViewerFilter);
	Filter->AllowedChildrenOfClasses.Add(Class);

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.bShowUnloadedBlueprints = true;
	Options.ClassFilters.Add(Filter.ToSharedRef());

	Options.DisplayMode = EClassViewerDisplayMode::TreeView;
	
	Options.bShowNoneOption = false;
	Options.InitiallySelectedClass = Class;

	Options.bExpandRootNodes = true;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;
	
	return SClassPickerDialog::PickClass(TitleText, Options, OutChosenClass, Class);
}

bool FMounteaAdvancedInventoryEditorUtilities::OpenBlueprintEditor(UBlueprint* Blueprint, EMounteaInventoryBlueprintOpenType OpenType, FName FunctionNameToOpen, bool bForceFullEditor, bool bAddBlueprintFunctionIfItDoesNotExist)
{
	if (!Blueprint)
	{
		return false;
	}

	Blueprint->bForceFullEditor = bForceFullEditor;

	// Find Function Graph
	UObject* ObjectToFocusOn = nullptr;
	if (OpenType != EMounteaInventoryBlueprintOpenType::None && FunctionNameToOpen != NAME_None)
	{
		UClass* Class = Blueprint->GeneratedClass;
		check(Class);

		if (OpenType == EMounteaInventoryBlueprintOpenType::Function)
		{
			ObjectToFocusOn = bAddBlueprintFunctionIfItDoesNotExist
				? BlueprintGetOrAddFunction(Blueprint, FunctionNameToOpen, Class)
				: BlueprintGetFunction(Blueprint, FunctionNameToOpen, Class);
		}
		else if (OpenType == EMounteaInventoryBlueprintOpenType::Event)
		{
			ObjectToFocusOn = bAddBlueprintFunctionIfItDoesNotExist
				? BlueprintGetOrAddEvent(Blueprint, FunctionNameToOpen, Class)
				: BlueprintGetEvent(Blueprint, FunctionNameToOpen, Class);
		}
	}

	// Default to the last uber graph
	if (ObjectToFocusOn == nullptr)
	{
		ObjectToFocusOn = Blueprint->GetLastEditedUberGraph();
	}
	if (ObjectToFocusOn)
	{
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ObjectToFocusOn);
		return true;
	}

	return OpenEditorForAsset(Blueprint);
}

UEdGraph* FMounteaAdvancedInventoryEditorUtilities::BlueprintGetOrAddFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	// Find existing function
	if (UEdGraph* GraphFunction = BlueprintGetFunction(Blueprint, FunctionName, FunctionClassSignature))
	{
		return GraphFunction;
	}

	// Create a new function
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	FBlueprintEditorUtils::AddFunctionGraph(Blueprint, NewGraph, /*bIsUserCreated=*/ false, FunctionClassSignature);
	Blueprint->LastEditedDocuments.Add(NewGraph);
	return NewGraph;
}

UEdGraph* FMounteaAdvancedInventoryEditorUtilities::BlueprintGetFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	// Find existing function
	for (UEdGraph* GraphFunction : Blueprint->FunctionGraphs)
	{
		if (FunctionName == GraphFunction->GetFName())
		{
			return GraphFunction;
		}
	}

	// Find in the implemented Interfaces Graphs
	for (const FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
	{
		for (UEdGraph* GraphFunction : Interface.Graphs)
		{
			if (FunctionName == GraphFunction->GetFName())
			{
				return GraphFunction;
			}
		}
	}

	return nullptr;
}

UK2Node_Event* FMounteaAdvancedInventoryEditorUtilities::BlueprintGetOrAddEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	// Find existing event
	if (UK2Node_Event* EventNode = BlueprintGetEvent(Blueprint, EventName, EventClassSignature))
	{
		return EventNode;
	}

	// Create a New Event
	if (Blueprint->UbergraphPages.Num())
	{
		int32 NodePositionY = 0;
		UK2Node_Event* NodeEvent = FKismetEditorUtilities::AddDefaultEventNode(
			Blueprint,
			Blueprint->UbergraphPages[0],
			EventName,
			EventClassSignature,
			NodePositionY
		);
		NodeEvent->SetEnabledState(ENodeEnabledState::Enabled);
		NodeEvent->NodeComment = "";
		NodeEvent->bCommentBubbleVisible = false;
		return NodeEvent;
	}

	return nullptr;
}

UK2Node_Event* FMounteaAdvancedInventoryEditorUtilities::BlueprintGetEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	TArray<UK2Node_Event*> AllEvents;
	FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_Event>(Blueprint, AllEvents);
	for (UK2Node_Event* EventNode : AllEvents)
	{
		if (EventNode->bOverrideFunction && EventNode->EventReference.GetMemberName() == EventName)
		{
			return EventNode;
		}
	}

	return nullptr;
}

bool FMounteaAdvancedInventoryEditorUtilities::OpenEditorForAsset(const UObject* Asset)
{
	if (!IsValid(Asset) || !GEditor)
	{
		return false;
	}

	return GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(const_cast<UObject*>(Asset));
}