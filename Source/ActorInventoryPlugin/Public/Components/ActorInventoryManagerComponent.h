// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Definitions/InventoryCategory.h"
#include "Helpers/InventoryHelpers.h"

#include "ActorInventoryManagerComponent.generated.h"

class UInventoryCategory;
class UInventoryItemRarity;
class UInventoryItem;
class UInventoryNotification;
class UInventoryNotificationContainer;
class UInventoryWidget;
class UInventoryItemSlot;
class UInventoryItemSlotTooltip;
class UInventoryItemSlotDrag;
class UInventoryCategoryWidget;
class UInventoryItemSlotUse;
class UInventoryCategoryTooltip;

/**
 * Implement a GameState component for Inventory Manager.
 *
 * An Inventory Manager Component is a non-transient component that contains Inventory settings and definitions.
 * Inventory Manager is responsible for valid data within the game. Inventory Manager is required for Inventory Component to work.
 *
 * Networking is not implemented.
 * 
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Inventory-Manager
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, HideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Manager", ShortTooltip="Inventory Manager responsible for Adding and Removing Items from Inventory."))
class ACTORINVENTORYPLUGIN_API UActorInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UActorInventoryManagerComponent();

protected:

	virtual void BeginPlay() override;

public:

#pragma region Items
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void RemoveItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void RemoveItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SubtractItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer, const int32 Amount) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SubtractItemsToInventory(TMap<UInventoryItem*,int32> Items, APlayerController* OwningPlayer) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	TArray<UInventoryItem*> GetItemsFromInventory(APlayerController* OwningPlayer) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management")
	FORCEINLINE TSet<UInventoryCategory*> GetInventoryCategories() const
	{
		return AllowedCategories;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management")
	FORCEINLINE TSet<UInventoryItemRarity*> GetInventoryRarities() const
	{
		return AllowedRarities;
	}

#pragma endregion

#pragma region Categories

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management|Categories")
	TArray<UInventoryCategory*> GetAllowedCategories() const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void ClearAllowedCategories();

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void SetAllowedCategories(TSet<UInventoryCategory*>& Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void AddAllowedCategory(UInventoryCategory* Category);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void AddAllowedCategories(TSet<UInventoryCategory*>& Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void RemoveAllowedCategory(UInventoryCategory* Category);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void RemoveAllowedCategories(TSet<UInventoryCategory*>& Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	FORCEINLINE bool ContainsAllowedCategory(const UInventoryCategory* Category) const
	{
		return AllowedCategories.Contains(Category);
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	FORCEINLINE bool ContainsAllowedCategoryGuid(const FGuid& CategoryGUID) const
	{
		for (const auto Itr : AllowedCategories)
		{
			if (Itr && Itr->GetCategoryGUID() == CategoryGUID)
			{
				return true;
			}
		}

		return false;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	FORCEINLINE UInventoryCategory* GetCategoryByGUID(const FGuid& CategoryGUID)
	{
		for (const auto Itr : AllowedCategories)
		{
			if (Itr && Itr->GetCategoryGUID() == CategoryGUID)
			{
				return Itr;
			}
		}

		return nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management|Categories")
	FORCEINLINE bool IsGeneralCategoryAllowed() const
	{
		return bAllowGeneralCategory;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management|Categories")
	bool ValidateCategory(const FGuid& CategoryGUID) const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	FORCEINLINE UInventoryCategory* GetDefaultCategory() const
	{
		if (DefaultCategory)
		{
			return DefaultCategory;
		}
		else
		{
			if (AllowedCategories.Num() > 0)
			{
				return AllowedCategories.Array()[0];
			}
		}

		return nullptr;
	}

#pragma endregion

#pragma region Rarities

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management|Rarity")
	TArray<UInventoryItemRarity*> GetAllowedRarities() const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void ClearAllowedRarities();

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void SetAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void AddAllowedRarity(UInventoryItemRarity* Rarity);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void AddAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void RemoveAllowedRarity(const UInventoryItemRarity* Rarity);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void RemoveAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	FORCEINLINE bool ContainsAllowedRarity(const UInventoryItemRarity* Rarity) const
	{
		return AllowedRarities.Contains(Rarity);
	}

#pragma endregion 

#pragma region Notifications
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Notifications")
	FORCEINLINE TMap<EInventoryContext, FInventoryNotificationInfo> GetNotificationsInfo () const
	{
		return NotificationInfo;
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Notifications")
	FInventoryNotificationInfo GetNotificationInfo(const EInventoryContext Context)
	{
		if (NotificationInfo.Contains(Context))
		{
			return NotificationInfo[Context];
		}

		return FInventoryNotificationInfo();
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|Notifications")
	FORCEINLINE UTexture2D* GetNotificationIcon(const EInventoryContext Context)
	{
		if (NotificationInfo.Contains(Context))
		{
			return NotificationInfo[Context].NotificationTexture;
		}

		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|Notifications")
	FORCEINLINE FText GetNotificationText(const EInventoryContext Context)
	{
		if (NotificationInfo.Contains(Context))
		{
			return NotificationInfo[Context].NotificationText;
		}

		return FText::FromString("Default notification message!");
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Notifications")
	FORCEINLINE TSubclassOf<UInventoryNotificationContainer> GetNotificationContainerClass () const
	{
		return NotificationContainerClass;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Notifications")
	FORCEINLINE TSubclassOf<UInventoryNotification> GetNotificationClass () const
	{
		return NotificationClass;
	}

#pragma endregion 

#pragma region Inventory
	
	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryWidgetClass(TSubclassOf<UInventoryWidget> NewInventoryWidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryWidget> GetInventoryWidgetClass() const
	{
		return InventoryWidgetClass;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryCategoryWidgetClass(const TSubclassOf<UInventoryCategoryWidget> NewInventoryCategoryWidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryCategoryWidget> GetInventoryCategoryWidgetClass() const
	{
		return InventoryCategoryClass;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryCategoryTooltip> GetInventoryCategoryTooltipWidgetClass() const
	{
		return InventoryCategoryTooltipClass;
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryItemSlotWidgetClass(const TSubclassOf<UInventoryItemSlot> NewInventoryItemSlotClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryItemSlot> GetInventoryItemSlotWidgetClass() const
	{
		return InventorySlotClass;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryItemTooltipWidgetClass(const TSubclassOf<UInventoryItemSlotTooltip> NewInventoryItemSlotTooltipClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryItemSlotTooltip> GetInventoryItemSlotTooltipWidgetClass() const
	{
		return InventorySlotTooltipClass;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryItemDragDropWidgetClass(const TSubclassOf<UInventoryItemSlotDrag> NewInventoryItemSlotDragDropClass);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryItemSlotDrag> GetInventoryItemSlotDragWidgetClass() const
	{
		return InventorySlotDragClass;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryItemSlotUseClass(const TSubclassOf<UInventoryItemSlotUse>& NewInventoryItemSlotUseClass);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryItemSlotUse> GetInventoryItemSlotUseWidgetClass() const
	{
		return InventorySlotUseClass;
	}

#pragma endregion

#pragma region Settings

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Settings")
	FORCEINLINE bool IsWeightLimitAllowed() const
	{
		return bAllowWeightLimit;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|Settings")
	FORCEINLINE float GetInventoryWeightLimit() const
	{
		return WeightLimit;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|Settings")
	void SetInventoryWeightLimit(const float NewWeightLimit)
	{
		WeightLimit = NewWeightLimit;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|Settings")
	void UpdateWeightLimit(const float UpdateValue)
	{
		const float TempWeightValue = WeightLimit + UpdateValue;
		WeightLimit = FMath::Max(0.f, TempWeightValue);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Settings")
	FORCEINLINE bool IsDragDropAllowed() const
	{
		return bAllowDragDrop;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Settings")
	FORCEINLINE bool IsSplittingAllowed() const
	{
		return bAllowPackSplit;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Validation")
	bool IsValidCategory(const FGuid& CategoryGUID) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Settings")
	FORCEINLINE FInventoryLayout GetInventoryLayout() const {return DefaultLayout; };

#pragma endregion 

#pragma region Transactions

	/**
	 * Calculates Transaction Type for two given Item Slots.
	 * Native C++ function returns null!
	 * 
	 * @param SourceItem Source Slot which was dragged and dropped
	 * @param TargetItem Target Slot to which you want to drop the Source
	 * @return Inventory Transaction if found, nullptr otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Inventory")
	TSubclassOf<UInventoryTransaction> CalculateTransactionType(UInventoryItemSlot* SourceItem, UInventoryItemSlot* TargetItem) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	UObject* ProcessTransaction(TSubclassOf<UInventoryTransaction> TransactionClass, UObject* TransactionPayload);

#pragma endregion 
	
protected:

#pragma region GeneralSettings

	/**
	 * If allowed, Inventory is limited by Weight
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings")
	uint8 bAllowWeightLimit : 1;

	/**
	 * Maximum Weight Limit allowed by Inventory.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings", meta=(EditCondition="bAllowWeightLimit"))
	float WeightLimit = 50.f;
	
	/**
	 * If allowed, Item Slots are allowed to be Grabbed and Dropped.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings")
	uint8 bAllowDragDrop : 1;

	/**
	 * If allowed, Item Pack can be split into multiple Packs.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings")
	uint8 bAllowPackSplit : 1;

	/**
	 * If Allowed, a new "empty" category shall be created in UI.
	 * If that category is selected, Category filter is reset and all Items will be displayed.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings")
	uint8 bAllowGeneralCategory : 1;

	/**
	 * If Category is added to Allowed Categories, should we allow its parent Categories as well?
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Categories")
	uint8 bAutoAllowParentCategories : 1;

	/**
	 * Defines how deep the search for Parent category iterates. Higher the value, higher the performance impact and more precise result.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Categories", meta=(EditCondition="bAutoAllowParentCategories", UIMin=1, ClampMin=1))
	int32 MaxRecursionDepth =  6;

	/**
	 * Default Category which will be set when opening Inventory.
	 * If left empty, first from Allowed Categories will be used instead.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Categories", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	UInventoryCategory* DefaultCategory = nullptr;
	
	/**
	 * List of allowed Categories to be displayed in the Inventory.
	 * Only selected Categories will be displayed.
	 * Only valid Categories will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Categories", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSet<UInventoryCategory*> AllowedCategories;

	/**
	 * List of allowed Rarities to be displayed in the Inventory.
	 * Only selected Rarities will be displayed.
	 * Only valid Rarities will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Rarities", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSet<UInventoryItemRarity*> AllowedRarities;
	
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Notifications", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TMap<EInventoryContext, FInventoryNotificationInfo> NotificationInfo;

	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Notifications", NoClear, meta=(NoResetToDefault))
	FInventoryLayout DefaultLayout;
	
#pragma endregion

#pragma region Subclasses

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|General", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Category", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryCategoryWidget> InventoryCategoryClass;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Category", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryCategoryTooltip> InventoryCategoryTooltipClass;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryItemSlotTooltip> InventorySlotTooltipClass;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryItemSlotDrag> InventorySlotDragClass;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryItemSlotUse> InventorySlotUseClass;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Notification", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryNotificationContainer> NotificationContainerClass;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Notification", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryNotification> NotificationClass;

#pragma endregion 
	
private:

	bool ValidateCategories(const bool bShouldCheckForFlags = true);
	
	void UpdateCategories();
	void AddParentCategory(UInventoryCategory* Category, int32& DepthIndex);
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
};