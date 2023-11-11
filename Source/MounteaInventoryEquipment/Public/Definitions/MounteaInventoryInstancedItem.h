// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MounteaInventoryItem.h"

#include "MounteaInventoryTableTypes.h"

#include "Interfaces/Item/MounteaInventoryInstancedItemInterface.h"
#include "Interfaces/MounteaInventoryEquipmentItem.h"

#include "Setup/MounteaInventoryItemConfig.h"

#include "Helpers/MounteaItemHelpers.h"
#include "MounteaInventoryInstancedItem.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInstancedItem"

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
UCLASS(BlueprintType, NotBlueprintable,  ClassGroup="Mountea", DisplayName="Inventory Item Instance")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInstancedItem : public UObject, public IMounteaInventoryEquipmentItem, public IMounteaInventoryInstancedItemInterface
{
	GENERATED_BODY()

public:

	UMounteaInstancedItem();
	virtual ~UMounteaInstancedItem() override;
	
/*===============================================================================
	Variables
	
	Contains the variables that hold the item's data, such as its source, quantity, flags, and configuration.
	These variables define the state and the properties of the inventory item instance.
===============================================================================*/
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

/*===============================================================================
	Events
	
	BlueprintAssignable events for responding to item-related actions, such as adding, removing, 
	initializing, or modifying the item. These events provide hooks for game logic and UI updates.
===============================================================================*/
#pragma region Events

public:
	
	virtual FItemGenericEvent& GetEventHandle_OnItemAdded() override
	{ return OnItemAdded; };

	virtual FItemGenericEvent& GetEventHandle_OnItemRemoved() override
	{ return OnItemRemoved; };

	virtual FItemGenericEvent& GetEventHandle_OnItemInitialized() override
	{ return OnItemInitialized; };

	virtual FItemGenericEvent& GetEventHandle_OnItemModified() override
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

/*===============================================================================
	Interface Functions
	
	Interface functions that provide access to item functionality and are implementing Interface functions.
	These include initialization, getter and setter methods, and other item manipulations.
===============================================================================*/
#pragma region InterfaceFunctions

#pragma region Public
	
public:
	
	virtual bool InitializeNewItem_Implementation(const FItemInitParams& InitParams) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual  TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const override
	{ return OwningInventory; };
	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	virtual void SetOwningInventory(TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory) override;
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual TSubclassOf<UMounteaInventoryItemConfig> GetItemConfigClass() const override
	{
		if (ItemConfig.ItemConfig)
		{
			return ItemConfig.ItemConfig->StaticClass();
		}
		
		return nullptr;
	}

	/** Returns instanced Item Config, which can include applied runes, specific properties, durability, etc. */
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
			return NewObject<UMounteaInventoryItemConfig>(GetPackage()->GetOuter(), ClassFilter);
		}

		return ItemConfig.ItemConfig->IsA(ClassFilter) ? ItemConfig.ItemConfig : NewObject<UMounteaInventoryItemConfig>(GetPackage()->GetOuter(), ClassFilter);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual EItemDataSource GetItemDataSource() const override
	{	return ItemDataSource; };
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void SetSourceItem(UMounteaInventoryItemBase* NewSourceItem);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Item")
	virtual UMounteaInventoryItemBase* GetSourceItem() const override
	{ return SourceItem; };
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	void SetSourceTable(UMounteaInventoryItemsTable* Table, const FString& RowName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Item")
	virtual UMounteaInventoryItemsTable*  GetSourceTable(FName& RowName) const override
	{
		RowName = SourceRow;
		return SourceTable;
	};
	
	bool ConstructItem();
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	virtual void EmptyItem() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual FMounteaInventoryItemData GetItemData() const override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual FGuid GetGuid() const override
	{ return InstanceID; };

	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	virtual void SetGuid(const FGuid& NewGuid) override
	{ InstanceID = NewGuid; };

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	virtual int32 GetQuantity() const override;

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	virtual void SetQuantity(const int32& NewValue) override;
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory")
	virtual int32 ModifyQuantity(const int32& ValueToAdd) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Item")
	virtual FGameplayTagContainer GetItemFlags() const override
	{ return ItemFlags; };
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	virtual void AddItemFlag(const FGameplayTag& NewFlag) override;

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	virtual void SetItemFlags(const FGameplayTagContainer& NewFlags) override;

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	virtual void RemoveItemFlag(const FGameplayTag& RemoveFlag) override;

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	virtual bool IsFlagSet(const FGameplayTag& QueryFlag) const override;

	UFUNCTION(BlueprintCallable, Category = "Mountea|Item")
	virtual bool AreFlagsSet(const FGameplayTagContainer& QueryFlags, const bool bSimpleSearch = true) const override;

	
	UFUNCTION(BlueprintCallable, Category="Mountea|Item")
	virtual void SetWorldFromLevel(ULevel* FromLevel);

	virtual void SetWorld(UWorld* NewWorld);
	
#pragma endregion

#pragma endregion

/*===============================================================================
	Class Functions
		
	Functions that are specific for the class are not inherited from any of its Interface parents.
	This section also includes the lifetime properties for replication.
===============================================================================*/
#pragma region ClassFunctions

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