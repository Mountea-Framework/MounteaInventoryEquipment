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
#include "MounteaAdvancedInventorySharedHUDInterface.generated.h"

struct FGameplayTagContainer;
struct FGameplayTag;
struct FInventoryNotificationData;

class UWidget;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventorySharedHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventorySharedHUDInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Creates and initializes the main wrapper widget for the inventory UI.
	 *
	 * @return True if the wrapper widget was created successfully, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	bool CreateWrapperWidget();
	virtual bool CreateWrapperWidget_Implementation() = 0;

	/**
	 * Returns the currently active main wrapper widget.
	 *
	 * @return Wrapper widget if available, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	UUserWidget* GetWrapperWidget() const;
	virtual UUserWidget* GetWrapperWidget_Implementation() const = 0;

	/**
	 * Removes and cleans up the main wrapper widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	void RemoveWrapperWidget();
	virtual void RemoveWrapperWidget_Implementation() = 0;
	
	/**
	 * Executes a widget command routed through the UI manager hierarchy.
	 * Use this to notify UI layers about external events (for example refreshing item count after an item action).
	 * 
	 * Command routing should respect wrapper-to-child hierarchy, so parent managers can delegate to active views.
	 * 
	 * @param Command Command to Process
	 * @param OptionalPayload Optional Payload to pass with the command
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory")
	void ExecuteWidgetCommand(const FString& Command, UObject* OptionalPayload);
	virtual void ExecuteWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload) = 0;
	
	// --- Notification

	/**
	 * Retrieves the notification container widget.
	 * @return A pointer to the notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	UWidget* GetNotificationContainer() const;
	virtual UWidget* GetNotificationContainer_Implementation() const = 0;

	/**
	 * Sets the notification container widget.
	 * @param NewNotificationContainer - The new notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	void SetNotificationContainer(UWidget* NewNotificationContainer);
	virtual void SetNotificationContainer_Implementation(UWidget* NewNotificationContainer) = 0;

	/**
	 * Creates a new inventory notification.
	 * @param NotificationData - The data defining the notification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;

	/**
	 * Removes all active inventory notifications.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	void RemoveInventoryNotifications();
	virtual void RemoveInventoryNotifications_Implementation() = 0;
	
	// --- Widget States
	
	/**
	 * Returns all currently active Widget State tags tracked by the target UI Manager.
	 *
	 * UI Manager states are used to keep track of which UI widgets are currently active/visible/in use
	 * within the Wrapper (for example Modal Window, Tooltip, Context Menu, Inventory Panel, etc.).
	 * Each widget that is created/added to the Wrapper should contribute its predefined Gameplay Tag
	 * so the Wrapper and UI Manager can quickly query what is active and react accordingly.
	 *
	 * @return Container of Gameplay Tags representing active UI states/widgets currently tracked by the UI Manager.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		meta=(Categories="Mountea_Inventory.WidgetState,State"),
		DisplayName="Get Widget States")
	FGameplayTagContainer GetWidgetStates() const;
	virtual FGameplayTagContainer GetWidgetStates_Implementation() const = 0;

	/**
	 * Overwrites UI Manager state tracking with a new tag container.
	 *
	 * This function replaces the current tracked Widget State tags with the provided container.
	 * It exists primarily as a write-back mechanism for systems that manipulate state tags in bulk
	 * (including helper functions such as AddWidgetStateTag, RemoveWidgetStateTag, ClearWidgetStateTags
	 * or AppendWidgetStateTags).
	 *
	 * @note
	 * - This function only updates state tracking. It should not be assumed to create/destroy widgets by itself.
	 * - Implementations should ensure the stored state remains consistent with actual active widgets.
	 *
	 * @param NewStates Container of Gameplay Tags that will become the UI Manager's active widget states.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Set Widget States")
	void SetWidgetStates(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& NewStates);
	virtual void SetWidgetStates_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& NewStates) = 0;

	/**
	 * Adds a new Widget State tag to the target UI Manager's active state container.
	 *
	 * Expected flow:
	 * - A child widget (e.g., ModalWindow) is created/added to the UI Manager.
	 * - That widget provides its state tag definition.
	 * - UI Manager registers the tag so the system can track that the widget is now active.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to add.
	 * @return True if the tag was added (was not present before). False if invalid or already present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Add Widget State Tag")
	bool AddWidgetStateTag(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag);
	virtual bool AddWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag) = 0;

	/**
	 * Removes an existing Widget State tag from the target UI Manager's active state container.
	 *
	 * This should be called when a UI element is removed/hidden/destroyed so the Wrapper no longer reports
	 * it as active, preventing stale state and enabling correct UI Manager decisions.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to remove.
	 * @return True if the tag was removed. False if invalid or not found.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Remove Widget State Tag")
	bool RemoveWidgetStateTag(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag);
	virtual bool RemoveWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag) = 0;

	/**
	 * Checks whether the target UI Manager currently contains a given Widget State tag.
	 *
	 * Used for querying whether a particular UI element/state is currently active within the Wrapper.
	 * Example: prevent opening another modal if "UI.Modal.Active" is already present, or change input rules.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to check.
	 * @param bExactMatch If true, requires an exact tag match. If false, allows hierarchical matching
	 *                    (e.g., checking "UI.Modal" would match "UI.Modal.Active").
	 * @return True if the UI Manager currently reports the tag as active; otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Has Widget State Tag")
	bool HasWidgetStateTag(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag, 
		bool bExactMatch) const;
	virtual bool HasWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag, 
		bool bExactMatch) const = 0;

	/**
	 * Clears all Widget State tags tracked by the target UI Manager.
	 *
	 * Typically used during full teardown/reset scenarios such as RemoveWrapperWidget, rebuilding the UI,
	 * or when the UI Manager needs to force the Wrapper into a clean baseline state.
	 *
	 * Note:
	 * - This does not automatically destroy UI widgets by itself. It only clears the tracked state tags.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Clear Widget State Tags")
	void ClearWidgetStateTags();
	virtual void ClearWidgetStateTags_Implementation() = 0;

	/**
	 * Appends multiple Widget State tags to the target UI Manager's active state container.
	 *
	 * Useful when adding a composite widget (or UI mode) that activates multiple tracked states at once,
	 * or when synchronizing the UI Manager to a known set of states provided by another system.
	 *
	 * @param TagsToAppend Container of Gameplay Tags to add to the UI Manager's active state container.
	 * @return True if at least one new tag was added. False if empty or all tags were already present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Append Widget State Tags")
	bool AppendWidgetStateTags(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& TagsToAppend);
	virtual bool AppendWidgetStateTags_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& TagsToAppend) = 0;
};
