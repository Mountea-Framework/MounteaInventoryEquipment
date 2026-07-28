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
#include "Components/ActorComponent.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantUIInterface.h"
#include "Interfaces/UserInterface/MounteaAdvancedCustomItemsMapInterface.h"
#include "Interfaces/Widgets/Category/MounteaCategoryWidgetEventRegistrarInterface.h"
#include "Interfaces/Widgets/Items/MounteaItemWidgetEventRegistrarInterface.h"
#include "MounteaCraftingParticipantUIComponent.generated.h"

class IMounteaAdvancedCraftingParticipantInterface;
class UMounteaAdvancedInventorySharedHUDSubsystem;

/**
 * UMounteaCraftingParticipantUIComponent represents an actor capable of displaying the crafting interface.
 * It shall be assigned only to players, as NPCs do not need to show user interface for crafting.
 * The purpose is to separate UI and functionality, as UI will be doing nothing but requesting the logic.
 *
 * @see IMounteaAdvancedCraftingParticipantUIInterface
 * @see UMounteaCraftingStationComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Crafting","Mountea|Crafting"),
	AutoCollapseCategories=("Variable,Sockets,Tags,Component Tick,Component Replication,Activation,Events,Replication,Asset User Data,Navigation"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Crafting Participant UI Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingParticipantUIComponent : public UActorComponent, public IMounteaAdvancedCraftingParticipantUIInterface, public IMounteaAdvancedCustomItemsMapInterface, public IMounteaCategoryWidgetEventRegistrarInterface, public IMounteaItemWidgetEventRegistrarInterface
{
	GENERATED_BODY()

public:
	
	UMounteaCraftingParticipantUIComponent();

public:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	
	virtual bool CreateCraftingWidget_Implementation() override;
	virtual UUserWidget* GetCraftingWidget_Implementation() const override
	{
		return CraftingWidget;
	}
	virtual void RemoveCraftingWidget_Implementation() override;
	virtual bool SetCraftingWidget_Implementation(UUserWidget* NewCraftingWidget) override;
	virtual TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> GetParentCraftingParticipant_Implementation() const override
	{
		return CraftingParticipant;
	}
	virtual bool SetCraftingParticipant_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant) override;
	virtual bool CraftingCategorySelected_Implementation(const FString& SelectedCategoryId) override;
	virtual FString GetCraftingSelectedCategoryId_Implementation() const override
	{
		return SelectedCategory;
	}
	virtual bool CraftableItemSelected_Implementation(const FGuid& NewSelectedItem) override;
	virtual FGuid GetActiveCraftableItemGuid_Implementation() const override
	{
		return SelectedItem;
	}

	virtual TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap_Implementation() const override
	{
		return CustomItemsMap;
	}
	virtual void AddCustomItemToMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) override;
	virtual void AppendCustomItemsMap_Implementation(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems) override;
	virtual void ClearCustomItemsMap_Implementation() override;
	virtual bool RemoveCustomItemFromMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) override;
	virtual bool IsItemStoredInCustomMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) override;

	virtual bool BindCategoryWidgetSelected_Implementation(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding) override;
	virtual bool UnbindCategoryWidgetSelected_Implementation(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding) override;
	virtual bool TriggerCategoryWidgetSelected_Implementation(UObject* CategoryWidget, const FString& CategoryId) override;

	virtual bool BindItemWidgetSelected_Implementation(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding) override;
	virtual bool UnbindItemWidgetSelected_Implementation(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding) override;
	virtual bool TriggerItemWidgetSelected_Implementation(UObject* ItemWidget, const FGuid& ItemId) override;

	virtual FCraftingUIScreenOpened& GetOnUIScreenOpenedHandle() override
	{ return OnUIScreenOpened; };
	virtual FCraftingUIScreenClosed& GetOnUIScreenClosedHandle() override
	{ return OnUIScreenClosed; };
	virtual FCraftingCategorySelected& GetOnCraftingCategorySelectedHandle() override
	{ return OnCraftingCategorySelected; };
	virtual FCraftableItemSelected& GetOnCraftableItemSelectedHandle() override
	{ return OnCraftableItemSelected; };

protected:

	UMounteaAdvancedInventorySharedHUDSubsystem* GetSharedHUDSubsystem() const;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	FString SelectedCategory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	FGuid SelectedItem;

	// Custom stored map, can be used to store unique Craftable items, like Favourites etc.
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	TMap<FGameplayTag, FInventoryUICustomData> CustomItemsMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	TObjectPtr<UUserWidget> CraftingWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(AllowPrivateAccess),
		meta=(ExposeOnSpawn),
		meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> CraftingParticipant;

	// Per-Category-widget-instance OnCategorySelected listeners, keyed by the widget itself. Not a UPROPERTY:
	// UHT does not support delegate value types in reflected TMaps, and TWeakObjectPtr keys need no GC tracking.
	TMap<TWeakObjectPtr<UObject>, FOnCategoryWidgetSelected> CategoryWidgetSelectedRegistry;

	// Per-Item-widget-instance OnItemWidgetSelected listeners, keyed by the widget itself. Not a UPROPERTY:
	// UHT does not support delegate value types in reflected TMaps, and TWeakObjectPtr keys need no GC tracking.
	TMap<TWeakObjectPtr<UObject>, FOnItemWidgetSelected> ItemWidgetSelectedRegistry;

	/**
	 * Delegate triggered after the crafting UI screen has been successfully opened.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Crafting",
		meta=(DisplayName="Crafting UI - On UI Screen Opened"))
	FCraftingUIScreenOpened OnUIScreenOpened;

	/**
	 * Delegate triggered when the crafting UI screen is being closed after a valid widget was present.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Crafting",
		meta=(DisplayName="Crafting UI - On UI Screen Closed"))
	FCraftingUIScreenClosed OnUIScreenClosed;

	/**
	 * Delegate triggered when a Crafting category is selected in the Crafting UI.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Crafting",
		meta=(DisplayName="Crafting UI - On Category Selected"))
	FCraftingCategorySelected OnCraftingCategorySelected;

	/**
	 * Delegate triggered when a Craftable item is selected in the Crafting UI.
	 */
	UPROPERTY(BlueprintAssignable, Category="Mountea|Crafting",
		meta=(DisplayName="Crafting UI - On Craftable Item Selected"))
	FCraftableItemSelected OnCraftableItemSelected;
};
