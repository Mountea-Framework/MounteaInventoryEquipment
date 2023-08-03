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

protected:

	virtual void BeginPlay() override;

public:
	
#pragma region FUNCTIONS

	virtual bool EquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex) override;
	virtual bool UnEquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex) override;
	virtual bool IsItemEquipped_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex) override;
	virtual TArray<FMounteaEquipmentSlotData> GetAllSlots_Implementation() const override;
	
#pragma endregion

protected:
	
#pragma region VARIABLES

	UPROPERTY(Category="1. Required", EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMounteaEquipmentSlotData> EquipmentSlotData;
	
#pragma endregion

#if WITH_EDITOR
protected:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
	
};