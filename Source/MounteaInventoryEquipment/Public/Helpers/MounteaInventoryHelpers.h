// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "InputCoreTypes.h"
#include "Templates/SubclassOf.h"
#include "Engine/Level.h"
#include "MounteaInventoryHelpers.generated.h"

class UMounteaInventoryItemBase;
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
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	UTexture2D* NotificationTexture = nullptr;

	// Notification text to be displayed.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	FText NotificationText = LOCTEXT("InventoryNotificationData_Success", "Item has been added successfully to Inventory");

	// Duration for how long the notification will be visible.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	float ShowDuration = 3.f;

	// Notification icon Tint.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	FLinearColor IconTint = FLinearColor::White;

	FInventoryNotificationData(){};

	FInventoryNotificationData(UTexture2D* Texture, const FText& Text, const float& Duration, const FLinearColor& Tint)
		: NotificationTexture(Texture), NotificationText(Text), ShowDuration(Duration), IconTint(Tint){};
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

#define LOCTEXT_NAMESPACE "MounteaInventoryResults"

UENUM(BlueprintType)
enum class EInventoryUpdateResult : uint8
{
	EIUR_Success							UMETA(DisplayName="Success"),
	EIUR_Failed								UMETA(DisplayName="Failed"),

	Default										UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemUpdateResult : uint8
{
	EIUR_Success_UpdateItem			UMETA(DisplayName="Success - Update Item"),
	EIUR_Success_AddItem				UMETA(DisplayName="Success - Add Item"),
	EIUR_Success_SomeAdd			UMETA(DisplayName="Success - Partially Added"),
	EIUR_Success_RemovedItem		UMETA(DisplayName="Success - Removed Item"),
	EIUR_Failed_InvalidItem				UMETA(DisplayName="Failed - Invalid Item"),
	EIUR_Failed_LimitReached			UMETA(DisplayName="Failed - Max Quantity"),

	Default										UMETA(Hidden)
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

USTRUCT(BlueprintType)
struct FMounteaItemQuantityData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=0, ClampMin=0))
	int32 CurrentQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=1, ClampMin=1))
	int32 MaxQuantity = 99;

	UPROPERTY(EditAnywhere)
	uint8 bIsStackable : 1;

	FMounteaItemQuantityData()
	{
		bIsStackable = true;
	}
};

/**
 * Basic structure which contains required Inventory Item data.
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemRequiredData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault))
	FGameplayTagContainer CompatibleGameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault))
	FText ItemName = LOCTEXT("MounteaInventoryItem_ItemName", "New Item");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault, MultiLine))
	FText ItemDescription = LOCTEXT("MounteaInventoryItem_ItemName", "This is a multiline description of a new Item.\nYou can use this to provide some lore data or any additional info about this Item.");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowAbstract=false, NoResetToDefault))
	TSubclassOf<class UMounteaInventoryItemCategory> ItemCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowAbstract=false, NoResetToDefault))
	TSubclassOf<class UMounteaInventoryItemRarity> ItemRarity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoResetToDefault, AllowedClasses="StaticMesh, SkeletalMesh"))
	UStreamableRenderAsset* ItemMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ShowOnlyInnerProperties))
	FMounteaItemQuantityData ItemQuantity;

public:

	bool operator==(const FMounteaInventoryItemRequiredData& Other) const
	{
		if (CompatibleGameplayTags.IsEmpty())
		{
			return ItemName.EqualTo(Other.ItemName);
		}

		return ItemName.EqualTo(Other.ItemName) && CompatibleGameplayTags == Other.CompatibleGameplayTags;
	}

	bool operator!=(const FMounteaInventoryItemRequiredData& Other) const
	{
		return !(*this == Other);
	}

	FMounteaInventoryItemRequiredData& operator=(const FMounteaInventoryItemRequiredData& Other)
	{
		return *this;
	}

	static friend uint32 GetTypeHash(const FMounteaInventoryItemRequiredData& Data)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(Data.ItemName.ToString()));
				
		return Hash;
	}
};

/**
 * Basic structure which contains additional Inventory Item data.
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemOptionalData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture* ItemIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasWeight : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=0.f, ClampMin=0.f, Units=kg, EditCondition="bHasWeight"))
	float BaseWeight = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasValue : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasValue"))
	float ItemBaseValue = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bHasDurability : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn, UIMin=0, ClampMin=0, EditCondition="bHasDurability"))
	float ItemBaseDurability = 10.f;
};

/**
 * Container for all Inventory Item Data
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMounteaInventoryItemRequiredData RequiredData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMounteaInventoryItemOptionalData OptionalData;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSearchByTag"))
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByClass : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSearchByClass"))
	TSubclassOf<UMounteaInventoryItemBase> Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByGUID : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSearchByGUID"))
	FGuid Guid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByItem : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSearchByGUID"))
	UMounteaInventoryItemBase* Item ;

	bool IsValid() const
	{
		return bSearchByClass || bSearchByTag || bSearchByGUID || bSearchByItem;
	}
};

#pragma endregion 