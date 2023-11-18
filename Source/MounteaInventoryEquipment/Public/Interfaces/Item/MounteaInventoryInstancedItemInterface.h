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
struct FItemInitParams;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemGenericEvent, const FString&, Message);

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

public:

	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Item")
	void OnItemBeginPlay(const FString& Message);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mountea|Item")
	bool InitializeNewItem(const FItemInitParams& InitParams);
	virtual bool InitializeNewItem_Implementation(const FItemInitParams& InitParams) = 0;
	
public:

	virtual void SetOwningInventory(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory) = 0;
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

	virtual UMounteaInventoryItemBase* GetSourceItem() const = 0;
	virtual UMounteaInventoryItemsTable*  GetSourceTable(FName& RowName) const = 0;

	virtual void NetFlush() = 0;
	
public:

	virtual FItemGenericEvent& GetEventHandle_OnItemAdded() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemRemoved() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemInitialized() = 0;
	virtual FItemGenericEvent& GetEventHandle_OnItemModified() = 0;
};
