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

#if WITH_EDITOR

#include "Slate/SPinTypeEquipmentSlotSelector.h"

#include "Components/MounteaAttachmentContainerComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Statics/MounteaEquipmentStatics.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

void SPinTypeEquipmentSlotSelector::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);

	TArray<FName> slotNames;
	BuildSlotOptions(slotNames);

	SlotOptions.Empty(slotNames.Num());
	for (const FName& slotName : slotNames)
	{
		SlotOptions.Add(MakeShared<FString>(slotName.ToString()));
	}

	if (SlotOptions.Num() > 0 && GraphPinObj && GraphPinObj->GetDefaultAsString().IsEmpty() && GraphPinObj->LinkedTo.Num() == 0)
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *SlotOptions[0]);
}

TSharedRef<SWidget> SPinTypeEquipmentSlotSelector::GetDefaultValueWidget()
{
	return SNew(SBox)
		.Visibility(this, &SPinTypeEquipmentSlotSelector::GetDefaultValueVisibility)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.OptionsSource(&SlotOptions)
			.OnGenerateWidget(this, &SPinTypeEquipmentSlotSelector::GenerateSlotWidget)
			.OnSelectionChanged(this, &SPinTypeEquipmentSlotSelector::OnSlotSelected)
			[
				SNew(STextBlock)
				.Text(this, &SPinTypeEquipmentSlotSelector::GetSelectedSlotText)
				.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			]
		];
}

EVisibility SPinTypeEquipmentSlotSelector::GetDefaultValueVisibility() const
{
	return (GraphPinObj && GraphPinObj->LinkedTo.Num() > 0) ? EVisibility::Collapsed : EVisibility::Visible;
}

void SPinTypeEquipmentSlotSelector::OnSlotSelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid() || !GraphPinObj)
		return;

	const FString selectedSlot = *Selection;
	if (GraphPinObj->GetDefaultAsString() == selectedSlot)
		return;

	const FScopedTransaction transaction(NSLOCTEXT("EquipmentSlotPin", "ChangeSlot", "Change Equipment Slot"));
	GraphPinObj->Modify();
	GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, selectedSlot);
}

FText SPinTypeEquipmentSlotSelector::GetSelectedSlotText() const
{
	if (GraphPinObj)
		return FText::FromString(GraphPinObj->GetDefaultAsString());
	return FText::GetEmpty();
}

TSharedRef<SWidget> SPinTypeEquipmentSlotSelector::GenerateSlotWidget(TSharedPtr<FString> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*Item))
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

void SPinTypeEquipmentSlotSelector::BuildSlotOptions(TArray<FName>& OutSlots) const
{
	OutSlots.Reset();
	OutSlots.Add(NAME_None);

	TArray<FName> resolvedSlots;
	const bool bHasLocalSource = TryGetSlotsFromCurrentBlueprint(resolvedSlots);
	if (!bHasLocalSource)
		GetSlotsFromSettings(resolvedSlots);

	resolvedSlots.Sort(FNameLexicalLess());
	for (const FName slotName : resolvedSlots)
	{
		if (!slotName.IsNone())
			OutSlots.AddUnique(slotName);
	}
}

bool SPinTypeEquipmentSlotSelector::TryGetSlotsFromCurrentBlueprint(TArray<FName>& OutSlots) const
{
	if (!GraphPinObj || !GraphPinObj->GetOwningNode())
		return false;

	const UBlueprint* blueprint = FBlueprintEditorUtils::FindBlueprintForNode(GraphPinObj->GetOwningNode());
	if (!blueprint)
		return false;

	bool bHasLocalSource = false;
	const UClass* resolvedClass = blueprint->GeneratedClass ? blueprint->GeneratedClass : blueprint->SkeletonGeneratedClass;
	const UBlueprintGeneratedClass* resolvedBlueprintClass = Cast<UBlueprintGeneratedClass>(resolvedClass);

	// Blueprint class implements attachment container interface.
	if (resolvedClass)
	{
		if (resolvedClass->ImplementsInterface(UMounteaAdvancedAttachmentContainerInterface::StaticClass()))
		{
			bHasLocalSource = true;

			if (UObject* classDefaultObject = resolvedClass->GetDefaultObject())
			{
				const TArray<UMounteaAdvancedAttachmentSlot*> slots =
					IMounteaAdvancedAttachmentContainerInterface::Execute_GetAttachmentSlots(classDefaultObject);

				for (const UMounteaAdvancedAttachmentSlot* slot : slots)
				{
					if (IsValid(slot) && !slot->SlotName.IsNone())
						OutSlots.AddUnique(slot->SlotName);
				}
			}
		}
	}

	// Blueprint owns one or more attachment container components.
	if (resolvedBlueprintClass && blueprint->SimpleConstructionScript)
	{
		const USimpleConstructionScript* simpleConstructionScript = blueprint->SimpleConstructionScript;
		for (const USCS_Node* node : simpleConstructionScript->GetAllNodes())
		{
			if (!node)
				continue;

			const UActorComponent* componentTemplate = node->GetActualComponentTemplate(
				const_cast<UBlueprintGeneratedClass*>(resolvedBlueprintClass));
			const auto* attachmentContainer = Cast<UMounteaAttachmentContainerComponent>(componentTemplate);
			if (!attachmentContainer)
				continue;

			bHasLocalSource = true;
			for (const UMounteaAdvancedAttachmentSlot* slot : attachmentContainer->AttachmentSlots)
			{
				if (IsValid(slot) && !slot->SlotName.IsNone())
					OutSlots.AddUnique(slot->SlotName);
			}
		}
	}

	return bHasLocalSource;
}

void SPinTypeEquipmentSlotSelector::GetSlotsFromSettings(TArray<FName>& OutSlots)
{
	const UMounteaAdvancedEquipmentSettingsConfig* equipmentConfig = UMounteaEquipmentStatics::GetEquipmentSettingsConfig();
	if (!equipmentConfig)
		return;

	TArray<FName> slotNames;
	equipmentConfig->AllowedEquipmentSlots.GetKeys(slotNames);

	for (const FName slotName : slotNames)
	{
		if (!slotName.IsNone())
			OutSlots.AddUnique(slotName);
	}
}

#endif
