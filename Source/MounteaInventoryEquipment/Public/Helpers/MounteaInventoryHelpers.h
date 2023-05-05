// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "InputCoreTypes.h"
#include "Templates/SubclassOf.h"
#include "Engine/Level.h"
#include "MounteaInventoryHelpers.generated.h"

class UMounteaInventoryItem_Base;
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
	FText NotificationText = LOCTEXT("InventoryNotificationData", "Item has been added successfully to Inventory");

	// Duration for how long the notification will be visible.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	float ShowDuration = 3.f;

	// Notification icon Tint.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, meta=(NoResetToDefault=true))
	FLinearColor IconTint = FLinearColor::White;
};

#undef LOCTEXT_NAMESPACE

#pragma endregion 

#pragma region ItemData

#define LOCTEXT_NAMESPACE "MounteaInventoryItem"

USTRUCT(BlueprintType)
struct FMounteaItemQuantityData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(UIMin=0, ClampMin=0))
	int32 CurrentQuantity = 1;

	UPROPERTY(EditAnywhere, meta=(UIMin=1, ClampMin=1))
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

	UPROPERTY(VisibleAnywhere)
	FGuid ItemGuid = FGuid::NewGuid();

public:

	bool operator==(const FMounteaInventoryItemRequiredData& Other) const
	{
		return ItemGuid == Other.ItemGuid;
	}

	bool operator!=(const FMounteaInventoryItemRequiredData& Other) const
	{
		return !(*this == Other);
	}

	FMounteaInventoryItemRequiredData& operator=(const FMounteaInventoryItemRequiredData& Other)
	{
		ItemGuid.Invalidate();
		ItemGuid = FGuid::NewGuid();

		return *this;
	}

	static friend uint32 GetTypeHash(const FMounteaInventoryItemRequiredData& Data)
	{
		return FCrc::MemCrc32(&Data.ItemGuid, sizeof(FGuid));
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
	TSubclassOf<UMounteaInventoryItem_Base> Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByGUID : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSearchByGUID"))
	FGuid Guid;

	bool IsValid() const
	{
		return bSearchByClass || bSearchByTag || bSearchByGUID;
	}
};

#pragma endregion 