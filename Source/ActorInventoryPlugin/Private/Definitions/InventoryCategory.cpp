// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Definitions/InventoryCategory.h"


#if WITH_EDITOR
void UInventoryCategory::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == "ParentCategory")
	{
		if (CategoryData.ParentCategory == this)
		{
			CategoryData.ParentCategory = nullptr;
		}
	}
}

#endif
