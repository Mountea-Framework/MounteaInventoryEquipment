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
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryBaseUIStatics.generated.h"

class AActor;
class APlayerController;
class IMounteaAdvancedInventorySharedHUDInterface;
class UCanvasPanel;
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

	/**
	 * Calculates the position and rotation of each child widget so they fan out evenly along a semi-elliptical
	 * arc spanning the full parent Canvas: the arc's two ends sit exactly on ParentCanvas's own bottom-left and
	 * bottom-right corners, and it bulges up to the full Canvas height at its horizontal center. There is no
	 * separate tuning config — the curve is entirely determined by ParentCanvas's own current size. Result keys
	 * mirror CurrentChildren's keys.
	 *
	 * Positions are absolute, Canvas-local coordinates (from ParentCanvas's own top-left origin) representing
	 * where each widget's CENTER should sit on the curve. Apply Position via `Canvas Panel Slot -> Set Position`
	 * and RotationAngle via `Set Render Transform Angle` on each child — but first set that child's Canvas
	 * Panel Slot Alignment AND Render Transform Pivot to (0.5, 0.5). Without that, Set Position places the
	 * widget's top-left corner (not its center) on the curve, and rotating swings it around a different
	 * point than where it was placed, throwing both off the curve. ApplyArcWidgetLayout does this for you.
	 * Do not feed Position into a Render Transform Translation — that stacks on top of whatever position
	 * the slot already has.
	 *
	 * Children are placed at the interior boundaries of (CurrentChildren.Num() + 1) equal angular gaps along
	 * the arc, so the margin before the first child and after the last child always matches the spacing
	 * between children — nobody sits exactly on the arc's own two corner endpoints, even at low counts. A
	 * single child naturally lands at the arc's apex (top-center of the Canvas), upright. The whole current
	 * CurrentChildren count always stretches to fill the full arc; it does not reserve fixed-width slots for
	 * some larger capacity.
	 *
	 * @param ParentCanvas Canvas Panel the children are laid out in; also the source of the arc's own geometry.
	 * @param CurrentChildren Currently present children, keyed by their slot index.
	 * @param SizeOverride Optional explicit Canvas size to use instead of reading ParentCanvas's live geometry.
	 * Use this when ParentCanvas's arranged size can't be trusted (e.g. it's inside an auto-sized ancestor
	 * and its own reported size is driven by its still-unpositioned children). Leave as (0,0) to auto-read.
	 * @return Map of slot index to the resulting widget + transform, mirroring CurrentChildren's keys.
	 *
	 * @note ParentCanvas's cached geometry must be valid (call after first frame/tick) unless SizeOverride is used.
	 * @see ApplyArcWidgetLayout
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Layout",
		meta=(MounteaGetter),
		DisplayName="Calculate Arc Widget Layout")
	static TMap<int32, FMounteaTransformedWidgetPositionData> CalculateArcWidgetLayout(
		UCanvasPanel* ParentCanvas,
		const TMap<int32, UUserWidget*>& CurrentChildren,
		FVector2D SizeOverride = FVector2D::ZeroVector);

	/**
	 * Same layout calculation as CalculateArcWidgetLayout, but applies the result directly to each
	 * child instead of returning it: forces that child's Canvas Panel Slot Alignment and Render
	 * Transform Pivot to (0.5, 0.5) — so Position/RotationAngle consistently mean "this widget's
	 * center" — then sets its Canvas Panel Slot position and its Render Transform Angle. Each child
	 * must already be parented to ParentCanvas (its Slot must be a Canvas Panel Slot) — children
	 * without one are skipped.
	 *
	 * @param ParentCanvas Canvas Panel the children are laid out in; also the source of the arc's own geometry.
	 * @param CurrentChildren Currently present children, keyed by their slot index.
	 * @param SizeOverride Optional explicit Canvas size to use instead of reading ParentCanvas's live geometry.
	 * Use this when ParentCanvas's arranged size can't be trusted (e.g. it's inside an auto-sized ancestor
	 * and its own reported size is driven by its still-unpositioned children). Leave as (0,0) to auto-read.
	 * @return True if the layout was applied to at least one child, false otherwise.
	 *
	 * @note ParentCanvas's cached geometry must be valid (call after first frame/tick) unless SizeOverride is used.
	 * @see CalculateArcWidgetLayout
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Layout",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Apply Arc Widget Layout")
	static bool ApplyArcWidgetLayout(
		UCanvasPanel* ParentCanvas,
		const TMap<int32, UUserWidget*>& CurrentChildren,
		FVector2D SizeOverride = FVector2D::ZeroVector);

private:

	static APlayerController* FindPlayerController(AActor* Actor, int SearchDepth);

	static TMap<int32, FMounteaTransformedWidgetPositionData> CalculateArcWidgetLayoutInternal(
		const FVector2D& CanvasSize,
		const TMap<int32, UUserWidget*>& CurrentChildren);
};
