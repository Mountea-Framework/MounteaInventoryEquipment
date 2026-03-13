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

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MounteaAdvancedInventoryLoadoutItem.generated.h"

class UMounteaInventoryItemTemplate;

/**
 * A class representing an advanced inventory loadout item in the Mountea Advanced Inventory System.
 *
 * This class is designed to define a single loadout item for a character or inventory system.
 * It allows customization of the item template, quantity, and additional behaviors such as
 * automatic equipping and slot assignment.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, DefaultToInstanced, EditInlineNew,
	AutoExpandCategories=("Mountea"),
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Mountea Inventory Loadout Item"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryLoadoutItem : public UObject
{
	GENERATED_BODY()
	
public:
	
	UMounteaAdvancedInventoryLoadoutItem();
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	TObjectPtr<UMounteaInventoryItemTemplate> ItemTemplate = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(EditCondition="bUseRandomQuantity", EditConditionHides),
		meta=(NoResetToDefault))
	FIntPoint RandomRange = FIntPoint(1, 100);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	int32 BaseQuantity = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	int32 BaseDurability = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	uint8 bUseRandomQuantity : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	uint8 bAutomaticallyEquip : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(GetOptions="GetAvailableSlotNames"),
		meta=(NoResetToDefault))
	FName EquipmentSlot = NAME_None;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	FName DisplayName = NAME_None;
#endif
	
protected:
	
	UFUNCTION()
	static TArray<FName> GetAvailableSlotNames();
	
	void ApplyDefaults();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
