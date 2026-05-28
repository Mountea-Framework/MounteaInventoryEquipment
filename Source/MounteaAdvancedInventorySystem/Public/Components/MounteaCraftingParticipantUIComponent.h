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
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingParticipantUIComponent : public UActorComponent, public IMounteaAdvancedCraftingParticipantUIInterface
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

protected:

	UMounteaAdvancedInventorySharedHUDSubsystem* GetSharedHUDSubsystem() const;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	FString SelectedCategory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	FGuid SelectedItem;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	TObjectPtr<UUserWidget> CraftingWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing", 
		meta=(AllowPrivateAccess), 
		meta=(ExposeOnSpawn),
		meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> CraftingParticipant;
};
