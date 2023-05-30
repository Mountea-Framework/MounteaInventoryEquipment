// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryThemeConfig.generated.h"

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
	TSubclassOf<UUserWidget> InventoryTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme|Base Class", meta=(ShowOnlyInnerProperties))
	TSubclassOf<UUserWidget> InventoryBackground;
};

USTRUCT(BlueprintType)
struct FInventoryThemeConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Theme", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	UMounteaInventoryThemeConfig* ThemeConfig = nullptr;
};