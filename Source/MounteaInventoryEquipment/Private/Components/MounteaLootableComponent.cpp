// All rights reserved Dominik Morse 2023.


#include "Components/MounteaLootableComponent.h"

#include "Helpers/BlueprintFunctionLibraries//MounteaInventoryItemBFL.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Net/UnrealNetwork.h"

#if WITH_EDITOR
#include "Kismet2/ComponentEditorUtils.h"
#endif

#define LOCTEXT_NAMESPACE "MounteaLootableComponent"

UMounteaLootableComponent::UMounteaLootableComponent()
{
	SetIsReplicatedByDefault(true);
	bAutoActivate = true;
}

void UMounteaLootableComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupSourceComponent();
}

void UMounteaLootableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UMounteaLootableComponent, SourceInventoryComponent, COND_InitialOrOwner);
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
	return TargetInventory;
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

bool UMounteaLootableComponent::CanLootItem_Implementation(const FItemTransfer& Item) const
{
	if (!Item.Item) return false;

	if (!UMounteaInventoryItemBFL::IsItemValid(Item.Item)) return false;

	if (Item.Quantity <= 0) return false;

	if (SourceInventory.GetObject() == nullptr) return false;

	return SourceInventory->Execute_CanAddItem(SourceInventory.GetObject(), Item);
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

	if (Execute_DoesHaveAuthority(this) == false)
	{
		LootItem_Server(Item);
		
		Result.OptionalPayload = Item.Item;
		Result.ResultID = Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_Processing;
		Result.ResultText = LOCTEXT("InventoryUpdateResult_Processing", "Server is processing request.");
		
		return Result;
	}
	
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
	
	//At this point we validated as much as we could, so here we just assume it is all good.
	SourceInventory->Execute_ReduceItemInInventory(SourceInventory.GetObject(), Item.Item, Item.Quantity);
	TargetInventory->Execute_AddItemToInventory(TargetInventory.GetObject(), Item.Item, Item.Quantity);

	// Call it out loud and clear. 
	Result.OptionalPayload = nullptr; // keep it empty so we don't mess UI
	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
	Result.ResultText =  LOCTEXT("InventoryUpdateResult_InvalidRequest", "Loot was successful.");
	
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

void UMounteaLootableComponent::LootItem_Server_Implementation(const FItemTransfer& Item)
{
	Execute_LootItem(this, Item);
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

void UMounteaLootableComponent::SetupSourceComponent()
{
	if (GetOwner() == nullptr) return;
	
	if (SourceInventoryComponent.OtherActor == nullptr)
	{
		const TArray<UActorComponent*> Components = GetOwner()->GetComponentsByInterface(UMounteaInventoryInterface::StaticClass());
		if (Components.Num() > 0)
		{
			for (const auto& Itr : Components)
			{
				if (Itr && Itr->GetFName().IsEqual(SourceInventoryComponent.ComponentProperty))
				{
					const TScriptInterface<IMounteaInventoryInterface> NewSource = Itr;
					SourceInventory = NewSource;

					return;
				}
			}

			// We found nothing by name, so we fallback to this default
			const TScriptInterface<IMounteaInventoryInterface> NewSource = Components[0];
			SourceInventory = NewSource;
		}
	}
	else
	{
		SourceInventory = SourceInventoryComponent.GetComponent( SourceInventoryComponent.OtherActor.Get() ? SourceInventoryComponent.OtherActor.Get() : GetOwner() );
	}
}

#if WITH_EDITOR

void UMounteaLootableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	const FName MemberPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaLootableComponent, SourceInventoryComponent) || MemberPropertyName == GET_MEMBER_NAME_CHECKED(UMounteaLootableComponent, SourceInventoryComponent))
	{
		SetupSourceComponent();
	}
}

#endif

#undef LOCTEXT_NAMESPACE
