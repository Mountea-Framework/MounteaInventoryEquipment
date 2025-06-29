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
#include "MounteaInventoryBaseEnums.generated.h"

/**
 * Flags representing the type of inventory.
 */
UENUM(BlueprintType)
enum class EInventoryType : uint8
{
	EIT_Player	UMETA(DisplayName = "Player", Tooltip = "Main character inventory with weight/equipment management."),
	EIT_NPC	UMETA(DisplayName = "NPC", Tooltip = "NPCs including companions, followers, and regular characters."),
	EIT_Storage	UMETA(DisplayName = "Storage", Tooltip = "Permanent storage like chests, stashes, and containers."),
	EIT_Merchant	UMETA(DisplayName = "Merchant", Tooltip = "Vendors with buy/sell functionality and gold limits."),
	EIT_Loot	UMETA(DisplayName = "Loot", Tooltip = "Temporary loot sources like corpses, containers, ground items."),
	EIT_Specialized	UMETA(DisplayName = "Specialized", Tooltip = "Special purpose inventories like loadout systems."),

	Default	UMETA(hidden)
};

/**
 * Flags representing special behaviors or access rules for an inventory.
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EInventoryFlags : uint8
{
	EIF_None		UMETA(DisplayName = "None", Tooltip = "No special behavior or access rules."),
	EIF_Public	 UMETA(DisplayName = "Public", Tooltip = "Inventory is accessible by all actors."),
	EIF_TeamShared	 UMETA(DisplayName = "Team Shared", Tooltip = "Inventory is accessible by teammates."),
	EIF_Lootable	UMETA(DisplayName = "Lootable", Tooltip = "Inventory is lootable by other actors."),
	EIF_Temporary	UMETA(DisplayName = "Temporary", Tooltip = "Inventory is temporary and removed after specific events."),
	EIF_Private	UMETA(DisplayName = "Private", Tooltip = "Inventory is only accessible by the owner.")
};
ENUM_CLASS_FLAGS(EInventoryFlags)

/**
 * Bitflags representing special behaviors or properties for inventory items.
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EInventoryItemFlags : uint8
{
	EIIF_None	= 0	UMETA(DisplayName = "None", Tooltip = "No special behavior or properties."),
	EIIF_Tradeable	= 1 << 0	UMETA(DisplayName = "Tradeable", Tooltip = "Item can be traded between inventories."),
	EIIF_Stackable	= 1 << 1	UMETA(DisplayName = "Stackable", Tooltip = "Item can stack with others of the same type."),
	EIIF_Craftable	= 1 << 2	UMETA(DisplayName = "Craftable", Tooltip = "Item can be used in crafting recipes."),
	EIIF_Dropable	= 1 << 3	UMETA(DisplayName = "Dropable", Tooltip = "Item can be dropped into the world."),
	EIIF_Consumable  = 1 << 4	UMETA(DisplayName = "Consumable", Tooltip = "Item can be consumed for an effect."),
	EIIF_QuestItem	= 1 << 5	UMETA(DisplayName = "Quest Item", Tooltip = "Item is required for a quest."),
	EIIF_Unique	= 1 << 6	UMETA(DisplayName = "Unique", Tooltip = "Item is unique and can be only once in Inventory!"),
	EIIF_Durable	  = 1 << 7	UMETA(DisplayName = "Durable", Tooltip = "Item has durability and can degrade.")
};
ENUM_CLASS_FLAGS(EInventoryItemFlags)

/**
 * Bitflags representing inventory type configuration and behavior.
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EInventoryTypeFlags : uint8
{
	EITF_None	= 0	UMETA(DisplayName = "None", Tooltip = "Default inventory behavior."),
	EITF_HasWeightLimit = 1 << 0	UMETA(DisplayName = "Has Weight Limit", Tooltip = "Inventory has a maximum weight capacity."),
	EITF_HasValueLimit  = 1 << 1	UMETA(DisplayName = "Has Value Limit", Tooltip = "Inventory has a maximum total value limit."),
	EITF_CanAddItems	 = 1 << 2	UMETA(DisplayName = "Can Add Items", Tooltip = "Items can be added to this inventory."),
	EITF_CanRemoveItems = 1 << 3	UMETA(DisplayName = "Can Remove Items", Tooltip = "Items can be removed from this inventory."),
	EITF_AllowStacking  = 1 << 4	UMETA(DisplayName = "Allow Stacking", Tooltip = "Inventory allows stacking of similar items."),
	EITF_AutoStack	= 1 << 5	UMETA(DisplayName = "Auto Stack", Tooltip = "Automatically stack similar items when added."),
	EITF_Persistent	  = 1 << 6	UMETA(DisplayName = "Persistent", Tooltip = "Inventory persists between game sessions.")
};
ENUM_CLASS_FLAGS(EInventoryTypeFlags)

/** Enum for defining how inventory items should be sorted */
UENUM(BlueprintType)
enum class EInventorySortType : uint8
{
	EIS_None UMETA(DisplayName = "None", Tooltip = "No sorting will be applied to inventory items."),
	EIS_NameAsc UMETA(DisplayName = "Name (A-Z)", Tooltip = "Sort items alphabetically by name in ascending order."),
	EIS_NameDesc UMETA(DisplayName = "Name (Z-A)", Tooltip = "Sort items alphabetically by name in descending order."),
	EIS_QuantityAsc UMETA(DisplayName = "Quantity (Low-High)", Tooltip = "Sort items by their stack size from lowest to highest."),
	EIS_QuantityDesc UMETA(DisplayName = "Quantity (High-Low)", Tooltip = "Sort items by their stack size from highest to lowest."),
	EIS_DurabilityAsc UMETA(DisplayName = "Durability (Low-High)", Tooltip = "Sort items by their durability percentage from lowest to highest."),
	EIS_DurabilityDesc UMETA(DisplayName = "Durability (High-Low)", Tooltip = "Sort items by their durability percentage from highest to lowest."),
	EIS_Custom UMETA(DisplayName = "Custom", Tooltip = "Custom sorting implementation defined by the user."),

	Default UMETA(Hidden)
 };