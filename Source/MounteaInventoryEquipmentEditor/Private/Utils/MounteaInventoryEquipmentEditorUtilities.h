// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "K2Node_Event.h"
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
