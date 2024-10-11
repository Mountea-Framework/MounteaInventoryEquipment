// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaItemAction.generated.h"

struct FMounteaInventoryCommandContext;
struct FGameplayTag;
class UMounteaInventoryItem;

#define LOCTEXT_NAMESPACE "ItemAction"

/**
 * Mountea Inventory Item Action (Base).
 *
 * Abstract base class for inventory item actions within the Mountea Inventory system.
 * Item actions define custom behaviors that can be performed on inventory items.
 * This class provides the foundation for creating new item actions, including initialization, display logic, and processing.
 *
 * @see UObject
 * @see UMounteaInventoryItem
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Required,Opional,ReadOnly"), DisplayName="Item Action (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemAction : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * Initializes the item action with the specified item and context.
	 *
	 * @param ItemInFocus The inventory item that this action is associated with.
	 * @param Context The dynamic delegate context for this action.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory&Equipment|ItemAction")
	void InitializeAction(UMounteaInventoryItem* ItemInFocus, FMounteaInventoryCommandContext Context);
	void InitializeAction_Implementation(UMounteaInventoryItem* ItemInFocus, FMounteaInventoryCommandContext Context);
	
	/**
	 * Determines whether this action should be displayed for the specified item.
	 *
	 * @param ItemInFocus The inventory item to check for action display.
	 * @return True if the action should be displayed; false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory&Equipment|ItemAction", DisplayName="Can Display Action")
	bool DisplayAction(UMounteaInventoryItem* ItemInFocus) const;
	bool DisplayAction_Implementation(UMounteaInventoryItem* ItemInFocus) const;
	
	/**
	 * Processes the action for the specified item.
	 *
	 * @param ItemInFocus The inventory item on which to process the action.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Inventory&Equipment|ItemAction")
	void ProcessAction(UMounteaInventoryItem* ItemInFocus);
	
public:

	/**
	 * Gets the unique identifier of the action.
	 *
	 * @return The FGuid representing the action's unique ID.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE FGuid GetActionGuid() const { return ActionGuid; }

	/**
	 * Sets the unique identifier of the action.
	 *
	 * @param NewGuid The new FGuid to assign to the action.
	 */
	FORCEINLINE void SetActionGuid(const FGuid& NewGuid) { ActionGuid = NewGuid; }

	/**
	 * Gets the gameplay tag associated with the action.
	 *
	 * @return The FGameplayTag representing the action's tag.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE FGameplayTag GetActionTag() const { return ActionTag; }

	/**
	 * Sets the gameplay tag associated with the action.
	 *
	 * @param NewTag The new FGameplayTag to assign to the action.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE void SetActionTag(const FGameplayTag& NewTag) { ActionTag = NewTag; }

	/**
	 * Gets the display name of the action.
	 *
	 * @return The FText representing the action's name.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE FText GetActionName() const { return ActionName; }

	/**
	 * Sets the display name of the action.
	 *
	 * @param NewName The new name to assign to the action.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE void SetActionName(const FText& NewName) { ActionName = NewName; }

	/**
	 * Gets the description of the action.
	 *
	 * @return The FText representing the action's description.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE FText GetActionDescription() const { return ActionDescription; }

	/**
	 * Sets the description of the action.
	 *
	 * @param NewDescription The new description to assign to the action.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE void SetActionDescription(const FText& NewDescription) { ActionDescription = NewDescription; }

	/**
	 * Gets the visual icon of the action.
	 *
	 * @return The FSlateBrush representing the action's icon.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE FSlateBrush GetActionIcon() const { return ActionBrush; }

	/**
	 * Sets the visual icon of the action.
	 *
	 * @param NewBrush The new FSlateBrush to assign to the action.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE void SetActionIcon(const FSlateBrush& NewBrush) { ActionBrush = NewBrush; }

	/**
	 * Gets the owning inventory item of this action.
	 *
	 * @return The UMounteaInventoryItem that owns this action.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE UMounteaInventoryItem* GetOwningItem() const { return OwningItem; }

	/**
	 * Sets the owning inventory item of this action.
	 *
	 * @param NewOwningItem The new UMounteaInventoryItem to assign as owner.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE void SetOwningItem(UMounteaInventoryItem* NewOwningItem) { OwningItem = NewOwningItem; }

	/**
	 * Gets the dynamic delegate context of the action.
	 *
	 * @return The FMounteaInventoryCommandContext for this action.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE FMounteaInventoryCommandContext GetActionContext() const { return ActionContext; }

	/**
	 * Sets the dynamic delegate context of the action.
	 *
	 * @param NewContext The new FMounteaInventoryCommandContext to assign to the action.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE void SetActionContext(const FMounteaInventoryCommandContext& NewContext) { ActionContext = NewContext; }

	/**
	 * Sets the world context for this action from a given level.
	 *
	 * @param FromLevel The level from which to set the world context.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	virtual void SetWorldFromLevel(ULevel* FromLevel);

	/**
	 * Sets the world context for this action.
	 *
	 * @param NewWorld The new UWorld instance.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|ItemAction")
	virtual void SetWorld(UWorld* NewWorld);

	/**
	 * Gets the level that owns this action.
	 *
	 * @return The ULevel instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemAction")
	FORCEINLINE ULevel* GetLevel() const { return GetTypedOuter<ULevel>(); }

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
	
protected:

	virtual bool IsSupportedForNetworking() const override { return true; }
	
protected:

	/** Unique identifier for the action. Used to distinguish this action from others. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault))
	FGuid ActionGuid;

	/** Gameplay tag associated with the action. Can be used for filtering or applying specific behaviors. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault))
	FGameplayTag ActionTag;

	/** Display name of the action. Used for UI representation and identification. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault))
	FText ActionName;

	/** Description of the action. Provides additional information about the action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault, MultiLine))
	FText ActionDescription;

	/** Visual representation of the action. Used to display an icon or image associated with the action in the UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="2. Optional", meta=(NoResetToDefault, ExposeOnSpawn))
	FSlateBrush ActionBrush;

private:

	/** Dynamic delegate context for the action. */
	UPROPERTY(Transient, VisibleAnywhere, Category="ReadOnly", meta=(DisplayThumbnail=false), AdvancedDisplay)
	FMounteaInventoryCommandContext ActionContext;

	/** The world context in which this action exists. */
	UPROPERTY(VisibleAnywhere, Category="ReadOnly", meta=(DisplayThumbnail=false), AdvancedDisplay)
	UWorld* World;

	/** The owning inventory item of this action. */
	UPROPERTY(VisibleAnywhere, Category="ReadOnly", meta=(DisplayThumbnail=false), AdvancedDisplay)
	TObjectPtr<UMounteaInventoryItem> OwningItem = nullptr;
};

#undef LOCTEXT_NAMESPACE