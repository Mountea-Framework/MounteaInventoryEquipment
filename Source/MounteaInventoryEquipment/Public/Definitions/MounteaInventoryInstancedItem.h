// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaInventoryItem.h"

#include "MounteaInventoryTableTypes.h"

#include "Interfaces/MounteaInventoryEquipmentItem.h"
#include "Setup/MounteaInventoryItemConfig.h"

#include "Helpers/MounteaItemHelpers.h"

#include "MounteaInventoryInstancedItem.generated.h"


#define LOCTEXT_NAMESPACE "MounteaInstancedItem"

USTRUCT(BlueprintType)
struct FItemInitParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IMounteaInventoryInterface> OwningInventory;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMounteaInventoryItemBase* SourceItem = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMounteaInventoryItemsTable* SourceTable = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName SourceRow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Quantity;
};

/**
 * Inventory Item Instance
 * 
 * Base class for inventory items.
 *
 * This class represents an item in the inventory system.
 * It serves as representation of the item, it knows its source base item/source table where defaults are stored.
 * This instance can be updated in runtime, quantity can be changed, flags can be adjusted etc.
 * Inventory items are used within an inventory and can be replicated.
 *
 * @see UObject
 * @see IMounteaInventoryEquipmentItem
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Instanced-Inventory-Item
 */
UCLASS(BlueprintType, Blueprintable,  ClassGroup="Mountea", DisplayName="Instanced Inventory Item")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInstancedItem : public UObject, public IMounteaInventoryEquipmentItem
{
	GENERATED_BODY()

public:

	UMounteaInstancedItem();
	virtual ~UMounteaInstancedItem() override;

#pragma region Variables
	
public:

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(ShowOnlyInnerProperties, EditFixedOrder), ReplicatedUsing=OnRep_Item)
	EItemDataSource ItemDataSource = EItemDataSource::EIDS_SourceTable;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(DisplayThumbnail=false, EditCondition="ItemDataSource!=EItemDataSource::EIDS_SourceTable", EditFixedOrder))
	UMounteaInventoryItemBase* SourceItem = nullptr;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(DisplayThumbnail=false, EditCondition="ItemDataSource==EItemDataSource::EIDS_SourceTable", EditFixedOrder))
	UMounteaInventoryItemsTable* SourceTable = nullptr;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Import", meta=(GetOptions="GetSourceTableRows", EditCondition="SourceTable!=nullptr&&ItemDataSource==EItemDataSource::EIDS_SourceTable", EditFixedOrder))
	FName SourceRow;

protected:

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Mountea|Item")
	FGuid InstanceID = FGuid::NewGuid();

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Mountea|Item", ReplicatedUsing=OnRep_Quantity)
	int32 Quantity = 0;

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Mountea|Item")
	FGameplayTagContainer ItemFlags;

	UPROPERTY(SaveGame, Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "4. Config", NoClear, meta=(NoResetToDefault))
	FMounteaItemConfig ItemConfig;

private:

	UPROPERTY(VisibleAnywhere, Category="5. Debug")
	int32 RepKey = 0;

	UPROPERTY(SaveGame, VisibleAnywhere, Category="5. Debug", meta=(DisplayThumbnail=false), ReplicatedUsing=OnRep_Item)
	TScriptInterface<IMounteaInventoryInterface> OwningInventory = nullptr;
	
	UPROPERTY(SaveGame, VisibleAnywhere, Category="5. Debug", meta=(DisplayThumbnail=false), ReplicatedUsing=OnRep_Item)
	class UWorld* World = nullptr;

#pragma endregion

#pragma region Events

public:
	
	FItemGenericEvent& GetEventHandle_OnItemAdded()
	{ return OnItemAdded; };
	FItemGenericEvent& GetEventHandle_OnItemRemoved()
	{ return OnItemRemoved; };
	FItemGenericEvent& GetEventHandle_OnItemInitialized()
	{ return OnItemInitialized; };
	FItemGenericEvent& GetEventHandle_OnItemModified()
	{ return OnItemModified; };

protected:

	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|5. Debug")
	FItemGenericEvent OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|5. Debug")
	FItemGenericEvent OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|5. Debug")
	FItemGenericEvent OnItemInitialized;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|5. Debug")
	FItemGenericEvent OnItemModified;
	
#pragma endregion 

#pragma region Functions

#pragma region Public
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Item")
	void OnItemBeginPlay(const FString& Message);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mountea|Item")
	bool InitializeNewItem(const FItemInitParams& InitParams);
	virtual bool InitializeNewItem_Implementation(const FItemInitParams& InitParams);
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void SetSourceItem(UMounteaInventoryItemBase* NewSourceItem);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void SetSourceTable(UMounteaInventoryItemsTable* Table, const FString& RowName);
	
	bool ConstructItem();
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void UpdateQuantity(const int32& NewValue);
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void DestroyItem();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Item")
	FGameplayTagContainer GetItemFlags() const
	{ return ItemFlags; };

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void AddItemFlag(const FGameplayTag& NewFlag);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void SetItemFlags(const FGameplayTagContainer& NewFlags);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void RemoveItemFlag(const FGameplayTag& RemoveFlag);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	bool IsFlagSet(const FGameplayTag& QueryFlag) const;

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	bool AreFlagsSet(const FGameplayTagContainer& QueryFlags, const bool bSimpleSearch = true) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	FORCEINLINE TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const
	{ return OwningInventory; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	TSubclassOf<UMounteaInventoryItemConfig> GetItemConfigClass() const
	{
		if (ItemConfig.ItemConfig)
		{
			return ItemConfig.ItemConfig->StaticClass();
		}
		
		return nullptr;
	}

	/** Returns instanced Item Config, which can include applied runes, specific properties, durability, etc. */
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

	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	void SetOwningInventory(TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	virtual void SetWorldFromLevel(ULevel* FromLevel);

	virtual void SetWorld(UWorld* NewWorld);
	
#pragma endregion

#pragma region Protected
	
protected:
	
	virtual void PostInitProperties() override;

	bool CopyFromBaseItem();
	bool CopyFromDataTable();
	void CleanupData();

	bool OwnerHasAuthority() const;

	/**Mark the object as needing replication. We must call this internally after modifying any replicated properties*/
	void MarkDirtyForReplication();
	
	void NetFlush()
	{
		RepKey++;
	}

	int GetRepKey() const
	{ return RepKey; }

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

#pragma endregion

#pragma region Private
	
private:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override
	{ return true; };
	
	virtual void SetValidData() override;

	UFUNCTION()
	virtual void OnRep_Item();

	UFUNCTION()
	virtual void OnRep_Quantity();

#pragma endregion

#pragma endregion 
};

#undef LOCTEXT_NAMESPACE