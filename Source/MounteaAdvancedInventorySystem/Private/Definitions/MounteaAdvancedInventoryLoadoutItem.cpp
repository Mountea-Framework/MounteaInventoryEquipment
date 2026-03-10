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


#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Statics/MounteaEquipmentStatics.h"

UMounteaAdvancedInventoryLoadoutItem::UMounteaAdvancedInventoryLoadoutItem() : 
	bUseRandomQuantity(false),
	bAutomaticallyEquip(true)
{
#if WITH_EDITORONLY_DATA
	DisplayName = IsValid(ItemTemplate) ? FName(ItemTemplate->DisplayName.ToString()) : NAME_None;
#endif
}

TArray<FName> UMounteaAdvancedInventoryLoadoutItem::GetAvailableSlotNames()
{
	const auto equipmentConfig = UMounteaEquipmentStatics::GetEquipmentSettingsConfig();
	if (!IsValid(equipmentConfig))
		return {};
	
	TArray<FName> returnValue;
	returnValue.Reserve(equipmentConfig->AllowedEquipmentSlots.Num() + 1);
	returnValue.Add(NAME_None);
	
	TArray<FName> definedSlots;
	definedSlots.Reserve(equipmentConfig->AllowedEquipmentSlots.Num());
	equipmentConfig->AllowedEquipmentSlots.GetKeys(definedSlots);
	
	returnValue.Append(definedSlots);
	return returnValue;
}

void UMounteaAdvancedInventoryLoadoutItem::ApplyDefaults()
{
	ItemTemplate = nullptr;
	RandomRange = FIntPoint(1, 100);
	BaseQuantity = 1;
	bUseRandomQuantity = false;
	bAutomaticallyEquip = true;
	EquipmentSlot = NAME_None;
	DisplayName = NAME_None;
}

#if WITH_EDITOR
void UMounteaAdvancedInventoryLoadoutItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventoryLoadoutItem, ItemTemplate))
	{
		if (!IsValid(ItemTemplate))
		{
			ApplyDefaults();
		}
		else
		{
			BaseQuantity = ItemTemplate->MaxQuantity;
			if (!ItemTemplate->AttachmentSlots.IsEmpty())
			{
				bAutomaticallyEquip = true;				
				EquipmentSlot = UMounteaEquipmentStatics::ResolveBestSlotIdFromTags(ItemTemplate->AttachmentSlots, ItemTemplate->EquipmentItemType);
			}
			DisplayName = FName(ItemTemplate->DisplayName.ToString());
		}
	}
}
#endif