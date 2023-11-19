// All rights reserved Dominik Morse 2023.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Looting/MounteaLootingInterface.h"
#include "MounteaLootableComponent.generated.h"


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
	virtual bool CanLootItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) const override;
	virtual FInventoryUpdateResult LootItem_Implementation(const FItemTransfer& Item) override;
	virtual TArray<FInventoryUpdateResult> LootAllItems_Implementation(const TArray<FItemTransfer>& Items) override;

	virtual AActor* GetOwningActor_Implementation() const override;
	virtual bool DoesHaveAuthority_Implementation() const override;

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void SetSourceInventory_Server(const TScriptInterface<IMounteaInventoryInterface>& NewSource);
	UFUNCTION(Server, Reliable, WithValidation)
	void SetTargetInventory_Server(const TScriptInterface<IMounteaInventoryInterface>& NewSource);
	UFUNCTION(Server, Reliable)
	void LootItem_Server(const FItemTransfer& Item);

protected:

	void SetupSourceComponent();

#if WITH_EDITOR
	
private:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
	
#pragma endregion

#pragma region Variables

protected:

	UPROPERTY(SaveGame, EditAnywhere, Category="1. Required", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, AllowedClasses="MounteaInventoryInterface, MounteaInventoryComponent", UseComponentPicker), Replicated)
	FComponentReference SourceInventoryComponent;

private:

	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false), Replicated)
	TScriptInterface<IMounteaInventoryInterface> SourceInventory;

	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false), Replicated)
	TScriptInterface<IMounteaInventoryInterface> TargetInventory;
	
#pragma endregion 
};
