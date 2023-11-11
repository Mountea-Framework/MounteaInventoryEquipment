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
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(ShowOnlyInnerProperties, EditFixedOrder))
	EItemDataSource ItemDataSource = EItemDataSource::EIDS_SourceTable;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(DisplayThumbnail=false, EditCondition="ItemDataSource==EItemDataSource::EIDS_SourceTable", EditFixedOrder))
	UMounteaInventoryItemsTable* SourceTable;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(GetOptions="GetSourceTableRows", EditCondition="SourceTable!=nullptr&&ItemDataSource==EItemDataSource::EIDS_SourceTable", EditFixedOrder))
	FName SourceRow;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="2. Required", meta=(ShowOnlyInnerProperties, EditCondition="ItemDataSource!=EItemDataSource::EIDS_SourceTable"))
	FMounteaInventoryItemData ItemData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="3. Optional")
	TArray<FMounteaItemAction> ItemActions;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "4. Config", NoClear, meta=(NoResetToDefault))
	FMounteaItemConfig ItemConfig;

private:

	UPROPERTY(SaveGame, EditDefaultsOnly, Category="5. Debug")
	FGuid ItemGuid = FGuid::NewGuid();

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	FORCEINLINE FMounteaInventoryItemData GetItemData() const
	{ return ItemData; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	TArray<UMounteaInventoryItemAction*> GetItemActions() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	FORCEINLINE TArray<FMounteaItemAction> GetItemActionsDefinitions() const
	{
		return ItemActions;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual TSubclassOf<UMounteaInventoryItemConfig> GetItemConfigClass() const override
	{
		if (ItemConfig.ItemConfig)
		{
			return ItemConfig.ItemConfig->StaticClass();
		}
		
		return nullptr;
	}

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
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Item", meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext ) )
	bool IsValid(UObject* WorldContextObject) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Item")
	virtual FGuid GetGuid() const override
	{ return ItemGuid; };

	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	virtual void SetGuid(const FGuid& NewGuid) override
	{ ItemGuid = NewGuid; };
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	void InitializeItemActions();
	
	virtual void SetValidData() override;
	
protected:
	
	UFUNCTION()
	virtual TArray<FName> GetSourceTableRows() const
	{
		if (!SourceTable)
		{
			return TArray<FName>();
		}

		return SourceTable->GetRowNames();
	};
	
	void ClearDataTable();
	void CopyFromTable();
	void ClearMappedValues();
	void CopyTagsFromTypes();
	void EnsureValidConfig();

#if WITH_EDITOR

protected:
	
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
public:
	
	UFUNCTION()
	void SetValidDataEditor();
	
#endif
	
};

#undef LOCTEXT_NAMESPACE