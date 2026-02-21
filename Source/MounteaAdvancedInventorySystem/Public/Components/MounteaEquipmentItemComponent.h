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
#include "MounteaAttachableComponent.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentItemInterface.h"
#include "MounteaEquipmentItemComponent.generated.h"


/**
 * UMounteaEquipmentItemComponent represents items that can be equipped by actors or components.
 * Equipment components inherits all behaviour from Attachable Component, and extends the behaviour
 * by providing Equipment State logic.
 *
 * @see [Equipment Components](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/EquipmentSystem)
 * @see IMounteaAdvancedEquipmentItemInterface
 * @see UMounteaAttachableComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Attachable","Mountea|Attachable","Mountea|Equipment","Equipment"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Equipment Item"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaEquipmentItemComponent : public UMounteaAttachableComponent, public IMounteaAdvancedEquipmentItemInterface
{
	GENERATED_BODY()
	
public:
	
	UMounteaEquipmentItemComponent();
	
public:
	
	virtual EEquipmentItemState GetEquipmentItemState_Implementation() const override
	{ return EquipmentItemState; };
	virtual bool SetEquipmentItemState_Implementation(const EEquipmentItemState NewEquipmentItemState) override;
	virtual bool DoesAutoActive_Implementation() const override
	{ return bAutoActivates; };
	virtual bool SetAutoActive_Implementation(const bool bValue) override;
	virtual bool DoesRequireActivationEvent_Implementation() const override
	{ return bRequiresActivationEvent; };
	virtual bool SetRequiresActivationEvent_Implementation(const bool bValue) override;
	virtual UAnimationAsset* GetActivationAnimation_Implementation() const override;
	virtual bool SetActivationAnimation_Implementation(UAnimationAsset* NewActivateAnimation) override;
	virtual FOnEquipmentItemStateChanged& GetOnEquipmentItemStateChangedHandle() override
	{ return OnEquipmentItemStateChanged; };
	virtual FName GetEquipmentItemPreferredSlot_Implementation() const override
	{ return PreferredSlot; };
	virtual FGameplayTag GetEquipmentPreferredSlotTag_Implementation() const override
	{ return PreferredSlotTag; };
	virtual FGuid GetEquippedItemId_Implementation() const override
	{ return EquippedItemId; };
	virtual void SetEquippedItemId_Implementation(const FGuid& NewEquippedItemId) override;
	
protected:
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault),
		meta=(EditCondition="!bAutoActivates && bRequiresActivationEvent"),
		meta=(AllowedClasses="/Script/Engine.AnimSequence,/Script/Engine.AnimMontage"),
		meta=(DisplayPriority=5))
	TSoftObjectPtr<UAnimationAsset> ActivationAnimation;
	
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=4))
	EEquipmentItemState EquipmentItemState;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault),
		meta=(GetOptions="GetAvailableSlots"),
		meta=(DisplayPriority=2))
	FName PreferredSlot = NAME_None;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault),
		meta=(Categories="Mountea_Inventory.AttachmentSlots,Slot,Attachment"),
		meta=(DisplayPriority=3))
	FGameplayTag PreferredSlotTag;

	/**
	 * Even Triggered when Equipment Item changes.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Equipment",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=8))
	FOnEquipmentItemStateChanged OnEquipmentItemStateChanged;

	// Defined the Equipped Item Id from Inventory
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=1))
	FGuid EquippedItemId;

	// If Item does activate when equipped (eg: Rings), like those Items which require no animation to activate.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=6))
	uint8 bAutoActivates : 1;

	/**
	 * Defines whether this Item requires an Event to be activated.
	 * 
	 * Example: 
	 * The equipped weapon (sword) is sheathed. To activate the Equipment Item should play some animation.
	 * That means this weapon will set this value to YES.
	 * 
	 * Editable only if Auto Activates is FALSE.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Mountea|Equipment",
		meta=(NoResetToDefault),
		meta=(EditCondition="!bAutoActivates"),
		meta=(DisplayPriority=7))
	uint8 bRequiresActivationEvent : 1;
	
public:
	
	UFUNCTION()
	static TArray<FName> GetAvailableSlots();
};
