// All rights reserved Dominik Pavlicek 2022.


#include "ActorInventoryEditorUtilities.h"

#include "Factories/ActorInventoryClassViewerFilter.h"
#include "Kismet2/SClassPickerDialog.h"

bool FActorInventoryEditorUtilities::PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class)
{
	// Create filter
	TSharedPtr<FActorInventoryClassViewerFilter> Filter = MakeShareable(new FActorInventoryClassViewerFilter);
	Filter->AllowedChildrenOfClasses.Add(Class);

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ClassFilter = Filter;
	Options.bShowUnloadedBlueprints = true;
	Options.bExpandRootNodes = true;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::Dynamic;
	
	return SClassPickerDialog::PickClass(TitleText, Options, OutChosenClass, Class);
}
