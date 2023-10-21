// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "ContentTheme.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryThemeConfig.generated.h"

class UCategoryTheme;
class UMounteaBaseEquipmentSlotWidget;
class UMounteaBaseUserWidget;

/**
 * The `MounteaInventoryThemeConfig` class represents the configuration settings for the Mountea inventory theme.
 * It is a data asset that stores various properties and classes related to the inventory UI.
 * Provides a simple way to dramatically change visuals using pre-defined data.
 *
 * To extend this class, create new Blueprint Class with `MounteaInventoryThemeConfig` as parent class.
 * In `Mountea Inventory & Equipment (data)` menu, this new class will be available for selection.
 *
 * Use `GetThemeConfig` method with dynamic outputs to get const reference to used Theme for selected `Mountea Inventory Widget Base`.
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryThemeConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UCategoryTheme> DefaultCategoryTheme = UCategoryTheme::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions", meta=(ShowOnlyInnerProperties))
	FIntPoint SlotBaseSize = FIntPoint(64,64);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions", meta=(ShowOnlyInnerProperties))
	FIntPoint ItemBaseSize = FIntPoint(1,1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions", meta=(ShowOnlyInnerProperties))
	FIntPoint InventoryBaseSize = FIntPoint(6,10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Slot", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UMounteaBaseUserWidget> InventorySlotClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Equipment", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> EquipmentBaseClass;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Title", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UMounteaBaseUserWidget> InventoryTitleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Background", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UMounteaBaseUserWidget> InventoryBackgroundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Item", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> InventoryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Item", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaTooltipWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> InventoryItemTooltipClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Item", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaDragDropWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> InventoryDragDropClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Category", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaCategoryWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> InventoryCategoryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Actions", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaItemActionWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> InventoryItemActionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Equipment", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface"))
	TSubclassOf<UMounteaBaseEquipmentSlotWidget> EquipmentSlotClass;
	
};