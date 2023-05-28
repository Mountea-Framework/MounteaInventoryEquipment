// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MounteaBaseUserWidget.generated.h"

class UMounteaInventoryThemeConfig;
/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()

	UMounteaBaseUserWidget(const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY(EditAnywhere, Category="1. Optional", meta=(AllowAbstract=false, DisplayThumbnail=false))
	UMounteaInventoryThemeConfig* ThemeConfigOverride = nullptr;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(CompactNodeTitle="Theme Config", HideSelfPin=true))
	UMounteaInventoryThemeConfig* GetThemeConfig() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Inventory", meta=(CompactNodeTitle="Defaults", HideSelfPin=true))
	void LoadFromConfig();
};
