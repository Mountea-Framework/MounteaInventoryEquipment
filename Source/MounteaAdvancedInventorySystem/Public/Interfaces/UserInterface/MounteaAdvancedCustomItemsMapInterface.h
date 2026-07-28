// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
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
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedCustomItemsMapInterface.generated.h"

struct FGameplayTag;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedCustomItemsMapInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedCustomItemsMapInterface exposes a small, gameplay-system-agnostic key/value store
 * (GameplayTag -> set of Guids) that any UI Manager (Inventory, Crafting, or future systems) can implement
 * to let Blueprint mark items as belonging to an arbitrary named container (e.g. Favorites, Recently Used).
 *
 * @see [Custom Items Map](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventorySystem)
 * @see UMounteaInventoryUIComponent
 * @see UMounteaCraftingParticipantUIComponent
 * @see UMounteaCustomItemsMapStatics
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedCustomItemsMapInterface
{
	GENERATED_BODY()

public:

	/**
	 * Retrieves local custom items map.
	 *
	 * @return Custom items map if any entries are present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Custom Items Map")
	TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap() const;
	virtual TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap_Implementation() const = 0;

	/**
	 * Adds (or replaces) a single entry in the custom items map.
	 *
	 * @param ItemTag   Gameplay tag representing the custom item key.
	 * @param ItemId    Guid value associated with the item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Custom Items Map")
	void AddCustomItemToMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual void AddCustomItemToMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

	/**
	 * Appends entries from another map into the custom items map.
	 * Existing keys will be overwritten with new values.
	 *
	 * @param OtherItems   Map of custom items to merge in.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Custom Items Map")
	void AppendCustomItemsMap(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems);
	virtual void AppendCustomItemsMap_Implementation(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems) = 0;

	/**
	 * Clears all entries from the custom items map.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Custom Items Map")
	void ClearCustomItemsMap();
	virtual void ClearCustomItemsMap_Implementation() = 0;

	/**
	 * Removes a single entry from the custom items map.
	 *
	 * @param ItemTag Key to remove the item from.
	 * @param ItemId Guid representing the custom item key to remove.
	 *
	 * @return True if an entry was removed, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Custom Items Map")
	bool RemoveCustomItemFromMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual bool RemoveCustomItemFromMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

	/**
	 * Validates whether the provided Item guid is stored in a map for specified tag.
	 * Example:
	 * - Is Item in Favorites
	 *
	 * @param ItemTag Tag which defines the key, like "Favorite"
	 * @param ItemId Item guid to search for
	 * @return True if item is stored in such container, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Custom Items Map")
	bool IsItemStoredInCustomMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual bool IsItemStoredInCustomMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;
};
