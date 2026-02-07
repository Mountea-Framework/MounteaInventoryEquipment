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
#include "MounteaAttachableComponent.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentItemInterface.h"
#include "MounteaEquipmentItemComponent.generated.h"


/**
 * UMounteaEquipmentItemComponent represents items that can be equipped by actors or components.
 * Equipment components inherits all behaviour from Attachable Component, and extends the behaviour
 * by providing Equipment State logic.
 *
 * @see [Equipment Components](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/EquipmentSystem)
 * @see IMounteaAdvancedEquipmentItemInterface
 * @see UMounteaAttachableComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Attachable","Mountea|Attachable","Mountea|Equipment","Equipment"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Equipment Item"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaEquipmentItemComponent : public UMounteaAttachableComponent, public IMounteaAdvancedEquipmentItemInterface
{
	GENERATED_BODY()
	
public:
	
	UMounteaEquipmentItemComponent();
	
public:
	
	virtual EEquipmentItemState GetEquipmentItemState_Implementation() const override
	{ return EquipmentItemState; };
	virtual bool SetEquipmentItemState_Implementation(const EEquipmentItemState NewEquipmentItemState) override;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault))
	EEquipmentItemState EquipmentItemState;
};
