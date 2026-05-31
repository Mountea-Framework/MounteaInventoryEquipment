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
#include "Engine/DataTable.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryModalWidgetInterface.generated.h"

class UUserWidget;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryModalWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryModalWidgetInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Modal")
	UUserWidget* CreateModalContentWidget(const FDataTableRowHandle& DataTableData, UObject* OptionalPayload, const FString& ModalType);
	virtual UUserWidget* CreateModalContentWidget_Implementation(const FDataTableRowHandle& DataTableData, UObject* OptionalPayload, const FString& ModalType) = 0;
};
