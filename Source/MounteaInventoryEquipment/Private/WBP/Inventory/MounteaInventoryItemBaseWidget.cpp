// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/Inventory/MounteaInventoryItemBaseWidget.h"

#include "Helpers/BlueprintFunctionLibraries/MounteaInventoryEquipmentBFL.h"
#include "Interfaces/UI/MounteaInventoryWBPInterface.h"
#include "Settings/MounteaInventoryThemeConfig.h"

TScriptInterface<IMounteaInventoryWBPInterface> UMounteaInventoryItemBaseWidget::GetOwningInventory_Implementation() const
{
	return OwningInventory;
}

void UMounteaInventoryItemBaseWidget::SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory)
{
	if (OwningInventory == NewOwningInventory) return;

	OwningInventory = NewOwningInventory;
}

FItemSlot UMounteaInventoryItemBaseWidget::GetItem_Implementation() const
{
	return ItemSlot;
}

UMounteaBaseUserWidget* UMounteaInventoryItemBaseWidget::GetItemUI_Implementation()
{
	return this;
}

FIntPoint UMounteaInventoryItemBaseWidget::GetRootCoords_Implementation() const
{
	return SlotCoordinates;
}

FIntPoint UMounteaInventoryItemBaseWidget::GetItemDimensions_Implementation() const
{
	return SlotDimensions;
}

bool UMounteaInventoryItemBaseWidget::SaveRootCoords_Implementation(const FIntPoint& NewCoords)
{
	if (NewCoords == SlotCoordinates) return false;

	SlotCoordinates = NewCoords;

	return true;
}

bool UMounteaInventoryItemBaseWidget::MoveToNewCoords_Implementation(const FIntPoint& NewCoords)
{
	if (NewCoords == SlotCoordinates) return false;

	SlotCoordinates = NewCoords;

	return true;
}

bool UMounteaInventoryItemBaseWidget::ReleaseOldCoords_Implementation(const FIntPoint& OldCoords)
{
	return true;
}

bool UMounteaInventoryItemBaseWidget::CanBeDragged_Implementation() const
{
	const UMounteaInventoryEquipmentSettings* Settings = UMounteaInventoryEquipmentBFL::GetSettings();
	if (Settings == nullptr) return false;

	const auto InventoryUI = Execute_GetOwningInventory(this);
	if (InventoryUI.GetObject() == nullptr) return false;

	bool bThemeFound;
	const UMounteaInventoryThemeConfig* Theme = InventoryUI->Execute_GetTheme(InventoryUI.GetObject(), bThemeFound);

	if (Theme == nullptr) return false;

	const UInventoryTheme* InventoryTheme = Theme->GetInventoryTheme();
	if (InventoryTheme == nullptr) return false;

	return InventoryTheme->bDragDropAllowed;
}

void UMounteaInventoryItemBaseWidget::OnDropped_Implementation()
{
}

void UMounteaInventoryItemBaseWidget::OnDragged_Implementation()
{
}
