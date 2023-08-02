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