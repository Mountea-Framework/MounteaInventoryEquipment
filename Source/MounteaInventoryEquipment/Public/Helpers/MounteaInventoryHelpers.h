// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "InputCoreTypes.h"
#include "Templates/SubclassOf.h"
#include "Engine/Level.h"
#include "MounteaInventoryFlagsLibrary.h"
#include "MounteaInventoryHelpers.generated.h"

class UMounteaInventoryItemCategory;
class UMounteaInventoryItemRarity;
class UMounteaInventoryItem;
class UTexture;
class UTexture2D;
class AActor;
class ULevel;
class UStreamableRenderAsset;

#pragma region NotificationData

#define LOCTEXT_NAMESPACE "InventoryNotificationData"

/**
 * In-game Inventory Notification Info.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInventoryNotificationData
{
	GENERATED_BODY()

	// Icon Texture to be displayed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	UTexture2D* NotificationTexture = nullptr;

	// Notification text to be displayed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	FText NotificationText = LOCTEXT("InventoryNotificationData_Success", "Item has been added successfully to Inventory");

	// Duration for how long the notification will be visible.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	float ShowDuration = 3.f;

	// Notification icon Tint.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	FLinearColor IconTint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	int32 Weight = 0;

	FInventoryNotificationData()
	{
	};

	FInventoryNotificationData(UTexture2D* Texture, const FText& Text, const float& Duration, const FLinearColor& Tint, const int32& NewWeight = 1)
		: NotificationTexture(Texture), NotificationText(Text), ShowDuration(Duration), IconTint(Tint), Weight(NewWeight)
	{
	};
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

#define LOCTEXT_NAMESPACE "MounteaInventoryResults"

UENUM(BlueprintType)
enum class EInventoryUpdateResult : uint8
{
	EIUR_Success					UMETA(DisplayName="Success"),
	EIUR_Failed						UMETA(DisplayName="Failed"),

	Default							UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemUpdateResult : uint8
{
	EIUR_Success_UpdateItem			UMETA(DisplayName="Success - Update Item"),
	EIUR_Success_AddItem			UMETA(DisplayName="Success - Add Item"),
	EIUR_Success_SomeAdd			UMETA(DisplayName="Success - Partially Added"),
	EIUR_Success_RemovedItem		UMETA(DisplayName="Success - Removed Item"),
	EIUR_Success_EquipItem			UMETA(DisplayName="Success - Equip Item"),
	EIUR_Success_UnEquipItem		UMETA(DisplayName="Success - Unequip Item"),
	EIUR_Failed_InvalidItem			UMETA(DisplayName="Failed - Invalid Item"),
	EIUR_Failed_LimitReached		UMETA(DisplayName="Failed - Max Quantity"),

	Default							UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FInventoryUpdateResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EInventoryUpdateResult InventoryUpdateResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FText UpdateMessage;
};

USTRUCT(BlueprintType)
struct FItemUpdateResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EItemUpdateResult ItemUpdateResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FText UpdateMessage;
};

#undef LOCTEXT_NAMESPACE

#pragma region ItemData

#define LOCTEXT_NAMESPACE "MounteaInventoryItem"

/**
 * Basic structure which contains required Inventory Item data.
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemRequiredData
{
	GENERATED_BODY()

public:

	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TObjectPtr<UMounteaInventoryItemCategory> ItemCategory;
	
	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TObjectPtr<UMounteaInventoryItemRarity> ItemRarity;

	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault))
	FText ItemName = LOCTEXT("MounteaInventoryItem_ItemName", "New Item");

	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault, MultiLine))
	FText ItemShortInfo = LOCTEXT("MounteaInventoryItem_ItemShortInfo", "");

	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault, MultiLine))
	FText ItemLongInfo = LOCTEXT("MounteaInventoryItem_ItemShortInfo", "");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Required", meta=(Bitmask, BitmaskEnum="EInventoryItemFlags"))
	uint8 ItemFlags;

	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault))
	FSlateBrush ItemThumbnail;

	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault))
	FSlateBrush ItemCover;
	
	UPROPERTY(Category="PrimaryData",EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault, AllowedClasses="StaticMesh,SkeletalMesh"))
	UStreamableRenderAsset* ItemMesh = nullptr;

public:

	bool operator==(const FMounteaInventoryItemRequiredData& Other) const
	{
		return ItemName.EqualTo(Other.ItemName) && ItemCategory == Other.ItemCategory && ItemRarity == Other.ItemRarity;
	}

	bool operator!=(const FMounteaInventoryItemRequiredData& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FMounteaInventoryItemRequiredData& Data)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(Data.ItemName.ToString()));
				
		return Hash;
	}

	static FString ToJson()
	{
		return FString();
	}
};

/**
 * Basic structure which contains additional Inventory Item data.
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemOptionalData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasDurability : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasDurability"))
	float MaxDurability = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasDurability"))
	float BaseDurability = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasDurability"))
	float DurabilityPenalization = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasDurability"))
	float DurabilityToPriceCoefficient = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasPrice : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasValue"))
	float BasePrice = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasValue"))
	float SellPriceCoefficient = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0))
	int32 MaxStackSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0))
	int32 MaxQuantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasWeight : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=0.f, ClampMin=0.f, Units=kg, EditCondition="bHasWeight"))
	float Weight = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SpawnActor;
};

USTRUCT(BlueprintType)
struct FMounteaInventoryItemInstancedData
{
	GENERATED_BODY()

public:

	FMounteaInventoryItemInstancedData() {};

	FMounteaInventoryItemInstancedData(const float InDurability, const int32 InPrice, const int32 InQuantity)
	: Durability(InDurability), Price(InPrice), Quantity(InQuantity) {};

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0))
	float Durability = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0))
	int32 Price = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, UIMin=0, ClampMin=0))
	int32 Quantity = 0;
};

#undef LOCTEXT_NAMESPACE

#pragma endregion

#pragma region Filtration

/**
 * Helper structure that contains options to filter items in an inventory system by Item, Class, Tag, or Guid.
 * It can be used to search for specific items in an inventory, or to retrieve all items that match a certain set of criteria.
 *
 * * Item: Specifies the exact item to search for in the inventory. If this member is set, the filter will only return single value that match the specified item.
 * * Class: Specifies the class of items to search for in the inventory. If this member is set, the filter will return items that belong to the specified class.
 * * Tag: Specifies the tag to search for in the inventory. If this  is set, the filter will only return items that have the specified tag.
 * * Guid: Specifies the GUID to search for in the inventory. If this member is set, the filter will only return items that have the specified GUID.
 *
 * Each member of the structure can be used alone or in combination with other members to create more complex filters.
 * For example, you could create a filter that searches for all items of a certain class and also items which have a specific tag.
 * All functions use additive OR logic, meaning:
 * * If you specify more than one filter, results will contain combined results for each selected criteria
 */
USTRUCT(BlueprintType)
struct FItemRetrievalFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByTag : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //, meta=(EditCondition="bSearchByTag"))
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByClass : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //, meta=(EditCondition="bSearchByClass"))
	TSubclassOf<UMounteaInventoryItem> Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByGUID : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //, meta=(EditCondition="bSearchByGUID"))
	FGuid Guid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByItem : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //, meta=(EditCondition="bSearchByGUID"))
	UMounteaInventoryItem* Item ;

	bool IsValid() const
	{
		return bSearchByClass || bSearchByTag || bSearchByGUID || bSearchByItem;
	}

	FItemRetrievalFilter(): bSearchByTag(0), bSearchByClass(0), bSearchByGUID(0), bSearchByItem(0), Item(nullptr)
	{};
	
	FItemRetrievalFilter(const bool ByTag, const FGameplayTagContainer& InTags, const bool ByClass, const TSubclassOf<UMounteaInventoryItem> InClass, const bool ByGUID, const FGuid InGUID, const bool ByItem, UMounteaInventoryItem* InItem)
		: bSearchByTag(ByTag), Tags(InTags), bSearchByClass(ByClass), Class(InClass), bSearchByGUID(ByGUID), Guid(InGUID), bSearchByItem(ByItem), Item(InItem)
	{};
};

#pragma endregion

USTRUCT(BlueprintType)
struct FMounteaInventoryCommandContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Command;

	UPROPERTY(BlueprintReadWrite)
	UObject* Payload = nullptr;
};