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

protected:
	
	virtual void BeginPlay() override;
	
public:
	
	virtual bool CreateWrapperWidget_Implementation() override;
	virtual virtual UUserWidget* GetWrapperWidget_Implementation() const override;
	virtual void RemoveWrapperWidget_Implementation() override;
	virtual bool CreateCraftingWidget_Implementation() override;
	virtual UUserWidget* GetCraftingWidget_Implementation() const override
	{
		return CraftingWidget;
	}
	virtual void RemoveCraftingWidget_Implementation() override;
	virtual bool SetCraftingWidget_Implementation(UUserWidget* NewCraftingWidget) override;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	TObjectPtr<UUserWidget> WrapperWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cratfing",
		meta=(NoResetToDefault))
	TObjectPtr<UUserWidget> CraftingWidget;
};
