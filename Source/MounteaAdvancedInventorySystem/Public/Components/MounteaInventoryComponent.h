// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Definitions/MounteaInventoryItem.h"
#include "MounteaInventoryComponent.generated.h"


/**
 *  Mountea Inventory Component
 * 
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,  AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"), HideCategories=("Cooking","Collision"), meta=(BlueprintSpawnableComponent, DisplayName="Mountea Inventory Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UMounteaInventoryComponent();

protected:

	virtual void BeginPlay() override;

private:

	// TODO: VisibleAnywhere
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TArray<FInventoryItem> InventoryItems;

};
