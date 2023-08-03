// All rights reserved Dominik Pavlicek 2023.


#include "Definitions/MounteaEquipmentSlot.h"

#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

void UMounteaEquipmentSlot::OnRep_Slot()
{
	RepKey = 0;
}

TArray<FString> UMounteaEquipmentSlot::GetSlotIDs() const
{
	TArray<FString> ReturnValues;
	if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
	{
		Settings->EquipmentSlotIDs.GetKeys(ReturnValues);
	}

	return ReturnValues;
}

void UMounteaEquipmentSlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaEquipmentSlot, ItemInSlot);
	DOREPLIFETIME_CONDITION(UMounteaEquipmentSlot, OwningEquipment, COND_InitialOrOwner);
}

bool UMounteaEquipmentSlot::IsSupportedForNetworking() const
{
	return true;
}

void UMounteaEquipmentSlot::MarkDirtyForReplication()
{
	RepKey++;
}

#if WITH_EDITOR
void UMounteaEquipmentSlot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UMounteaEquipmentSlot, SlotID))
	{
		if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
		{
			SlotCompatibleTag = *Settings->EquipmentSlotIDs.Find(SlotID);
		}
	}
}
#endif
