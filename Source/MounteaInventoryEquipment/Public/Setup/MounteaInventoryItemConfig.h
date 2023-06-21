// All rights reserved Dominik Pavlicek 2023.

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
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Item Config (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemConfig : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * ItemSlotSize represents the size of an item slot in a grid-based inventory system.
	 * The ItemSlotSize variable is typically used in the context of a grid-based inventory system, where items are organized and stored in slots.
	 * It represents the size of each individual item slot in the grid.
	 *
	 * By accessing the ItemSlotSize variable, you can retrieve the dimensions of an item slot, which can be useful for various inventory-related operations such as positioning, layout calculations, and determining the size and placement of items within the inventory grid.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta=(ShowOnlyInnerProperties))
	FIntPoint ItemSlotSize = FIntPoint(1,1);
};

USTRUCT(BlueprintType)
struct FMounteaItemConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	UMounteaInventoryItemConfig* ItemConfig = nullptr;
};

#undef LOCTEXT_NAMESPACE