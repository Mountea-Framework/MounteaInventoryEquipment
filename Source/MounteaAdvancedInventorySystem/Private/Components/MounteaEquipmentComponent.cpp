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


#include "Components/MounteaEquipmentComponent.h"

#include "Statics/MounteaEquipmentStatics.h"


UMounteaEquipmentComponent::UMounteaEquipmentComponent()
{
	ComponentTags.Append( { TEXT("Equipment") } );
}

AActor* UMounteaEquipmentComponent::EquipItem_Implementation(const FInventoryItem& ItemDefinition) const
{
	AActor* spawnedActor = nullptr;

	// Try override binding first
	if (OverrideEquipItemFunction.GetMemberName() != NAME_None)
	{
		if (UClass* scopeClass = OverrideEquipItemFunction.GetMemberParentClass())
		{
			if (UFunction* targetFunction = scopeClass->FindFunctionByName(OverrideEquipItemFunction.GetMemberName()))
			{
				struct FParams
				{
					FInventoryItem ItemDefinition;
					AActor* OutSpawnedActor;
					bool ReturnValue;
				};

				FParams params;
				params.ItemDefinition = ItemDefinition;
				params.OutSpawnedActor = nullptr;
				params.ReturnValue = false;

				UObject* targetObject = nullptr;

				if (targetFunction->HasAnyFunctionFlags(FUNC_Static))
					targetObject = scopeClass->GetDefaultObject();
				else
				{
					// Member function: call on something that is-a scopeClass.
					// In our case, we want the component itself or its owner.
					if (const_cast<UMounteaEquipmentComponent*>(this)->IsA(scopeClass))
						targetObject = const_cast<UMounteaEquipmentComponent*>(this);
					else if (AActor* owner = GetOwner(); owner && owner->IsA(scopeClass))
						targetObject = owner;
				}

				if (targetObject)
				{
					targetObject->ProcessEvent(targetFunction, &params);
					if (params.ReturnValue)
						return params.OutSpawnedActor;
				}
			}
		}
	}

	// Fallback
	const bool bResult = UMounteaEquipmentStatics::EquipItem(ItemDefinition, spawnedActor);
	return bResult ? spawnedActor : nullptr;
}

AActor* UMounteaEquipmentComponent::EquipItemToSlot_Implementation(const FName& SlotId, const FInventoryItem& ItemDefinition)
{
	if (SlotId.IsNone())
		return nullptr;
	
	if (!Execute_IsValidSlot(this, SlotId))
		return nullptr;

	AActor* spawnedActor = EquipItem_Implementation(ItemDefinition);
	if (!spawnedActor)
		return nullptr;

	return spawnedActor;
}


bool UMounteaEquipmentComponent::UnequipItem_Implementation(const FInventoryItem& ItemDefinition, bool bUseFallbackSlot)
{
	return true;
}

bool UMounteaEquipmentComponent::UnequipItemFromSlot_Implementation(const FName& SlotId, bool bUseFallbackSlot)
{
	return true;
}

bool UMounteaEquipmentComponent::IsEquipmentItemEquipped_Implementation(const FInventoryItem& ItemDefinition) const
{
	return true;
}