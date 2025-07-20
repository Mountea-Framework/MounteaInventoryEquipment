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

	virtual void InitializeItemAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI, TSubclassOf<UMounteaInventoryItemAction> ItemAction, const FGuid& ItemId) override;
	virtual bool IsActionEnabled_Implementation() const override;
	virtual bool IsActionValid_Implementation() const override;
	virtual void ExecuteItemAction_Implementation() override;
};
