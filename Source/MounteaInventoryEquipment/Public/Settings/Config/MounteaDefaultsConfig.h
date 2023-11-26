// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaDefaultsConfig.generated.h"

class UMounteaBaseUserWidget;
/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Defaults Config")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaDefaultsConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Item", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Item", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaTooltipWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemTooltipClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Class|Inventory|Slot", meta=(ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaInventorySlotWBPInterface"))
	TSoftClassPtr<UMounteaBaseUserWidget> DefaultInventoryItemSlotClass;
};
