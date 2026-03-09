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
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "MounteaEquipmentComponent.generated.h"

class UAnimMontage;
class UAnimInstance;
class UMounteaAdvancedAttachmentSlot;
class AActor;

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
	virtual bool ActivateQuickUseItem_Implementation(const FName& SlotId, const FName& TargetSlotId) override;
	virtual bool DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId) override;
	virtual bool AnimAttachItem_Implementation() override;
	virtual bool AnimQuickItemUsed_Implementation() override;
	virtual bool TryGetPendingEquipmentActivation(FPendingEquipmentActivation& OutPendingActivation) const override;

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Register Quick Use Placeholder Actor")
	bool RegisterQuickUsePlaceholderActor(const FGuid& ItemGuid, AActor* PlaceholderActor);

protected:

	bool BuildEquipmentTransitionContext(const FGuid& ItemGuid, const FName& TargetSlotId, EEquipmentItemState ExpectedState,
		EEquipmentItemState NewState, bool bResolveAsActivation, FEquipmentTransitionContext& OutContext);
	bool BuildTransitionContextForType(const FGuid& ItemGuid, const FName& TargetSlotId,
		EEquipmentTransitionType TransitionType, FEquipmentTransitionContext& OutContext);
	bool ResolveTransitionDefinition(EEquipmentTransitionType TransitionType, EEquipmentItemState& OutExpectedState,
		EEquipmentItemState& OutNewState, bool& OutResolveAsActivation) const;
	bool ExecuteEquipmentStateTransition(const FEquipmentTransitionContext& Context, bool bLocalOnly = false);
	bool ExecuteItemTransitionRequest(const FInventoryItem& ItemDefinition, const FName& TargetSlotId,
		EEquipmentTransitionType TransitionType);
	bool ShouldUseDeferredTransition(const FEquipmentTransitionContext& Context, EEquipmentTransitionType TransitionType) const;
	bool ValidateEquipRequest(const FInventoryItem& ItemDefinition) const;
	bool TryExecuteEquipOverride(const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor) const;
	void ArmPendingActivation(const FInventoryItem& ItemDefinition, const FEquipmentTransitionContext& Context,
		EEquipmentTransitionType TransitionType, UAnimMontage* Montage = nullptr, float MontageDuration = -1.f);
	double ResolvePendingTransitionTimeout(UAnimMontage* Montage, float MontageDuration) const;
	bool IsPendingActivationExpired() const;
	void ResetPendingActivationIfExpired();
	bool IsTransitionInProgress(EEquipmentTransitionType RequestedTransitionType = EEquipmentTransitionType::EET_None) const;
	void SetCurrentTransitionType(EEquipmentTransitionType NewTransitionType);
	void ResetCurrentTransitionType();
	UAnimInstance* ResolveOwnerAnimInstance() const;
	UAnimMontage* ResolveTransitionMontage(const FEquipmentTransitionContext& Context, EEquipmentTransitionType TransitionType) const;
	bool TryStartTransitionMontage(const FInventoryItem& ItemDefinition, const FEquipmentTransitionContext& Context,
		EEquipmentTransitionType TransitionType);
	
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
	void Server_AnimAttachItem(const FGuid& ItemGuid, const FName& TargetSlotId, EEquipmentTransitionType TransitionType);
	UFUNCTION(Server, Reliable)
	void Server_AnimQuickItemUsed(const FGuid& ItemGuid);

	bool ConsumeQuickUsePlaceholderActor(const FGuid& ItemGuid, bool bIgnoreItemGuidMismatch);

	FPendingEquipmentActivation PendingActivation;
	EEquipmentTransitionType CurrentTransitionType = EEquipmentTransitionType::EET_None;
	TWeakObjectPtr<AActor> QuickUsePlaceholderActor;
	FGuid QuickUsePlaceholderItemGuid;

	UFUNCTION()
	void OnTransitionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(EditAnywhere, Category="Mountea|Equipment",
		meta=(ClampMin="0.0", UIMin="0.0", ForceUnits="s"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=20))
	float PendingActivationTimeoutSeconds = 1.f;

	UPROPERTY(EditAnywhere, Category="Mountea|Equipment",
		meta=(ClampMin="0.0", UIMin="0.0"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=21))
	float PendingActivationTimeoutMarginPercent = 0.2f;

public:

	UPROPERTY(EditAnywhere, Category="Mountea|Equipment",
		meta=(FunctionReference, AllowFunctionLibraries, PrototypeFunction="/Script/MounteaAdvancedInventorySystem.MounteaEquipmentStatics.Prototype_EquipItem"),
		meta=(DefaultBindingName="On Equip Item Requested"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=0))
	FMemberReference OverrideEquipItemFunction;
};
