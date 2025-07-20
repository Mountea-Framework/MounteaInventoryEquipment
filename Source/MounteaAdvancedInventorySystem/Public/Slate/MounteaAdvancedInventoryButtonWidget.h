// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "MounteaAdvancedInventoryButtonWidget.generated.h"

/**
 * UMounteaAdvancedInventoryButtonWidget provides gameplay tag-based button functionality for inventory interfaces.
 * Inventory buttons support action tag classification, contextual command processing, and theme application
 * for consistent and extensible user interaction within the inventory system.
 *
 * @see [Inventory UI Controls](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/SlateElements)
 * @see IMounteaInventoryGenericWidgetInterface
 * @see FGameplayTag
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API
	UMounteaAdvancedInventoryButtonWidget : public UButton, public IMounteaInventoryGenericWidgetInterface
{
	GENERATED_BODY()

public:
	
	virtual void ProcessInventoryWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload) override {};
	virtual void ApplyTheme_Implementation() override;
	virtual void RefreshWidget_Implementation() override {};

private:
	
#if WITH_EDITOR
	
	virtual const FText GetPaletteCategory() override
	{
		return NSLOCTEXT("MounteaAdvancedInventory", "WidgetPaletteCategory", "Mountea Advanced Inventory & Equipment");
	}

#endif

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button", meta=(NoResetToDefault))
	FGameplayTag ButtonActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button", meta=(NoResetToDefault))
	FString ButtonActionContext;
};
