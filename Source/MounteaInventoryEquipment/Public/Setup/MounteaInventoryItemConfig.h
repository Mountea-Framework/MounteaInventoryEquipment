// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryItemConfig.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemConfig"

/**
 *	Mountea Inventory Item Config
 *
 * Defines the visual representation of an item in the user interface.
 * It contains properties that determine the appearance of the item, such as its slot size, Item Actions, and other logical defaults.
 * These properties can be customized by designers and artists to create a consistent and appealing look for the game's inventory system.
 *
 * This class allows creating of Child classes with specified defaults or introducing new attributes per project needs.
 * By using `UMounteaInventoryItemConfig`, game developers can easily create and manage the visual aspects of the inventory system, while keeping the underlying logic separate and modular.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Item Config (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemConfig : public UObject
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FMounteaItemConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Mountea|Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	UMounteaInventoryItemConfig* ItemConfig = nullptr;
};

#undef LOCTEXT_NAMESPACE