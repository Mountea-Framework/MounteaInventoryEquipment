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

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	UPROPERTY(EditAnywhere, Category="1. Optional", meta=(AllowAbstract=false, DisplayThumbnail=false))
	UMounteaInventoryThemeConfig* ThemeConfigOverride = nullptr;

public:

	UFUNCTION(CallInEditor, BlueprintCallable, Category="Mountea|Debug", meta=(DevelopmentOnly))
	void RenderPreview();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool BindEvents();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool UnBindEvents();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryThemeConfig"), meta=(DeterminesOutputType = "ClassFilter"))//, meta=(CompactNodeTitle="Theme Config", HideSelfPin=true))
	UMounteaInventoryThemeConfig* GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Inventory")//, meta=(CompactNodeTitle="Defaults", HideSelfPin=true))
	void LoadFromConfig();

};
