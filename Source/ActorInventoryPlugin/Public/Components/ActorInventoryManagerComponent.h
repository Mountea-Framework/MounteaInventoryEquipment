// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorInventoryManagerComponent.generated.h"

class UInventoryCategory;


UCLASS(ClassGroup=(Inventory), Blueprintable, meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Manager", ShortTooltip="Inventory Manager responsible for Adding and Removing Items from Inventory."))
class ACTORINVENTORYPLUGIN_API UActorInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UActorInventoryManagerComponent();

protected:
	
	virtual void BeginPlay() override;

	/**
	 * List of allowed Categories to be displayed in the Inventory.
	 * Only selected Categories will be displayed.
	 * Only valid Categories will be displayed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TArray<TSubclassOf<UInventoryCategory>> AllowedCategories;
		
};
