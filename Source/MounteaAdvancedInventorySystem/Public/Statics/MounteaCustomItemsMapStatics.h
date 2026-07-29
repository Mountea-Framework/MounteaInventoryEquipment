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
#include "Interfaces/UserInterface/MounteaAdvancedCustomItemsMapInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaCustomItemsMapStatics.generated.h"

struct FGameplayTag;

/**
 * Shared Blueprint-callable statics for the Custom Items Map (GameplayTag -> set of Guids), usable by any
 * gameplay system (Inventory, Crafting, or future systems) that implements IMounteaAdvancedCustomItemsMapInterface,
 * without callers needing system-specific knowledge of which manager they are targeting.
 *
 * @see [Custom Items Map](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventorySystem)
 * @see IMounteaAdvancedCustomItemsMapInterface
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCustomItemsMapStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Retrieves local custom items map.
	 *
	 * @param Target     UI Manager implementing IMounteaAdvancedCustomItemsMapInterface.
	 * @return Custom items map if any entries are present.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Custom Items Map",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Custom Items Map - Get Custom Items")
	static TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCustomItemsMapInterface")) UObject* Target);

	/**
	 * Adds (or replaces) a custom item entry in the target UI Manager's custom items map.
	 *
	 * @param Target     UI Manager implementing IMounteaAdvancedCustomItemsMapInterface.
	 * @param ItemTag    Gameplay tag representing the custom item key.
	 * @param ItemId     Guid value associated with the item.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Custom Items Map",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Custom Items Map - Add Custom Item")
	static void AddCustomItem(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCustomItemsMapInterface")) UObject* Target,
		const FGameplayTag& ItemTag, const FGuid& ItemId);

	/**
	 * Appends custom item entries to the target UI Manager's custom items map.
	 * Existing keys will be overwritten.
	 *
	 * @param Target       UI Manager implementing IMounteaAdvancedCustomItemsMapInterface.
	 * @param OtherItems   Map of custom items to append.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Custom Items Map",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Custom Items Map - Append Custom Items")
	static void AppendCustomItems(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCustomItemsMapInterface")) UObject* Target,
		const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems);

	/**
	 * Clears all custom items from the target UI Manager's custom items map.
	 *
	 * @param Target     UI Manager implementing IMounteaAdvancedCustomItemsMapInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Custom Items Map",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Custom Items Map - Clear Custom Items")
	static void ClearCustomItems(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCustomItemsMapInterface")) UObject* Target);

	/**
	 * Removes a single custom item from the target UI Manager's custom items map.
	 *
	 * @param Target    UI Manager implementing IMounteaAdvancedCustomItemsMapInterface.
	 * @param ItemTag   Gameplay tag representing the custom item key to remove.
	 * @param ItemId	Guid of the item to be removed.
	 *
	 * @return True if an entry was removed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Custom Items Map",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Custom Items Map - Remove Custom Item")
	static bool RemoveCustomItem(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCustomItemsMapInterface")) UObject* Target,
		const FGameplayTag& ItemTag, const FGuid& ItemId);

	/**
	 * Validates whether the provided Item guid is stored in a map for specified tag.
	 * Example:
	 * - Is Item in Favorites
	 *
	 * @param Target UI Manager implementing IMounteaAdvancedCustomItemsMapInterface.
	 * @param ItemTag Tag which defines the key, like "Favorite"
	 * @param ItemId Item guid to search for
	 * @return True if item is stored in such container, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Custom Items Map",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		DisplayName="Custom Items Map - Is Item Stored In Custom Map")
	static bool IsItemStoredInCustomMap(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCustomItemsMapInterface")) UObject* Target,
		const FGameplayTag& ItemTag, const FGuid& ItemId);

private:

	/** Validates that Target is non-null and implements IMounteaAdvancedCustomItemsMapInterface, logging a warning naming the calling node otherwise. */
	static bool ValidateTarget(const UObject* Target, const TCHAR* CallingFunction);
};
