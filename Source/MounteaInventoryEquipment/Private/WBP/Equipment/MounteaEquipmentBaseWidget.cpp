// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/Equipment/MounteaEquipmentBaseWidget.h"

#include "Interfaces/UI/MounteaEquipmentSlotWBPInterface.h"

void UMounteaEquipmentBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TScriptInterface<IMounteaEquipmentInterface> UMounteaEquipmentBaseWidget::GetOwningEquipment_Implementation() const
{
	return OwningEquipment;
}

void UMounteaEquipmentBaseWidget::SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentInterface>& NewOwningEquipment)
{
	if (NewOwningEquipment == OwningEquipment) return;

	OwningEquipment = NewOwningEquipment;
}

TArray<UUserWidget*> UMounteaEquipmentBaseWidget::GetEquipmentSlotsWidgets_Implementation() const
{
	return EquipmentSlotWidgets;
}

void UMounteaEquipmentBaseWidget::ResetEquipmentSlotsWidgets_Implementation()
{
	EquipmentSlotWidgets.Empty();
}

void UMounteaEquipmentBaseWidget::AddEquipmentSlotsWidget_Implementation(UUserWidget* NewSlot)
{
	if (NewSlot == nullptr) return;

	if (NewSlot->Implements<UMounteaEquipmentSlotWBPInterface>() == false) return;

	if (EquipmentSlotWidgets.Contains(NewSlot)) return;

	EquipmentSlotWidgets.AddUnique(NewSlot);

	const TScriptInterface<IMounteaEquipmentSlotWBPInterface> ChildEquipmentSlot = NewSlot;
	if (ChildEquipmentSlot.GetObject())
	{
		ChildEquipmentSlot->Execute_SetOwningEquipment(NewSlot, this);
	}
}

void UMounteaEquipmentBaseWidget::SetEquipmentSlotsWidgets_Implementation(const TArray<UUserWidget*>& NewSlots)
{
	for (UUserWidget* Itr : NewSlots)
	{
		Execute_AddEquipmentSlotsWidget(this, Itr);
	}
}
