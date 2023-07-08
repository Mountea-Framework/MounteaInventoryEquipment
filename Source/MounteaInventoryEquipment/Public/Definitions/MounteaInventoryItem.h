// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventoryTableTypes.h"
#include "MounteaItemAdditionalData.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Interfaces/MounteaInventoryEquipmentItem.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Setup/MounteaInventoryItemConfig.h"
#include "MounteaInventoryItem.generated.h"

struct FMounteaItemConfig;
class UMounteaItemAdditionalData;
class IMounteaInventoryPickupInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemGenericEvent, const FString&, Message);

UENUM(BlueprintType)
enum class EItemDataSource : uint8
{
	EIDS_SourceTable				UMETA(DisplayName="Source Table"),
	EIDS_ManualInput				UMETA(DisplayName="Manual Input"),

	Default								UMETA(hidden)
};

#define LOCTEXT_NAMESPACE "MounteaInventoryItem"

/**
 * Inventory Item Data Asset
 * 
 * Base class for inventory items,
 *
 * This class represents an item in the inventory system.
 * It serves as a data asset base class and provides functionality for holding item data.
 * Inventory items are used within an inventory and can be replicated.
 *
 * @see UDataAsset
 * @see IMounteaInventoryEquipmentItem
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Item-Object
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea", DisplayName="Inventory Item (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBase : public UDataAsset, public IMounteaInventoryEquipmentItem
{
	GENERATED_BODY()

	UMounteaInventoryItemBase();

private:
	
	virtual void PostInitProperties() override;

public:
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(ShowOnlyInnerProperties), ReplicatedUsing=OnRep_Item)
	EItemDataSource ItemDataSource = EItemDataSource::EIDS_SourceTable;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(DisplayThumbnail=false, EditCondition="ItemDataSource==EItemDataSource::EIDS_SourceTable"))
	UMounteaInventoryItemsTable* SourceTable;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(GetOptions="GetSourceTableRows", EditCondition="SourceTable!=nullptr&&ItemDataSource==EItemDataSource::EIDS_SourceTable"))
	FName SourceRow;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="2. Required", meta=(ShowOnlyInnerProperties, EditCondition="ItemDataSource!=EItemDataSource::EIDS_SourceTable"), ReplicatedUsing=OnRep_Item)
	FMounteaInventoryItemRequiredData ItemData;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="3. Optional", meta=(ShowOnlyInnerProperties, EditCondition="ItemDataSource!=EItemDataSource::EIDS_SourceTable"))
	FMounteaInventoryItemOptionalData ItemOptionalData;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "4. Config", NoClear, meta=(NoResetToDefault))
	FMounteaItemConfig ItemConfig;
	
	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|5. Debug")
	FItemGenericEvent OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|5. Debug")
	FItemGenericEvent OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|5. Debug")
	FItemGenericEvent OnItemInitialized;

	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|5. Debug")
	FItemGenericEvent OnItemModified;

private:

	UPROPERTY(SaveGame, VisibleAnywhere, Category="5. Debug")
	FGuid ItemGuid = FGuid::NewGuid();
	
	UPROPERTY(VisibleAnywhere, Category="5. Debug")
	int32 RepKey = 0;

    UPROPERTY(VisibleAnywhere, Category="5. Debug", meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaInventoryInterface> OwningInventory = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category="5. Debug", meta=(DisplayThumbnail=false))
	class UWorld* World;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryItemConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	UMounteaInventoryItemConfig* GetItemConfig(const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter, bool& bResult) const
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaItemAdditionalData"), meta=(DeterminesOutputType = "ClassFilter"))
	UMounteaItemAdditionalData* GetItemAdditionalData(const TSubclassOf<UMounteaItemAdditionalData> ClassFilter, bool& bResult) const
	{
		if (ClassFilter == nullptr)
		{
			bResult = false;
			return nullptr;
		}

		bResult = true;
		if (ItemOptionalData.ItemAdditionalData == nullptr)
		{
			return NewObject<UMounteaItemAdditionalData>(GetTransientPackage(), ClassFilter);
		}

		return ItemOptionalData.ItemAdditionalData->IsA(ClassFilter) ? ItemOptionalData.ItemAdditionalData : NewObject<UMounteaItemAdditionalData>(GetTransientPackage(), ClassFilter);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea Inventory & Equipment|Item", meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext ) )
	bool IsValid(UObject* WorldContextObject) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea Inventory & Equipment|Item")
	FGuid GetItemGuid() const
	{ return ItemGuid; };

	UFUNCTION(BlueprintCallable, Category="Mountea Inventory & Equipment|Item")
	void AddQuantity(const int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category="Mountea Inventory & Equipment|Item")
	void SetQuantity(const int32 NewQuantity);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea Inventory & Equipment|Item")
	int32 GetQuantity() const;
	
	virtual FItemGenericEvent& GetItemAddedHandle()
	{ return OnItemAdded; };
	virtual FItemGenericEvent& GetItemUpdatedHandle()
	{ return OnItemModified; };
	virtual FItemGenericEvent& GetItemInitializedHandle()
	{ return OnItemInitialized; };
	virtual FItemGenericEvent& GetItemRemovedHandle()
	{ return OnItemRemoved; };

	void NetFlush()
	{
		RepKey++;
	}

	int GetRepKey() const
	{ return RepKey; }

	FGameplayTag GetFirstTag() const
	{
		return ItemData.CompatibleGameplayTags.First();
	};
	FGameplayTagContainer GetTags() const
	{
		return ItemData.CompatibleGameplayTags;
	};

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea Inventory & Equipment|Item")
	TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const
	{ return OwningInventory; }
	
	UFUNCTION(BlueprintCallable, Category="Mountea Inventory & Equipment|Item")
	virtual void SetWorldFromLevel(ULevel* FromLevel);

	virtual void SetWorld(UWorld* NewWorld);
	
	UFUNCTION(BlueprintCallable, Category="Mountea Inventory & Equipment|Item")
	void InitializeNewItem(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory);

protected:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea Inventory & Equipment|Item")
	void OnItemBeginPlay(const FString& Message);

	UFUNCTION()
	virtual void OnRep_Item();
	
	UFUNCTION()
	void ItemAdded(const FString& Message);
	UFUNCTION()
	void ItemInitialized(const FString& Message);
	UFUNCTION()
	void ItemModified(const FString& Message);
	UFUNCTION()
	void ItemRemoved(const FString& Message);
	
	UFUNCTION()
	virtual TArray<FName> GetSourceTableRows() const
	{
		if (!SourceTable)
		{
			return TArray<FName>();
		}

		return SourceTable->GetRowNames();
	};

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	
	FORCEINLINE ULevel* GetLevel() const
	{
		return GetTypedOuter<ULevel>();
	}
	
	virtual UWorld* GetWorld() const override
	{
		if (World) return World;
		
		// CDO objects do not belong to a world
		// If the actors outer is destroyed or unreachable we are shutting down and the world should be nullptr
		if (
			!HasAnyFlags(RF_ClassDefaultObject) && ensureMsgf(GetOuter(), TEXT("Actor: %s has a null OuterPrivate in AActor::GetWorld()"), *GetFullName())
			&& !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable()
			)
		{
			if (ULevel* Level = GetLevel())
			{
				return Level->OwningWorld;
			}
		}
		return nullptr;
	}

	/**Mark the object as needing replication. We must call this internally after modifying any replicated properties*/
	void MarkDirtyForReplication();

	void ClearDataTable();
	void CopyFromTable();
	void ClearMappedValues();
	void CopyTagsFromCategory();

#if WITH_EDITOR
	
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif

public:
	
	UFUNCTION(CallInEditor, BlueprintCallable, Category="1. Import")
	virtual void SetValidData() override;
	
};

#undef LOCTEXT_NAMESPACE