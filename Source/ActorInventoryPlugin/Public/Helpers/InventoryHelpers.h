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

#pragma region ItemQuantity

/**
 * Item Quantity Definition.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FItemQuantityData
{
	GENERATED_BODY()

public:

	// How many instances are in Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, UIMin=1, ClampMin=1))
	int32 Quantity = 1;

	// How many Items of this one are allowed in Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, UIMin=1, ClampMin=1))
	int32 MaxQuantity = 99;

	// Defines whether stacking multiple Item instances of this Item is allowed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true))
	uint8 bIsStackable : 1;

public:
	
	inline bool operator==(const FItemQuantityData& Other) const
	{
		return
		bIsStackable == Other.bIsStackable &&
		MaxQuantity == Other.MaxQuantity &&
		Quantity == Other.Quantity;
	}

	inline bool operator!=(const FItemQuantityData& Other) const
	{
		return !(*this==Other);
	}
};

#pragma endregion

#pragma region InventoryRarityData

#define LOCTEXT_NAMESPACE "InventoryRarityData"

USTRUCT(BlueprintType, Blueprintable)
struct FInventoryRarityData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true))
	FText RarityName = LOCTEXT("InventoryRarity", "Common");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true, MultiLine))
	FText RarityDescription = LOCTEXT("InventoryRarityDescription", "Most common items in the world.");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true))
	FLinearColor RarityColor = FLinearColor::Gray;
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

#pragma region InventoryCategorydata

#define LOCTEXT_NAMESPACE "InventoryCategoryData"

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryCategoryData
{
	GENERATED_BODY()

	FInventoryCategoryData()
	{
		bIsAllCategories = false;
	};
	
	FInventoryCategoryData(const FText& CategoryName, const FText& CategoryDescription, const int32 MaxQuantityPerStack, UTexture2D* CategoryTexture,
		UInventoryCategory* ParentCategory, const uint8 IsAllCategories = false)
		: CategoryName(CategoryName),
	      CategoryDescription(CategoryDescription),
		  MaxQuantityPerStack(MaxQuantityPerStack),
		  CategoryTexture(CategoryTexture),
		  ParentCategory(ParentCategory),
		  bIsAllCategories(IsAllCategories)
	{}

	// Name of this Category, useful for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true))
	FText CategoryName = LOCTEXT("InventoryCategory", "Default");

	// Description text of this Category, useful for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true, MultiLine))
	FText CategoryDescription = LOCTEXT("InventoryCategoryDescription", "Description Text.");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true))
	int32 MaxQuantityPerStack = 99;
	
	// Icon of this Category, useful for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true))
	UTexture2D* CategoryTexture = nullptr;

	// Parent Category of this one, useful for Inventory sorting, however, optional
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true))
	UInventoryCategory* ParentCategory = nullptr;

protected:

	// If true, works as placeholder for all categories
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	uint8 bIsAllCategories : 1;

public:

	FORCEINLINE	bool IsAllCategories() const
	{
		return bIsAllCategories;
	}
	
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

#pragma region InventoryItemAdditionalData

#define LOCTEXT_NAMESPACE "InventoryItemAdditionalData"

/**
 * Data of the Inventory Item.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryItemAdditionalData : public FTableRowBase
{
	GENERATED_BODY()

	FInventoryItemAdditionalData()
	{
		bHasWeight = true;
	};

	FInventoryItemAdditionalData(const FInventoryItemAdditionalData& Other)
	: ItemBasePrice(Other.ItemBasePrice)
	, ItemDurability(Other.ItemDurability)
	, bHasWeight(Other.bHasWeight)
	, ItemWeight(Other.ItemWeight)
	, PriceDurabilityRatio(Other.PriceDurabilityRatio)
	{};

public:

	// Price of Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Additional Data", meta = (ExposeOnSpawn = true, UIMin=0, ClampMin=0))
	float ItemBasePrice = 10.f;

	// Durability of Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Additional Data", meta = (ExposeOnSpawn = true, UIMin = 0, ClampMin = 0, UIMax=1, ClampMax=1))
	float ItemDurability = 1.f;

	// Defines whether the Item has weight or not 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Additional Data", meta = (ExposeOnSpawn = true))
	uint8 bHasWeight : 1;

	// Weight of Item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Additional Data", meta = (ExposeOnSpawn = true, UIMin = 0, ClampMin = 0, Units="Kilograms", EditCondition ="bHasWeight==true"))
	float ItemWeight = 1.5f;

	// How much does Durability affects the Price
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Additional Data", meta = (ExposeOnSpawn = true, UIMin = 0.1, ClampMin = 0.1))
	float PriceDurabilityRatio = 1.f;

public:

	inline FInventoryItemAdditionalData& operator=(const FInventoryItemAdditionalData& Other)
	{
		ItemBasePrice = Other.ItemBasePrice;
		ItemDurability = Other.ItemDurability;
		ItemWeight = Other.ItemWeight;
		PriceDurabilityRatio = Other.PriceDurabilityRatio;

		return *this;
	}
};

#undef LOCTEXT_NAMESPACE

#pragma endregion

#pragma region InventoryItemData

#define LOCTEXT_NAMESPACE "InventoryItemData"

/**
 * Data of the Inventory Item.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryItemData : public FTableRowBase
{
	GENERATED_BODY()

	FInventoryItemData() : ItemQuantityData()
	{};

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

	// Item Quantity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data", meta = (ExposeOnSpawn = true, ShowOnlyInnerProperties))
	FInventoryItemAdditionalData ItemAdditionalData;

	// Item Mesh. Static and Skeletal Mesh allowed. For preview only.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, AllowedClasses="StaticMesh, SkeletalMesh"))
	UStreamableRenderAsset* ItemMesh = nullptr;

	// Actor to spawn from this Item.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data", meta=(ExposeOnSpawn=true, BlueprintBaseOnly=true, AllowAbstract=false))
	TSoftClassPtr<AActor> SpawnItemClass = nullptr;

public:
	
	inline bool operator==(const FInventoryItemData& Other) const
	{
		return
		ItemCategory == Other.ItemCategory &&
		ItemRarity == Other.ItemRarity &&
		ItemTittle.ToString() == Other.ItemTittle.ToString() &&
		ItemDescription.ToString() == Other.ItemDescription.ToString() &&
		ItemThumbnail == Other.ItemThumbnail &&
		ItemMesh == Other.ItemMesh &&
		ItemQuantityData == Other.ItemQuantityData;
	}

	inline bool operator==(FInventoryItemData& Other) const
	{
		return
		ItemCategory == Other.ItemCategory &&
		ItemRarity == Other.ItemRarity &&
		ItemTittle.ToString() == Other.ItemTittle.ToString() &&
		ItemDescription.ToString() == Other.ItemDescription.ToString() &&
		ItemThumbnail == Other.ItemThumbnail &&
		ItemMesh == Other.ItemMesh &&
		ItemQuantityData == Other.ItemQuantityData;
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

	FORCEINLINE void UpdateItemQuantity(const int32 AddAmount)
	{
		ItemQuantityData.Quantity += AddAmount;
	}

	FORCEINLINE FGuid GetGuid() const
	{
		return ItemDataGUID;
	}

	FORCEINLINE bool CompareGuid(const FGuid& Other) const
	{
		return ItemDataGUID == Other;
	}

	FORCEINLINE void ForceGUID(const FGuid& Other)
	{
		ItemDataGUID = Other;
	}
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

#pragma region NotificationInfo

#define LOCTEXT_NAMESPACE "InventoryNotificationData"

/**
 * In-game Inventory Notification Info.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryNotificationInfo
{
	GENERATED_BODY()

	// Icon Texture to be displayed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault))
	UTexture2D* NotificationTexture = nullptr;

	// Notification text to be displayed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault))
	FText NotificationText = LOCTEXT("NotificationInfo", "Item has been added successfully to Inventory");

	// Duration for how long the notification will be visible.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault))
	float ShowDuration = 3.f;

	// Notification icon Tint.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault))
	FLinearColor IconTint = FLinearColor::White;
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

#pragma region InventorySlotData
/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventorySlotData
{
	GENERATED_BODY()

	FInventorySlotData() : Quantity(0), bCreatedManually(false)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	class UInventoryItem* Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	uint8 bCreatedManually : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	FIntPoint SlotCoordinates = FIntPoint(0,0);
	
};
#pragma endregion

#pragma region InventoryLayout
/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryLayout
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	FIntPoint InventoryLayout = FIntPoint(10,6);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	TMap<FIntPoint, FInventorySlotData> SavedInventoryLayout;
};
#pragma endregion 

#pragma region InventoryKeyActionData

#define LOCTEXT_NAMESPACE "InventoryKeyActionData"

USTRUCT(BlueprintType)
struct FInventoryKeyActionData
{
	GENERATED_BODY()
	
	FInventoryKeyActionData()
	{
		PlatformName = LOCTEXT("ActionKey", "Default");
		ActionKey = FKey(TEXT("E"));
		ActionKeyTexture = nullptr;
	};

	FInventoryKeyActionData(const FText& InName, const FKey& InKey, UTexture* InTexture) :
		PlatformName(InName),
		ActionKey(InKey),
		ActionKeyTexture(InTexture)
	{};

	FInventoryKeyActionData(const FInventoryKeyActionData& Other) :
		PlatformName(Other.PlatformName),
		ActionKey(Other.ActionKey),
		ActionKeyTexture(Other.ActionKeyTexture)
	{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=True))
	FText PlatformName = LOCTEXT("ActionKey", "Default");;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=True))
	FKey ActionKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn=True))
	UTexture* ActionKeyTexture = nullptr;

public:
	
	bool operator==(const FInventoryKeyActionData& Other) const
	{
		return
		PlatformName.ToString().Equals(Other.PlatformName.ToString()) &&
		ActionKey == Other.ActionKey &&
		ActionKeyTexture == Other.ActionKeyTexture;
	}

	bool operator!=(const FInventoryKeyActionData& Other) const
	{
		return
		!(*this == Other);
	}
	
	static friend uint32 GetTypeHash(const FInventoryKeyActionData& ActionKeyData)
	{
		return FCrc::MemCrc32(&ActionKeyData, sizeof(FInventoryKeyActionData));
	}
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

/**
 * 
 */
UENUM(BlueprintType)
enum class EInventoryContext : uint8
{
	EIC_Success						UMETA(DisplayName="Success"),
	EIC_Success_SplitStack			UMETA(DisplayName="Success - Split Stacks"),
	EIC_Success_RemovedItem			UMETA(DisplayName="Success - Removed Item"),
	EIC_Failed_InvalidItem			UMETA(DisplayName="Failed - Invalid Item"),
	EIC_Failed_LimitReached			UMETA(DisplayName="Failed - Max Quantity"),

	Default UMETA(Hidden)
};