// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "MounteaAdvancedInventoryBaseWidget.h"
#include "MounteaAdvancedInventoryButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API
	UMounteaAdvancedInventoryButtonWidget : public UButton, public IMounteaInventoryGenericWidgetInterface
{
	GENERATED_BODY()

public:
	
	virtual void ProcessInventoryWidgetCommand_Implementation(const FString& Command) override {};

private:
	
#if WITH_EDITOR
	
	virtual const FText GetPaletteCategory() override
	{
		return NSLOCTEXT("MounteaAdvancedInventory", "WidgetPaletteCategory", "Mountea Advanced Inventory & Equipment");
	}

#endif

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button", meta=(NoResetToDefault))
	FGameplayTag ButtonActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button", meta=(NoResetToDefault))
	FString ButtonActionContext;
};
