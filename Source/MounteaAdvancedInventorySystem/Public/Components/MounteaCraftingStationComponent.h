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

class IMounteaAdvancedCraftingParticipantInterface;

/**
 * UMounteaCraftingStationComponent represents a world-placed crafting workstation.
 * It tracks occupancy state and manages participants interacting with it,
 * replicated so all clients observe consistent station state and participant list.
 *
 * @see IMounteaAdvancedCraftingStationInterface
 * @see UMounteaCraftingParticipantComponent
 */
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CraftingStationState();

public:

	virtual FGameplayTag GetCraftingPlaceType_Implementation() const override
	{
		return CraftingStationType;
	};
	virtual bool IsCraftingPlaceOccupied_Implementation() const override;
	virtual int32 GetCraftingPlaceCapacity_Implementation() const override
	{
		return CraftingPlaceCapacity;
	}
	virtual ECraftingStationState GetCraftingStationState_Implementation() const override
	{
		return CraftingStationState;
	}
	virtual bool SetCraftingStationState_Implementation(const ECraftingStationState NewCraftingStationState) override;
	virtual bool CanBeUsed_Implementation() const override;
	virtual bool StartUsing_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant) override;
	virtual bool StopUsing_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant) override;

public:

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Crafting",
		meta=(Categories="Mountea_Inventory.Crafting,Crafting"),
		meta=(NoResetToDefault))
	FGameplayTag CraftingStationType;

	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_CraftingStationState, VisibleAnywhere, BlueprintReadOnly, Category="Crafting",
		meta=(NoResetToDefault))
	ECraftingStationState CraftingStationState;

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Crafting",
		meta=(NoResetToDefault),
		meta=(UIMin=1, ClampMin=1))
	int32 CraftingPlaceCapacity = 1;

private:

	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadOnly, Category="Crafting",
		meta=(NoResetToDefault),
		meta=(AllowPrivateAccess))
	TArray<TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>> CraftingParticipants;

protected:

	UFUNCTION(Server, Reliable)
	void Server_StartUsing(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);

	UFUNCTION(Server, Reliable)
	void Server_StopUsing(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);

	UFUNCTION(Server, Reliable)
	void Server_SetCraftingStationState(ECraftingStationState NewState);
};
