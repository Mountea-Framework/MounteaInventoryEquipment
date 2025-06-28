// Fill out your copyright notice in the Description page of Project Settings.


#include "MounteaAdvancedInventoryComponent_Factory.h"

#include "Components/MounteaInventoryComponent.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedInventoryComponent_Factory::UMounteaAdvancedInventoryComponent_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryComponent::StaticClass();
}

UObject* UMounteaAdvancedInventoryComponent_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
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

bool UMounteaAdvancedInventoryComponent_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Component Blueprint"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}
