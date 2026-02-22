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
#include "MounteaAttachmentContainerComponent.h"
#include "Components/ActorComponent.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentItemInterface.h"
#include "MounteaEquipmentComponent.generated.h"

class UAnimMontage;
class UAnimInstance;
class UMounteaAdvancedAttachmentSlot;
class IMounteaAdvancedEquipmentItemInterface;

struct FPendingEquipmentActivation
{
	FGuid ItemGuid;
	FName SourceSlotId;
	FName TargetSlotId;
	TWeakObjectPtr<UAnimMontage> Montage;
	bool bIsActivating = true;

	bool IsValid() const { return ItemGuid.IsValid(); }
	void Reset() { *this = FPendingEquipmentActivation(); }
};

struct FEquipmentTransitionContext
{
	FGuid ItemGuid;
	FName ResolvedTargetSlotId = NAME_None;
	UMounteaAdvancedAttachmentSlot* CurrentSlot = nullptr;
	UMounteaAdvancedAttachmentSlot* TargetSlot = nullptr;
	TScriptInterface<IMounteaAdvancedEquipmentItemInterface> EquipItemInterface;
	EEquipmentItemState ExpectedState = EEquipmentItemState::EES_Idle;
	EEquipmentItemState NewState = EEquipmentItemState::EES_Idle;
	bool bResolveAsActivation = false;
	bool bNeedsSlotSwitch = false;
};

/**
 * UMounteaEquipmentComponent extends attachment containers with specialized equipment functionality.
 * Equipment components provide enhanced attachment management specifically designed for character equipment
 * systems with additional equipment-specific interfaces and behaviors.
 * Enhances Attachable System.
 *
 * @see [Equipment System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/EquipmentSystem)
 * @see UMounteaAttachmentContainerComponent
 * @see IMounteaAdvancedEquipmentInterface
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
    AutoExpandCategories=("Mountea","Equipment","Mountea|Equipment"),
    AutoCollapseCategories=("Variable,Sockets,Tags,Component Tick,Component Replication,Activation,Events,Replication,Asset User Data,Navigation"),
    HideCategories=("Cooking","Collision"),
    meta=(BlueprintSpawnableComponent, DisplayName="Mountea Equipment Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaEquipmentComponent : public UMounteaAttachmentContainerComponent, public IMounteaAdvancedEquipmentInterface
{
	GENERATED_BODY()

public:

	UMounteaEquipmentComponent();
	
public:
	
	virtual AActor* EquipItem_Implementation(const FInventoryItem& ItemDefinition) override;
	virtual AActor* EquipItemToSlot_Implementation(const FName& SlotId, const FInventoryItem& ItemDefinition) override;
	virtual bool UnequipItem_Implementation(const FInventoryItem& ItemDefinition, bool bUseFallbackSlot = false) override;
	virtual bool UnequipItemFromSlot_Implementation(const FName& SlotId, bool bUseFallbackSlot = false) override;
	virtual bool IsEquipmentItemEquipped_Implementation(const FInventoryItem& ItemDefinition) const override;
	virtual bool IsEquipmentItemEquippedInSlot_Implementation(const FInventoryItem& ItemDefinition, const FName& SlotName) const override;
	virtual bool ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId) override;
	virtual bool DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId) override;
	virtual bool AnimAttachItem_Implementation() override;

protected:

	bool BuildEquipmentTransitionContext(const FGuid& ItemGuid, const FName& TargetSlotId, EEquipmentItemState ExpectedState,
		EEquipmentItemState NewState, bool bResolveAsActivation, FEquipmentTransitionContext& OutContext);
	bool ExecuteEquipmentStateTransition(const FEquipmentTransitionContext& Context, bool bLocalOnly = false);
	UAnimInstance* ResolveOwnerAnimInstance() const;
	bool TryStartTransitionMontage(const FInventoryItem& ItemDefinition, const FEquipmentTransitionContext& Context, bool bIsActivating);
	
	bool IsAuthority() const;
	UFUNCTION(Server, Reliable)
	void Server_EquipItem(const FInventoryItem& ItemDefinition);
	UFUNCTION(Server, Reliable)
	void Server_EquipItemToSlot(const FInventoryItem& ItemDefinition, const FName& SlotId);
	UFUNCTION(Server, Reliable)
	void Server_UnequipItem(const FInventoryItem& ItemDefinition, const bool bUseFallbackSlot);
	UFUNCTION(Server, Reliable)
	void Server_ActivateEquipmentItem(const FInventoryItem& ItemDefinition, const FName& TargetSlotId);
	UFUNCTION(Server, Reliable)
	void Server_DeactivateEquipmentItem(const FInventoryItem& ItemDefinition, const FName& TargetSlotId);
	UFUNCTION(Server, Reliable)
	void Server_AnimAttachItem(const FGuid& ItemGuid, const FName& TargetSlotId, bool bIsActivating);

	FPendingEquipmentActivation PendingActivation;

	UFUNCTION()
	void OnActivationMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:

	UPROPERTY(EditAnywhere, Category="Mountea|Equipment",
		meta=(FunctionReference, AllowFunctionLibraries, PrototypeFunction="/Script/MounteaAdvancedInventorySystem.MounteaEquipmentStatics.Prototype_EquipItem"),
		meta=(DefaultBindingName="On Equip Item Requested"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=0))
	FMemberReference OverrideEquipItemFunction;
};
