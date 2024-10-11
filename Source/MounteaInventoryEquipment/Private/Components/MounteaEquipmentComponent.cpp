// All rights reserved Dominik Morse (Pavlicek) 2024


#include "Components/MounteaEquipmentComponent.h"

#include "Engine/ActorChannel.h"
#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Interfaces/MounteaEquipmentWBPInterface.h"
#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "WBP/MounteaBaseUserWidget.h"

UMounteaEquipmentComponent::UMounteaEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UMounteaEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMounteaEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UMounteaEquipmentComponent, EquipmentSlotData);
}

bool UMounteaEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}

/*
bool UMounteaEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bUpdated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	//Check if the array of items needs to replicate
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		TArray<FMounteaEquipmentSlotData> AllSlots = EquipmentSlotData;
				
		for (const auto& SlotData : AllSlots)
		{
			if (SlotData.Slot && Channel->KeyNeedsToReplicate(SlotData.Slot->GetUniqueID(), SlotData.Slot->GetRepKey()))
			{
				bUpdated |= Channel->ReplicateSubobject(SlotData.Slot, *Bunch, *RepFlags);
			}
		}
	}
}
*/

#if WITH_EDITOR

void UMounteaEquipmentComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == TEXT("EquipmentSlotData"))
	{
		/*
		for (FMounteaEquipmentSlotData& Itr : EquipmentSlotData)
		{
			if (Itr.Slot == nullptr)
			{
				Itr.Slot = NewObject<UMounteaEquipmentSlot>(GetPackage(), UMounteaEquipmentSlot::StaticClass());
			}
		}
		*/
	}
}

#endif
