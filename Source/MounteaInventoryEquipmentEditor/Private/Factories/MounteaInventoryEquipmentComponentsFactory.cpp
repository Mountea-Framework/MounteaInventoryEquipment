// All rights reserved Dominik Pavlicek 2023


#include "MounteaInventoryEquipmentComponentsFactory.h"

#include "Components/MounteaEquipmentComponent.h"
#include "Components/MounteaInventoryComponent.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"

UMounteaInventoryComponentFactory::UMounteaInventoryComponentFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryComponent::StaticClass();
}

UObject* UMounteaInventoryComponentFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (ParentClass == nullptr)
	{
		ParentClass = UMounteaInventoryComponent::StaticClass();
	}
	
	// Something is not right!
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), FText::FromString(ParentClass->GetName())));
		return nullptr;
	}

	// Create new Blueprint
	const auto CreatedBlueprint = FKismetEditorUtilities::CreateBlueprint(
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

bool UMounteaInventoryComponentFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Component"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}

UMounteaEquipmentComponentFactory::UMounteaEquipmentComponentFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaEquipmentComponent::StaticClass();
}

UObject* UMounteaEquipmentComponentFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (ParentClass == nullptr)
	{
		ParentClass = UMounteaEquipmentComponent::StaticClass();
	}
	
	// Something is not right!
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), FText::FromString(ParentClass->GetName())));
		return nullptr;
	}

	// Create new Blueprint
	const auto CreatedBlueprint = FKismetEditorUtilities::CreateBlueprint(
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

bool UMounteaEquipmentComponentFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Equipment Component"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}
