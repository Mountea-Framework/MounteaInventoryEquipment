// All rights reserved Dominik Pavlicek 2023


#include "MounteaItemAssetFactory.h"


#include "Definitions/MounteaInventoryItem.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"
#include "Kismet2/KismetEditorUtilities.h"

UMounteaItemAssetFactory::UMounteaItemAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemBase::StaticClass();
}

UObject* UMounteaItemAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Something is not right!
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), FText::FromString(ParentClass->GetName())));
		return nullptr;
	}

	return NewObject<UMounteaInventoryItemBase>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaItemAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Item"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}
