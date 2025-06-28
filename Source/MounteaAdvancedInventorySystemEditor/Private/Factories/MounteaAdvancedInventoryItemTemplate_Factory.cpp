// Fill out your copyright notice in the Description page of Project Settings.


#include "MounteaAdvancedInventoryItemTemplate_Factory.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"


UMounteaAdvancedInventoryItemTemplate_Factory::UMounteaAdvancedInventoryItemTemplate_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemTemplate::StaticClass();
}

UObject* UMounteaAdvancedInventoryItemTemplate_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInventoryItemTemplate>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryItemTemplate_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Settings Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass
	(
		TitleText, ChosenClass, UMounteaInventoryItemTemplate::StaticClass()
	);

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}
