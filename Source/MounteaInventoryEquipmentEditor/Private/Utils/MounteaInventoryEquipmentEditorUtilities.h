// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "ClassViewerFilter.h"
#include "K2Node_Event.h"

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

private:
	// Disallowed class flags.
	EClassFlags DisallowedClassFlags = CLASS_Deprecated;
};

class FMounteaInventoryEquipmentEditorUtilities
{
	
public:
	
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
