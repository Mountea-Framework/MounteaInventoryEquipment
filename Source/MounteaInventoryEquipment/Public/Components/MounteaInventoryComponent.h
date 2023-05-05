// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "MounteaInventoryComponent.generated.h"

class UMounteaInventoryItem_Base;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Mountea Inventory"))
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryComponent : public UActorComponent, public IMounteaInventoryInterface
{
	GENERATED_BODY()

public:	

	UMounteaInventoryComponent();

protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:

	virtual bool LoadInventoryFromDataTable_Implementation(const UDataTable* SourceTable) override;
	virtual void SaveInventory_Implementation() override;

	virtual UMounteaInventoryItem_Base* GetItem_Implementation(const FGuid& ItemGUID) override;
	virtual TArray<UMounteaInventoryItem_Base*> GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) override;
	
	virtual bool AddItem_Implementation(UMounteaInventoryItem_Base* NewItem) override;
	virtual bool AddItems_Implementation(TArray<UMounteaInventoryItem_Base*>& NewItems) override;

private:

	UFUNCTION()
	void OnRep_Items();

protected:

	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="1. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	TArray<UMounteaInventoryItem_Base*> Items;

private:

	UPROPERTY()
	int32 ReplicatedItemsKey;
};
