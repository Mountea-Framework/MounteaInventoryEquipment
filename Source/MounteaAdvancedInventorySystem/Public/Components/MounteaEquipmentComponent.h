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
 *  Mountea Equipment Component
 * 
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
