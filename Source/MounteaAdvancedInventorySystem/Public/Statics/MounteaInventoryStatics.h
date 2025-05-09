// MounteaInventoryStatics.h
#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryStatics.generated.h"

enum class EInventoryNotificationCategory : uint8;
class UMounteaAdvancedInventorySettingsConfig;
enum class EInventoryNotificationType : uint8;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryStatics : public UBlueprintFunctionLibrary
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

	/*************************************************************/
	/******************* BLUEPRINTABLE *********************/
	/*************************************************************/
public:

	/**
	 * Checks if the specified flags are set in the value.
	 * @param Value - The value to check
	 * @param FlagMask - The flag(s) to check for
	 * @return - True if all flags in the mask are set in the value
	 */
	UFUNCTION(BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|Helpers", meta=(CustomTag="MounteaK2Getter"), meta=(ExpandBoolAsExecs="ReturnValue"))
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Validate"), meta=(ExpandBoolAsExecs="ReturnValue"))
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Validate"), meta=(ExpandBoolAsExecs="ReturnValue"))
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Management", meta=(CustomTag="MounteaK2Getter"), meta=(ExpandBoolAsExecs="ReturnValue"))
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
	
	/*************************************************************/
	/************************ INTERNAL ***********************/
	/*************************************************************/
public:
	static FInventoryNotificationData CreateNotificationData(
		const FString& Type,
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& SourceInventory,
		const FGuid& ItemGuid,
		const int32 QuantityDelta
	);
};