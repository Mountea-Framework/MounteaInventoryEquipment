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
#include "Interfaces/Loadouts/MounteaAdvancedInventoryLoadoutsInterface.h"
#include "MounteaAdvancedInventoryLoadoutComponent.generated.h"

class IMounteaAdvancedEquipmentInterface;
class IMounteaAdvancedInventoryInterface;
class UMounteaAdvancedInventoryLoadoutConfig;

/**
 * This component allows actors to have specific loadouts with items that can be loaded, managed,
 * and equipped dynamically at runtime. It integrates with the inventory and equipment systems,
 * ensuring seamless interaction within the Mountea Advanced Inventory System.
 *
 * The loadout is configurable via the editor, enabling persistent and modular inventory setups.
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Loadout","Mountea|Loadout"),
	AutoCollapseCategories=("Variable,Sockets,Tags,Component Tick,Component Replication,Activation,Events,Replication,Asset User Data,Navigation"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Loadout Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryLoadoutComponent : public UActorComponent, public IMounteaAdvancedInventoryLoadoutsInterface
{
	GENERATED_BODY()

public:
	UMounteaAdvancedInventoryLoadoutComponent();
	
public:
	
	virtual bool LoadLoadout_Implementation() override;
	virtual UMounteaAdvancedInventoryLoadoutConfig* GetLoadout_Implementation() const override
	{ return LoadoutConfiguration.LoadSynchronous(); };
	virtual TArray<UMounteaAdvancedInventoryLoadoutItem*> GetLoadoutItems_Implementation() const override;
	virtual bool ShouldLoadAutomatically_Implementation() const override
	{ return bAutoLoad; };
	virtual void SetAutomaticLoad_Implementation(const bool bNewValue) override
	{ if (bNewValue != bAutoLoad) bAutoLoad = bNewValue; }
	
protected:
	
	UFUNCTION(Server, Reliable)
	void Server_LoadLoadout();

protected:
	void InitializeInventoryAndEquipment();
	virtual void BeginPlay() override;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	TScriptInterface<IMounteaAdvancedInventoryInterface> RelatedInventory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	TScriptInterface<IMounteaAdvancedEquipmentInterface> RelatedEquipment;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	TSoftObjectPtr<UMounteaAdvancedInventoryLoadoutConfig> LoadoutConfiguration;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	uint8 bAutoLoad : 1;
};
