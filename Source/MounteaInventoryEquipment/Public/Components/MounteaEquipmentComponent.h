// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Helpers/MounteaEquipmentHelpers.h"
#include "Interfaces/MounteaEquipmentInterface.h"
#include "MounteaEquipmentComponent.generated.h"

/**
 * Mountea Equipment Component.
 *
 * Implements `IMounteaEquipmentInterface`.
 *
 * Provides equipment functionality to Actors that it is attached to.
 * It allows Actors to equip and manage various types of equipment, which can be added, removed, and manipulated in different ways.
 * The component provides a customizable equipment system that can be tailored to suit the needs of different types of games, such as RPGs or action games.
 * It also offers a range of features such as equipment slots, equipment properties, and events that can be used to create complex interactions between equipment items and game mechanics.
 *
 * This system is designed to be modular and extensible, allowing developers to easily add new features or modify existing ones to suit their specific requirements.
 * If the current implementation does not meet your needs, the `IMounteaEquipmentInterface` exposes all functions, allowing you to recreate the entire system in Blueprints.
 *
 * @see UActorComponent
 * @see IMounteaEquipmentInterface
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Equipment-Interface
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Equipment-Component
*/
UCLASS(ClassGroup=(Mountea), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Mountea Equipment"))
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentComponent : public UActorComponent, public IMounteaEquipmentInterface
{
	GENERATED_BODY()

public:	

	UMounteaEquipmentComponent();

#pragma region FUNCTIONS
protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:

	virtual AActor* GetOwningActor_Implementation() const override;
	
	virtual FText FindSlotForItem_Implementation(const UMounteaInstancedItem* Item) const override;
	virtual int32 FindSlotByID_Implementation(const FText& SlotID) const override;
	virtual TArray<FEquipmentSlot> GetAllSlots_Implementation() const override;
	virtual bool DoesHaveAuthority_Implementation() const override;

	virtual UMounteaBaseUserWidget* GetEquipmentUI_Implementation() const override;
	virtual bool IsItemEquipped_Implementation(const UMounteaInstancedItem* Item, const FText& SlotID) const override;
	
	virtual bool SetEquipmentUI_Implementation(UMounteaBaseUserWidget* NewUI) override;

	virtual void SetEquipmentUIClass_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentWBPInterface")) TSubclassOf<UMounteaBaseUserWidget> NewWBPClass) override;
	virtual TSubclassOf<UMounteaBaseUserWidget> GetEquipmentUIClass_Implementation() const override;

	/**
	 * Checks if the owning actor has a network role of either Authority or Autonomous Proxy.
	 * The function is used to determine if the actor is in a suitable state to perform certain operations,
	 * particularly those that are visual or UI-related and may not be relevant or appropriate for actors in other network roles.
	 * 
	 * @return True if the owning actor is either Authority or Autonomous Proxy, false otherwise.
	 */
	bool IsAuthorityOrAutonomousProxy() const;
	
	virtual FInventoryUpdateResult EquipItem_Implementation(UMounteaInstancedItem* ItemToEquip, const FText& SlotID) override;
	virtual FInventoryUpdateResult UnEquipItem_Implementation(UMounteaInstancedItem* Item, const FText& SlotID) override;
	
	virtual bool CanEquipItem_Implementation(const UMounteaInstancedItem* ItemToEquip) const override;
	virtual bool CanUnEquipItem_Implementation(const UMounteaInstancedItem* ItemToUnequip) const override;

protected:
	
	UFUNCTION(Server, Reliable, WithValidation)
	void EquipItem_Server(UMounteaInstancedItem* ItemToEquip, const FText& SlotID);
	UFUNCTION(Server, Reliable, WithValidation)
	void UnEquipItem_Server(UMounteaInstancedItem* Item, const FText& SlotID);
	
	UFUNCTION(Server, Unreliable)
	void PostEquipmentUpdated(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemEquipped(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemUnequipped(const FInventoryUpdateResult& UpdateContext);

	UFUNCTION(Client, Unreliable)
	void PostEquipmentUpdated_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemEquipped_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemUnequipped_Client(const FInventoryUpdateResult& UpdateContext);

	UFUNCTION(NetMulticast, Unreliable)
	void PostEquipmentUpdated_Multicast(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(NetMulticast, Unreliable)
	void PostItemEquipped_Multicast(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(NetMulticast, Unreliable)
	void PostItemUnequipped_Multicast(const FInventoryUpdateResult& UpdateContext);

	UFUNCTION()
	void PostEquipmentUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemEquipped_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemUnequipped_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	
	UFUNCTION()
	void PostEquipmentUpdated_Multicast_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemEquipped_Multicast_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemUnequipped_Multicast_RequestUpdate(const FInventoryUpdateResult& UpdateContext);

public:
	
	virtual FOnEquipmentUpdated& GetEquipmentUpdatedHandle() override { return OnEquipmentUpdated; };
	virtual FOnEquipmentUpdated& GetSlotEquippedHandle() override { return OnSlotEquipped; };
	virtual FOnEquipmentUpdated& GetSlotUnEquippedHandle() override { return OnSlotUnequipped; }

/*===============================================================================
		IN PROGRESS
		
		Following functions are using being changed.
===============================================================================*/

		
/*===============================================================================
		SUBJECT OF CHANGE
		
		Following functions are using outdated, wrong class definitions and functions.
===============================================================================*/
		
protected:
	
	UFUNCTION()
	void OnRep_Equipment();

#if WITH_EDITOR
protected:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
	
#pragma endregion
	
#pragma region VARIABLES
	
protected:
	
	UPROPERTY(SaveGame, EditAnywhere, Category="1. Required", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> EquipmentUIClass;
	
	UPROPERTY(SaveGame, Category="1. Required", EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Equipment)
	TArray<FEquipmentSlot> EquipmentSlots;
	
private:

	// Filled from RemoveFromItem to keep track of Items that were removed
	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	TArray<FEquipmentSlot> ModifiedSlots;

	UPROPERTY(Transient, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false))
	UMounteaBaseUserWidget* EquipmentUI;
	
	UPROPERTY(Transient, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false))
	int32 ReplicatedItemsKey = 0;

	UPROPERTY(EditAnywhere, Category="2. Debug")
	float Duration_RequestSyncTimerHandle = 0.1f;

	UPROPERTY()
	FTimerHandle TimerHandle_RequestEquipmentSyncTimerHandle;
	
#pragma endregion

#pragma region EVENTS

	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnEquipmentUpdated (Server)" )
	FOnEquipmentUpdated OnEquipmentUpdated;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnEquipmentUpdated (Client)" )
	FOnEquipmentUpdated OnEquipmentUpdated_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnEquipmentUpdated (Multicast)")
	FOnEquipmentUpdated OnEquipmentUpdated_Multicast;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotEquipped (Server)" )
	FOnEquipmentUpdated OnSlotEquipped;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotEquipped (Client)" )
	FOnEquipmentUpdated OnSlotEquipped_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotEquipped (Multicast)")
	FOnEquipmentUpdated OnSlotEquipped_Multicast;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotUnequipped (Server)" )
	FOnEquipmentUpdated OnSlotUnequipped;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotUnequipped (Multicast)")
	FOnEquipmentUpdated OnSlotUnequipped_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotUnequipped (Multicast)")
	FOnEquipmentUpdated OnSlotUnequipped_Multicast;
	
#pragma endregion 
	
};