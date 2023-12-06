// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaDefaultsConfig.generated.h"

class UMounteaInventorySlotBaseWidget;
class UMounteaEquipmentSlotBaseWidget;
class UMounteaTransactionPayload;
class UMounteaBaseUserWidget;

/**
 * A Data Asset class that holds default configuration settings for various inventory-related classes.
 * This configuration is utilized throughout the Mountea Inventory & Equipment System to ensure consistent and customizable behavior.
 *
 * @see UDataAsset
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Defaults-Config
 */
UCLASS( Blueprintable, BlueprintType, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Defaults Config")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaDefaultsConfig : public UDataAsset
{
	GENERATED_BODY()

public:

/*==================================
		INVENTORY
==================================*/

	/**
	 * Default class for the inventory UI widget.
	 * This class must implement the MounteaInventoryWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryClass;

	/**
	 * Default class for the inventory item container widget.
	 * This class must implement the MounteaInventoryItemContainerWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Item", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemContainerWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemContainerClass;

	/**
	 * Default class for the individual inventory item widget.
	 * This class must implement the MounteaInventoryItemWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Item", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemClass;

	/**
	 * Default class for the inventory item tooltip widget.
	 * This class must implement the MounteaTooltipWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Item", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaTooltipWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemTooltipClass;

	/**
	 * Default class for the drag-and-drop functionality for inventory items.
	 * This class must implement the MounteaDragDropWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Item", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaDragDropWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemDragDropClass;

	/**
	* Default class for the inventory slot widget.
	* This class must implement the MounteaInventorySlotWBPInterface.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Slot", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventorySlotWBPInterface"))
	TSoftClassPtr<UMounteaInventorySlotBaseWidget> DefaultInventoryItemSlotClass;

	/**
	 * Default class for the drag-and-drop payload.
	 * This class is used to manage the data transferred during drag-and-drop operations.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Item", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryNotificationWBPInterface"))
	TSoftClassPtr<UMounteaTransactionPayload> DefaultDragDropPayloadClass;

	/**
	 * Default class for the inventory category container widget.
	 * This class must implement the MounteaCategoryContainerWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Category", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaCategoryContainerWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryCategoryContainerClass;

	/**
	 * Default class for the inventory category widget.
	 * This class must implement the MounteaCategoryWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Category", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaCategoryWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryCategoryClass;

	/**
	 * Default class for the inventory item action container widget.
	 * This class must implement the MounteaItemActionContainerWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Action", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaItemActionContainerWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemActionContainerClass;

	/**
	 * Default class for the inventory item action widget.
	 * This class must implement the MounteaItemActionWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Action", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaItemActionWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemActionClass;

	/**
	 * Default class for the inventory notification widget.
	 * This class must implement the MounteaInventoryNotificationWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Notification", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryNotificationWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryNotificationClass;

/*==================================
		EQUIPMENT
==================================*/

	/**
	 * Default class for the equipment UI widget.
	 * This class must implement the MounteaEquipmentWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Equipment", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultEquipmentClass;

	/**
	* Default class for the equipment slot widget.
	* This class must implement the MounteaEquipmentSlotWBPInterface.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Equipment|Slot", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface"))
	TSoftClassPtr<UMounteaEquipmentSlotBaseWidget> DefaultEquipmentSlotClass;

	/**
	 * Default class for the individual equipment item widget.
	 * This class must implement the MounteaEquipmentItemWBPInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Equipment|Item", meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentItemWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultEquipmentItemClass;
};
