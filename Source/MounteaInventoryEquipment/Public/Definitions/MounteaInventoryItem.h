// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MounteaInventoryTableTypes.h"

#include "Helpers/MounteaInventoryHelpers.h"
#include "Interfaces/MounteaInventoryEquipmentItem.h"

#include "Helpers/MounteaItemHelpers.h"

#include "Setup/MounteaInventoryItemConfig.h"
#include "Definitions/MounteaItemAction.h"

#include "MounteaInventoryItem.generated.h"

struct FMounteaItemAction;
struct FMounteaItemConfig;
class IMounteaInventoryPickupInterface;


#define LOCTEXT_NAMESPACE "MounteaInventoryItem"

/**
 * Inventory Item Data Asset
 * 
 * Base class for inventory item templates.
 *
 * This class represents an item template to create instances from.
 * It serves as a data asset base class and provides functionality for holding item data.
 *
 * @see UDataAsset
 * @see IMounteaInventoryEquipmentItem
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Item-Object
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea", DisplayName="Inventory Item (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBase : public UPrimaryDataAsset, public IMounteaInventoryEquipmentItem
{
	GENERATED_BODY()

private:
	
	virtual void PostInitProperties() override;

public:
	
	/**
	 * Indicates the source of the item's data, which determines whether the item's data is derived from a source table or another source.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(ShowOnlyInnerProperties, EditFixedOrder))
	EItemDataSource ItemDataSource = EItemDataSource::EIDS_SourceTable;

	/**
	 * A reference to the data table that contains the definitions of the item if the item is based on a table source.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(DisplayThumbnail=false, EditCondition="ItemDataSource==EItemDataSource::EIDS_SourceTable", EditFixedOrder))
	UMounteaInventoryItemsTable* SourceTable;

	/**
	 * The row identifier within the source table that corresponds to this item's data.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(GetOptions="GetSourceTableRows", EditCondition="SourceTable!=nullptr&&ItemDataSource==EItemDataSource::EIDS_SourceTable", EditFixedOrder))
	FName SourceRow;

	/**
	 * The core data of the item, including its name, description, stats, and any other relevant information.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="2. Required", meta=(ShowOnlyInnerProperties, EditCondition="ItemDataSource!=EItemDataSource::EIDS_SourceTable"))
	FMounteaInventoryItemData ItemData;
    
	/**
	 * A list of actions that can be performed with the item, such as use, equip, or any custom actions defined within the game.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="3. Optional")
	TArray<FMounteaItemAction> ItemActions;
	
protected:
    
	/**
	 * Configuration data for the item, which may include dynamic properties, behaviors, and other modifiable aspects of the item.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "4. Config", NoClear, meta=(NoResetToDefault))
	FMounteaItemConfig ItemConfig;

private:

	/**
	 * A unique identifier for this item template, ensuring that it can be distinguished from other items within the game.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="5. Debug")
	FGuid ItemGuid = FGuid::NewGuid();


public:

	/**
	 * Retrieves the item data structure containing essential information about this item.
	 * @return A FMounteaInventoryItemData structure containing the item's data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual FMounteaInventoryItemData GetItemData() const override
	{ return ItemData; }

	/**
	 * Obtains the action objects associated with the item, which can be used to perform various interactions with the item.
	 * @return An array of UMounteaInventoryItemAction objects.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	TArray<UMounteaInventoryItemAction*> GetItemActions() const;

	/**
	 * Retrieves the definitions of item actions that this item can perform.
	 * @return An array of FMounteaItemAction structures defining the item's actions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	FORCEINLINE TArray<FMounteaItemAction> GetItemActionsDefinitions() const
	{
		return ItemActions;
	}

	/**
	 * Gets the class type of the item's configuration, allowing access to specific configuration properties.
	 * @return A TSubclassOf reference to the UMounteaInventoryItemConfig class.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual TSubclassOf<UMounteaInventoryItemConfig> GetItemConfigClass() const override
	{
		if (ItemConfig.ItemConfig)
		{
			return ItemConfig.ItemConfig->StaticClass();
		}
		
		return nullptr;
	}

	/**
	 * Provides the item configuration object, which can be used to customize and query item properties.
	 * @param ClassFilter The class type to filter the configuration object.
	 * @param bResult Whether the configuration object was successfully retrieved.
	 * @return A pointer to the UMounteaInventoryItemConfig object.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryItemConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	virtual UMounteaInventoryItemConfig* GetItemConfig(const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter, bool& bResult) const override
	{
		if (ClassFilter == nullptr)
		{
			bResult = false;
			return nullptr;
		}

		bResult = true;
		if (ItemConfig.ItemConfig == nullptr)
		{
			return NewObject<UMounteaInventoryItemConfig>(GetPackage(), ClassFilter);
		}

		return ItemConfig.ItemConfig->IsA(ClassFilter) ? ItemConfig.ItemConfig : NewObject<UMounteaInventoryItemConfig>(GetPackage(), ClassFilter);
	}

	/**
	 * Checks the validity of the item's data within the context of the provided world.
	 * @param WorldContextObject The context within which to validate the item.
	 * @return True if the item is valid within the specified context, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Item", meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext ) )
	bool IsValid(UObject* WorldContextObject) const;

	/**
	 * Retrieves the unique identifier for this item template.
	 * @return The FGuid that represents the unique identifier of this item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Item")
	virtual FGuid GetGuid() const override
	{ return ItemGuid; };

	/**
	 * Sets a new unique identifier for this item template.
	 * @param NewGuid The new FGuid to assign as this item's unique identifier.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	virtual void SetGuid(const FGuid& NewGuid) override
	{ ItemGuid = NewGuid; };

	/**
	 * Initializes the actions associated with this item, setting them up for usage within the game.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	void InitializeItemActions();

	/**
	 * Ensures the item's data is valid and sets default values if necessary.
	 */
	virtual void SetValidData() override;
	
protected:

	/**
	 * Retrieves the names of rows from the source table that contains item definitions.
	 * @return An array of FName identifiers for the rows in the source table.
	 */
	UFUNCTION()
	virtual TArray<FName> GetSourceTableRows() const
	{
		if (!SourceTable)
		{
			return TArray<FName>();
		}

		return SourceTable->GetRowNames();
	};

	/**
	 * Clears the reference to the source data table, typically used when the item data source changes or is no longer needed.
	 */
	void ClearDataTable();
	/**
	 * Copies item data from the source table based on the specified source row identifier.
	 */
	void CopyFromTable();
	/**
	* Clears any mapped values that were previously copied from the source table, resetting the item template.
	*/
	void ClearMappedValues();
	/**
	 * Copies gameplay tags from the item types defined in the source table to this item's data structure.
	 */
	void CopyTagsFromTypes();
	/**
	 * Ensures the item's configuration is valid and sets default values if necessary.
	 */
	void EnsureValidConfig();

#if WITH_EDITOR

protected:
	
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
public:

	/**
	 * An editor-only function that ensures the item template's data is valid within the editor.
	 */
	UFUNCTION()
	void SetValidDataEditor();
	
#endif
	
};

#undef LOCTEXT_NAMESPACE