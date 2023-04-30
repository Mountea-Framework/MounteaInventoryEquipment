// All rights reserved Dominik Pavlicek 2022.


#include "Definitions/MounteaInventoryItem.h"
#include "Net/UnrealNetwork.h"


void UMounteaInventoryItem_Base::Initialize(UWorld* NewWorld, TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory, const FMounteaInventoryItemRequiredData& NewItemData, const FMounteaInventoryItemOptionalData NewOptionalData)
{
	World = NewWorld;
	OwningInventory = NewOwningInventory;
	ItemData = NewItemData;
	ItemOptionalData = NewOptionalData;

	FString Message = ItemData.ItemName.ToString();
	Message.Append(": Initialization completed.");
	OnItemInitialized.Broadcast(Message);
}

void UMounteaInventoryItem_Base::OnRep_Item()
{
	FString Message = ItemData.ItemName.ToString();
	Message.Append(" has been modified.");
	
	OnItemModified.Broadcast(Message);
}

void UMounteaInventoryItem_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryItem_Base, ItemData);
}

bool UMounteaInventoryItem_Base::IsSupportedForNetworking() const
{
	return true;
}

UWorld* UMounteaInventoryItem_Base::GetWorld() const
{
	return World;
}

void UMounteaInventoryItem_Base::MarkDirtyForReplication()
{
	++RepKey;

	// TODO: Request OwningInventory to increment ReplicatedItemsKeys
}

#if WITH_EDITOR
void UMounteaInventoryItem_Base::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// ...
}
#endif