// All rights reserved Dominik Pavlicek 2023.


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

	DOREPLIFETIME(UMounteaEquipmentComponent, EquipmentSlotData);
}

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

	return bUpdated;
}

AActor* UMounteaEquipmentComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

FString UMounteaEquipmentComponent::FindSlotForItem_Implementation(const UMounteaInventoryItemBase* Item) const
{
	FString SlotID;
	
	if (!Item) return SlotID;
	if (EquipmentSlotData.IsEmpty()) return SlotID;

	for (const auto& Itr : EquipmentSlotData)
	{
		/* BREAKING
		if (Itr.Slot && Itr.Slot->GetSlotTag().IsValid() && Item->GetTags().HasTagExact(Itr.Slot->GetSlotTag()))
		{
			SlotID = Itr.Slot->GetSlotID();
			break;
		}
		*/
	}

	return SlotID;
}

UMounteaEquipmentSlot* UMounteaEquipmentComponent::FindSlotByID_Implementation(const FString& SlotID) const
{
	for (const FMounteaEquipmentSlotData& Itr : EquipmentSlotData)
	{
		if (Itr.Slot && Itr.Slot->GetSlotID().Equals(SlotID)) return Itr.Slot;
	}

	return nullptr;
}

#pragma region EQUIP

bool UMounteaEquipmentComponent::EquipItem_Implementation(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	if (!ItemToEquip) return false;
	
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		// Cleanup Slot before populating new, so populated slot can return to Inventory
		// Local stuff should be fine, as it will get updates once server response is received
		if (const auto TempEquipmentUI = Execute_GetEquipmentUI(this))
		{
			if (UMounteaEquipmentSlot* const FoundSlot = UMounteaInventoryEquipmentBPF::FindEquipmentSlot(EquipmentSlotData, SlotID))
			{
				FoundSlot->UpdateItem(ItemToEquip);
			
				TempEquipmentUI->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::UnequipItemWidget, FoundSlot);
				TempEquipmentUI->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::EquipItemWidget, FoundSlot);
			}
		}

		EquipItem_Server(ItemToEquip, SlotID);
	}
	
	if (Execute_CanEquipItem(this, ItemToEquip))
	{
		OnSlotEquipped_Client.Broadcast(ItemToEquip, SlotID);
		
		return true;
	}
	
	return false;
}

void UMounteaEquipmentComponent::EquipItem_Server_Implementation(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	if (Execute_CanEquipItem(this, ItemToEquip))
	{
		if (UMounteaEquipmentSlot* Slot = UMounteaInventoryEquipmentBPF::FindEquipmentSlot(EquipmentSlotData, SlotID))
		{
			Slot->UpdateItem(ItemToEquip);
			
			OnSlotEquipped.Broadcast(ItemToEquip, SlotID);
		
			OnRep_Equipment();

			EquipItem_Multicast(ItemToEquip, SlotID);
		}
	}
}

bool UMounteaEquipmentComponent::EquipItem_Server_Validate(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	return true;
}

void UMounteaEquipmentComponent::EquipItem_Multicast_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	OnSlotEquipped_Multicast.Broadcast(ItemToEquip, SlotID);
	OnEquipmentUpdated_Multicast.Broadcast(SlotID);
}

#pragma endregion

#pragma region UNEQUIP

bool UMounteaEquipmentComponent::UnEquipItem_Implementation(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	if (!ItemToEquip) return false;
	
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		// Cleanup Slot and return possible slot back to Inventory
		if (const auto TempEquipmentUI = Execute_GetEquipmentUI(this))
		{
			TempEquipmentUI->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::UnequipItemWidget, UMounteaInventoryEquipmentBPF::FindEquipmentSlot(EquipmentSlotData, SlotID));
		}
		
		UnEquipItem_Server(ItemToEquip, SlotID);
	}
	
	if (Execute_IsItemEquipped(this, ItemToEquip, SlotID))
	{
		OnSlotUnequipped_Client.Broadcast(ItemToEquip, SlotID);
		
		return true;
	}
	
	return false;
}

void UMounteaEquipmentComponent::UnEquipItem_Server_Implementation(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	if (Execute_IsItemEquipped(this, ItemToEquip, SlotID))
	{
		if (UMounteaEquipmentSlot* Slot = UMounteaInventoryEquipmentBPF::FindEquipmentSlot(EquipmentSlotData, SlotID))
		{
			Slot->UpdateItem(nullptr);
			
			OnSlotUnequipped.Broadcast(ItemToEquip, SlotID);
					
			OnRep_Equipment();

			UnEquipItem_Multicast(ItemToEquip, SlotID);
		}
	}
}

bool UMounteaEquipmentComponent::UnEquipItem_Server_Validate(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	return true;
}

void UMounteaEquipmentComponent::UnEquipItem_Multicast_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID)
{
	OnSlotUnequipped_Multicast.Broadcast(ItemToEquip, SlotID);
	OnEquipmentUpdated_Multicast.Broadcast(SlotID);
}

#pragma endregion 


bool UMounteaEquipmentComponent::IsItemEquipped_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID) const
{
	return EquipmentSlotData.Contains(FMounteaEquipmentSlotDataCompare(ItemToEquip, SlotID));
}

TArray<FMounteaEquipmentSlotData> UMounteaEquipmentComponent::GetAllSlots_Implementation() const
{
	return EquipmentSlotData;
}

bool UMounteaEquipmentComponent::CanEquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip) const
{
	if (!ItemToEquip) return false;

	const FString SlotID = Execute_FindSlotForItem(this, ItemToEquip);

	if (SlotID.IsEmpty()) return false;

	return !Execute_IsItemEquipped(this, ItemToEquip, SlotID);
}

UMounteaBaseUserWidget* UMounteaEquipmentComponent::GetEquipmentUI_Implementation() const
{
	return EquipmentUI;
}

bool UMounteaEquipmentComponent::SetEquipmentUI_Implementation(UMounteaBaseUserWidget* NewUI)
{
	if(NewUI != EquipmentUI)
	{
		EquipmentUI = NewUI;
		return true;
	}

	return false;
}

void UMounteaEquipmentComponent::OnRep_Equipment()
{
	OnEquipmentUpdated.Broadcast(TEXT("Equipment Replicated"));
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
