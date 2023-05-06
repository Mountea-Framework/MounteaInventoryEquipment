// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Interfaces/MounteaInventoryInterface.h"
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

public:

	UPROPERTY(EditAnywhere, Category="1. Required", meta=(ShowOnlyInnerProperties), ReplicatedUsing=OnRep_Item)
	FMounteaInventoryItemRequiredData ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="2. Optional", meta=(ShowOnlyInnerProperties))
	FMounteaInventoryItemOptionalData ItemOptionalData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="2. Optional", meta=(MustImplement="MounteaInventoryPickupInterface"))
	TSubclassOf<AActor> SpawnActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="3. Import", meta=(DisplayThumbnail=false))
	UDataTable* SourceTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="3. Import", meta=(GetOptions="GetSourceTableRows", EditCondition="SourceTable!=nullptr"))
	FName SourceRow;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "2. Optional", NoClear, meta=(NoResetToDefault, TitleProperty="DisplayName"))
	TArray<FMounteaItemConfig> ItemConfigs;

	UPROPERTY(BlueprintAssignable, Category="4. Debug")
	FItemGenericEvent OnItemInitialized;

	UPROPERTY(BlueprintAssignable, Category="4. Debug")
	FItemGenericEvent OnItemModified;

private:
	
	UPROPERTY(VisibleAnywhere, Category="4. Debug")
	int32 RepKey = 0;

    UPROPERTY(VisibleAnywhere, Category="4. Debug", meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaInventoryInterface> OwningInventory = nullptr;
	
	UPROPERTY(Transient)
	class UWorld* World;

public:

	int GetRepKey() const
	{ return RepKey; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const
	{ return OwningInventory; }
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory")
	void Initialize(UWorld* NewWorld, TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory, const FMounteaInventoryItemRequiredData& NewItemData, const FMounteaInventoryItemOptionalData NewOptionalData);

protected:

	UFUNCTION()
	virtual void OnRep_Item();
	
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
	virtual class UWorld* GetWorld() const override;

	/**Mark the object as needing replication. We must call this internally after modifying any replicated properties*/
	void MarkDirtyForReplication();

#if WITH_EDITOR
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

#undef LOCTEXT_NAMESPACE