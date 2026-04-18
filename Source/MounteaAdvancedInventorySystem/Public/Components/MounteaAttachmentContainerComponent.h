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
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "MounteaAttachmentContainerComponent.generated.h"

enum class EAttachmentSlotState : uint8;
class UMounteaAdvancedAttachmentSlot;

/**
 * UMounteaAttachmentContainerComponent is a runtime attachment container for actors.
 * It owns a set of attachment slots, resolves a default scene target, and handles attach/detach
 * operations while keeping state synchronized over the network. The component also exposes
 * events so gameplay code and UI can react to attachment and slot state changes.
 *
 * @see [Attachment Containers](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see IMounteaAdvancedAttachmentContainerInterface
 * @see UMounteaAdvancedAttachmentSlot
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
    AutoExpandCategories=("Mountea","AttachableContainer","Mountea|AttachableContainer"),
    AutoCollapseCategories=("Variable,Sockets,Tags,Component Tick,ComponentReplication,Activation,Events,Replication,AssetUserData,Navigation"),
    HideCategories=("Cooking","Collision"),
    meta=(BlueprintSpawnableComponent, DisplayName="Mountea Attachment Container Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAttachmentContainerComponent : public UActorComponent, public IMounteaAdvancedAttachmentContainerInterface
{
	GENERATED_BODY()

public:

	UMounteaAttachmentContainerComponent();

protected:
	
	virtual void BeginPlay() override;
	
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:

	virtual AActor* GetOwningActor_Implementation() const override;
	virtual FName GetDefaultAttachmentTarget_Implementation() const override
	{ return DefaultAttachmentTarget; }
	virtual USceneComponent* GetAttachmentTargetComponent_Implementation() const override
	{ return DefaultAttachmentTargetComponent; };
	virtual void SetDefaultAttachmentTargetComponent_Implementation(USceneComponent* NewTarget) override;
	virtual bool IsValidSlot_Implementation(const FName& SlotId) const override;
	virtual UMounteaAdvancedAttachmentSlot* GetSlot_Implementation(const FName& SlotId) const override;
	virtual bool IsSlotOccupied_Implementation(const FName& SlotId) const override;
	virtual bool DisableSlot_Implementation(const FName& SlotId) override;
	virtual bool TryAttach_Implementation(const FName& SlotId, UObject* Attachment) override;
	virtual bool TryDetach_Implementation(const FName& SlotId) override;
	virtual bool ForceAttach_Implementation(const FName& SlotId, UObject* Attachment) override;
	virtual bool ForceDetach_Implementation(const FName& SlotId) override;
	virtual FName FindFirstFreeSlotWithTags_Implementation(const FGameplayTagContainer& RequiredTags) const override;
	virtual FName GetSlotIdForAttachable_Implementation(const UMounteaAttachableComponent* Attachable) const override;
	virtual FName GetFirstEmptySlot_Implementation() const override;
	virtual void ClearAll_Implementation() override;
	virtual TArray<UMounteaAdvancedAttachmentSlot*> GetAttachmentSlots_Implementation() const override
	{ return AttachmentSlots; };
	void ApplyParentContainer();

	virtual FOnAttachmentChanged& GetOnAttachmentChangedEventHandle() override
	{ return OnAttachmentChanged; };
	virtual FOnSlotStateChanged& GetOnSlotStateChangedEventHandle() override
	{ return OnSlotStateChanged; };
	virtual FOnContainerCleared& GetOnContainerClearedEventHandle() override
	{ return OnContainerCleared; };

protected:

	UFUNCTION(Server, Reliable)
	void Server_TryAttach(const FName& SlotId, UObject* Attachment);
	bool TryAttachInternal(const FName& SlotId, UObject* Attachment);
	UFUNCTION(Server, Reliable)
	void Server_TryDetach(const FName& SlotId);
	
public:
	
	/**
	 * Slot definitions managed by this container.
	 *
	 * Each slot describes attachment rules, current occupancy, and target socket/component
	 * configuration. Slot objects are instanced on the component, persisted in SaveGame data,
	 * and replicated so attachment state is synchronized in multiplayer sessions.
	 */
	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachment Container",
		Instanced,
		meta=(TitleProperty="DisplayName"),
		meta=(NoResetToDefault),
		meta=(ForceInlineRow, ShowInnerProperties, ShowTreeView),
		meta=(DisplayPriority=2))
	TArray<TObjectPtr<UMounteaAdvancedAttachmentSlot>> AttachmentSlots;
	
	/**
	 * Name of the default scene component used as an attachment target.
	 *
	 * This value is resolved at runtime into DefaultAttachmentTargetComponent during BeginPlay.
	 * The options list is populated from the owning actor's available scene components.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachment Container",
		meta=(GetOptions="GetAvailableTargetNames"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=0))
	FName DefaultAttachmentTarget;

	/**
	 * Cached scene component resolved from DefaultAttachmentTarget.
	 *
	 * This runtime reference is used by slot attachment operations and is updated when
	 * SetDefaultAttachmentTargetComponent is called.
	 */
	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Mountea|Attachment Container",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=1))
	TObjectPtr<USceneComponent> DefaultAttachmentTargetComponent = nullptr;
	
	/**
	 * Broadcast when an attachment change occurs in any slot.
	 *
	 * Triggered after successful attach operations and provides slot identifier plus old/new
	 * attachment values for listeners.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Attachment Container",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FOnAttachmentChanged OnAttachmentChanged;

	/**
	 * Broadcast when a slot changes its enabled/disabled state.
	 *
	 * Typically emitted when slot availability is modified at runtime.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Attachment Container",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FOnSlotStateChanged OnSlotStateChanged;

	/**
	 * Broadcast when the container is cleared of attachments.
	 *
	 * Use this event to refresh UI, gameplay state, or dependent systems after mass detach
	 * operations.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Mountea|Attachment Container",
		meta=(IsBindableEvent=true),
		meta=(NoResetToDefault))
	FOnContainerCleared OnContainerCleared;

protected:

	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;

protected:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	
#endif
};
