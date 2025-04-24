// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryUIStatics.generated.h"

class IMounteaAdvancedInventoryCategoryWidgetInterface;
class IMounteaInventoryBaseWidgetInterface;
/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*************************************************************/
	/********************* TEMPLATES **********************/
	/*************************************************************/
public:
	template<typename ReturnType, typename Func, typename... Args>
	static ReturnType ExecuteIfImplements(UObject* Target, const TCHAR* FunctionName, Func Function, Args&&... args)
	{
		if (!IsValid(Target))
		{
			LOG_ERROR(TEXT("[%s] Invalid Target provided!"), FunctionName);
			if constexpr (!std::is_void_v<ReturnType>)
				return ReturnType{};
			else return;
		}

		if (Target->Implements<UMounteaAdvancedInventoryUIInterface>())
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				Function(Target, Forward<Args>(args)...);
				return;
			}
			return Function(Target, Forward<Args>(args)...);
		}

		LOG_ERROR(TEXT("[%s] Target does not implement 'MounteaAdvancedInventoryUIInterface'!"), FunctionName);
		if constexpr (!std::is_void_v<ReturnType>)
			return ReturnType{};
		else return;
	}

	/*************************************************************/
	/******************* BLUEPRINTABLE *********************/
	/*************************************************************/
public:

	static APlayerController* FindPlayerController(AActor* Actor, int SearchDepth);
	
	// --- Inventory  ------------------------------
	
	/**
	 * Retrieves the parent inventory associated with this UI.
	 * @param Target The UI interface to get the parent inventory from
	 * @return The parent inventory interface that manages the items and slots
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Getter"))
	static TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Sets the parent inventory for this UI.
	 * @param Target The UI interface to set the parent inventory on
	 * @param NewParentInventory The inventory interface to associate with this UI
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);

	// --- Main UI  ------------------------------

	/**
	 * Creates and initializes the inventory UI widgets.
	 * @param Target The UI interface to create the inventory UI for
	 * @return True if UI was successfully created and initialized, otherwise false
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static bool CreateInventoryUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Returns the inventory UI.
	 * @param Target The UI interface to get the inventory UI from
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Getter"))
	static UUserWidget* GetInventoryUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Removes the inventory UI from the viewport and cleans up resources.
	 * @param Target The UI interface to remove the inventory UI from
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveInventoryUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Sets the visibility state of the inventory UI widget.
	 * @param Target The UI interface to change visibility for
	 * @param NewVisibility The new visibility state to apply to the UI (Visible, Hidden, Collapsed, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryUIWrapperVisibility(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const bool bShowInventory);

	// --- Notification  ------------------------------
#pragma region Notification
	
	/**
	 * Retrieves the widget that contains and manages inventory notifications.
	 * @param Target The UI interface to get the notification container from
	 * @return A pointer to the user widget that serves as the notification container
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Getter"))
	static UUserWidget* GetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Sets the widget that will contain and manage inventory notifications.
	 * @param Target The UI interface to set the notification container for
	 * @param NewNotificationContainer The user widget to use as the notification container
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void SetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, UUserWidget* NewNotificationContainer);

	/**
	 * Creates and displays a new inventory notification based on the provided data.
	 * @param Target The UI interface to create the notification in
	 * @param NotificationData The data structure containing all notification parameters (text, duration, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void CreateInventoryNotification(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FInventoryNotificationData& NotificationData);

	/**
	 * Removes all currently active inventory notifications from the UI.
	 * @param Target The UI interface to remove notifications from
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveInventoryNotifications(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

#pragma endregion
	
	// --- InventoryUI  ------------------------------
#pragma region InventoryUI
	
	/**
	 * 
	 * @param Target 
	 * @param Parent 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void InitializeInventoryWidget(const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent);

	/**
	 * 
	 * @param Target 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveInventoryWidgetWrapper(const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target);

	/**
	 * 
	 * @param Target 
	 * @param ParentInventory 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static bool SetSourceInventory(const TScriptInterface<IMounteaInventoryBaseWidgetInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory);

	/**
	 * 
	 * @param Target 
	 * @param SelectedCategoryId 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void CategorySelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FString& SelectedCategoryId);

	/**
	 * 
	 * @param Target 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Getter"))
	static FString GetSelectedCategoryId(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);
	
	/**
	 * 
	 * @param Target 
	 * @param ItemGuid 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void ItemSelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FGuid& ItemGuid);

	/**
	 * 
	 * @param Target 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Getter"))
	static FGuid GetSelectedItemGuid(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

#pragma endregion

	// --- Inventory
#pragma region Inventory

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryOwningInventoryUI(UUserWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	
#pragma endregion

	// --- Categories Wrapper Widget ------------------------------
#pragma region CategoriesWrapper

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetCategoriesWrapperOwningInventoryUI(UUserWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetActiveCategoryId(UUserWidget* Target, const FString& ActiveCategoryId);

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Getter"))
	static FString GetActiveCategoryId(UUserWidget* Target);
	
#pragma endregion
	
	// --- Category Widget ------------------------------
#pragma region Category
	
	/**
	 * 
	 * @param Target 
	 * @param CategoryId 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryCategoryKey(UUserWidget* Target, const FString& CategoryId);

	/**
	 * 
	 * @param Target  
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Getter"))
	static FString GetInventoryCategoryKey(UUserWidget* Target);

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetCategoryOwningInventoryUI(UUserWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * 
	 * @param Target 
	 * @param bIsActive 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetActiveState(UUserWidget* Target, const bool bIsActive);
	
#pragma endregion	
	
	// --- Item Widget ------------------------------
#pragma region Item
	
	/**
	 * 
	 * @param Target 
	 * @param ItemGuid 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryItemId(UUserWidget* Target, const FGuid& ItemGuid);

	/**
	 * 
	 * @param Target 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Items", meta=(CustomTag="MounteaK2Getter"))
	static FGuid GetInventoryItemId(UUserWidget* Target);

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items", meta=(CustomTag="MounteaK2Setter"))
	static void SetItemOwningInventoryUI(UUserWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	
#pragma endregion

	// --- Item Slot Widget ------------------------------
#pragma region ItemSlot

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots", meta=(CustomTag="MounteaK2Setter"))
	static void SetItemSlotOwningInventoryUI(UUserWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	
#pragma endregion

	// --- Item Slots Wrapper ------------------------------
#pragma region ItemSlotsWrapper

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots", meta=(CustomTag="MounteaK2Setter"))
	static void SetItemSlotsWrapperOwningInventoryUI(UUserWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	
#pragma endregion
};
