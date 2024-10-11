// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryItem.h"
#include "UObject/Interface.h"
#include "MounteaEquipmentInterface.generated.h"

class UMounteaEquipmentSlot;
struct FMounteaEquipmentSlotData;
struct FMounteaEquipmentSlots;
class UMounteaInventoryItem;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentUpdated, const FString&, UpdateContext);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotUpdated,  const class UMounteaInventoryItemBase*, Item, const FString&, UpdateContext);

/**
 * Mountea Equipment Interface.
 *
 * Provides functionality to manage Equipment, for example:
 * * Equip Item
 * * Unequip Item
 * * Move item to Equipment slot
 *
 * Equipment is standalone, however, works best with `IMounteaInventoryInterface` as source of Items.
 *
 * Equipment is player-specific and thus should be attached to Player Controller.
 * If player equipment is tied to Player pawn (if player dies, his loot stays on his corpse), then use Player Pawn instead.
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaEquipmentInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	FString FindSlotForItem(const UMounteaInventoryItem* Item) const;
	virtual FString FindSlotForItem_Implementation(const UMounteaInventoryItem* Item) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	UMounteaEquipmentSlot* FindSlotByID(const FString& SlotID) const;
	virtual UMounteaEquipmentSlot* FindSlotByID_Implementation(const FString& SlotID) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool EquipItem(UMounteaInventoryItem* ItemToEquip, const FString& SlotID);
	virtual bool EquipItem_Implementation(UMounteaInventoryItem* ItemToEquip,const FString& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool UnEquipItem(UMounteaInventoryItem* ItemToEquip, const FString& SlotID);
	virtual bool UnEquipItem_Implementation(UMounteaInventoryItem* ItemToEquip, const FString& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool IsItemEquipped(const UMounteaInventoryItem* ItemToEquip, const FString& SlotID) const;
	virtual bool IsItemEquipped_Implementation(const UMounteaInventoryItem* ItemToEquip, const FString& SlotID) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	TArray<FMounteaEquipmentSlotData> GetAllSlots() const;
	virtual TArray<FMounteaEquipmentSlotData> GetAllSlots_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool CanEquipItem(const UMounteaInventoryItem* ItemToEquip) const;
	virtual bool CanEquipItem_Implementation(const UMounteaInventoryItem* ItemToEquip) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	UMounteaBaseUserWidget* GetEquipmentUI() const;
	virtual UMounteaBaseUserWidget* GetEquipmentUI_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool SetEquipmentUI(UMounteaBaseUserWidget* NewUI);
	virtual bool SetEquipmentUI_Implementation(UMounteaBaseUserWidget* NewUI) = 0;
	
public:

	virtual FOnEquipmentUpdated& GetEquipmentUpdatedHandle() = 0;
	virtual FOnSlotUpdated& GetSlotEquippedHandle() = 0;
	virtual FOnSlotUpdated& GetSlotUnEquippedHandle() = 0;
};
