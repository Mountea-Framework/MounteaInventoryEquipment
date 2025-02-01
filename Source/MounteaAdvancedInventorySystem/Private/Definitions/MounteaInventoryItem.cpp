// All rights reserved Dominik Morse 2024


#include "Definitions/MounteaInventoryItem.h"

#include "Definitions/MounteaInventoryItemTemplate.h"

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
