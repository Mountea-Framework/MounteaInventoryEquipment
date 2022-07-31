// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "InventoryKeyAction.generated.h"

struct FInventoryKeyActionData;

#define LOCTEXT_NAMESPACE "InventoryKeyActions"

/**
 * Inventory Key Actions is an Object which defines what Action is performed when its assigned Key is pressed.
 * Inventory Category is owning Class.
 *
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Inventory-Key-Action
 */
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class ACTORINVENTORYPLUGIN_API UInventoryKeyAction : public UObject
{
	GENERATED_BODY()

	UInventoryKeyAction();

	UInventoryKeyAction(const FText& NewActionName, const FGuid& NewActionGuid, const TSet<FInventoryKeyActionData>& NewPlatformBasedMapping) :
		ActionName(NewActionName),
		ActionGuid(NewActionGuid),
		PlatformBasedMapping(NewPlatformBasedMapping)
	{};
	
public:

	/**
	 * Execution of this specific Action.
	 *
	 * Action unique code which defines what is supposed to happen when this Action is requested.
	 * For example, when Consumable Item requests Action Use, this Action should verify whether it can be done, and if so, it should execute all code specified in the Blueprint.
	 * 
	 * @return Returns whether the function has been called nor not, for instance invalid Category cannot execute any actions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	bool ExecuteAction();
	bool ExecuteAction_Implementation()
	{
		return false;
	}

	/** Returns Name of the Action, Use, for instance. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetActionName() const { return ActionName; };

	/** Returns Guid (unique identification) of this Action. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FGuid GetActionGuid() const {return ActionGuid; };

	/**
	 * Returns Set of Mappings for this Action.
	 * Mappings are Platform specific and for now allow only single Key mapping to an Action.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSet<FInventoryKeyActionData> GetPlatformBasedMapping() const {return PlatformBasedMapping; };
	
protected:

	// Name of the Action. Will be displayed in UI.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=True))
	FText ActionName = LOCTEXT("InventoryActionName", "Use");

	// Unique Guid of this Action.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FGuid ActionGuid = FGuid::NewGuid();

	// Set of Key Mappings per Platform.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=True, ShowOnlyInnerProperties, TitleProperty="PlatformName"))
	TSet<FInventoryKeyActionData> PlatformBasedMapping;
};

#undef LOCTEXT_NAMESPACE