// All rights reserved Dominik Pavlicek 2023.


#include "WBP/Inventory/MounteaInventoryBaseWidget.h"

void UMounteaInventoryBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(InventoryVisibility);
}

UMounteaInventoryThemeConfig* UMounteaInventoryBaseWidget::GetTheme_Implementation(bool& bResult) const
{
	bResult = InventoryTheme != nullptr;
	return InventoryTheme;
}

ESlateVisibility UMounteaInventoryBaseWidget::GetInventoryVisibility_Implementation() const
{
	return InventoryVisibility;
}

TScriptInterface<IMounteaInventoryInterface> UMounteaInventoryBaseWidget::GetOwningInventory_Implementation() const
{
	return OwningInventory;
}

void UMounteaInventoryBaseWidget::SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory)
{
	if (NewOwningInventory == OwningInventory) return;

	OwningInventory = NewOwningInventory;
}

void UMounteaInventoryBaseWidget::CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData)
{
	
}