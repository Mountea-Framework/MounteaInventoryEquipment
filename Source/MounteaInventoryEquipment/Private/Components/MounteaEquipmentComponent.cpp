// All rights reserved Dominik Pavlicek 2023.


#include "Components/MounteaEquipmentComponent.h"

UMounteaEquipmentComponent::UMounteaEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UMounteaEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UMounteaEquipmentComponent::EquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex)
{
	//TODO: Call to server

	return true;
}

bool UMounteaEquipmentComponent::UnEquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex)
{
	//TODO: Call to server

	return true;
}

bool UMounteaEquipmentComponent::IsItemEquipped_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex)
{
	//TODO: access saved data

	return true;
}

TArray<FMounteaEquipmentSlotData> UMounteaEquipmentComponent::GetAllSlots_Implementation() const
{
	return EquipmentSlotData;
}

#if WITH_EDITOR

void UMounteaEquipmentComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == TEXT("EquipmentSlotData"))
	{
		for (FMounteaEquipmentSlotData& Itr : EquipmentSlotData)
		{
			if (Itr.Slot == nullptr)
			{
				Itr.Slot = NewObject<UMounteaEquipmentSlot>(GetPackage(), UMounteaEquipmentSlot::StaticClass());
			}
		}
	}
}

#endif
