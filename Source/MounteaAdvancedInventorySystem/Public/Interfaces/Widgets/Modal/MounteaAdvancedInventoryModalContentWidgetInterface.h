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
#include "MounteaAdvancedInventoryModalContentWidgetInterface.generated.h"

class UMounteaModalsPayload;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModalContentConfirmed, UObject*, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnModalContentCancelled);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMounteaModalContentConfirmedBinding, UObject*, Payload);
DECLARE_DYNAMIC_DELEGATE(FMounteaModalContentCancelledBinding);

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryModalContentWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryModalContentWidgetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Modal|Content")
	void ConstructModalContent(UMounteaModalsPayload* Payload);
	virtual void ConstructModalContent_Implementation(UMounteaModalsPayload* Payload) = 0;

	virtual FOnModalContentConfirmed& GetOnModalContentConfirmedHandle() = 0;
	virtual FOnModalContentCancelled& GetOnModalContentCancelledHandle() = 0;
};
