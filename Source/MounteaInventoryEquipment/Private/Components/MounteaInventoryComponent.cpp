// All rights reserved Dominik Morse (Pavlicek) 2024


#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Engine/ActorChannel.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Interfaces/MounteaInventoryWBPInterface.h"

#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "WBP/MounteaBaseUserWidget.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryComponent"

UMounteaInventoryComponent::UMounteaInventoryComponent()
{	
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UMounteaInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	//OnInventoryUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostInventoryUpdated);
	
	//OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemAdded);
	//OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemRemoved);
	//OnItemUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemUpdated);
}

void UMounteaInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//FDoRepLifetimeParams Params;
	//Params.bIsPushBased = true;
	//Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	//DOREPLIFETIME_WITH_PARAMS(UMounteaInventoryComponent, Items, Params)
	
	//DOREPLIFETIME(UMounteaInventoryComponent, Items);
	//DOREPLIFETIME(UMounteaInventoryComponent, InventoryConfig);
	//DOREPLIFETIME(UMounteaInventoryComponent, OtherInventory);
}

bool UMounteaInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bUpdated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	//Check if the array of items needs to replicate
	/*
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		TArray<UMounteaInventoryItemBase*> AllItems = Items;
		AllItems.Append(RemovedItems);
		
		for (const auto& Item : AllItems)
		{
			if (Item && Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->GetRepKey()))
			{
				bUpdated |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}

		RemovedItems.Empty();

		if (Channel->KeyNeedsToReplicate(InventoryConfig.MounteaInventoryConfig->GetUniqueID(), InventoryConfig.MounteaInventoryConfig->GetRepKey()))
		{
			bUpdated |= Channel->ReplicateSubobject(InventoryConfig.MounteaInventoryConfig, *Bunch, *RepFlags);
		}
	}
	*/

	return bUpdated;
}

#if WITH_EDITOR

void UMounteaInventoryComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == TEXT("MounteaInventoryConfig"))
	{
		/*
		if (InventoryConfig.MounteaInventoryConfig == nullptr)
		{
			bool bFound = false;
			const TSubclassOf<UMounteaInventoryConfig> Class = UMounteaInventoryEquipmentBPF::GetSettings()->DefaultInventoryConfigClass.LoadSynchronous();
			InventoryConfig.MounteaInventoryConfig = UMounteaInventoryEquipmentBPF::GetInventoryConfig(this, Class, bFound);
		}
		*/
	}
}

#endif

#undef LOCTEXT_NAMESPACE
