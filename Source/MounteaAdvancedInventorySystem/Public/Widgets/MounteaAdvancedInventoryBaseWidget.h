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
#include "Blueprint/UserWidget.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Widgets/BaseWidget/MounteaAdvancedBaseInventoryWidgetInterface.h"
#include "MounteaAdvancedInventoryBaseWidget.generated.h"

/**
 * UMounteaAdvancedInventoryBaseWidget is the base class for all inventory-related UI widgets.
 * Base widgets provide core functionality including theme application, widget refresh, command processing,
 * and parent UI component management for consistent inventory interface behavior.
 *
 * @see [Inventory UI System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventoryUI)
 * @see IMounteaAdvancedInventoryUIInterface
 * @see IMounteaInventoryGenericWidgetInterface
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryBaseWidget : public UUserWidget,
public IMounteaInventoryGenericWidgetInterface,
public IMounteaAdvancedBaseInventoryWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	virtual void ProcessInventoryWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload = nullptr) override {};
	virtual void ApplyTheme_Implementation() override {};
	virtual void RefreshWidget_Implementation() override {};

	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI) override
	{
		if (ParentUIComponent != NewOwningInventoryUI)
			ParentUIComponent = NewOwningInventoryUI;
	};

	virtual TScriptInterface<IMounteaAdvancedInventoryUIInterface> GetOwningInventoryUI_Implementation() const override
	{ return ParentUIComponent; };

protected:
	
	/** Reference to the owning inventory UI interface.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Primary Data",
		AdvancedDisplay, meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaAdvancedInventoryUIInterface> ParentUIComponent = nullptr;
};
