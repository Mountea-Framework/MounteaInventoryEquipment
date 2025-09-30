// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools


#include "Definitions/MounteaInventoryItem.h"

#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Engine/Texture2D.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Statics/MounteaInventoryStatics.h"

FInventoryItemSnapshot::FInventoryItemSnapshot(const struct FInventoryItem& Item)
	: Quantity(Item.GetQuantity())
	, Durability(Item.GetDurability())
	, CustomData(Item.GetCustomData())
{}

bool FInventoryItemSnapshot::HasQuantityChanged(const FInventoryItem& Current) const
{
	return Quantity != Current.GetQuantity();
}

int32 FInventoryItemSnapshot::GetQuantityDelta(const FInventoryItem& Current) const
{
	return Current.GetQuantity() - Quantity;
}

bool FInventoryItemSnapshot::HasDurabilityChanged(const FInventoryItem& Current) const
{
	return !FMath::IsNearlyEqual(Durability, Current.GetDurability());
}

float FInventoryItemSnapshot::GetDurabilityDelta(const FInventoryItem& Current) const
{
	return Current.GetDurability() - Durability;
}

bool FInventoryItemSnapshot::HasCustomDataChanged(const FInventoryItem& Current) const
{
	return CustomData != Current.GetCustomData();
}

FInventoryItem::FInventoryItem() : Guid(FGuid()),  Template(nullptr), Quantity(-1), Durability(-1.f), OwningInventory(nullptr)
{
}

FInventoryItem::FInventoryItem(UMounteaInventoryItemTemplate* InTemplate, const int32 InQuantity, const float InDurability, TScriptInterface<IMounteaAdvancedInventoryInterface> InOwningInventory)
	: Guid(FGuid())
	, Template(nullptr)
	, OwningInventory(InOwningInventory)
{
	if (!IsValid(InTemplate))
		return;
			
	Template = InTemplate;
	Guid = FGuid::NewGuid();
		
	Template = InTemplate;
	Quantity = FMath::Clamp(InQuantity, 0, Template->MaxQuantity);
			
	if (Template->bHasDurability)
		Durability = FMath::Clamp(InDurability, 0.f, Template->MaxDurability);
}

bool FInventoryItem::IsItemValid() const
{
	return Guid.IsValid() && Template != nullptr;
}

bool FInventoryItem::IsItemInInventory() const
{
	return OwningInventory != nullptr;
}

FString FInventoryItem::ToString() const
{
	//FString Builder;
	TStringBuilder<512> Builder;

	if (!IsItemValid())
		return TEXT("Invalid Item!");
	
	Builder.Appendf(TEXT("Item [%s] (GUID: %s)\n"), 
		*GetTemplate()->DisplayName.ToString(), 
		*GetGuid().ToString());
	
	Builder.Appendf(TEXT("Quantity: %d/%d\n"), 
		GetQuantity(), 
		GetTemplate()->MaxQuantity);
	
	if (static_cast<uint8>(GetTemplate()->ItemFlags) & static_cast<uint8>(EInventoryItemFlags::EIIF_Durable))
	{
		Builder.Appendf(TEXT("Durability: %.1f/%.1f\n"), 
			GetDurability(), 
			GetTemplate()->MaxDurability);
	}
	
	if (!CustomData.IsEmpty())
	{
		Builder.Append(TEXT("Custom Data: "));
		Builder.Append(CustomData.ToString());
		Builder.Append(TEXT("\n"));
	}
	
	if (AffectorSlots.Num() > 0)
	{
		Builder.Append(TEXT("Affectors:\n"));
		for (const auto& Pair : AffectorSlots)
		{
			Builder.Appendf(TEXT("  - %s: %s\n"), 
				*Pair.Key.ToString(), 
				*Pair.Value.ToString());
		}
	}
	
	return Builder.ToString();
}

FText FInventoryItem::GetItemName() const
{
	return IsValid(Template) ? Template->DisplayName : NSLOCTEXT("InventoryItem", "InvalidName", "Invalid");
}

FText FInventoryItem::GetItemShortInfo() const
{
	return IsValid(Template) ? Template->ItemShortInfo : NSLOCTEXT("InventoryItem", "InvalidShortInfo", "Invalid");
}

FText FInventoryItem::GetItemLongInfo() const
{
	return IsValid(Template) ? Template->ItemLongInfo : NSLOCTEXT("InventoryItem", "ItemLongInfo", "Invalid");
}

UTexture2D* FInventoryItem::GetCover() const
{
	return IsValid(Template) ? Template->ItemCover.LoadSynchronous() : nullptr;
}

bool FInventoryItem::SetTemplate(UMounteaInventoryItemTemplate* InTemplate)
{
	if (InTemplate != Template && IsValid(InTemplate))
	{
		Template = InTemplate;
		return true;
	}
	return false;
}

bool FInventoryItem::SetQuantity(const int32 InQuantity)
{
	if (InQuantity == Quantity || !IsValid(Template))
		return false;

	if (InQuantity >= 0 && InQuantity <= Template->MaxQuantity)
	{
		Quantity = InQuantity;
		return true;
	}
	
	return false;
}

bool FInventoryItem::SetDurability(const float InDurability)
{
	if (InDurability == Durability || !IsValid(Template))
		return false;

	if (!Template->bHasDurability)
		return false;

	if ((FMath::IsNearlyZero(InDurability) || InDurability > 0.0f) && InDurability <= Template->MaxDurability)
	{
		Durability = InDurability;
		return true;
	}
	
	return false;
}

bool FInventoryItem::SetAffectorSlots(const TMap<FGameplayTag, FGuid>& InAffectorSlots)
{
	if (InAffectorSlots.Num() != AffectorSlots.Num())
	{
		for (const auto& Pair : InAffectorSlots)
		{
			if (!Pair.Value.IsValid())
				return false;
		}
		
		AffectorSlots = InAffectorSlots;
		return true;
	}
	
	bool bHasDifference = false;
	
	for (const auto& Pair : InAffectorSlots)
	{
		const FGuid* ExistingGuid = AffectorSlots.Find(Pair.Key);
		
		if (!ExistingGuid || *ExistingGuid != Pair.Value)
		{
			if (!Pair.Value.IsValid())
				return false;
			
			bHasDifference = true;
			break;
		}
	}

	if (bHasDifference)
	{
		AffectorSlots = InAffectorSlots;
		return true;
	}

	return false;
}

bool FInventoryItem::SetOwningInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& InOwningInventory)
{
	if (InOwningInventory == OwningInventory)
		return false;

	OwningInventory = InOwningInventory;
	return true;
}

bool FInventoryItem::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Guid;
	Ar << Template;
	Ar << Quantity;
	Ar << Durability;
	CustomData.NetSerialize(Ar, Map, bOutSuccess);

	// Handle TScriptInterface serialization
	UObject* OwningInventoryObj = nullptr;
	if (Ar.IsSaving())
		OwningInventoryObj = OwningInventory.GetObject();
	Ar << OwningInventoryObj;
	if (Ar.IsLoading())
		OwningInventory = TScriptInterface<IMounteaAdvancedInventoryInterface>(OwningInventoryObj);

	if (Ar.IsLoading())
		AffectorSlots.Empty();
		
	int32 NumPairs = AffectorSlots.Num();
	Ar << NumPairs;

	if (Ar.IsLoading())
	{
		for (int32 i = 0; i < NumPairs; ++i)
		{
			FGameplayTag Key;
			FGuid Value;
			Ar << Key;
			Ar << Value;
			AffectorSlots.Add(Key, Value);
		}
	}
	else
	{
		for (const auto& Pair : AffectorSlots)
		{
			FGameplayTag Key = Pair.Key;
			FGuid Value = Pair.Value;
			Ar << Key;
			Ar << Value;
		}
	}
	
	bOutSuccess = true;
	return true;
}

void FInventoryItem::PostReplicatedAdd(const struct FInventoryItemArray& InArraySerializer)
{
	if (IsValid(OwningInventory.GetObject()))
	{
		CapturePreReplicationSnapshot();
		
		OwningInventory->Execute_ProcessInventoryNotification
		(
			OwningInventory.GetObject(),
			UMounteaInventoryStatics::CreateNotificationData(
				MounteaInventoryNotificationBaseTypes::ItemAdded,
				OwningInventory,
				Guid,
				Quantity
			)
		);
	}
}

void FInventoryItem::PostReplicatedChange(const FInventoryItemArray& InArraySerializer)
{
	if (!IsValid(OwningInventory.GetObject()))
	{
		return;
	}

	if (!IsValid(Template))
	{
		LOG_ERROR(TEXT("[Inventory Item Client Update] Invalid Template! Aborting!"))
		return;
	}

	// Notify if quantity changed
	if (PreReplicationSnapshot.HasQuantityChanged(*this))
	{
		const int32 QuantityDelta = PreReplicationSnapshot.GetQuantityDelta(*this);
		const FString NotificationType = QuantityDelta > 0 
			? MounteaInventoryNotificationBaseTypes::ItemAdded 
			: MounteaInventoryNotificationBaseTypes::ItemRemoved;

		OwningInventory->Execute_ProcessInventoryNotification(
			OwningInventory.GetObject(),
			UMounteaInventoryStatics::CreateNotificationData(
				NotificationType,
				OwningInventory,
				Guid,
				QuantityDelta > 0 ? QuantityDelta : -QuantityDelta
			)
		);
	}

	if (Template->bHasDurability)
	{
		// Notify if durability changed
		if (PreReplicationSnapshot.HasDurabilityChanged(*this))
		{
			const float DurabilityDelta = PreReplicationSnapshot.GetDurabilityDelta(*this);
			if (DurabilityDelta > 0)
			{
				// If the increased durability reaches the maximum, notify accordingly.
				if (FMath::IsNearlyEqual(GetDurability(), GetTemplate()->MaxDurability))
				{
					OwningInventory->Execute_ProcessInventoryNotification(
						OwningInventory.GetObject(),
						UMounteaInventoryStatics::CreateNotificationData(
							MounteaInventoryNotificationBaseTypes::DurabilityMax,
							OwningInventory,
							Guid,
							DurabilityDelta
						)
					);
				}
				else
				{
					OwningInventory->Execute_ProcessInventoryNotification(
						OwningInventory.GetObject(),
						UMounteaInventoryStatics::CreateNotificationData(
							MounteaInventoryNotificationBaseTypes::DurabilityIncreased,
							OwningInventory,
							Guid,
							-DurabilityDelta
						)
					);
				}
			}
			else if (DurabilityDelta < 0)
			{
				// If the decreased durability drops to zero, notify accordingly.
				if (FMath::IsNearlyZero(GetDurability()))
				{
					OwningInventory->Execute_ProcessInventoryNotification(
						OwningInventory.GetObject(),
						UMounteaInventoryStatics::CreateNotificationData(
							MounteaInventoryNotificationBaseTypes::DurabilityZero,
							OwningInventory,
							Guid,
							0
						)
					);
				}
				else
				{
					OwningInventory->Execute_ProcessInventoryNotification(
						OwningInventory.GetObject(),
						UMounteaInventoryStatics::CreateNotificationData(
							MounteaInventoryNotificationBaseTypes::DurabilityDecreased,
							OwningInventory,
							Guid,
							-DurabilityDelta
						)
					);
				}
			}
		}
	}
	
	// Update snapshot for next replication cycle
	CapturePreReplicationSnapshot();
}

void FInventoryItem::PreReplicatedRemove(const struct FInventoryItemArray& InArraySerializer)
{
	if (IsValid(OwningInventory.GetObject()))
	{
		OwningInventory->Execute_ProcessInventoryNotification
		(
			OwningInventory.GetObject(),
			UMounteaInventoryStatics::CreateNotificationData(
				MounteaInventoryNotificationBaseTypes::ItemRemoved,
				OwningInventory,
				Guid,
				-Quantity
			)
		);
	}
}
