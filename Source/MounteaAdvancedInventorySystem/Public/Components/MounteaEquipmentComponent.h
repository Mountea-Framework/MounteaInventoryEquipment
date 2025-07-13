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
#include "MounteaAttachmentContainerComponent.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "MounteaEquipmentComponent.generated.h"


/**
 * UMounteaEquipmentComponent extends attachment containers with specialized equipment functionality.
 * Equipment components provide enhanced attachment management specifically designed for character equipment
 * systems with additional equipment-specific interfaces and behaviors.
 * Enhances Attachable System.
 *
 * @see [Equipment System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/EquipmentSystem)
 * @see UMounteaAttachmentContainerComponent
 * @see IMounteaAdvancedEquipmentInterface
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
    AutoExpandCategories=("Mountea","Equipment","Mountea|Equipment"),
    HideCategories=("Cooking","Collision"),
    meta=(BlueprintSpawnableComponent, DisplayName="Mountea Equipment Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaEquipmentComponent : public UMounteaAttachmentContainerComponent, public IMounteaAdvancedEquipmentInterface
{
	GENERATED_BODY()

public:

	UMounteaEquipmentComponent();
};
