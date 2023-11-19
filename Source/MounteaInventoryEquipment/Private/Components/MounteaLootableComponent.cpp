// All rights reserved Dominik Pavlicek 2023.


#include "Components/MounteaLootableComponent.h"

#include "Helpers/MounteaInventoryItemBFL.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "MounteaLootableComponent"

UMounteaLootableComponent::UMounteaLootableComponent()
{
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UMounteaLootableComponent::BeginPlay()
{
	Super::BeginPlay();

	SourceInventory = Execute_GetSourceInventory(this);
	TargetInventory = Execute_GetTargetInventory(this);
}

void UMounteaLootableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UMounteaLootableComponent, SourceInventoryComponent, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaLootableComponent, TargetInventoryComponent, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaLootableComponent, SourceInventory, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaLootableComponent, TargetInventory, COND_InitialOrOwner);
}

TScriptInterface<IMounteaInventoryInterface> UMounteaLootableComponent::GetSourceInventory_Implementation() const
{
	const TScriptInterface<IMounteaInventoryInterface> Result = SourceInventoryComponent.GetComponent(GetOwner());

	return Result;
}

void UMounteaLootableComponent::SetSourceInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewSource)
{
	if (Execute_DoesHaveAuthority(this) == false)
	{
		SetSourceInventory_Server(NewSource);

		return;
	}

	SourceInventory = NewSource;
}

TScriptInterface<IMounteaInventoryInterface> UMounteaLootableComponent::GetTargetInventory_Implementation() const
{
	const TScriptInterface<IMounteaInventoryInterface> Result = TargetInventoryComponent.GetComponent(GetOwner());

	return Result;
}

void UMounteaLootableComponent::SetTargetInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewTarget)
{
	if (Execute_DoesHaveAuthority(this) == false)
	{
		SetTargetInventory_Server(NewTarget);

		return;
	}

	TargetInventory = NewTarget;
}

TArray<UMounteaInstancedItem*> UMounteaLootableComponent::GetLootableItems_Implementation() const
{
	TArray<UMounteaInstancedItem*> Result;
	
	if (SourceInventory.GetObject())
	{
		return SourceInventory->GetItems();
	}

	return Result;
}

bool UMounteaLootableComponent::CanLootItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity) const
{
	if (!Item) return false;

	if (!UMounteaInventoryItemBFL::IsItemValid(Item)) return false;

	if (Quantity <= 0) return false;

	if (SourceInventory.GetObject() == nullptr) return false;

	return SourceInventory->Execute_CanAddItem(SourceInventory.GetObject(), FItemTransfer(Item, Quantity));
}

FInventoryUpdateResult UMounteaLootableComponent::LootItem_Implementation(const FItemTransfer& Item)
{
	FInventoryUpdateResult Result;
	
	if (SourceInventory.GetObject() == nullptr)
	{
		Result.OptionalPayload = this;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden;
		Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Source Inventory is not setup.");

		return Result;
	}

	if (TargetInventory.GetObject() == nullptr)
	{
		Result.OptionalPayload = this;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden;
		Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Target Inventory is not setup.");

		return Result;
	}

	if (Item.Item == nullptr)
	{
		Result.OptionalPayload = this;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden;
		Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid Loot Item.");

		return Result;
	}

	if (Item.Quantity <= 0)
	{
		Result.OptionalPayload = this;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden;
		Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Invalid Loot Item Quantity.");

		return Result;
	}

	// TODO: Add some validations and logic here
	if (SourceInventory->Execute_CanReduceItem(SourceInventory.GetObject(), Item) == false)
	{
		Result.OptionalPayload = this;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden;
		Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Loot item reduction forbidden.");

		return Result;
	}
	
	if (TargetInventory->Execute_CanAddItem(SourceInventory.GetObject(), Item) == false)
	{
		Result.OptionalPayload = this;
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Forbidden;
		Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Loot item addition forbidden.");

		return Result;
	}
	
	SourceInventory->Execute_ReduceItemInInventory(SourceInventory.GetObject(), Item.Item, Item.Quantity);
	TargetInventory->Execute_AddItemToInventory(TargetInventory.GetObject(), Item.Item, Item.Quantity);
	
	return Result;
}

TArray<FInventoryUpdateResult> UMounteaLootableComponent::LootAllItems_Implementation(const TArray<FItemTransfer>& Items)
{
	TArray<FInventoryUpdateResult> Results;

	for (const auto& Itr : Items)
	{
		FInventoryUpdateResult Result = Execute_LootItem(this, Itr);
		Results.Add(Result);
	}

	return Results;
}

AActor* UMounteaLootableComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

bool UMounteaLootableComponent::DoesHaveAuthority_Implementation() const
{
	if (GetOwner()==nullptr) return false;
	return GetOwner()->HasAuthority();
}

void UMounteaLootableComponent::SetTargetInventory_Server_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewSource)
{
	Execute_SetTargetInventory(this, NewSource);
}

bool UMounteaLootableComponent::SetTargetInventory_Server_Validate(const TScriptInterface<IMounteaInventoryInterface>& NewSource)
{ return true; }

void UMounteaLootableComponent::SetSourceInventory_Server_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewSource)
{
	Execute_SetSourceInventory(this, NewSource);
}

bool UMounteaLootableComponent::SetSourceInventory_Server_Validate(const TScriptInterface<IMounteaInventoryInterface>& NewSource)
{ return true; }

#undef LOCTEXT_NAMESPACE
