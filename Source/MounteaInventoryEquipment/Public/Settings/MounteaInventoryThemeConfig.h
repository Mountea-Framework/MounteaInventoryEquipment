// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryThemeConfig.generated.h"

class UMounteaBaseUserWidget;

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryThemeConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Dimensions", meta=(ShowOnlyInnerProperties))
	FIntPoint SlotBaseSize = FIntPoint(64,64);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Dimensions", meta=(ShowOnlyInnerProperties))
	FIntPoint ItemBaseSize = FIntPoint(1,1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Dimensions", meta=(ShowOnlyInnerProperties))
	FIntPoint InventoryBaseSize = FIntPoint(6,10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Base Class", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UMounteaBaseUserWidget> InventoryTitleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Base Class", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UMounteaBaseUserWidget> InventoryBackgroundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Base Class", meta=(ShowOnlyInnerProperties, MustImplement="MounteaInventoryItemWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> InventoryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Base Class", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UMounteaBaseUserWidget> InventoryCategoryClass;
};