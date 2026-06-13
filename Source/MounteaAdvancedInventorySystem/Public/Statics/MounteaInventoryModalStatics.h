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
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalContentWidgetInterface.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalWidgetInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryModalStatics.generated.h"

class APlayerController;
class UMounteaAdvancedInventoryGlobalUIConfig;
class UMounteaModalResponsePayload;
class UUserWidget;
class UMounteaModalsPayload;

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
		meta=(WorldContext="Context"),
		meta=(DefaultToSelf="Context"),
		meta=(DisplayName="Modal - Create Modal Content Widget"))
	static UUserWidget* CreateModalContentWidget(UObject* Context, UMounteaModalsPayload* Payload);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal Window - Add Modal Content"))
	static void AddModalContentToModalWindow(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		UUserWidget* ModalContentWidget, UMounteaModalsPayload* Payload);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Bind On Modal Content Added To Modal Window"))
	static bool BindToOnModalContentAddedToModalWindow(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalContentAddedToModalWindowBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Unbind From On Modal Content Added To Modal Window"))
	static bool UnbindFromOnModalContentAddedToModalWindow(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalContentAddedToModalWindowBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Bind On Modal Confirmed"))
	static bool BindToOnModalConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalConfirmedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Unbind From On Modal Confirmed"))
	static bool UnbindFromOnModalConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalConfirmedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Bind On Modal Cancelled"))
	static bool BindToOnModalCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalCancelledBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Unbind From On Modal Cancelled"))
	static bool UnbindFromOnModalCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalCancelledBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Bind On Modal Content Confirmed"))
	static bool BindToOnModalContentConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentConfirmedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Unbind From On Modal Content Confirmed"))
	static bool UnbindFromOnModalContentConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentConfirmedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Bind On Modal Content Cancelled"))
	static bool BindToOnModalContentCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentCancelledBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Unbind From On Modal Content Cancelled"))
	static bool UnbindFromOnModalContentCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentCancelledBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(MounteaPayloadConstructor),
		meta=(MounteaModalType),
		meta=(MounteaModalTypePin="ModalType"),
		meta=(MounteaModalRow),
		meta=(MounteaModalRowPin="Key"),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal - Construct Payload"))
	static UMounteaModalsPayload* ConstructModalPayload(UObject* Target, const FString& ModalType, const FString& Key, UObject* OptionalPayload);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(MounteaPayloadConstructor),
		meta=(MounteaModalType),
		meta=(MounteaModalTypePin="ModalType"),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal - Construct Response"))
	static UMounteaModalResponsePayload* CreateModalResponse(UObject* Target, const FString& ModalType, bool bConfirmationType);

private:

	static TSubclassOf<UMounteaModalsPayload> ResolveModalPayloadClass(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& ModalType);
	static TSubclassOf<UMounteaModalResponsePayload> ResolveModalResponseClass(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& ModalType);
	static FDataTableRowHandle ResolveModalDataTableRow(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& Key);
	static UDataTable* FindModalDataTableForRow(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& Key);
	static UUserWidget* CreateWidgetFromClass(UObject* Context, TSubclassOf<UUserWidget> WidgetClass);
	static APlayerController* ResolveOwningPlayer(UObject* Context);
	static UMounteaAdvancedInventoryGlobalUIConfig* GetGlobalUIConfig();
};
