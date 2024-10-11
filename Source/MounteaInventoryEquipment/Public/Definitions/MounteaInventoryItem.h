// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaInventoryItem.generated.h"

class IMounteaInventoryInterface;
class UMounteaInventoryItemBlueprint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemGenericEvent, const FString&, Message);


#define LOCTEXT_NAMESPACE "MounteaInventoryItem"

/**
 * Mountea Inventory Item.
 *
 * Represents an item in the Mountea Inventory system.
 * This class serves as the base for inventory items and provides functionality for holding item data.
 * Inventory items are used within an inventory and can be replicated.
 *
 * Implements event delegates for item lifecycle events such as added, removed, initialized, and modified.
 *
 * @see UObject
 * @see UMounteaInventoryItemBlueprint
 * @see FMounteaInventoryItemInstancedData
 * @see IMounteaInventoryInterface
 */
UCLASS(BlueprintType, NotBlueprintable, ClassGroup="Mountea", DisplayName="Inventory Item")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItem : public UObject
{
	GENERATED_BODY()
		
public:

	UMounteaInventoryItem();
	virtual ~UMounteaInventoryItem() override;

protected:

	/** The source blueprint from which this item was created. */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Read Only")
	TObjectPtr<UMounteaInventoryItemBlueprint> SourceBlueprint;

	/** Instanced data specific to this item instance. */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Read Only")
	FMounteaInventoryItemInstancedData InstancedData;

	/** Unique identifier for this item instance. */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Read Only")
	FGuid ItemGuid;

private:

	/** The inventory that owns this item. */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Read Only", meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaInventoryInterface> OwningInventory = nullptr;
		
	/** The world context in which this item exists. */
	UPROPERTY(SaveGame, VisibleAnywhere, Category="Read Only", meta=(DisplayThumbnail=false))
	UWorld* World;

protected:

	/** Event triggered when the item is added to an inventory. */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|Events")
	FItemGenericEvent OnItemAdded;

	/** Event triggered when the item is removed from an inventory. */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|Events")
	FItemGenericEvent OnItemRemoved;

	/** Event triggered when the item is initialized. */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|Events")
	FItemGenericEvent OnItemInitialized;

	/** Event triggered when the item is modified. */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Item|Events")
	FItemGenericEvent OnItemModified;

public:

	/**
	 * Gets the source blueprint from which this item was created.
	 *
	 * @return The source UMounteaInventoryItemBlueprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE UMounteaInventoryItemBlueprint* GetSourceBlueprint() const { return SourceBlueprint; }

	/**
	 * Sets the source blueprint for this item.
	 *
	 * @param NewSourceBlueprint The new source blueprint.
	 */
	FORCEINLINE void SetSourceBlueprint(UMounteaInventoryItemBlueprint* NewSourceBlueprint) { SourceBlueprint = NewSourceBlueprint; }

	/**
	 * Gets the instanced data specific to this item instance.
	 *
	 * @return The FMounteaInventoryItemInstancedData.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FMounteaInventoryItemInstancedData GetItemInstancedData() const { return InstancedData; }

	/**
	 * Sets the instanced data for this item instance.
	 *
	 * @param NewInstancedData The new instanced data.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetItemInstancedData(const FMounteaInventoryItemInstancedData& NewInstancedData) { InstancedData = NewInstancedData; }

	/**
	 * Gets the unique identifier for this item instance.
	 *
	 * @return The FGuid representing the item's unique ID.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FGuid GetItemGuid() const { return ItemGuid; }
	
	FORCEINLINE void SetItemGuid(const FGuid& NewGuid) { ItemGuid = NewGuid; }

	/**
	 * Gets the owning inventory of this item.
	 *
	 * @return The owning inventory interface.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const { return OwningInventory; }

	/**
	 * Sets the owning inventory of this item.
	 *
	 * @param NewOwningInventory The new owning inventory interface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetOwningInventory(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory) { OwningInventory = NewOwningInventory; }

	/**
	 * Gets the world context in which this item exists.
	 *
	 * @return The UWorld instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE UWorld* GetItemWorld() const { return World; }

	/**
	 * Sets the world context for this item.
	 *
	 * @param NewWorld The new UWorld instance.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Item", meta=(CustomTag="MounteaK2Setter"))
	virtual void SetWorld(UWorld* NewWorld);

	/**
	 * Sets the world context for this item from a given level.
	 *
	 * @param FromLevel The level from which to set the world context.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Item")
	virtual void SetWorldFromLevel(ULevel* FromLevel);

public:

	virtual FItemGenericEvent& GetItemAddedHandle() { return OnItemAdded; }
	virtual FItemGenericEvent& GetItemUpdatedHandle() { return OnItemModified; }
	virtual FItemGenericEvent& GetItemInitializedHandle() { return OnItemInitialized; }
	virtual FItemGenericEvent& GetItemRemovedHandle() { return OnItemRemoved; }

protected:

	/**
	 * Called when the item begins play.
	 *
	 * @param Message A message providing context.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Item")
	void OnItemBeginPlay(const FString& Message);

	/**
	 * Internal handler for when the item is added.
	 *
	 * @param Message A message providing context.
	 */
	UFUNCTION()
	void ItemAdded(const FString& Message);

	/**
	 * Internal handler for when the item is initialized.
	 *
	 * @param Message A message providing context.
	 */
	UFUNCTION()
	void ItemInitialized(const FString& Message);

	/**
	 * Internal handler for when the item is modified.
	 *
	 * @param Message A message providing context.
	 */
	UFUNCTION()
	void ItemModified(const FString& Message);

	/**
	 * Internal handler for when the item is removed.
	 *
	 * @param Message A message providing context.
	 */
	UFUNCTION()
	void ItemRemoved(const FString& Message);

	void PostWorldCreated(UWorld* NewWorld);

#if WITH_EDITOR

	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

public:

	FORCEINLINE ULevel* GetLevel() const
	{
		return GetTypedOuter<ULevel>();
	}
	
	virtual UWorld* GetWorld() const override
	{
		if (World) return World;
			
		// CDO objects do not belong to a world
		// If the actor's outer is destroyed or unreachable we are shutting down and the world should be nullptr
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
};

#undef LOCTEXT_NAMESPACE