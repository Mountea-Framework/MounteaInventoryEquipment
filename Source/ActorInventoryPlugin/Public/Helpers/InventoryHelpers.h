// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InventoryHelpers.generated.h"

#define INVENTORY_TAG_NAME			TEXT(Inventory)
#define INVENTORY_ITEM_TAG_NAME		TEXT(InventoryItem)

#define OUT
#define IN

/**
 * Template that allows reading Name value from any given UENUM.
 * @param Name: Name of the UENUM (ECollisionChannel, for instance)
 * @param Value: Enum value to be translated (ECC_Visibility, for instance)
 * @return FString of the Value (ECC_Visibility in our example, or invalid of name not specified nor UENUM does not exist)
 */
template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr) return FString(invalid);
	return enumPtr->GetNameByValue((int64)Value).ToString();
}

class UInventoryCategory;
class UInventoryItemRarity;

#define LOCTEXT_NAMESPACE "InventoryItemData"

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FItemQuantityData
{
	GENERATED_BODY()

	// How many Items will be added to Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true, UIMin=1, ClampMin=1))
	int32 DefaultAmount = 1;

	// Defines whether stacking multiple Item instances of this Item is allowed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true, BlueprintBaseOnly))
	bool bIsStackable = false;

	// If stacking is possible, how much is allowed to stack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true, UIMin=1, ClampMin=1, EditCondition="bIsStackable"))
	int32 MaxStackAmount = 99;
};

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryItemData : public FTableRowBase
{
	GENERATED_BODY();

public:

	// Category of Item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true, BlueprintBaseOnly))
	TSubclassOf<UInventoryCategory> ItemCategory;

	// Rarity of Item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true, BlueprintBaseOnly))
	TSubclassOf<UInventoryItemRarity> ItemRarity;

	// Item thumbnail texture, can be null
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true))
	UTexture2D* ItemThumbnail = nullptr;

	// Tittle/Name of Item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true))
	FText ItemTittle = LOCTEXT("ItemTittle", "Default Object");

	// Description of Item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true, MultiLine=true))
	FText ItemDescription = LOCTEXT("ItemDescription", "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
	
	// Item Quantity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta=(ExposeOnSpawn=true, ShowOnlyInnerProperties))
	FItemQuantityData ItemQuantityData;
};

#undef LOCTEXT_NAMESPACE