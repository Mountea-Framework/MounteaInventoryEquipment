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
		meta=(MounteaSetter),
		meta=(WorldContext="Context"),
		meta=(DefaultToSelf="Context"),
		meta=(DisplayName="Modal - Create Modal Window Widget"))
	static UUserWidget* CreateModalWindowWidget(UObject* Context);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(MounteaModalType),
		meta=(MounteaModalTypePin="ModalType"),
		meta=(MounteaModalRow),
		meta=(MounteaModalRowPin="Key"),
		meta=(WorldContext="Context"),
		meta=(DefaultToSelf="Context"),
		meta=(DisplayName="Modal - Create Modal Content Widget"))
	static UUserWidget* CreateModalContentWidget(
		UObject* Context, const FString& ModalType, const FString& Key, UObject* OptionalPayload);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal Window - Add Modal Content"))
	static void AddModalContentToModalWindow(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		UUserWidget* ModalContentWidget);

private:

	static FDataTableRowHandle ResolveModalDataTableRow(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& Key);
	static UDataTable* FindModalDataTableForRow(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& Key);
	static UUserWidget* CreateWidgetFromClass(UObject* Context, TSubclassOf<UUserWidget> WidgetClass);
	static APlayerController* ResolveOwningPlayer(UObject* Context);
	static UMounteaAdvancedInventoryGlobalUIConfig* GetGlobalUIConfig();
};
