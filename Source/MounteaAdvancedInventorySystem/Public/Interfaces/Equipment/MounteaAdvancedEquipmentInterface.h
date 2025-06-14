// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedEquipmentInterface.generated.h"

struct FAttachmentContainer;
struct FAttachmentSlot;
struct FInventoryItem;
class UMounteaInventoryItemTemplate;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedEquipmentInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	const FAttachmentContainer& GetAttachmentContainer() const;
	virtual const FAttachmentContainer& GetAttachmentContainer_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool TryAttachItem(const FInventoryItem& Item);
	virtual bool TryAttachItem_Implementation(const FInventoryItem& Item) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool AttachItem(const FInventoryItem& Item, const FName& SlotID);
	virtual bool AttachItem_Implementation(const FInventoryItem& Item, const FName& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool TryDetachItem(const FName& SlotID);
	virtual bool TryDetachItem_Implementation(const FName& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool DetachItem(const FName& SlotID);
	virtual bool DetachItem_Implementation(const FName& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool IsSlotOccupied(const FName& SlotID) const;
	virtual bool IsSlotOccupied_Implementation(const FName& SlotID) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool IsItemAttached(const FGuid& ItemGuid) const;
	virtual bool IsItemAttached_Implementation(const FGuid& ItemGuid) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	FName FindFirstCompatibleSlot(const FInventoryItem& Item) const;
	virtual FName FindFirstCompatibleSlot_Implementation(const FInventoryItem& Item) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	FAttachmentSlot GetSlotByID(const FName& SlotID) const;
	virtual FAttachmentSlot GetSlotByID_Implementation(const FName& SlotID) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	void ClearAllEquipment();
	virtual void ClearAllEquipment_Implementation() = 0;
};

