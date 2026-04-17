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
#include "Components/ActorComponent.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingStationInterface.h"
#include "MounteaCraftingStationComponent.generated.h"

UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Crafting","Mountea|Crafting"),
	AutoCollapseCategories=("Variable,Sockets,Tags,Component Tick,Component Replication,Activation,Events,Replication,Asset User Data,Navigation"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Crafting Station Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingStationComponent : public UActorComponent, public IMounteaAdvancedCraftingStationInterface
{
	GENERATED_BODY()

public:
	
	UMounteaCraftingStationComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	
	virtual FGameplayTag GetCraftingPlaceType_Implementation() const override
	{
		return CraftingStationType;
	};
	virtual bool IsCraftingPlaceOccupied_Implementation() const override;
	virtual int32 GetCraftingPlaceCapacity_Implementation() const override;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crafting",
		meta=(Categories="Mountea_Inventory.Crafting,Crafting"),
		meta=(NoResetToDefault))
	FGameplayTag CraftingStationType;
};
