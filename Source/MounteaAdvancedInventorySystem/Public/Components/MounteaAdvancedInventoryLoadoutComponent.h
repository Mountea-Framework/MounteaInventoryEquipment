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
 * UMounteaAdvancedInventoryLoadoutComponent applies predefined item sets to an actor at runtime.
 * It resolves the owner's inventory and equipment interfaces, adds configured loadout items to
 * inventory, and optionally equips compatible items after they are loaded.
 *
 * @see [Loadout System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/LoadoutSystem)
 * @see IMounteaAdvancedInventoryLoadoutsInterface
 * @see UMounteaAdvancedInventoryLoadoutConfig
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
	
	/**
	 * Cached inventory interface found on the owning actor.
	 *
	 * This is initialized during BeginPlay and used as the target inventory when loading
	 * items from the configured loadout.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	TScriptInterface<IMounteaAdvancedInventoryInterface> RelatedInventory;
	
	/**
	 * Cached equipment interface found on the owning actor.
	 *
	 * When valid, loadout items marked for automatic equip are equipped through this interface
	 * after they are successfully added to inventory.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	TScriptInterface<IMounteaAdvancedEquipmentInterface> RelatedEquipment;
	
	/**
	 * Soft reference to the loadout configuration asset used by this component.
	 *
	 * The asset defines which items should be granted (and optionally equipped) when the
	 * loadout is executed.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	TSoftObjectPtr<UMounteaAdvancedInventoryLoadoutConfig> LoadoutConfiguration;
	
	/**
	 * If true, the component attempts to load the configured loadout automatically on BeginPlay.
	 *
	 * Automatic loading is authority-driven and runs only on the owning actor's authoritative instance.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoEditInline))
	uint8 bAutoLoad : 1;
};
