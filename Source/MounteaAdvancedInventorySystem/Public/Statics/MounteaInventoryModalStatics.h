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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryModalStatics.generated.h"

class APlayerController;
class UMounteaAdvancedInventoryGlobalUIConfig;
class UUserWidget;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryModalStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaGetter),
		meta=(MounteaModalType),
		meta=(MounteaModalTypePin="ModalType"),
		meta=(MounteaModalRow),
		meta=(MounteaModalRowPin="Key"),
		meta=(WorldContext="Context"),
		meta=(DefaultToSelf="Context"),
		meta=(DisplayName="Modal - Create Modal Content Widget"))
	static UUserWidget* CreateModalContentWidget(
		UObject* Context,
		const FString& ModalType,
		const FString& Key,
		UObject* OptionalPayload);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal - Construct Modals Content"))
	static void ConstructModalsContent(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FDataTableRowHandle& DataTableData,
		UObject* OptionalPayload,
		const FString& ModalContentType);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Content",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal Content - Construct Content"))
	static void ConstructModalContent(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FDataTableRowHandle& DataTableData,
		UObject* OptionalPayload,
		const FString& ModalContentType);

private:

	static UDataTable* FindModalDataTableForRow(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& Key);
	static APlayerController* ResolveOwningPlayer(UObject* Context);
	static UMounteaAdvancedInventoryGlobalUIConfig* GetGlobalUIConfig();
};
