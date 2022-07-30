// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Helpers/InventoryHelpers.h"

#include "ActorInventoryItemComponent.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EInventoryItemSetup : uint8
{
	EIIS_FromItem		UMETA(DisplayName="From Item"),
	EIIS_FromDataTable	UMETA(DisplayName="From Data Table"),

	EIIS_Default		UMETA(Hidden)
};

class UInventoryItem;

/**
 * Helper Component which defines that Actor has Inventory Item.
 * This is a way to avoid Inheritance issues which might occur if using PickupItem Class. For more details visit Wiki page.
 * 
 * Usable for Pickups, like Weapons or Food. Does not bind owning Actor to specific type.
 *
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Inventory-Item-Component
 */
UCLASS(ClassGroup=(Inventory), HideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(DisplayName = "Inventory Item Component"))
class ACTORINVENTORYPLUGIN_API UActorInventoryItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION()
	FInventoryItemData GetItemDefinition() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryItem* GetItem() const
	{
		return SourceItem;
	}

protected:

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(NoResetToDefault))
	EInventoryItemSetup SetupMode = EInventoryItemSetup::EIIS_FromItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(EditCondition="SetupMode==EInventoryItemSetup::EIIS_FromItem", EditConditionHides, NoResetToDefault, AllowAbstract=false))
	TSubclassOf<UInventoryItem> SourceItemClass;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(EditCondition="SetupMode==EInventoryItemSetup::EIIS_FromDataTable", EditConditionHides, NoResetToDefault, ShowOnlyInnerProperties))
	FDataTableRowHandle SourceItemRow;

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	UInventoryItem* SourceItem = nullptr;
	
protected:

	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
};