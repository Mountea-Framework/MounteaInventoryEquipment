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
#include "Subsystems/LocalPlayerSubsystem.h"
#include "MounteaAdvancedCraftingUISubsystem.generated.h"

class IMounteaAdvancedCraftingParticipantUIInterface;

/**
 *
 */
UCLASS(ClassGroup=(Mountea),
	meta=(DisplayName="Mountea Crafting UI Subsystem"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedCraftingUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Registers a Crafting UI Manager instance.
	 *
	 * Stores the provided object as the active Crafting UI Manager used
	 * to control crafting-related user interface logic.
	 *
	 * @param NewCraftingUIManager Object instance to register as the Crafting UI Manager.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Crafting|Subsystem",
		meta=(MounteaSetter),
		DisplayName="Register Crafting UI Manager")
	void RegisterCraftingUIManager(UObject* NewCraftingUIManager);

	/**
	 * Unregisters the currently registered Crafting UI Manager.
	 *
	 * If the provided object matches the currently registered Crafting UI Manager,
	 * it will be cleared and no longer used by the crafting system.
	 *
	 * @param DirtyCraftingUIManager Crafting UI Manager instance to unregister.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Crafting|Subsystem",
		meta=(MounteaSetter),
		DisplayName="Unregister Crafting UI Manager")
	void UnregisterCraftingUIManager(const UObject* DirtyCraftingUIManager);

	/**
	 * Returns the currently registered Crafting UI Manager.
	 *
	 * @return The active Crafting UI Manager instance, or nullptr if none is registered.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Crafting|Subsystem",
		meta=(MounteaGetter),
		DisplayName="Get Crafting UI Manager")
	UObject* GetCraftingUIManager() const;

	/**
	 * Returns the currently registered Crafting UI Manager.
	 *
	 * @return The active Crafting UI Manager instance, or nullptr if none is registered.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Crafting|Subsystem",
		meta=(MounteaGetter),
		DisplayName="Get Crafting UI Manager (Interface)")
	TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface> GetCraftingUIManager_Interface() const;

private:

	// Reference to Crafting UI Manager, usually lives withing Player Pawn.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Managers",
		meta=(AllowPrivateAccess))
	TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface> CraftingUIManager;
};
