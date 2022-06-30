// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Definitions/InventoryCategory.h"
#include "Helpers/InventoryHelpers.h"
#include "Helpers/ActorInventoryPluginLog.h"

#include "ActorInventoryManagerComponent.generated.h"

class UInventoryCategory;
class UInventoryItemRarity;
class UInventoryItem;
class UInventoryNotification;
class UInventoryNotificationContainer;
class UInventoryWidget;

UCLASS(ClassGroup=(Inventory), Blueprintable, HideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Manager", ShortTooltip="Inventory Manager responsible for Adding and Removing Items from Inventory."))
class ACTORINVENTORYPLUGIN_API UActorInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UActorInventoryManagerComponent();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer);

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
	
	UFUNCTION(BlueprintCallable, Category="Inventory", meta=(DeprecatedFunction, DeprecatedMessage="Use this function with caution as this might be really performance heavy."))
	void UpdateCategories();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Notifications")
	FORCEINLINE TMap<EInventoryContext, FNotificationInfo> GetNotificationsInfo () const
	{
		return NotificationInfo;
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Notifications")
	FNotificationInfo GetNotificationInfo(const EInventoryContext Context)
	{
		if (NotificationInfo.Contains(Context))
		{
			return NotificationInfo[Context];
		}

		return FNotificationInfo();
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

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryWidgetClass(TSubclassOf<UInventoryWidget> NewInventoryWidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE TSubclassOf<UInventoryWidget> GetInventoryWidgetClass() const
	{
		return InventoryWidgetClass;
	}
	
protected:
	
	virtual void BeginPlay() override;

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
	TMap<EInventoryContext, FNotificationInfo> NotificationInfo;

	UPROPERTY(EditDefaultsOnly, Category="Inventory|Notifications", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly))
	TSubclassOf<UInventoryNotificationContainer> NotificationContainerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI", meta=(BlueprintBaseOnly=true))
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI", meta=(BlueprintBaseOnly=true))
	TSubclassOf<UInventoryNotificationContainer> InventoryNotificationContainerClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory|Notifications", NoClear, meta=(NoResetToDefault, BlueprintBaseOnly))
	TSubclassOf<UInventoryNotification> NotificationClass;

	UPROPERTY(EditDefaultsOnly, Category="Inventory|Quantity", meta=(NoResetToDefault))
	int32 DefaultMaxStacking = 99;
	
private:
	
	void AddParentCategory(UInventoryCategory* Category, int32& DepthIndex);
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
};

