// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InventoryHelpers.generated.h"

#define INVENTORY_TAG_NAME			TEXT(Inventory)
#define INVENTORY_ITEM_TAG_NAME		TEXT(InventoryItem)

#define OUT
#define IN

#pragma region TEMPLATES

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
	if (!enumPtr) return FString("invalid");
	return enumPtr->GetNameByValue((int64)Value).ToString();
}

#pragma endregion TEMPLATES

class UInventoryCategory;
class UInventoryItemRarity;

#define LOCTEXT_NAMESPACE "InventoryItemData"

/**
 * Item Quantity Definition.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FItemQuantityData
{
	GENERATED_BODY()
	
	// How many Items will be added to Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, UIMin=1, ClampMin=1))
	int32 DefaultAmount = 1;

	// Defines whether stacking multiple Item instances of this Item is allowed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true))
	uint8 bIsStackable : 1;

	// If stacking is possible, how much is allowed to stack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, UIMin=1, ClampMin=1, EditCondition="bIsStackable"))
	int32 MaxStackAmount = 99;

public:

	inline bool operator==(const FItemQuantityData& Other) const
	{
		return
		DefaultAmount == Other.DefaultAmount &&
		bIsStackable == Other.bIsStackable &&
		MaxStackAmount == Other.MaxStackAmount;
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct FInventoryCategoryData
{
	GENERATED_BODY()

	// Name of this Category, useful for UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	FText CategoryName = LOCTEXT("InventoryCategory", "Default");

	// Icon of this Category, useful for UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	UTexture2D* CategoryTexture = nullptr;

	// Parent Category of this one, useful for Inventory sorting, however, optional
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	UInventoryCategory* ParentCategory = nullptr;
	
};

/**
 * Data of the Inventory Item.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryItemData : public FTableRowBase
{
	GENERATED_BODY();

	FInventoryItemData() : ItemQuantityData() {};

	FInventoryItemData(const FInventoryItemData& Other)
		: ItemDataGUID(FGuid::NewGuid())
		, ItemCategory(Other.ItemCategory)
		, ItemRarity(Other.ItemRarity)
		, ItemThumbnail(Other.ItemThumbnail)
		, ItemTittle(Other.ItemTittle)
		, ItemDescription(Other.ItemDescription)
		, ItemQuantityData(Other.ItemQuantityData)
		, ItemMesh(Other.ItemMesh)
		, SpawnItemClass(Other.SpawnItemClass)
	{}

private:

	UPROPERTY(VisibleAnywhere, Category="Item Data|GUID")
	FGuid ItemDataGUID = FGuid::NewGuid();

public:
	
	// Category of Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, BlueprintBaseOnly=true))
	UInventoryCategory* ItemCategory = nullptr;

	// Rarity of Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, BlueprintBaseOnly=true))
	UInventoryItemRarity* ItemRarity = nullptr;

	// Item thumbnail texture, can be null
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true))
	UTexture2D* ItemThumbnail = nullptr;

	// Tittle/Name of Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true))
	FText ItemTittle = LOCTEXT("ItemTittle", "Default Object");

	// Description of Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, MultiLine=true))
	FText ItemDescription = LOCTEXT("ItemDescription", "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
	
	// Item Quantity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, ShowOnlyInnerProperties))
	FItemQuantityData ItemQuantityData;

	// Item Mesh. Static and Skeletal Mesh allowed. For preview only.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, AllowedClasses="StaticMesh, SkeletalMesh"))
	UStreamableRenderAsset* ItemMesh = nullptr;

	// Actor to spawn from this Item.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, BlueprintBaseOnly=true))
	TSoftClassPtr<AActor> SpawnItemClass = nullptr;

public:
	
	inline bool operator==(const FInventoryItemData& Other) const
	{
		return
		ItemDataGUID == Other.ItemDataGUID; /* &&
		ItemCategory == Other.ItemCategory &&
		ItemRarity.GetDefaultObject() == Other.ItemRarity.GetDefaultObject() &&
		ItemTittle.ToString() == Other.ItemTittle.ToString() &&
		ItemDescription.ToString() == Other.ItemDescription.ToString() &&
		ItemThumbnail == Other.ItemThumbnail &&
		ItemMesh == Other.ItemMesh &&
		ItemQuantityData == Other.ItemQuantityData;
		*/
	}

	inline bool operator!=(const FInventoryItemData& Other) const
	{
		return !(*this == Other);
	}

	inline FInventoryItemData& operator=(const FInventoryItemData& Other)
	{
		ItemDataGUID = Other.ItemDataGUID;
		ItemCategory = Other.ItemCategory;
		ItemRarity = Other.ItemRarity;
		ItemThumbnail = Other.ItemThumbnail;
		ItemTittle = Other.ItemTittle;
		ItemDescription = Other.ItemDescription;
		ItemQuantityData = Other.ItemQuantityData;
		ItemMesh = Other.ItemMesh;
		SpawnItemClass = Other.SpawnItemClass;
		
		return *this;
	}
	
};

#undef LOCTEXT_NAMESPACE