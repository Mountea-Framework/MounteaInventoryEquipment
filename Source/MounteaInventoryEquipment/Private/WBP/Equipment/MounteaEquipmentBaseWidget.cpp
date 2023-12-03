// All rights reserved Dominik Pavlicek 2023.


#include "WBP/Equipment/MounteaEquipmentBaseWidget.h"

#include "Interfaces/UI/MounteaEquipmentSlotWBPInterface.h"

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
}

void UMounteaEquipmentBaseWidget::SetEquipmentSlotsWidgets_Implementation(const TArray<UUserWidget*>& NewSlots)
{
	for (UUserWidget* Itr : NewSlots)
	{
		Execute_AddEquipmentSlotsWidget(this, Itr);
	}
}
