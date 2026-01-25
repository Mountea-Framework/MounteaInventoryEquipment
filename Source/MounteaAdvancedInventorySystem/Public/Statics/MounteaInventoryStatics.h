// MounteaInventoryStatics.h
#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryStatics.generated.h"

class UMounteaAdvancedInventorySettings;
class UMounteaAdvancedInventorySettingsConfig;
class UMounteaSelectableInventoryItemAction;
class UMounteaCallbackInventoryItemAction;
class UMounteaInventoryItemAction;

enum class EInventoryNotificationCategory : uint8;
enum class EInventoryNotificationType : uint8;
enum class EInventoryItemActionCallback : uint8;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
#pragma region ItemActions
	
	/**
	 * Initializes the action with the target item and owning inventory.
	 * This must be called before executing the action to establish the operating context.
	 * 
	 * @param Target The Item Action.
	 * @param TargetItem The inventory item this action will operate on.
	 * @param OwningInventory The inventory interface that contains the target item.
	 * @param ContextPayload Optional context object for additional data (e.g., player controller, UI context).
	 * 
	 * @return True if initialization was successful and action is ready to execute, false if setup failed.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Initialize Inventory Action")
	static bool InitializeItemAction(UMounteaInventoryItemAction* Target, const FInventoryItem& TargetItem, 
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& OwningInventory, UObject* ContextPayload = nullptr);
	
	/**
	 * Executes the inventory action on the specified target item.
	 * 
	 * This is the main entry point for all inventory actions, regardless of implementation type:
	 * - Selectable actions: Execute logic and does UI related stuff
	 * - Callback actions: Execute logic directly and returns result
	 * 
	 * @param Target The Item Action.
	 * @param TargetItem The inventory item to perform the action on.
	 * 
	 * @return True if the action was executed successfully, false if it failed.
	 * 
	 * @note Always call InitializeItemAction() before executing actions to ensure proper context.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="start"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Execute Inventory Action")
	static bool ExecuteInventoryAction(UMounteaInventoryItemAction* Target, const FInventoryItem& TargetItem);
	
	/**
	 * Gets the inventory interface that owns the target item.
	 * 
	 * @param Target The Item Action.
	 * @return Script interface to the inventory containing the target item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Inventory Action Tags")
	static FGameplayTagContainer GetItemActionTags(UMounteaInventoryItemAction* Target);

	/**
	 * Retrieves the item action flags for the specified target.
	 * This function returns the current set of flags that indicate which item actions are available for the target.
	 *
	 * @param Target The Item Action for which to retrieve the item action flags.
	 * @return The current set of item action flags as an EInventoryItemActionCallback enum value.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Inventory Action Flags")
	static EInventoryItemActionCallback GetItemActionFlags(const UMounteaSelectableInventoryItemAction* Target);
	
	/**
	 * Checks if a specific flag is set in the given flag container.
	 *
	 * @param Target Item Action to check for flags.
	 * @param FlagToCheck The specific flag to test for.
	 * @return True if the flag is set, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Has Inventory Action Flag")
	static bool ItemAction_HasActionFlag(UMounteaSelectableInventoryItemAction* Target, const EInventoryItemActionCallback FlagToCheck);

	/**
	 * Adds a specific flag to the flag container.
	 *
	 * @param Target Item Action to add new flags to.
	 * @param FlagToAdd The flag to add.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Add Inventory Action Flag")
	static void ItemAction_AddActionFlag(UMounteaSelectableInventoryItemAction* Target, EInventoryItemActionCallback FlagToAdd);

	/**
	 * Removes a specific flag from the flag container.
	 *
	 * @param Target Item Action to remove flags from.
	 * @param FlagToRemove The flag to clear.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Remove Inventory Action Flag")
	static void ItemAction_RemoveActionFlag(UMounteaSelectableInventoryItemAction* Target, const EInventoryItemActionCallback FlagToRemove);

	/**
	 * Clears all flags.
	 *
	 * @param Target Item Action to clear all flags from.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Clear Inventory Action Flags")
	static void ItemAction_ClearAllActionFlags(UMounteaSelectableInventoryItemAction* Target);

	/**
	 * Gets called once Action has finished/cancelled to restore resources.
	 * 
	 * @param Target Item Action to clear/reset.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="reset"),
		DisplayName="Cleanup Inventory Action")
	static void CleanupInventoryAction(UMounteaInventoryItemAction* Target);
	
#pragma endregion
	
	//------------------------------------------------------------------------------------------

#pragma region Category
	
	/**
	 * Returns Category Data if category exists.
	 * You can search for 
	 * 
	 * @param CategoryName Category to search data for.
	 * @param ParentCategory Optional parent category. If left empty, only first level categories are searched.
	 * @param bResult Returns whether this operation was success or not.
	 * @return Category data structure.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"))
	static FInventoryCategoryData GetInventoryCategoryData(const FString& CategoryName, const FString ParentCategory, 
		UPARAM(DisplayName="Success") bool& bResult);
	
#pragma endregion
	
#pragma region Settings
	// --- Settings & Configuration
	
	/**
	 * Retrieves the configuration settings for the Mountea Inventory System.
	 * Provides access to inventory-related configurations such as inventory types, rarities, and categories.
	 *
	 * @return - A reference to the Mountea Advanced Inventory Settings configuration object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventorySettings* GetInventorySettings();

	/**
	 * Retrieves the advanced configuration settings for the Mountea Inventory System.
	 * Facilitates access to inventory-specific configurations like inventory types, rarities, and categories.
	 *
	 * @return - A pointer to the Mountea Advanced Inventory Settings configuration object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventorySettingsConfig* GetInventorySettingsConfig();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"))
	static UPrimaryDataAsset* GetTemplateConfig(const FString& Key);
#pragma endregion
	
#pragma region Inventory
	//--- Inventory
	
	/**
	 * Creates a new inventory item associated with the given unique identifier.
	 * This item is NOT valid item! This is to simplify searching for item related data!
	 * 
	 * @param ItemGuid - The globally unique identifier (GUID) for the new item
	 * @return - A new inventory item initialized with the specified GUID
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", 
		meta = (Keywords = "construct,build,make", NativeMakeFunc), 
		meta=(CustomTag="MounteaK2Getter"))
	static FInventoryItem NewInventoryItem(const FGuid& ItemGuid);

	/**
	 * Checks if the specified flags are set in the value.
	 * @param Value - The value to check
	 * @param FlagMask - The flag(s) to check for
	 * @return - True if all flags in the mask are set in the value
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", 
		meta=(CustomTag="MounteaK2Getter"))
	static bool HasInventoryFlags(const uint8 Value, UPARAM(meta = (Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemFlags")) const int32 FlagMask)
	{
		return (Value & FlagMask) == FlagMask;
	}
	
	/**
	 * Returns the owning actor for this Dialogue Inventory Component.
	 * @param Target The inventory interface to execute on
	 * @return The owning actor for this Dialogue Inventory Component.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Getter"))
	static AActor* GetOwningActor(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target);

	/**
	 * Adds an item to the inventory
	 * @param Target The inventory interface to execute on
	 * @param Item The item to add
	 * @return True if item was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool AddItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItem& Item);

	/**
	 * Removes an item from inventory
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid GUID of item to remove
	 * @return True if item was removed
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool RemoveItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid);

	/**
	 * Checks if can add item to inventory
	 * @param Target The inventory interface to execute on
	 * @param Item Item to check
	 * @return True if item can be added
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Validate"))
	static bool CanAddItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItem& Item);

	/**
	 * Adds an item from template
	 * @param Target The inventory interface to execute on
	 * @param Template The template to create item from
	 * @param Quantity Amount to add
	 * @param Durability Initial durability
	 * @return True if item was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool AddItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, 
		const int32 Quantity = 1, const float Durability = 1.f);

	/**
	 * Removes items by template
	 * @param Target The inventory interface to execute on
	 * @param Template Template to match
	 * @param Quantity Amount to remove
	 * @return True if items were removed
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool RemoveItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, 
		const int32 Quantity = 1);

	/**
	 * Checks if can add item from template
	 * @param Target The inventory interface to execute on
	 * @param Template Template to check
	 * @param Quantity Amount to validate
	 * @return True if item can be added
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Validate"))
	static bool CanAddItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, 
		const int32 Quantity = 1);

	/**
	 * Finds item based on Search conditions.
	 * @param Target The inventory interface to execute on
	 * @param SearchParams Search params to search by.
	 * @return Found item or nullptr
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Search", 
		meta=(CustomTag="MounteaK2Getter"))
	static FInventoryItem FindItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);

	/**
	 * Finds item index based on Search conditions.
	 * @param Target The inventory interface to execute on
	 * @param SearchParams Search params to search by.
	 * @return Found item index or -1
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Search", 
		meta=(CustomTag="MounteaK2Getter"))
	static int32 FindItemIndex(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);
	
	/**
	 * Finds items based on Search conditions.
	 * @param Target The inventory interface to execute on
	 * @param SearchParams Search params to search by.
	 * @return Found items or empty array
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Search", 
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<FInventoryItem> FindItems(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);

	/**
	 * Gets all items in inventory
	 * @param Target The inventory interface to execute on
	 * @return Array of inventory items
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<FInventoryItem> GetAllItems(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target);

	/**
	 * Clears all items from inventory
	 * @param Target The inventory interface to execute on
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Setter"))
	static void ClearInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target);

	/**
	 * Checks if the provided Inventory Interface contains an item matching the specified search parameters.
	 *
	 * @param Target The inventory interface to execute on.
	 * @param SearchParams The parameters to filter and match the desired item in the inventory.
	 * @return True if the specified item exists in the inventory, False otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", 
		meta=(CustomTag="MounteaK2Getter"))
	static bool HasItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);
	
#pragma endregion 
	
#pragma region Item
	
	/**
	 * Increases item quantity
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid Target item GUID
	 * @param Amount Amount to increase
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Stack", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool IncreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, 
		int32 Amount = 1);

	/**
	 * Decreases item quantity
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid Target item GUID
	 * @param Amount Amount to decrease
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Stack", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool DecreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, 
		int32 Amount = 1);

	/**
	 * Modifies item durability
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid Target item GUID
	 * @param DeltaDurability Change in durability
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Durability", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool ModifyItemDurability(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, 
		float DeltaDurability);
	
#pragma endregion
	
#pragma region Notification
	
	/**
	 * Processes inventory notification
	 * @param Target The inventory interface to execute on
	 * @param Notification The notification to process
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Notifications", 
		meta=(CustomTag="MounteaK2Setter"))
	static void ProcessInventoryNotification(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryNotificationData& Notification);

	static FInventoryNotificationData CreateNotificationData(const FString& Type, const TScriptInterface<IMounteaAdvancedInventoryInterface>& SourceInventory,
		const FGuid& ItemGuid, const int32 QuantityDelta
	);
#pragma endregion 
	
	/**
	 * Translates Inventory to list of Items.
	 * Add some additional info.
	 * 
	 * @param Target Inventory interface.
	 * @return A message that provides details about inventory content.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FString InventoryToString(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target);
	
	// --- Item
	
	/**
	 * Translates Inventory Item to string.
	 * @param Item Item to be translated to String.
	 * @return  String representation of the Inventory Item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FString InventoryItemToString(const FInventoryItem& Item);

	/**
	 * 
	 * @param Item 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FInventoryCategory GetInventoryCategory(const FInventoryItem& Item);

	/**
	 * 
	 * @param Item 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FString GetInventoryCategoryKey(const FInventoryItem& Item);

	/**
	 * 
	 * @param Item 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FInventoryRarity GetInventoryRarity(const FInventoryItem& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FString GetInventoryRarityKey(const FInventoryItem& Item);

	/**
	 * Retrieves the display name of the specified inventory item.
	 * This name represents how the item is identified or labeled in the inventory system.
	 *
	 * @param Item - A reference to the inventory item for which the name is to be retrieved
	 * @return - The display name of the specified inventory item as an FText object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FText GetInventoryItemName(const FInventoryItem& Item);

	/**
	 * Retrieves a short descriptive text for the specified inventory item.
	 * This text provides a concise summary of the item's key information.
	 *
	 * @param Item - The inventory item for which the short information is to be retrieved.
	 * @return - A localized text containing the short info of the specified inventory item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FText GetInventoryItemShortInfo(const FInventoryItem& Item);

	/**
	 * Retrieves a detailed description of the specified inventory item.
	 * Provides a comprehensive string representation of the item's attributes, suitable for display in UI or logs.
	 *
	 * @param Item - The inventory item for which detailed information is to be retrieved.
	 * @return - A localized text representation containing the long description of the specified inventory item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static FText GetInventoryItemLongInfo(const FInventoryItem& Item);

	/**
	 * Retrieves the cover image associated with a specified inventory item.
	 * This method provides access to the visual representation of the inventory item if a cover image is assigned.
	 *
	 * @param Item - The inventory item for which the cover image is to be retrieved.
	 * @return - A pointer to the UTexture2D representing the cover image of the inventory item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static UTexture2D* GetInventoryItemCover(const FInventoryItem& Item);
	
	/**
	 * Determines whether the provided inventory item is valid.
	 * Validity is based on the item's internal validation rules.
	 *
	 * @param Item - The inventory item to be evaluated for validity
	 * @return - True if the inventory item is valid, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Validate"))
	static bool IsInventoryItemValid(const FInventoryItem& Item);

	/**
	 * Retrieves the actions associated with a specified inventory item.
	 * This function returns an array of Inventory Action data representing the actions that can be performed on the item.
	 * Actions must implement the `MounteaAdvancedInventoryItemActionInterface` interface.
	 *
	 * @param Item - The inventory item for which to retrieve the associated actions
	 * @return - An array of Inventory Action data containing the item actions
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<UMounteaCallbackInventoryItemAction*> GetItemActions(const FInventoryItem& Item);
	
	/**
	 * Retrieves the actions associated with a specified inventory item which are allowed to be displayed in UI.
	 * This function returns an array of Inventory Action data representing the actions that can be performed on the item.
	 * Actions must implement the `MounteaAdvancedInventoryItemActionInterface` interface.
	 *
	 * @param Item - The inventory item for which to retrieve the associated actions
	 * @return - An array of Inventory Action data containing the item actions which can be displayed in UI
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"),
		meta=(Keywords="selectable,display"))
	static TArray<UMounteaSelectableInventoryItemAction*> GetDisplayableItemActions(const FInventoryItem& Item);
	
	/**
	 * Retrieves the Custom Data Tags Container, which serves a purpose of being a write/read container for
	 * transient data, like `QuestItem` for items, which are not by default Quest Items.
	 *
	 * @param Item - The inventory item for which to retrieve the associated actions
	 * @return The custom data tags associated with this item
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Item - Get Item Custom Data")
	static FGameplayTagContainer GetItemCustomData(const FInventoryItem& Item);

	// --- Items
	
	/**
	 * Sorts provided Inventory Items based on Sorting Criteria.
	 * 
	 * Defaults:
	 * * Name
	 * * Value
	 * * Weight
	 * * Rarity
	 * 
	 * @param Items List of cached Items. This is to avoid touching the "source" Items, so we rather sort temp. data.
	 * @param SortingCriteria Defines what criteria are applied
	 * @return Sorted Items.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", 
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<FInventoryItem> SortInventoryItems(const TArray<FInventoryItem>& Items, const TArray<FInventorySortCriteria>& SortingCriteria );
	
#pragma region ItemTemplate

	/**
	 * Retrieve JSON manifest of the Inventory Item Template.
	 * 
	 * @param ItemTemplate Template to read JSON from
	 * @return Returns JSON manifest of the Item Template if already written.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Item Actions",
		meta=(DisplayName="Get Item Template Json"), 
		meta=(CustomTag="MounteaK2Getter"))
	static FString ItemTemplate_GetItemTemplateJson(UMounteaInventoryItemTemplate* ItemTemplate);
	
	static bool ItemTemplate_CalculateItemTemplateJson(UMounteaInventoryItemTemplate* ItemTemplate);
	
#pragma endregion
};