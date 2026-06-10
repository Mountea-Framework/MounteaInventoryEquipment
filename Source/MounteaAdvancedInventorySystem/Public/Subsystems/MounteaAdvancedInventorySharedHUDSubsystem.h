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
#include "Interfaces/UserInterface/MounteaAdvancedInventorySharedHUDInterface.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "MounteaAdvancedInventorySharedHUDSubsystem.generated.h"

class UMounteaAdvancedInventoryGlobalUIConfig;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea),
	meta=(DisplayName="Mountea Inventory & Equipment Shared HUD Subsystem"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySharedHUDSubsystem : public ULocalPlayerSubsystem, public IMounteaAdvancedInventorySharedHUDInterface
{
	GENERATED_BODY()
	
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual bool CreateWrapperWidget_Implementation() override;
	virtual UUserWidget* GetWrapperWidget_Implementation() const override
	{
		return SharedUserInterface;
	};
	virtual void RemoveWrapperWidget_Implementation() override;
	
	virtual UWidget* GetNotificationContainer_Implementation() const override;
	virtual void SetNotificationContainer_Implementation(UWidget* NewNotificationContainer) override;
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) override;
	virtual void RemoveInventoryNotifications_Implementation() override;
	
	virtual void ExecuteWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload) override;
	
	virtual FGameplayTagContainer GetWidgetStates_Implementation() const override
	{
		return WidgetStatesContainer;
	};
	virtual void SetWidgetStates_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& NewStates) override;
	virtual bool AddWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag) override;
	virtual bool RemoveWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag) override;
	virtual bool HasWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag, 
		bool bExactMatch) const override;
	virtual void ClearWidgetStateTags_Implementation() override { WidgetStatesContainer.Reset(); };
	virtual bool AppendWidgetStateTags_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& TagsToAppend) override;
	
protected:
	
	/**
	 * This container defines what states the Manager has.
	 * Imagine this as a flag container, where each "major" widget adds its flag as long as it exists.
	 * This container defines whether Wrapper is empty or not.
	 */
	UPROPERTY(BlueprintReadOnly, Category="HUD",
		meta=(Categories="Mountea_Inventory.WidgetState,State"))
	FGameplayTagContainer WidgetStatesContainer;
	
	UPROPERTY(BlueprintReadOnly, Category="HUD")
	TObjectPtr<UWidget> InventoryNotificationContainerWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="HUD")
	TObjectPtr<UUserWidget> SharedUserInterface;
	
	UPROPERTY(BlueprintReadOnly, Category="HUD")
	TObjectPtr<UMounteaAdvancedInventoryGlobalUIConfig> UIConfig;
};
