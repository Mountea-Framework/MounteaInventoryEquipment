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
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryBaseUIStatics.generated.h"

class AActor;
class APlayerController;
class IMounteaAdvancedInventorySharedHUDInterface;
class UMounteaAdvancedInventoryGlobalUIConfig;
class UMounteaAdvancedInventorySharedHUDSubsystem;
class UUserWidget;
class UWidget;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryBaseUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static bool IsValidBaseUIHandler(const UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Context"),
		meta=(CompactNodeTitle="Inventory & Equipment Shared HUD Subsystem"),
		DisplayName="Get Inventory & Equipment Shared HUD Subsystem")
	static UMounteaAdvancedInventorySharedHUDSubsystem* GetSharedHUDSubsystem(UObject* Context);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Settings",
		meta=(MounteaGetter),
		DisplayName="Get Global UI Config")
	static UMounteaAdvancedInventoryGlobalUIConfig* GetGlobalUIConfig();

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(Keywords="container,viewport,make,add"),
		DisplayName="Create Wrapper Widget")
	static bool CreateWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaGetter),
		meta=(Keywords="container,viewport"),
		DisplayName="Get Wrapper Widget")
	static UUserWidget* GetWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaSetter),
		meta=(Keywords="container,viewport,destroy"),
		DisplayName="Remove Wrapper Widget")
	static void RemoveWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager|Commands",
		meta=(MounteaSetter),
		meta=(MounteaCommand),
		DisplayName="Inventory UI Manager - Execute Widget Command")
	static void ExecuteWidgetCommandFromManager(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
		const FString& Command, UObject* OptionalPayload);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Notifications",
		meta=(MounteaGetter))
	static UWidget* GetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Notifications",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"))
	static void SetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target, UWidget* NewNotificationContainer);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Notifications",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"))
	static void CreateInventoryNotification(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target, const FInventoryNotificationData& NotificationData);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Notifications",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"))
	static void RemoveInventoryNotifications(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Widget States")
	static FGameplayTagContainer GetManagerWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Set Widget States")
	static void SetManagerWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
		UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& NewStates);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="Tag"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Add Widget State Tag")
	static bool AddWidgetStateTag(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
		UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="Tag"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Remove Widget State Tag")
	static bool RemoveWidgetStateTag(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
		UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="Tag"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Has Widget State Tag")
	static bool HasWidgetStateTag(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
		UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag, bool bExactMatch = true);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Clear Widget State Tags")
	static void ClearWidgetStateTags(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Append Widget State Tags")
	static bool AppendWidgetStateTags(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
		UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& TagsToAppend);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Has Any Widget States")
	static bool HasAnyWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(Keywords="container,viewport,start"),
		DisplayName="Initialize Wrapper Widget")
	static void InitializeWrapperWidget(UObject* Target, const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Parent);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(Keywords="container,viewport,destroy"),
		DisplayName="Remove Wrapper Widget")
	static void RemoveBaseWrapperWidget(UObject* Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(Keywords="new,create"),
		DisplayName="Add Modal Window")
	static bool AddModalWindow(UObject* Target, UUserWidget* ModalWindow);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(Keywords="clear,reset"),
		DisplayName="Cleanup Modal Window")
	static bool CleanupModalWindow(UObject* Target);

private:

	static APlayerController* FindPlayerController(AActor* Actor, int SearchDepth);
};
