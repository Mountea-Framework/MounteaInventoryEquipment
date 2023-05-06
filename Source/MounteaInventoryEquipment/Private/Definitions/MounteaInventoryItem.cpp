// All rights reserved Dominik Pavlicek 2022.


#include "Definitions/MounteaInventoryItem.h"
#include "Net/UnrealNetwork.h"


void UMounteaInventoryItemBase::Initialize(UWorld* NewWorld, TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory, const FMounteaInventoryItemRequiredData& NewItemData, const FMounteaInventoryItemOptionalData NewOptionalData)
{
	World = NewWorld;
	OwningInventory = NewOwningInventory;
	ItemData = NewItemData;
	ItemOptionalData = NewOptionalData;

	MarkDirtyForReplication();

	FString Message = ItemData.ItemName.ToString();
	Message.Append(": Initialization completed.");
	OnItemInitialized.Broadcast(Message);
}

void UMounteaInventoryItemBase::OnRep_Item()
{
	FString Message = ItemData.ItemName.ToString();
	Message.Append(" has been modified.");
	
	OnItemModified.Broadcast(Message);
}

void UMounteaInventoryItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryItemBase, ItemData);
}

bool UMounteaInventoryItemBase::IsSupportedForNetworking() const
{
	return true;
}

UWorld* UMounteaInventoryItemBase::GetWorld() const
{
	return World;
}

void UMounteaInventoryItemBase::MarkDirtyForReplication()
{
	++RepKey;

	// TODO: Request OwningInventory to increment ReplicatedItemsKeys
}

#if WITH_EDITOR
void UMounteaInventoryItemBase::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);

	ItemData.ItemGuid = FGuid::NewGuid();
}

void UMounteaInventoryItemBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// ...
}
#endif