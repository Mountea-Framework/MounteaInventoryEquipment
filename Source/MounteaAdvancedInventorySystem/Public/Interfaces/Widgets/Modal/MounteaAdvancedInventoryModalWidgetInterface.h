// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
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
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryModalWidgetInterface.generated.h"

class UUserWidget;
class UMounteaJsonObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModalContentAddedToModalWindow, UUserWidget*, ModalContentWidget, UMounteaJsonObject*, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModalConfirmed, UObject*, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnModalCancelled);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FMounteaModalContentAddedToModalWindowBinding, UUserWidget*, ModalContentWidget, UMounteaJsonObject*, Payload);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMounteaModalConfirmedBinding, UObject*, Payload);
DECLARE_DYNAMIC_DELEGATE(FMounteaModalCancelledBinding);

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
	void AddModalContentToModalWindow(UUserWidget* ModalContentWidget, UMounteaJsonObject* Payload);
	virtual void AddModalContentToModalWindow_Implementation(UUserWidget* ModalContentWidget, UMounteaJsonObject* Payload) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Modal")
	void ModalContentConfirmed(UObject* Payload);
	virtual void ModalContentConfirmed_Implementation(UObject* Payload) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Modal")
	void ModalContentCancelled();
	virtual void ModalContentCancelled_Implementation() = 0;
	
	virtual FOnModalContentAddedToModalWindow& GetOnModalContentAddedToModalWindowHandle() = 0;
	virtual FOnModalConfirmed& GetOnModalConfirmedHandle() = 0;
	virtual FOnModalCancelled& GetOnModalCancelledHandle() = 0;
};
