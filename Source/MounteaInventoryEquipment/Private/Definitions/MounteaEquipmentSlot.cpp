// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Definitions/MounteaEquipmentSlot.h"

#include "Net/UnrealNetwork.h"
#include "Settings/MounteaEquipmentConfigData.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

void UMounteaEquipmentSlot::UpdateItem(UMounteaInventoryItemBase* NewItem)
{
	if (NewItem!=ItemInSlot)
	{
		ItemInSlot = NewItem;

		OnRep_Slot();
	}
}

void UMounteaEquipmentSlot::OnRep_Slot()
{
	RepKey++;
}

TArray<FString> UMounteaEquipmentSlot::GetSlotIDs()
{
	TArray<FString> ReturnValues;
	if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
	{
		if (const UMounteaEquipmentConfigData* EquipmentData = Settings->EquipmentConfigData.LoadSynchronous())
		{
			EquipmentData->EquipmentSlotIDs.GetKeys(ReturnValues);
		} 
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
			if (const UMounteaEquipmentConfigData* EquipmentData = Settings->EquipmentConfigData.LoadSynchronous())
			{
				if (EquipmentData->EquipmentSlotIDs.Contains(SlotID))
				{
					SlotCompatibleTag = *EquipmentData->EquipmentSlotIDs.Find(SlotID);
				}
			} 
		}
	}
}
#endif
