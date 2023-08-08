// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Definitions/MounteaEquipmentSlot.h"
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
	
	virtual FString FindSlotForItem_Implementation(const UMounteaInventoryItemBase* Item) const override;
	virtual UMounteaEquipmentSlot* FindSlotByID_Implementation(const FString& SlotID) const override;
	virtual TArray<FMounteaEquipmentSlotData> GetAllSlots_Implementation() const override;
	
	virtual bool EquipItem_Implementation(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID) override;
	virtual bool UnEquipItem_Implementation(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID) override;
	
	virtual bool IsItemEquipped_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID) const override;
	
	virtual bool CanEquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip) const override;

	virtual FOnEquipmentUpdated& GetEquipmentUpdatedHandle() override { return OnEquipmentUpdated; };
	virtual FOnSlotUpdated& GetSlotEquippedHandle() override { return OnSlotEquipped; };
	virtual FOnSlotUpdated& GetSlotUnEquippedHandle() override { return OnSlotUnequipped; };

protected:
	
	UFUNCTION(Server, Reliable, WithValidation) void EquipItem_Server(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID);
	UFUNCTION(Server, Reliable, WithValidation) void UnEquipItem_Server(UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID);
	UFUNCTION(NetMulticast, Unreliable) void EquipItem_Multicast(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID);
	UFUNCTION(NetMulticast, Unreliable) void UnEquipItem_Multicast(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID);
	UFUNCTION() void OnRep_Equipment();
	
#pragma endregion
	
#pragma region VARIABLES
protected:
	
	UPROPERTY(SaveGame, Category="1. Required", EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Equipment)
	TArray<FMounteaEquipmentSlotData> EquipmentSlotData;

private:
	
	UPROPERTY()
	int32 ReplicatedItemsKey = 0;
	
#pragma endregion

#pragma region EVENTS

	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnEquipmentUpdated (Server)" )
	FOnEquipmentUpdated OnEquipmentUpdated;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnEquipmentUpdated (Client)" )
	FOnEquipmentUpdated OnEquipmentUpdated_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnEquipmentUpdated (Multicast)")
	FOnEquipmentUpdated OnEquipmentUpdated_Multicast;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotEquipped (Server)" )
	FOnSlotUpdated OnSlotEquipped;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotEquipped (Client)" )
	FOnSlotUpdated OnSlotEquipped_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotEquipped (Multicast)")
	FOnSlotUpdated OnSlotEquipped_Multicast;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotUnequipped (Server)" )
	FOnSlotUpdated OnSlotUnequipped;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotUnequipped (Multicast)")
	FOnSlotUpdated OnSlotUnequipped_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Equipment", DisplayName="OnSlotUnequipped (Multicast)")
	FOnSlotUpdated OnSlotUnequipped_Multicast;
	
#pragma endregion 

#if WITH_EDITOR
protected:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
	
};