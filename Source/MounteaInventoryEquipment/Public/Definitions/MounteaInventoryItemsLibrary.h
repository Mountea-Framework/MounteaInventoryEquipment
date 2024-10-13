// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaInventoryItemsLibrary.generated.h"

class UMounteaInventoryItemBlueprint;

/**
 * Represents a row in the Mountea Inventory Items Library.
 *
 * Holds information about an inventory item blueprint and its initial quantity.
 * Used in the inventory system to define the blueprint reference and initial count.
 *
 * @see UMounteaInventoryItemBlueprint
 * @see UMounteaInventoryItemsLibrary
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemsLibraryRow
{
	GENERATED_BODY()
	
	FMounteaInventoryItemsLibraryRow() : InitialQuantity(0), InventoryItemBlueprint(nullptr) {}

	explicit FMounteaInventoryItemsLibraryRow(UMounteaInventoryItemBlueprint* NewItem, const int32 Quantity = 1);

public:

	/** Defines the initial quantity of the inventory item. Cannot be negative. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault), meta=(NoResetToDefault, UIMin=0,ClampMin=0))
	int32 InitialQuantity;

	/** Reference to the Inventory Item Blueprint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault))
	TSoftObjectPtr<UMounteaInventoryItemBlueprint> InventoryItemBlueprint;

	/**
	 * Display name for the Inventory Item Blueprint, used in UI representation.
	 * Hidden in UI.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault, EditCondition="InitialQuantity<0",EditConditionHides))
	FString InventoryItemBlueprintName = TEXT("");

public:

	/** Updates the display name of the inventory item blueprint for UI or other representations. */
	void UpdateDisplayName();

	bool IsRowValid() const;

public:

	bool operator==(const FGuid& ItemGuid) const;
	bool operator==(const UMounteaInventoryItemBlueprint* ItemBlueprint) const;
	bool operator==(const FMounteaInventoryItemsLibraryRow& Other) const;
	bool operator!=(const FGuid& ItemGuid) const;
	bool operator!=(const UMounteaInventoryItemBlueprint* ItemBlueprint) const;
	bool operator!=(const FMounteaInventoryItemsLibraryRow& Other) const;

	friend uint32 GetTypeHash(const FMounteaInventoryItemsLibraryRow& Row);
};

/**
 * Represents the Inventory Items Library containing multiple item blueprints.
 * 
 * This library holds the inventory item blueprints and their associated initial quantities,
 * used throughout the inventory system to manage items.
 * This library is used to define starting inventory (for example, NPC has pre-defined inventory) or loadouts.
 * This library can be used to also contain definitions for all items in the game world.
 *
 * @see UMounteaInventoryItemBlueprint
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea", DisplayName="Inventory Item Blueprints Library")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemsLibrary : public UDataAsset
{
	GENERATED_BODY()

public:
	
	/**
	 * Retrieves all inventory item blueprints stored in the library.
	 * 
	 * @return Array of Inventory Item Blueprints.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprintsLibrary", meta=(CustomTag="MounteaK2Getter"))
	TArray<UMounteaInventoryItemBlueprint*> GetInventoryItemBlueprints() const;

	/**
	 * Retrieves all inventory item rows containing the blueprint and quantity.
	 * 
	 * @return Array of Inventory Item Blueprint Rows.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprintsLibrary", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE TArray<FMounteaInventoryItemsLibraryRow> GetInventoryItemBlueprintRows() const {return InventoryItemBlueprints; };

	/**
	 * Retrieves a single inventory item row based on the provided item Guid.
	 *
	 * @param ItemGuid The Guid of the inventory item to search for.
	 * @param bItemFound Output parameter, true if the item was found, false otherwise.
	 * @return The found inventory item row if found, or a default row if not.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemBlueprintsLibrary", meta=(CustomTag="MounteaK2Getter"))
	UMounteaInventoryItemBlueprint* GetInventoryItemByGuid(const FGuid& ItemGuid, bool& bItemFound) const;

	/**
	 * Retrieves a single inventory item row based on the provided blueprint.
	 *
	 * @param ItemBlueprint The inventory item blueprint to search for.
	 * @param bItemFound Output parameter, true if the item was found, false otherwise.
	 * @return The found inventory item row if found, or a default row if not.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemBlueprintsLibrary", meta=(CustomTag="MounteaK2Getter"))
	UMounteaInventoryItemBlueprint* GetInventoryItemByBlueprint(UMounteaInventoryItemBlueprint* ItemBlueprint, bool& bItemFound) const;
	
protected:
	
	/** Array of inventory item blueprint rows, storing blueprint references and initial quantities. */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault, UIMin=0,ClampMin=0, TitleProperty="{InitialQuantity} x {InventoryItemBlueprintName}"))
	TArray<FMounteaInventoryItemsLibraryRow> InventoryItemBlueprints;

#if WITH_EDITOR
	/** Called whenever a property changes in the editor to handle custom logic. */
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	
};

