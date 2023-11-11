// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "MounteaInventoryInstancedItemInterface.generated.h"

class IMounteaInventoryInterface;
class UMounteaInventoryItemBase;
class UMounteaInventoryItemsTable;

struct FMounteaInventoryItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemGenericEvent, const FString&, Message);

/**
 * FItemInitParams
 * 
 * This structure encapsulates the necessary parameters for initializing an inventory item within the game.
 * It provides a flexible way to either link to an existing item or create a new one from a data table,
 * with additional context provided by an optional payload if needed.
 *
 * Properties:
 * - OwningInventory: The inventory interface that will contain and manage the item. It is crucial for
 *   the item's interaction with the inventory system, such as adding or removing items.
 * 
 * - SourceItem: A reference to an existing item used as a template for creating a new item. It is used
 *   when duplicating items or when a direct reference to an item object is required.
 * 
 * - SourceTable: Points to a data table that holds item definitions. It is utilized when creating items
 *   from data-driven configurations, allowing for dynamic item instantiation.
 * 
 * - SourceRow: Identifies the specific row in the data table from which the item's properties are derived.
 *   It is used in conjunction with 'SourceTable' to pinpoint the exact data entry for item creation.
 * 
 * - Quantity: Specifies the number of items to be created. It is particularly relevant for items that can
 *   be stacked or those that can exist in multiples within the inventory.
 * 
 * - OptionalPayload: Provides a way to pass additional, context-specific data during item initialization. 
 *   This data can be utilized to apply custom behaviors or modifications to the item beyond the standard properties.
 * 
 * Initialization:
 * Populate the struct with relevant data before initiating the item creation process. The 'OwningInventory' is
 * always required, while 'SourceItem' or 'SourceTable' (with 'SourceRow') is necessary to determine the item's origin.
 */
USTRUCT(BlueprintType)
struct FItemInitParams
{
	GENERATED_BODY()

	FItemInitParams(): Quantity(0)
	{};

	FItemInitParams(const TScriptInterface<IMounteaInventoryInterface>& InOwningInventory, UMounteaInventoryItemBase* InSourceItem, const int32 InQuantity, UObject* InOptionalPayload = nullptr):
		OwningInventory(InOwningInventory), SourceItem(InSourceItem), Quantity(InQuantity), OptionalPayload(InOptionalPayload)
	{};

	FItemInitParams(const TScriptInterface<IMounteaInventoryInterface>& InOwningInventory, UMounteaInventoryItemsTable* InSourceTable, const FName& InSourceRow, const int32 InQuantity, UObject* InOptionalPayload = nullptr):
	OwningInventory(InOwningInventory), SourceTable(InSourceTable), SourceRow(InSourceRow), Quantity(InQuantity), OptionalPayload(InOptionalPayload)
	{};
	
	FItemInitParams(const TScriptInterface<IMounteaInventoryInterface>& InOwningInventory, UMounteaInventoryItemBase* InSourceItem, UMounteaInventoryItemsTable* InSourceTable, const FName& InSourceRow, const int32 InQuantity, UObject* InOptionalPayload = nullptr):
		OwningInventory(InOwningInventory), SourceItem(InSourceItem), SourceTable(InSourceTable), SourceRow(InSourceRow), Quantity(InQuantity), OptionalPayload(InOptionalPayload)
	{};

public:

	/**
	 * The inventory interface reference that holds the ownership of the item. It is critical for ensuring the
	 * item's interactions and lifecycle are properly managed within the inventory context.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IMounteaInventoryInterface> OwningInventory;
	
	/**
	 * A pointer to an existing item instance used as a blueprint for the new item. It serves as a direct link
	 * to an item object, enabling the cloning or referencing of specific item properties and behaviors.
	 */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMounteaInventoryItemBase* SourceItem = nullptr;

	/**
     * A reference to a data table that provides a template for item creation. It allows for the instantiation
	 * of items based on predefined data entries, supporting a data-driven approach to item management.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMounteaInventoryItemsTable* SourceTable = nullptr;

	/**
	 * An identifier for the row within the 'SourceTable' that contains the item definition. It enables precise
	 * selection of item data for creating a new item instance with the desired attributes and stats.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName SourceRow;

	/**
	 * The number of instances of the item to be initialized. It defines how many identical items should be
	 * created or referenced, which is essential for managing stackable or bulk items within the inventory.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Quantity;

	/**
	* An optional UObject that can carry additional data or context required during the item's initialization process.
	* This payload does not influence the basic validity of the item parameters but can be utilized by the
	* initialization logic to apply custom behaviors or attributes to the newly created or configured item.
	*
	* Example usage includes passing a custom configuration struct, a modifier object, or any other relevant UObject
	* that should be considered when the item is being set up within the inventory system.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UObject* OptionalPayload = nullptr;

public:

	/**
	 * Checks the validity of the item initialization parameters.
	 * Ensures that a non-null reference to the owning inventory is present and that either a source item or a source table with a valid row is provided.
	 * These conditions are necessary to either link to an existing item or instantiate a new one from a data table.
	 * 
	 * @return True if the parameters are valid and sufficient for item initialization, false otherwise.
	 */
	bool IsValid() const
	{
		if (OwningInventory.GetObject() == nullptr) return false;
		if (SourceItem == nullptr && SourceTable == nullptr) return false;
		if (Quantity < 0) return false;
		
		return true;
	};
};

// This class does not need to be modified.
UINTERFACE()
class UMounteaInventoryInstancedItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryInstancedItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Item")
	void OnItemBeginPlay(const FString& Message);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mountea|Item")
	bool InitializeNewItem(const FItemInitParams& InitParams);
	virtual bool InitializeNewItem_Implementation(const FItemInitParams& InitParams) = 0;
	
public:

	virtual void SetOwningInventory(TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory) = 0;
	virtual TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const = 0;
	
	virtual int32 GetQuantity() const = 0;
	virtual void SetQuantity(const int32& NewValue) = 0;
	virtual int32 ModifyQuantity(const int32& ValueToAdd) = 0;

	virtual void EmptyItem() = 0;

	virtual FGameplayTagContainer GetItemFlags() const = 0;
	virtual void AddItemFlag(const FGameplayTag& NewFlag) = 0;
	virtual void SetItemFlags(const FGameplayTagContainer& NewFlags) = 0;
	virtual void RemoveItemFlag(const FGameplayTag& RemoveFlag) = 0;
	virtual bool IsFlagSet(const FGameplayTag& QueryFlag) const = 0;
	virtual bool AreFlagsSet(const FGameplayTagContainer& QueryFlags, const bool bSimpleSearch = true) const = 0;
	
public:

	virtual FItemGenericEvent& GetEventHandle_OnItemAdded() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemRemoved() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemInitialized() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemModified() = 0;
};
