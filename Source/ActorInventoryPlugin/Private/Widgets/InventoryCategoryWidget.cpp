// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Widgets/InventoryCategoryWidget.h"

#include "Definitions/InventoryCategory.h"
#include "Widgets/InventoryWidget.h"

void UInventoryCategoryWidget::OnCategorySelected_Implementation()
{
	if (OwningInventory && ParentCategory)
	{
		OwningInventory->SetSelectedCategory(ParentCategory->GetCategoryGUID());
	}
}

void UInventoryCategoryWidget::OnSelectedCategoryUpdated_Implementation(const FGuid& SelectedCategoryGUID)
{
	//...
}
