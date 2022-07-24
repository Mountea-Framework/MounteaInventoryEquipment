// Fill out your copyright notice in the Description page of Project Settings.


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
