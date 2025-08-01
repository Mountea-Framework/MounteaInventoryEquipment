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
enum class EInventoryNotificationCategory : uint8;
enum class EInventoryNotificationType : uint8;
enum class EInventoryItemActionCallback : uint8;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*************************************************************/
	/************************* TEMPLATES *************************/
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

		if (Target->Implements<UMounteaAdvancedInventoryInterface>())
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				Function(Target, Forward<Args>(args)...);
				return;
			}
			return Function(Target, Forward<Args>(args)...);
		}

		LOG_ERROR(TEXT("[%s] Target does not implement 'MounteaAdvancedInventoryInterface'!"), FunctionName);
		if constexpr (!std::is_void_v<ReturnType>)
			return ReturnType{};
		else return;
	}

public:
	
	/**
	 * Retrieves the configuration settings for the Mountea Inventory System.
	 * Provides access to inventory-related configurations such as inventory types, rarities, and categories.
	 *
	 * @return - A reference to the Mountea Advanced Inventory Settings configuration object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventorySettings* GetInventorySettings();

	/**
	 * Retrieves the advanced configuration settings for the Mountea Inventory System.
	 * Facilitates access to inventory-specific configurations like inventory types, rarities, and categories.
	 *
	 * @return - A pointer to the Mountea Advanced Inventory Settings configuration object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventorySettingsConfig* GetInventorySettingsConfig();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", meta=(CustomTag="MounteaK2Getter"))
	static UPrimaryDataAsset* GetTemplateConfig(const FString& Key);

	/**
	 * Creates a new inventory item associated with the given unique identifier.
	 * This item is NOT valid item! This is to simplify searching for item related data!
	 * 
	 * @param ItemGuid - The globally unique identifier (GUID) for the new item
	 * @return - A new inventory item initialized with the specified GUID
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", meta = (Keywords = "construct,build,make", NativeMakeFunc), meta=(CustomTag="MounteaK2Getter"))
	static FInventoryItem NewInventoryItem(const FGuid& ItemGuid);

	/**
	 * Checks if the specified flags are set in the value.
	 * @param Value - The value to check
	 * @param FlagMask - The flag(s) to check for
	 * @return - True if all flags in the mask are set in the value
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", meta=(CustomTag="MounteaK2Getter"))
	static bool HasInventoryFlags(const uint8 Value, UPARAM(meta = (Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemFlags")) const int32 FlagMask)
	{
		return (Value & FlagMask) == FlagMask;
	}
	
	/**
	 * Returns the owning actor for this Dialogue Inventory Component.
	 * @param Target The inventory interface to execute on
	 * @return The owning actor for this Dialogue Inventory Component.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Getter"))
	static AActor* GetOwningActor(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target);

	/**
	 * Adds an item to the inventory
	 * @param Target The inventory interface to execute on
	 * @param Item The item to add
	 * @param bAutoStack If true, will try to stack with existing items
	 * @return True if item was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool AddItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItem& Item);

	/**
	 * Removes an item from inventory
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid GUID of item to remove
	 * @return True if item was removed
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool RemoveItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid);

	/**
	 * Checks if can add item to inventory
	 * @param Target The inventory interface to execute on
	 * @param Item Item to check
	 * @return True if item can be added
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Validate"))
	static bool CanAddItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItem& Item);

	/**
	 * Adds an item from template
	 * @param Target The inventory interface to execute on
	 * @param Template The template to create item from
	 * @param Quantity Amount to add
	 * @param Durability Initial durability
	 * @return True if item was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool AddItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1, const float Durability = 1.f);

	/**
	 * Removes items by template
	 * @param Target The inventory interface to execute on
	 * @param Template Template to match
	 * @param Quantity Amount to remove
	 * @return True if items were removed
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool RemoveItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1);

	/**
	 * Checks if can add item from template
	 * @param Target The inventory interface to execute on
	 * @param Template Template to check
	 * @param Quantity Amount to validate
	 * @return True if item can be added
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Validate"))
	static bool CanAddItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1);

	/**
	 * Finds item based on Search conditions.
	 * @param Target The inventory interface to execute on
	 * @param SearchParams Search params to search by.
	 * @return Found item or nullptr
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Search", meta=(CustomTag="MounteaK2Getter"))
	static FInventoryItem FindItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);

	/**
	 * Finds item index based on Search conditions.
	 * @param Target The inventory interface to execute on
	 * @param SearchParams Search params to search by.
	 * @return Found item index or -1
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Search", meta=(CustomTag="MounteaK2Getter"))
	static int32 FindItemIndex(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);
	
	/**
	 * Finds items based on Search conditions.
	 * @param Target The inventory interface to execute on
	 * @param SearchParams Search params to search by.
	 * @return Found items or empty array
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Search", meta=(CustomTag="MounteaK2Getter"))
	static TArray<FInventoryItem> FindItems(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);

	/**
	 * Gets all items in inventory
	 * @param Target The inventory interface to execute on
	 * @return Array of inventory items
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Getter"))
	static TArray<FInventoryItem> GetAllItems(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target);

	/**
	 * Clears all items from inventory
	 * @param Target The inventory interface to execute on
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Setter"))
	static void ClearInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target);

	/**
	 * Checks if the provided Inventory Interface contains an item matching the specified search parameters.
	 *
	 * @param Target The inventory interface to execute on.
	 * @param SearchParams The parameters to filter and match the desired item in the inventory.
	 * @return True if the specified item exists in the inventory, False otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Getter"))
	static bool HasItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams);
	
	/**
	 * Increases item quantity
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid Target item GUID
	 * @param Amount Amount to increase
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Stack", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool IncreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, int32 Amount = 1);

	/**
	 * Decreases item quantity
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid Target item GUID
	 * @param Amount Amount to decrease
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Stack", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool DecreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, int32 Amount = 1);

	/**
	 * Modifies item durability
	 * @param Target The inventory interface to execute on
	 * @param ItemGuid Target item GUID
	 * @param DeltaDurability Change in durability
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Durability", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool ModifyItemDurability(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, float DeltaDurability);

	/**
	 * Processes inventory notification
	 * @param Target The inventory interface to execute on
	 * @param Notification The notification to process
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Notifications", meta=(CustomTag="MounteaK2Setter"))
	static void ProcessInventoryNotification(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryNotificationData& Notification);
	
	/**
	 * Translates Inventory Item to string.
	 * @param Item Item to be translated to String.
	 * @return  String representation of the Inventory Item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FString InventoryItemToString(const FInventoryItem& Item);

	/**
	 * 
	 * @param Item 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FInventoryCategory GetInventoryCategory(const FInventoryItem& Item);

	/**
	 * 
	 * @param Item 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FString GetInventoryCategoryKey(const FInventoryItem& Item);

	/**
	 * 
	 * @param Item 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FInventoryRarity GetInventoryRarity(const FInventoryItem& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FString GetInventoryRarityKey(const FInventoryItem& Item);

	/**
	 * Retrieves the display name of the specified inventory item.
	 * This name represents how the item is identified or labeled in the inventory system.
	 *
	 * @param Item - A reference to the inventory item for which the name is to be retrieved
	 * @return - The display name of the specified inventory item as an FText object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FText GetInventoryItemName(const FInventoryItem& Item);

	/**
	 * Retrieves a short descriptive text for the specified inventory item.
	 * This text provides a concise summary of the item's key information.
	 *
	 * @param Item - The inventory item for which the short information is to be retrieved.
	 * @return - A localized text containing the short info of the specified inventory item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FText GetInventoryItemShortInfo(const FInventoryItem& Item);

	/**
	 * Retrieves a detailed description of the specified inventory item.
	 * Provides a comprehensive string representation of the item's attributes, suitable for display in UI or logs.
	 *
	 * @param Item - The inventory item for which detailed information is to be retrieved.
	 * @return - A localized text representation containing the long description of the specified inventory item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static FText GetInventoryItemLongInfo(const FInventoryItem& Item);

	/**
	 * Retrieves the cover image associated with a specified inventory item.
	 * This method provides access to the visual representation of the inventory item if a cover image is assigned.
	 *
	 * @param Item - The inventory item for which the cover image is to be retrieved.
	 * @return - A pointer to the UTexture2D representing the cover image of the inventory item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static UTexture2D* GetInventoryItemCover(const FInventoryItem& Item);
	
	/**
	 * Determines whether the provided inventory item is valid.
	 * Validity is based on the item's internal validation rules.
	 *
	 * @param Item - The inventory item to be evaluated for validity
	 * @return - True if the inventory item is valid, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Validate"))
	static bool IsInventoryItemValid(const FInventoryItem& Item);

	/**
	 * Retrieves the actions associated with a specified inventory item.
	 * This function returns an array of soft class pointers representing the actions that can be performed on the item.
	 * Actions must implement the `MounteaAdvancedInventoryItemActionInterface` interface.
	 *
	 * @param Item - The inventory item for which to retrieve the associated actions
	 * @return - An array of TSoftClassPtr<UObject> containing the item actions
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	static TArray<TSoftClassPtr<UObject>> GetItemActions(const FInventoryItem& Item);

	// --- Item Actions ------------------------------

#pragma region ItemActions

	/**
	 * Retrieves the item action flags for the specified target.
	 * This function returns the current set of flags that indicate which item actions are available for the target.
	 *
	 * @param Target The object for which to retrieve the item action flags.
	 * @return The current set of item action flags as an EInventoryItemActionCallback enum value.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	static EInventoryItemActionCallback GetItemActionFlags(const UObject* Target);
	
	/**
	 * Checks if a specific flag is set in the given flag container.
	 *
	 * @param Flags The current set of flags.
	 * @param FlagToCheck The specific flag to test for.
	 * @return True if the flag is set, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	static bool ItemAction_HasActionFlag(UObject* Target, const EInventoryItemActionCallback FlagToCheck);

	/**
	 * Adds a specific flag to the flag container.
	 *
	 * @param Flags The current flags.
	 * @param FlagToAdd The flag to add.
	 * @return Updated flag container with the new flag set.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	static void ItemAction_AddActionFlag(UObject* Target, EInventoryItemActionCallback FlagToAdd);

	/**
	 * Removes a specific flag from the flag container.
	 *
	 * @param FlagToRemove The flag to clear.
	 * @return Updated flag container with the flag cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	static void ItemAction_RemoveActionFlag(UObject* Target, const EInventoryItemActionCallback FlagToRemove);

	/**
	 * Clears all flags.
	 *
	 * @return An empty flag container (EIIAC_None).
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	static void ItemAction_ClearAllActionFlags(UObject* Target);

#pragma endregion
	
	/*************************************************************/
	/************************ INTERNAL **************************/
	/*************************************************************/
public:
	static FInventoryNotificationData CreateNotificationData(
		const FString& Type,
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& SourceInventory,
		const FGuid& ItemGuid,
		const int32 QuantityDelta
	);
};