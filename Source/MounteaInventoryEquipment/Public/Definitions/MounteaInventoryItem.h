// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Setup/MounteaInventoryItemConfig.h"
#include "MounteaInventoryItem.generated.h"

struct FMounteaItemConfig;
class IMounteaInventoryPickupInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemGenericEvent, const FString&, Message);

#define LOCTEXT_NAMESPACE "MounteaInventoryItem"

/**
 * Inventory Item Object.
 *
 * Abstract Base Class.
 * Serves sole purpose of being a base class for individual Items.
 *
 * Inventory contains a list of those Items.
 * 
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Inventory-Item-Object
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea", DisplayName="Inventory Item (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBase : public UObject
{
	GENERATED_BODY()

	UMounteaInventoryItemBase();

private:
	
	virtual void PostInitProperties() override;

public:

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="1. Required", meta=(ShowOnlyInnerProperties), ReplicatedUsing=OnRep_Item)
	FMounteaInventoryItemRequiredData ItemData;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="2. Optional", meta=(ShowOnlyInnerProperties))
	FMounteaInventoryItemOptionalData ItemOptionalData;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="2. Optional", meta=(MustImplement="MounteaInventoryPickupInterface"))
	TSubclassOf<AActor> SpawnActor;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="3. Import", meta=(DisplayThumbnail=false))
	UDataTable* SourceTable;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="3. Import", meta=(GetOptions="GetSourceTableRows", EditCondition="SourceTable!=nullptr"))
	FName SourceRow;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "2. Optional", NoClear, meta=(NoResetToDefault))
	FMounteaItemConfig ItemConfig;
	
	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|4. Debug")
	FItemGenericEvent OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|4. Debug")
	FItemGenericEvent OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|4. Debug")
	FItemGenericEvent OnItemInitialized;

	UPROPERTY(BlueprintAssignable, Category="Mountea Inventory & Equipment|Item|4. Debug")
	FItemGenericEvent OnItemModified;

private:

	UPROPERTY(SaveGame, VisibleAnywhere, Category="4. Debug")
	FGuid ItemGuid = FGuid::NewGuid();
	
	UPROPERTY(VisibleAnywhere, Category="4. Debug")
	int32 RepKey = 0;

    UPROPERTY(VisibleAnywhere, Category="4. Debug", meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaInventoryInterface> OwningInventory = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category="4. Debug", meta=(DisplayThumbnail=false))
	class UWorld* World;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryItemConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	UMounteaInventoryItemConfig* GetItemConfig(const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter) const
	{
		if (ClassFilter == nullptr) return nullptr;

		if (ItemConfig.ItemConfig == nullptr)
		{
			return NewObject<UMounteaInventoryItemConfig>(nullptr, ClassFilter);
		}

		return ItemConfig.ItemConfig->IsA(ClassFilter) ? ItemConfig.ItemConfig : NewObject<UMounteaInventoryItemConfig>(nullptr, ClassFilter);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea Inventory & Equipment|Item", meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext ) )
	bool IsValid(UObject* WorldContextObject) const;
	
	UFUNCTION(BlueprintGetter, Category="Mountea Inventory & Equipment|Item")
	FGuid GetItemGuid() const
	{ return ItemGuid; };

	void AddQuantity(const int32 Amount);
	void SetQuantity(const int32 NewQuantity);
	
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
	virtual void SetWorld(UWorld* NewWorld);
	
	UFUNCTION(BlueprintCallable, Category="Mountea Inventory & Equipment|Item")
	void InitializeNewItem(UWorld* NewWorld, TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory, const FMounteaInventoryItemRequiredData& NewItemData, const FMounteaInventoryItemOptionalData NewOptionalData);

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

#if WITH_EDITOR
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

#undef LOCTEXT_NAMESPACE