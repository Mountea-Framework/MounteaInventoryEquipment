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


#include "MounteaAdvancedEquipmentComponent_Factory.h"

#include "Components/MounteaEquipmentComponent.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedEquipmentComponent_Factory::UMounteaAdvancedEquipmentComponent_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaEquipmentComponent::StaticClass();
}

UObject* UMounteaAdvancedEquipmentComponent_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Something is not right!
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), FText::FromString(ParentClass->GetName())));
		return nullptr;
	}

	// Create new Blueprint
	auto CreatedBlueprint = FKismetEditorUtilities::CreateBlueprint(
		ParentClass,
		InParent,
		Name,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		NAME_None
	);

	CreatedBlueprint->HideCategories.Add("Hidden");
	CreatedBlueprint->HideCategories.Add("Private");
	CreatedBlueprint->HideCategories.Add("Base");
	CreatedBlueprint->HideCategories.Add("Hide");
	CreatedBlueprint->HideCategories.Add("Editor");

	CreatedBlueprint->BlueprintCategory = FString(TEXT("Mountea Inventory & Equipment"));

	return CreatedBlueprint;
}

bool UMounteaAdvancedEquipmentComponent_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Equipment Component Blueprint"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}
