// All rights reserved Dominik Morse (Pavlicek) 2024


#include "MounteaInventoryItemBlueprintFactory.h"

#include "Definitions/MounteaInventoryItemBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"

UMounteaInventoryItemBlueprintFactory::UMounteaInventoryItemBlueprintFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemBlueprint::StaticClass();
}

UObject* UMounteaInventoryItemBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (ParentClass == nullptr)
	{
		ParentClass = UMounteaInventoryItemBlueprint::StaticClass();
	}
	
	// Something is not right!
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), FText::FromString(ParentClass->GetName())));
		return nullptr;
	}

	return NewObject<UMounteaInventoryItemBlueprint>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaInventoryItemBlueprintFactory::ConfigureProperties()
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
