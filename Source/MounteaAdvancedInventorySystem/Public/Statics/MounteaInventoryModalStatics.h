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
#include "Definitions/MounteaAdvancedInventoryGlobalUIDataTypes.h"
#include "Engine/DataTable.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalContentWidgetInterface.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalWidgetInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryModalStatics.generated.h"

class APlayerController;
class UMounteaAdvancedInventoryGlobalUIConfig;
class UMounteaJsonObject;
class UUserWidget;

/**
 *
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryModalStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Creates a modal window widget using the configured global UI settings.
	 *
	 * @param Context World context used to resolve the owning player.
	 * @param OptionalPayload Optional payload passed to the modal window.
	 * @return Created modal window widget, or nullptr when creation fails.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(WorldContext="Context"),
		meta=(DefaultToSelf="Context"),
		meta=(DisplayName="Modal - Create Modal Window Widget"))
	static UUserWidget* CreateModalWindowWidget(UObject* Context, UObject* OptionalPayload);

	/**
	 * Creates modal content for the specified modal type.
	 *
	 * @param Context World context used to resolve the owning player.
	 * @param ModalType Modal type key used to locate content configuration.
	 * @param InPayload Payload passed to the modal content.
	 * @return Created modal content widget, or nullptr when creation fails.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(MounteaModalType),
		meta=(MounteaModalTypePin="ModalType"),
		meta=(WorldContext="Context"),
		meta=(DefaultToSelf="Context"),
		meta=(DisplayName="Modal - Create Modal Content Widget"))
	static UUserWidget* CreateModalContentWidget(UObject* Context, const FString& ModalType, UMounteaJsonObject* InPayload);

	/**
	 * Returns the string value for a modal type.
	 *
	 * @param ModalType Modal type to convert.
	 * @return Modal type string.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaGetter),
		meta=(MounteaModalType),
		meta=(MounteaModalTypePin="ModalType"),
		meta=(DisplayName="Modal - Get Modal Type String"))
	static FString GetModalTypeString(const FString& ModalType);

	/**
	 * Adds modal content to a modal window.
	 *
	 * @param Target Modal window that receives content.
	 * @param ModalContentWidget Modal content widget to add.
	 * @param Payload Payload passed to the modal window.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal Window - Add Modal Content"))
	static void AddModalContentToModalWindow(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		UUserWidget* ModalContentWidget, UMounteaJsonObject* Payload);

	/**
	 * Binds a dynamic delegate to the modal window content-added event.
	 *
	 * @param Target Modal window to bind to.
	 * @param Binding Delegate to add to the content-added event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Window - Bind On Modal Content Added"))
	static bool BindToOnModalContentAddedToModalWindow(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalContentAddedToModalWindowBinding& Binding);

	/**
	 * Removes a dynamic delegate from the modal window content-added event.
	 *
	 * @param Target Modal window to unbind from.
	 * @param Binding Delegate to remove from the content-added event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Window - Unbind From On Modal Content Added"))
	static bool UnbindFromOnModalContentAddedToModalWindow(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalContentAddedToModalWindowBinding& Binding);

	/**
	 * Binds a dynamic delegate to the modal window confirmed event.
	 *
	 * @param Target Modal window to bind to.
	 * @param Binding Delegate to add to the confirmed event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Window - Bind On Modal Confirmed"))
	static bool BindToOnModalConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalConfirmedBinding& Binding);

	/**
	 * Removes a dynamic delegate from the modal window confirmed event.
	 *
	 * @param Target Modal window to unbind from.
	 * @param Binding Delegate to remove from the confirmed event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Window - Unbind From On Modal Confirmed"))
	static bool UnbindFromOnModalConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalConfirmedBinding& Binding);

	/**
	 * Binds a dynamic delegate to the modal window cancelled event.
	 *
	 * @param Target Modal window to bind to.
	 * @param Binding Delegate to add to the cancelled event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Window - Bind On Modal Cancelled"))
	static bool BindToOnModalCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalCancelledBinding& Binding);

	/**
	 * Removes a dynamic delegate from the modal window cancelled event.
	 *
	 * @param Target Modal window to unbind from.
	 * @param Binding Delegate to remove from the cancelled event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Window - Unbind From On Modal Cancelled"))
	static bool UnbindFromOnModalCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface")) UObject* Target,
		const FMounteaModalCancelledBinding& Binding);

	/**
	 * Binds a dynamic delegate to the modal content confirmed event.
	 *
	 * @param Target Modal content to bind to.
	 * @param Binding Delegate to add to the confirmed event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Content - Bind On Modal Content Confirmed"))
	static bool BindToOnModalContentConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentConfirmedBinding& Binding);

	/**
	 * Removes a dynamic delegate from the modal content confirmed event.
	 *
	 * @param Target Modal content to unbind from.
	 * @param Binding Delegate to remove from the confirmed event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Content - Unbind From On Modal Content Confirmed"))
	static bool UnbindFromOnModalContentConfirmed(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentConfirmedBinding& Binding);

	/**
	 * Binds a dynamic delegate to the modal content cancelled event.
	 *
	 * @param Target Modal content to bind to.
	 * @param Binding Delegate to add to the cancelled event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Content - Bind On Modal Content Cancelled"))
	static bool BindToOnModalContentCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentCancelledBinding& Binding);

	/**
	 * Removes a dynamic delegate from the modal content cancelled event.
	 *
	 * @param Target Modal content to unbind from.
	 * @param Binding Delegate to remove from the cancelled event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Modal Content - Unbind From On Modal Content Cancelled"))
	static bool UnbindFromOnModalContentCancelled(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface")) UObject* Target,
		const FMounteaModalContentCancelledBinding& Binding);

	/**
	 * Finds the first modal configuration row whose name matches the key.
	 *
	 * @param Key Modal row key to search for across configured data tables.
	 * @param OutConfig Found modal configuration.
	 * @return True if a matching configuration was found.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal",
		meta=(MounteaGetter),
		meta=(MounteaModalRow),
		meta=(MounteaModalRowPin="Key"),
		meta=(CallableWithoutWorldContext),
		meta=(DisplayName="Modal - Get Config From Data Tables"))
	static bool GetModalConfigFromDataTables(const FString& Key, FMounteaModalsConfig& OutConfig);
	
	/**
	 * Starts modal content expiration.
	 *
	 * @param Target Modal content to expire.
	 * @param WidgetLifetime Lifetime in seconds before expiration.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Modal|Bindings",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Modal Content - Start Expiration"))
	static void StartModalExpiration(UObject* Target, const float WidgetLifetime);
	

private:

	static UUserWidget* CreateWidgetFromClass(UObject* Context, TSubclassOf<UUserWidget> WidgetClass);
	static APlayerController* ResolveOwningPlayer(UObject* Context);
	static UMounteaAdvancedInventoryGlobalUIConfig* GetGlobalUIConfig();
};
