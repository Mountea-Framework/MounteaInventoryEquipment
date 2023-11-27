// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Components/MounteaEquipmentComponent.h"

#include "Definitions/MounteaInventoryInstancedItem.h"
#include "Engine/ActorChannel.h"
#include "Helpers/MounteaEquipmentBFL.h"
#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Interfaces/UI/MounteaEquipmentWBPInterface.h"
#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "Settings/Config/MounteaDefaultsConfig.h"
#include "WBP/MounteaBaseUserWidget.h"

#define LOCTEXT_NAMESPACE "MounteaEquipmentComponent"

UMounteaEquipmentComponent::UMounteaEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;

	ReplicatedItemsKey = 0;
}

void UMounteaEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	OnEquipmentUpdated.AddUniqueDynamic(this, &UMounteaEquipmentComponent::UMounteaEquipmentComponent::PostEquipmentUpdated);

	OnSlotEquipped.AddUniqueDynamic(this, &UMounteaEquipmentComponent::PostItemEquipped);
	OnSlotUnequipped.AddUniqueDynamic(this, &UMounteaEquipmentComponent::PostItemUnequipped);
}

void UMounteaEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UMounteaEquipmentComponent, EquipmentSlots);
}

bool UMounteaEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bUpdated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	//Check if the array of items needs to replicate
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		// Setup all Items, even from Slots that should be empty at this point
		TSet<UMounteaInstancedItem*> AllItems;
		for (auto const& Itr : ModifiedSlots)
		{
			if (Itr.Item)
			{
				AllItems.Add(Itr.Item);
			}
		}

		// TODO: Do we really need to replicate items?
		for (const auto& Item : AllItems)
		{
			if (Item && Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->GetRepKey()))
			{
				bUpdated |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}

		// Cleanup all Nullified Slots
		ModifiedSlots.Empty();
	}

	return bUpdated;
}

AActor* UMounteaEquipmentComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

FText UMounteaEquipmentComponent::FindSlotForItem_Implementation(const UMounteaInstancedItem* Item) const
{
	FText SlotID;
	
	if (!Item) return SlotID;

	if (const FEquipmentSlot* ValidSlot = EquipmentSlots.FindByPredicate(
			[Item](const FEquipmentSlot& Slot)
			{
				return Item != nullptr && Slot.SlotTags.HasAny(Item->GetItemFlags());
			}))
	{
		SlotID = ValidSlot->SlotName;
		return SlotID;
	}

	return SlotID;
}

int32 UMounteaEquipmentComponent::FindSlotByID_Implementation(const FText& SlotID) const
{
	return UMounteaEquipmentBFL::FindEquipmentSlotID(EquipmentSlots, SlotID);
}

bool UMounteaEquipmentComponent::DoesHaveAuthority_Implementation() const
{
	if (GetOwner()==nullptr) return false;
	return GetOwner()->HasAuthority();
}

bool UMounteaEquipmentComponent::IsAuthorityOrAutonomousProxy() const
{
	if (!GetOwner()) return false;
	
	switch (GetOwnerRole())
	{
		case ROLE_MAX:
		case ROLE_None:
		case ROLE_Authority:
		case ROLE_AutonomousProxy: return true;
		case ROLE_SimulatedProxy: break;
		default: break;
	}

	return false;
}

bool UMounteaEquipmentComponent::CanEquipItem_Implementation(const UMounteaInstancedItem* ItemToEquip) const
{
	if (!ItemToEquip) return false;

	const FText SlotID = Execute_FindSlotForItem(this, ItemToEquip);

	if (SlotID.IsEmpty()) return false;

	const int32 SlotIndex = Execute_FindSlotByID(ItemToEquip, SlotID);
	if (SlotIndex == INDEX_NONE) return  false;

	const FEquipmentSlot* FoundSlot = EquipmentSlots.FindByKey(SlotID);
	if (FoundSlot == nullptr) return false;

	return !Execute_IsItemEquipped(this, ItemToEquip, SlotID);
}

bool UMounteaEquipmentComponent::CanUnEquipItem_Implementation(const UMounteaInstancedItem* ItemToUnequip) const
{
	if (!ItemToUnequip) return false;

	const FText SlotID = Execute_FindSlotForItem(this, ItemToUnequip);

	if (SlotID.IsEmpty()) return false;

	const int32 SlotIndex = Execute_FindSlotByID(ItemToUnequip, SlotID);
	if (SlotIndex == INDEX_NONE) return  false;

	const FEquipmentSlot* FoundSlot = EquipmentSlots.FindByKey(SlotID);
	if (FoundSlot == nullptr) return false;

	return Execute_IsItemEquipped(this, ItemToUnequip, SlotID);
}

FInventoryUpdateResult UMounteaEquipmentComponent::EquipItem_Implementation(UMounteaInstancedItem* ItemToEquip, const FText& SlotID)
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_UMounteaEquipmentComponent_EquipItem );
	
	// Create an empty result struct.
	FInventoryUpdateResult Result;
	
	// Validate the request
	if (!ItemToEquip)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest; // Bad Request
		Result.ResultText = LOCTEXT("EquipmentUpdateResult_InvalidRequest", "Invalid item.");
		
		return Result;
	}

	// Check if there is Authority
	if (!Execute_DoesHaveAuthority(this))
	{
		EquipItem_Server(ItemToEquip, SlotID);
		
		Result.OptionalPayload = ItemToEquip;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing; // Corresponds to "Processing"
		Result.ResultText = LOCTEXT("EquipmentUpdateResult_Processing", "Server is processing request.");
		
		return Result;
	}

	if (!Execute_CanEquipItem(this, ItemToEquip))
	{
		Result.OptionalPayload = ItemToEquip;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
		Result.ResultText =  LOCTEXT("EquipmentUpdateResult_InvalidRequest", "Cannot equip item.");
    	
		return Result;
	}
	
	FEquipmentSlot* FoundSlot = EquipmentSlots.FindByKey(SlotID);
	if (FoundSlot)
	{
		if (FoundSlot->IsEmpty() == false)
		{
			Execute_UnEquipItem(this, ItemToEquip, SlotID);	
		}
		
		FoundSlot->UpdateSlot(ItemToEquip);
		
		ModifiedSlots.Add(*FoundSlot);

		Result.OptionalPayload = ItemToEquip;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK; // Analogous to "OK"
		Result.ResultText = LOCTEXT("EquipmentUpdateResult_ItemEquipped", "Item equipped.");

		OnSlotEquipped.Broadcast(Result);
		
		// Multicast to Other clients, too, so there can be played animation etc.
		OnEquipmentUpdated_Multicast.Broadcast(Result);
	}
	else
	{
		Result.OptionalPayload = ItemToEquip;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden; // Corresponds to "Forbidden"
		Result.ResultText =  LOCTEXT("EquipmentUpdateResult_InvalidRequest", "Cannot equip item.");
    	
		return Result;
	}
	
	// Remove OptionalPayload so UI does not need to refresh - UI is refreshing only if OptionalPayload is not null
	Result.OptionalPayload = this;
	OnEquipmentUpdated.Broadcast(Result);
	
	return Result;
}

void UMounteaEquipmentComponent::EquipItem_Server_Implementation(UMounteaInstancedItem* ItemToEquip, const FText& SlotID)
{
	Execute_EquipItem(this, ItemToEquip, SlotID);
}

bool UMounteaEquipmentComponent::EquipItem_Server_Validate(UMounteaInstancedItem* ItemToEquip, const FText& SlotID)
{ return true; }

void UMounteaEquipmentComponent::PostEquipmentUpdated_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!IsAuthorityOrAutonomousProxy()) return;

	if (UpdateContext.OptionalPayload)
	{
		PostEquipmentUpdated_Client(UpdateContext);

		PostEquipmentUpdated_Multicast(UpdateContext);
	}
}

void UMounteaEquipmentComponent::PostItemEquipped_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!IsAuthorityOrAutonomousProxy()) return;

	if (UpdateContext.OptionalPayload)
	{
		PostItemEquipped_Client(UpdateContext);

		PostItemEquipped_Multicast(UpdateContext);
	}
}

void UMounteaEquipmentComponent::PostItemUnequipped_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!IsAuthorityOrAutonomousProxy()) return;

	if (UpdateContext.OptionalPayload)
	{
		PostItemUnequipped_Client(UpdateContext);

		PostItemUnequipped_Multicast(UpdateContext);
	}
}

void UMounteaEquipmentComponent::PostEquipmentUpdated_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	if (!IsAuthorityOrAutonomousProxy()) return;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestEquipmentSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostEquipmentUpdated_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestEquipmentSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaEquipmentComponent::PostItemEquipped_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	if (!IsAuthorityOrAutonomousProxy()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestEquipmentSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemEquipped_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestEquipmentSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaEquipmentComponent::PostItemUnequipped_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	if (!UpdateContext.OptionalPayload) return;
	
	if (!IsAuthorityOrAutonomousProxy()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestEquipmentSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostItemUnequipped_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestEquipmentSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaEquipmentComponent::PostEquipmentUpdated_Multicast_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	// TODO
}

void UMounteaEquipmentComponent::PostItemEquipped_Multicast_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	// TODO
}

void UMounteaEquipmentComponent::PostItemUnequipped_Multicast_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	// TODO
}

FInventoryUpdateResult UMounteaEquipmentComponent::UnEquipItem_Implementation(UMounteaInstancedItem* Item, const FText& SlotID)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UMounteaEquipmentComponent_UnEquipItem);
    
    FInventoryUpdateResult Result;
    
    // Validate the request
    if (!Item)
    {
        Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
        Result.ResultText = LOCTEXT("EquipmentUpdateResult_InvalidRequest", "Invalid item.");
        return Result;
    }

    // Check if there is Authority
    if (!Execute_DoesHaveAuthority(this))
    {
        UnEquipItem_Server(Item, SlotID);
        Result.OptionalPayload = Item;
        Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing;
        Result.ResultText = LOCTEXT("EquipmentUpdateResult_Processing", "Server is processing request.");
        return Result;
    }

    // Find the slot by SlotID
    FEquipmentSlot* FoundSlot = EquipmentSlots.FindByKey(SlotID);
    if (FoundSlot && FoundSlot->Item == Item)
    {
        // Remove the item from the slot
        FoundSlot->UpdateSlot(nullptr);
    	
        ModifiedSlots.Add(*FoundSlot);

        // Broadcast that the item was unequipped
        Result.OptionalPayload = Item;
        Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
        Result.ResultText = LOCTEXT("EquipmentUpdateResult_ItemUnequipped", "Item unequipped.");

        OnSlotUnequipped.Broadcast(Result);

    	// Multicast to Other clients, too, so there can be played animation etc.
        OnEquipmentUpdated_Multicast.Broadcast(Result);
    }
    else
    {
        Result.OptionalPayload = Item;
        Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden;
        Result.ResultText = LOCTEXT("EquipmentUpdateResult_InvalidRequest", "Cannot unequip item. Item not found in the specified slot.");
    }

    Result.OptionalPayload = this;
    OnEquipmentUpdated.Broadcast(Result);

    return Result;
}

void UMounteaEquipmentComponent::UnEquipItem_Server_Implementation(UMounteaInstancedItem* Item, const FText& SlotID)
{
	Execute_UnEquipItem(this, Item, SlotID);
}

bool UMounteaEquipmentComponent::UnEquipItem_Server_Validate(UMounteaInstancedItem* ItemToEquip, const FText& SlotID)
{ return true;}

void UMounteaEquipmentComponent::PostEquipmentUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestEquipmentSyncTimerHandle);
	
	if (const auto TempEquipmentUI = Execute_GetEquipmentUI(this))
	{
		TempEquipmentUI->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshEquipmentWidget);
	}

	OnEquipmentUpdated_Client.Broadcast(UpdateContext);
}

void UMounteaEquipmentComponent::PostItemEquipped_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestEquipmentSyncTimerHandle);
	
	if (const auto TempEquipmentUI = Execute_GetEquipmentUI(this))
	{
		TempEquipmentUI->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::EquipItemWidget, UpdateContext.OptionalPayload);
	}
	
	OnSlotEquipped_Client.Broadcast(UpdateContext);
}

void UMounteaEquipmentComponent::PostItemUnequipped_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestEquipmentSyncTimerHandle);
	
	if (const auto TempEquipmentUI = Execute_GetEquipmentUI(this))
	{
		TempEquipmentUI->ProcessMounteaWidgetCommand(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::UnequipItemWidget, UpdateContext.OptionalPayload);
	}
	
	OnSlotUnequipped_Client.Broadcast(UpdateContext);
}

void UMounteaEquipmentComponent::PostEquipmentUpdated_Multicast_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	OnEquipmentUpdated_Multicast.Broadcast(UpdateContext);
}

void UMounteaEquipmentComponent::PostItemEquipped_Multicast_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	OnSlotEquipped_Multicast.Broadcast(UpdateContext);
}

void UMounteaEquipmentComponent::PostItemUnequipped_Multicast_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	OnSlotUnequipped_Multicast.Broadcast(UpdateContext);
}

bool UMounteaEquipmentComponent::IsItemEquipped_Implementation(const UMounteaInstancedItem* Item, const FText& SlotID) const
{
	return EquipmentSlots.Contains(FMounteaEquipmentSlotCompare(Item, SlotID));
}

TArray<FEquipmentSlot> UMounteaEquipmentComponent::GetAllSlots_Implementation() const
{
	return EquipmentSlots;
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

void UMounteaEquipmentComponent::SetEquipmentUIClass_Implementation(TSubclassOf<UMounteaBaseUserWidget> NewWBPClass)
{
	if (NewWBPClass == nullptr) return;

	if (NewWBPClass->ImplementsInterface(UMounteaEquipmentWBPInterface::StaticClass()))
	{
		EquipmentUIClass = NewWBPClass;
	}
}

TSubclassOf<UMounteaBaseUserWidget> UMounteaEquipmentComponent::GetEquipmentUIClass_Implementation() const
{
	if (EquipmentUIClass != nullptr ) { return EquipmentUIClass; };

	const UMounteaInventoryEquipmentSettings* Settings = UMounteaInventoryEquipmentBPF::GetSettings();

	if (!Settings) return nullptr;

	const UMounteaDefaultsConfig* DefaultsConfig = Settings->DefaultsConfig.LoadSynchronous();

	if (!DefaultsConfig) return nullptr;

	return DefaultsConfig->DefaultEquipmentClass.LoadSynchronous();
}

/*===============================================================================
		IN PROGRESS
		
		Following functions are using being changed.
===============================================================================*/

/*===============================================================================
		SUBJECT OF CHANGE
		
		Following functions are using outdated, wrong class definitions and functions.
===============================================================================*/

void UMounteaEquipmentComponent::OnRep_Equipment()
{
	// TODO
}

#if WITH_EDITOR

void UMounteaEquipmentComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == TEXT("EquipmentSlotData"))
	{
		for (FEquipmentSlot& Itr : EquipmentSlots)
		{
			// TODO:
		}
	}
}

#endif

#undef LOCTEXT_NAMESPACE