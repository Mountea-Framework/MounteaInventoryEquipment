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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryModalStatics.generated.h"

class UDataTable;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryModalStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal - Construct Modals Content"))
	static void ConstructModalsContent(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		UDataTable* DataTable,
		UObject* OptionalPayload,
		const FString& Key);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Content",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal Content - Construct Content"))
	static void ConstructModalContent(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		UDataTable* DataTable,
		UObject* OptionalPayload,
		const FString& Key);
};
