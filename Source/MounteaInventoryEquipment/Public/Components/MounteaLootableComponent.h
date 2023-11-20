// All rights reserved Dominik Morse 2023.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Looting/MounteaLootingInterface.h"
#include "MounteaLootableComponent.generated.h"

/**
 * Component that provides functionality for lootable objects in the game.
 * It allows actors to have an inventory from which items can be looted or picked up.
 *
 * This system is designed to be modular and extensible, allowing developers to easily add new features or modify existing ones to suit their specific requirements.
 * If current implementation is not what you are looking for, the `IMounteaInventoryInterface` comes with all functions exposed, allowing recreating the whole system in Blueprints.
 *
 * @see UActorComponent
 * @see IMounteaLootingInterface
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Lootable-Component
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Mountea Loot & Pickup"))
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaLootableComponent : public UActorComponent, public IMounteaLootingInterface
{
	GENERATED_BODY()

public:

	UMounteaLootableComponent();

#pragma region FUNCTIONS
	
protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	virtual TScriptInterface<IMounteaInventoryInterface> GetSourceInventory_Implementation() const override;
	virtual void SetSourceInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewSource) override;
	virtual TScriptInterface<IMounteaInventoryInterface> GetTargetInventory_Implementation() const override;
	virtual void SetTargetInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewTarget) override;
	virtual TArray<UMounteaInstancedItem*> GetLootableItems_Implementation() const override;
	virtual bool CanLootItem_Implementation(const FItemTransfer& Item) const override;
	virtual FInventoryUpdateResult LootItem_Implementation(const FItemTransfer& Item) override;
	virtual TArray<FInventoryUpdateResult> LootAllItems_Implementation(const TArray<FItemTransfer>& Items) override;

	virtual AActor* GetOwningActor_Implementation() const override;
	virtual bool DoesHaveAuthority_Implementation() const override;

protected:

	/**
	* Server-side function to set the source inventory.
	* @param NewSource The new source inventory to set.
	*/
	UFUNCTION(Server, Reliable, WithValidation)
	void SetSourceInventory_Server(const TScriptInterface<IMounteaInventoryInterface>& NewSource);
	
	/**
	 * Server-side function to set the target inventory.
	 * @param NewSource The new target inventory to set.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void SetTargetInventory_Server(const TScriptInterface<IMounteaInventoryInterface>& NewSource);

	/**
	 * Server-side function to handle an item looting request.
	 * @param Item The item and quantity being looted.
	 */
	UFUNCTION(Server, Reliable)
	void LootItem_Server(const FItemTransfer& Item);

protected:

	/**
	 * Configures the component based on the assigned source inventory.
	 */
	void SetupSourceComponent();

#if WITH_EDITOR
	
private:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
	
#pragma endregion

#pragma region Variables

protected:

	/**
	 * Reference to the inventory component that serves as the source of lootable items.
	 */
	UPROPERTY(SaveGame, EditAnywhere, Category="1. Required", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, AllowedClasses="MounteaInventoryInterface, MounteaInventoryComponent", UseComponentPicker), Replicated)
	FComponentReference SourceInventoryComponent;

private:

	/**
	 * The actual inventory component that serves as the source of lootable items.
	 */
	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false), Replicated)
	TScriptInterface<IMounteaInventoryInterface> SourceInventory;

	/**
	 * The actual inventory component that serves as the target for looting operations.
	 */
	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false), Replicated)
	TScriptInterface<IMounteaInventoryInterface> TargetInventory;
	
#pragma endregion 
};
