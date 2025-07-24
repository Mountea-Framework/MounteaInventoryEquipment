// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidgetInterface.h"
#include "Widgets/MounteaAdvancedInventoryBaseWidget.h"
#include "MounteaAdvancedInventoryItemActionWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API
	UMounteaAdvancedInventoryItemActionWidget : public UMounteaAdvancedInventoryBaseWidget, public IMounteaAdvancedInventoryItemActionWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void InitializeItemAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI, const TSoftClassPtr<UMounteaInventoryItemAction>& ItemActionClass, UUserWidget* ParentWidget) override;
	virtual bool IsActionEnabled_Implementation() const override;
	virtual bool IsActionValid_Implementation() const override;
	virtual void ExecuteItemAction_Implementation() override;
	virtual TSoftClassPtr<UMounteaInventoryItemAction> GetItemAction_Implementation() const override
	{ return ActionClass; };

	virtual FOnItemActionSelected& GetOnItemActionSelectedEventHandle() override
	{ return OnItemActionSelected; };

protected:

	/** The class of the item action to be executed by this widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Action")
	TSoftClassPtr<UMounteaInventoryItemAction> ActionClass;

	/** The parent Item component that owns this item action widget. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Action",
		meta=(DisplayThumbnail=false))
	TObjectPtr<UUserWidget> ParentItemWidget;

	/** Event triggered when an item action is selected. */
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Item Action")
	FOnItemActionSelected OnItemActionSelected;
};
