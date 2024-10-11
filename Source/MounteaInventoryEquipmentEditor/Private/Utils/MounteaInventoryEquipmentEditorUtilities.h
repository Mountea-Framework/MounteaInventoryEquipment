// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "K2Node_Event.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"

enum class EMounteaInventoryEquipmentBlueprintOpenType : uint8
{
	None = 0,
	Function,
	Event
};

class FMounteaInventoryEquipmentViewerFilter : public IClassViewerFilter
{
	
public:
	// All children of these classes will be included unless filtered out by another setting.
	TSet<const UClass*> AllowedChildrenOfClasses;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
	
	// Disallowed class flags.
	EClassFlags DisallowedClassFlags = CLASS_Deprecated;
};

class FMounteaInventoryEquipmentEditorUtilities
{
	
public:

	static bool PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class);
	
	static bool IsABlueprintClass(const UClass* Class)
	{
		return Cast<UBlueprintGeneratedClass>(Class) != nullptr;
	}

	static bool GetAllChildClassesOf(const UClass* ParentClass, TArray<UClass*>& OutNativeClasses, TArray<UClass*>& OutBlueprintClasses)
	{
		// Iterate over UClass, this might be heavy on performance
		for (TObjectIterator<UClass> It; It; ++It)
		{
			UClass* ChildClass = *It;
			if (!ChildClass->IsChildOf(ParentClass))
			{
				continue;
			}

			// It is a child of the Parent Class
			// make sure we don't include our parent class in the array
			if (ChildClass == ParentClass)
			{
				continue;
			}

			if (IsABlueprintClass(ChildClass))
			{
				// Blueprint
				OutBlueprintClasses.Add(ChildClass);
			}
			else
			{
				// Native
				OutNativeClasses.Add(ChildClass);
			}
		}

		return OutNativeClasses.Num() > 0 || OutBlueprintClasses.Num() > 0;
	}

	static bool OpenBlueprintEditor(UBlueprint* Blueprint, EMounteaInventoryEquipmentBlueprintOpenType OpenType, FName FunctionNameToOpen, bool bForceFullEditor, bool bAddBlueprintFunctionIfItDoesNotExist);
	
	static UEdGraph* BlueprintGetOrAddFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature);
	
	static UEdGraph* BlueprintGetFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature);
	
	static UK2Node_Event* BlueprintGetOrAddEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature);
	
	static UK2Node_Event* BlueprintGetEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature);

	static bool OpenEditorForAsset(const UObject* Asset);
};

inline bool FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class)
{
	// Create filter
	const TSharedPtr<FMounteaInventoryEquipmentViewerFilter> Filter = MakeShareable(new FMounteaInventoryEquipmentViewerFilter);
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

	Options.bShowDefaultClasses = false;
	Options.bShowObjectRootClass = false;
	Options.ExtraPickerCommonClasses.Add(Class);

	Options.bShowBackgroundBorder = true;
	
	return SClassPickerDialog::PickClass(TitleText, Options, OutChosenClass, Class);
}

inline bool FMounteaInventoryEquipmentEditorUtilities::OpenBlueprintEditor(UBlueprint* Blueprint, EMounteaInventoryEquipmentBlueprintOpenType OpenType, FName FunctionNameToOpen, bool bForceFullEditor, bool bAddBlueprintFunctionIfItDoesNotExist)
{
	if (!Blueprint)
	{
		return false;
	}

	Blueprint->bForceFullEditor = bForceFullEditor;

	// Find Function Graph
	UObject* ObjectToFocusOn = nullptr;
	if (OpenType != EMounteaInventoryEquipmentBlueprintOpenType::None && FunctionNameToOpen != NAME_None)
	{
		UClass* Class = Blueprint->GeneratedClass;
		check(Class);

		if (OpenType == EMounteaInventoryEquipmentBlueprintOpenType::Function)
		{
			ObjectToFocusOn = bAddBlueprintFunctionIfItDoesNotExist
				? BlueprintGetOrAddFunction(Blueprint, FunctionNameToOpen, Class)
				: BlueprintGetFunction(Blueprint, FunctionNameToOpen, Class);
		}
		else if (OpenType == EMounteaInventoryEquipmentBlueprintOpenType::Event)
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

inline UEdGraph* FMounteaInventoryEquipmentEditorUtilities::BlueprintGetOrAddFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature)
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

inline UEdGraph* FMounteaInventoryEquipmentEditorUtilities::BlueprintGetFunction(UBlueprint* Blueprint, FName FunctionName, UClass* FunctionClassSignature)
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

inline UK2Node_Event* FMounteaInventoryEquipmentEditorUtilities::BlueprintGetOrAddEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature)
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

inline UK2Node_Event* FMounteaInventoryEquipmentEditorUtilities::BlueprintGetEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature)
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

inline bool FMounteaInventoryEquipmentEditorUtilities::OpenEditorForAsset(const UObject* Asset)
{
	if (!IsValid(Asset) || !GEditor)
	{
		return false;
	}

	return GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(const_cast<UObject*>(Asset));
}
