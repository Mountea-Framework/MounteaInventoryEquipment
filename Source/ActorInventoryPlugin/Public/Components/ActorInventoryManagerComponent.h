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

/**
 * 
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, HideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Manager", ShortTooltip="Inventory Manager responsible for Adding and Removing Items from Inventory."))
class ACTORINVENTORYPLUGIN_API UActorInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UActorInventoryManagerComponent();

public:

#pragma region Items
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer) const;

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management|Categories")
	FORCEINLINE bool IsGeneralCategoryAllowed() const
	{
		return bAllowGeneralCategory;
	}

#pragma endregion

#pragma region Rarities
	
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

#pragma endregion

#pragma region Settings

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

#pragma endregion 

protected:

#pragma region GeneralSettings
	
	/**
	 * If allowed, Item Slots are allowed to be Grabbed and Dropped.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings")
	uint8 bAllowDragDrop : 1;

	/**
	 * If allowed, Item Pack can be split into multiple Packs.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings")
	uint8 bAllowPackSplit : 1;

	/**
	 * If Allowed, a new "empty" category shall be created in UI.
	 * If that category is selected, Category filter is reset and all Items will be displayed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Settings")
	uint8 bAllowGeneralCategory : 1;

	/**
	 * If Category is added to Allowed Categories, should we allow its parent Categories as well?
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Categories")
	uint8 bAutoAllowParentCategories : 1;

	/**
	 * Defines how deep the search for Parent category iterates. Higher the value, higher the performance impact and more precise result.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Categories", meta=(EditCondition="bAutoAllowParentCategories", UIMin=1, ClampMin=1))
	int32 MaxRecursionDepth =  6;
	
	/**
	 * List of allowed Categories to be displayed in the Inventory.
	 * Only selected Categories will be displayed.
	 * Only valid Categories will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory|Categories", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly))
	TSet<UInventoryCategory*> AllowedCategories;

	/**
	 * List of allowed Rarities to be displayed in the Inventory.
	 * Only selected Rarities will be displayed.
	 * Only valid Rarities will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory|Rarities", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly))
	TSet<UInventoryItemRarity*> AllowedRarities;

	UPROPERTY(EditDefaultsOnly, Category="Inventory|Notifications", NoClear, meta=(NoResetToDefault))
	TMap<EInventoryContext, FInventoryNotificationInfo> NotificationInfo;
	
#pragma endregion

#pragma region Subclasses

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|General", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly=true, AllowAbstract=false))
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly=true, AllowAbstract=false))
	TSubclassOf<UInventoryCategoryWidget> InventoryCategoryClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly=true, AllowAbstract=false))
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly=true, AllowAbstract=false))
	TSubclassOf<UInventoryItemSlotTooltip> InventorySlotTooltipClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Item", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly=true, AllowAbstract=false))
	TSubclassOf<UInventoryItemSlotDrag> InventorySlotDragClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Notification", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly=true, AllowAbstract=false))
	TSubclassOf<UInventoryNotificationContainer> NotificationContainerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI|Notification", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly=true, AllowAbstract=false))
	TSubclassOf<UInventoryNotification> NotificationClass;

#pragma endregion 
	
private:

	void ValidateCategories();
	void UpdateCategories();
	void AddParentCategory(UInventoryCategory* Category, int32& DepthIndex);
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
};