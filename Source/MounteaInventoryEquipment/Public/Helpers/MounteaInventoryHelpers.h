// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "InputCoreTypes.h"
#include "Templates/SubclassOf.h"
#include "Engine/Level.h"
#include "MounteaInventoryHelpers.generated.h"

struct FMounteaItemAction;
class UMounteaInventoryItemCategory;
class UMounteaInventoryItemRarity;
class UMounteaInventoryItemBase;
class UTexture;
class UTexture2D;
class AActor;
class ULevel;
class UStreamableRenderAsset;
class UMounteaInstancedItem;

class UMounteaInventoryItemConfig;

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
	EIUR_Success  UMETA(DisplayName="Success"),
	EIUR_Failed	      UMETA(DisplayName="Failed"),

	Default										UMETA(Hidden)
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EInventoryTransactionType : uint8
{
	None = 0                                             UMETA(Hidden),
	EITT_FullTransaction = 1 << 0       UMETA(DisplayName="Full Transaction"),       // 0001
	EITT_PartialTransaction = 1 << 1  UMETA(DisplayName="Partial Transaction"), // 0010
	EITT_Sync = 1 << 2                           UMETA(DisplayName="Sync Operation"),       // 0100
	EITT_Async = 1 << 3                         UMETA(DisplayName="Async Operation")      // 1000
};
ENUM_CLASS_FLAGS(EInventoryTransactionType);

UENUM(BlueprintType)
enum class EItemUpdateResult : uint8
{
	EIUR_Success_UpdateItem			UMETA(DisplayName="Success - Update Item"),
	EIUR_Success_AddItem				UMETA(DisplayName="Success - Add Item"),
	EIUR_Success_SomeAdd				UMETA(DisplayName="Success - Partially Added"),
	EIUR_Success_RemovedItem			UMETA(DisplayName="Success - Removed Item"),
	EIUR_Success_EquipItem				UMETA(DisplayName="Success - Equip Item"),
	EIUR_Success_UnEquipItem			UMETA(DisplayName="Success - Unequip Item"),
	EIUR_Failed_InvalidItem					UMETA(DisplayName="Failed - Invalid Item"),
	EIUR_Failed_LimitReached			UMETA(DisplayName="Failed - Max Quantity"),

	Default											UMETA(Hidden)
};

/*
	ResultID Values:
	102 Processing: Operation cannot proceed on Client side and is moved to Server side to process.
	200 OK: The operation was successful. All items were added/removed as expected.
	201 Created: A new item was successfully created and added to the inventory.
	204 No Content: The operation was successful, but there was nothing to add/remove (e.g., the quantity was 0).
	206 Partial Content: Only a part of the requested operation was completed. For example, some but not all of the items were added to a stack because it reached its maximum capacity.
	400 Bad Request: The request to update the inventory was invalid. This could be due to bad input parameters or an illegal state (e.g., trying to add a negative quantity).
	404 Not Found: The specified item was not found in the inventory.
	409 Conflict: The operation could not be completed due to a conflict. For example, trying to add an item to a full inventory.
	413 Payload Too Large: The quantity of the item being added exceeds the maximum allowed stack size or inventory capacity.
	422 Unprocessable Entity: The operation is understood but cannot be processed. For example, the item type cannot be stacked.
	500 Internal Server Error: A generic error code indicating that something went wrong with the inventory system that was not due to user input.
 */

/**
 * FInventoryUpdateResult
 * 
 * Encapsulates the outcome of inventory operations such as add, remove, or move items.
 * This structure is intended to provide standardized feedback after an inventory transaction,
 * allowing the system to handle the results accordingly.
 * 
 * Fields:
 * - OptionalPayload: A pointer to any UObject that might provide additional context or data
 *   related to the inventory operation. This could be used to pass along objects that were
 *   involved in or affected by the transaction, such as the actor who picked up the item.
 * 
 * - ResultID: An integer that represents the outcome of the inventory operation. It adheres to
 *   HTTP-like status codes for ease of understanding and handling. For instance, a value of 200
 *   indicates a successful operation, while 404 indicates that the requested item was not found.
 * 
 * - ResultText: A localized text that gives a human-readable description of the result. This can
 *   be displayed in logs, UI elements, or debugging tools to inform the user or developer about
 *   the specifics of the inventory operation's outcome.
 * 
 * Usage:
 * - Initialize the structure with default values or with specific result information.
 * - Use the structure to communicate the outcome of an inventory operation, attaching any additional
 *   context as needed via the OptionalPayload.
 * - Check the ResultID and handle the operation's outcome in the game's logic, UI updates, or error handling.
 */
USTRUCT(BlueprintType)
struct FInventoryUpdateResult
{
	GENERATED_BODY()

	// Default constructor.
	FInventoryUpdateResult() :
		OptionalPayload(nullptr), ResultID(-1), ResultText(LOCTEXT("MounteaInventoryResults_InventoryUpdateResult", "Empty Message"))
	{};

	// Constructor for initializing with a specific result ID and descriptive text.
	FInventoryUpdateResult(const int32 InResultID, const FText& InResultText) :
		OptionalPayload(nullptr), ResultID(InResultID), ResultText(InResultText)
	{};

	// Constructor for initializing with a specific result ID, descriptive text, and an optional payload.
	FInventoryUpdateResult(const int32 InResultID, const FText& InResultText, UObject* InOptionalPayload = nullptr) :
		OptionalPayload(InOptionalPayload), ResultID(InResultID), ResultText(InResultText)
	{};
	
public:

	// An optional payload that may carry additional information related to the inventory operation.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UObject* OptionalPayload = nullptr;

	// A numeric identifier that represents the result of the inventory operation, akin to HTTP status codes.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 ResultID;

	// Descriptive text providing details about the outcome of the inventory operation.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FText ResultText;
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

public:
	
	FItemRetrievalFilter(): bSearchByTag(0), bSearchByClass(0), bSearchByGUID(0), bSearchByItem(0), Item(nullptr)
	{};
	
	FItemRetrievalFilter(const bool ByTag, const FGameplayTagContainer& InTags, const bool ByClass, const TSubclassOf<UMounteaInventoryItemBase> InClass, const bool ByGUID, const FGuid InGUID, const bool ByItem, UMounteaInstancedItem* InItem)
		: bSearchByTag(ByTag), Tags(InTags), bSearchByClass(ByClass), Class(InClass), bSearchByGUID(ByGUID), Guid(InGUID), bSearchByItem(ByItem), Item(InItem)
	{};

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByTag : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByClass : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMounteaInventoryItemBase> Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByGUID : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Guid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	uint8 bSearchByItem : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMounteaInstancedItem* Item;

public:

	bool IsValid() const
	{
		return bSearchByClass || bSearchByTag || bSearchByGUID || bSearchByItem;
	};
};

#pragma endregion

USTRUCT(BlueprintType)
struct FMounteaItemDescription : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(MultiLine=true))
	FText ItemShortDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(MultiLine=true))
	FText ItemLongDescription;
};

USTRUCT(BlueprintType)
struct FMounteaDynamicDelegateContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Command;

	UPROPERTY(BlueprintReadWrite)
	UObject* Payload = nullptr;
};