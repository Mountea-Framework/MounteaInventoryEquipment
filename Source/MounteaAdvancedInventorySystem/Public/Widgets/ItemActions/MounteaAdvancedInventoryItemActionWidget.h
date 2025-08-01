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
#include "Interfaces/Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidgetInterface.h"
#include "Widgets/MounteaAdvancedInventoryBaseWidget.h"
#include "MounteaAdvancedInventoryItemActionWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Category=Mountea, meta=(DisplayName="Mountea Advanced Inventory Item Action Widget (Base)"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API
	UMounteaAdvancedInventoryItemActionWidget : public UMounteaAdvancedInventoryBaseWidget, public IMounteaAdvancedInventoryItemActionWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void InitializeItemAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI,
		const TSoftClassPtr<UObject>& ItemActionClass, UWidget* ParentWidget) override;
	virtual bool IsActionEnabled_Implementation() const override;
	virtual bool IsActionValid_Implementation() const override;
	virtual void ExecuteItemAction_Implementation() override;
	virtual TSoftClassPtr<UObject> GetItemAction_Implementation() const override
	{ return ActionClass; };
	virtual FMounteaItemActionData GetItemActionData_Implementation() const override;

	virtual FOnItemActionSelected& GetOnItemActionSelectedEventHandle() override
	{ return OnItemActionSelected; };

protected:

	/** The class of the item action to be executed by this widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Action",
		meta=(MustImplement="/Script/ActorInteractionPlugin.MounteaAdvancedInventoryItemActionInterface"))
	TSoftClassPtr<UObject> ActionClass;

	/** The parent Item component that owns this item action widget. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Action",
		meta=(DisplayThumbnail=false))
	TObjectPtr<UWidget> ParentItemWidget;

	/** Event triggered when an item action is selected. */
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Item Action")
	FOnItemActionSelected OnItemActionSelected;
};
