// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryUIInterface.generated.h"

struct FInventoryNotificationData;
class IMounteaAdvancedInventoryInterface;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for advanced inventory UI interactions.
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryUIInterface
{
	GENERATED_BODY()

public:
	// --- Inventory  ------------------------------

	/**
	 * Retrieves the parent inventory associated with this UI.
	 * @return The parent inventory interface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const = 0;

	/**
	 * Sets the parent inventory for this UI.
	 * @param NewParentInventory - The inventory interface to associate.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);
	virtual void SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) = 0;

	// --- Main UI  ------------------------------

	/**
	 * Creates the inventory UI.
	 * @return True if UI was successfully created, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	bool CreateInventoryUIWrapper();
	virtual bool CreateInventoryUIWrapper_Implementation() = 0;

	/**
	 * Returns the inventory UI.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	UUserWidget* GetInventoryUIWrapper() const;
	virtual UUserWidget* GetInventoryUIWrapper_Implementation() const = 0;

	/**
	 * Removes the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void RemoveInventoryUIWrapper();
	virtual void RemoveInventoryUIWrapper_Implementation() = 0;

	/**
	 * Sets the visibility of the inventory UI.
	 * @param NewVisibility - The new visibility state of the UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void SetInventoryUIWrapperVisibility(const bool bShowInventory);
	virtual void SetInventoryUIWrapperVisibility_Implementation(const bool bShowInventory) = 0;

	// --- Notification  ------------------------------

	/**
	 * Retrieves the notification container widget.
	 * @return A pointer to the notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	UUserWidget* GetNotificationContainer() const;
	virtual UUserWidget* GetNotificationContainer_Implementation() const = 0;

	/**
	 * Sets the notification container widget.
	 * @param NewNotificationContainer - The new notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void SetNotificationContainer(UUserWidget* NewNotificationContainer);
	virtual void SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer) = 0;

	/**
	 * Creates a new inventory notification.
	 * @param NotificationData - The data defining the notification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;

	/**
	 * Removes all active inventory notifications.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void RemoveInventoryNotifications();
	virtual void RemoveInventoryNotifications_Implementation() = 0;
};
