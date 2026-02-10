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
#include "UObject/Interface.h"
#include "MounteaAdvancedEquipmentInterface.generated.h"

struct FAttachmentContainer;
struct FAttachmentSlot;
struct FInventoryItem;

class UMounteaInventoryItemTemplate;

enum class EEquipmentItemState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipmentItemActivated, UObject*, EquipmentItem);

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedEquipmentInterface provides specialized functionality for equipment management systems.
 * Equipment interfaces extend attachment containers with equipment-specific operations, stat modifications,
 * and character integration for comprehensive equipment systems within the inventory framework.
 *
 * @see [Equipment Interface](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/EquipmentSystem)
 * @see UMounteaEquipmentComponent
 * @see IMounteaAdvancedAttachmentContainerInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedEquipmentInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	AActor* EquipItem(const FInventoryItem& ItemDefinition) const;
	virtual AActor* EquipItem_Implementation(const FInventoryItem& ItemDefinition) const = 0; 
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	AActor* EquipItemToSlot(const FName& SlotId, const FInventoryItem& ItemDefinition);
	virtual AActor* EquipItemToSlot_Implementation(const FName& SlotId, const FInventoryItem& ItemDefinition) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool UnequipItem(const FInventoryItem& ItemDefinition, bool bUseFallbackSlot = false);
	virtual bool UnequipItem_Implementation(const FInventoryItem& ItemDefinition, bool bUseFallbackSlot = false) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool UnequipItemFromSlot(const FName& SlotId, bool bUseFallbackSlot = false);
	virtual bool UnequipItemFromSlot_Implementation(const FName& SlotId, bool bUseFallbackSlot = false) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool IsEquipmentItemEquipped(const FInventoryItem& ItemDefinition) const;
	virtual bool IsEquipmentItemEquipped_Implementation(const FInventoryItem& ItemDefinition) const = 0;
};

