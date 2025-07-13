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
 * UMounteaAttachmentContainerComponent manages dynamic attachment systems for equipment at runtime.
 * Container components provide slot-based attachment management with network replication, event broadcasting,
 * and comprehensive attachment operations for flexible equipment systems on actors.
 *
 * @see [Attachment Containers](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see IMounteaAdvancedAttachmentContainerInterface
 * @see UMounteaAdvancedAttachmentSlot
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
    AutoExpandCategories=("Mountea","AttachableContainer","Mountea|AttachableContainer"),
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
	void ServerTryAttach(const FName& SlotId, UObject* Attachment);
	bool TryAttachInternal(const FName& SlotId, UObject* Attachment);
	UFUNCTION(Server, Reliable)
	void ServerTryDetach(const FName& SlotId);
	
public:
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Attachment Container",
		meta=(GetOptions="GetAvailableTargetNames"))
	FName DefaultAttachmentTarget;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Attachment Container")
	TObjectPtr<USceneComponent> DefaultAttachmentTargetComponent = nullptr;
	
	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category="Attachment Container")
	EAttachmentSlotState State;
	
	// Does not support runtime addition/removal of slots.
	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite,
		Category="Attachment Container",
		Instanced,
		meta=(TitleProperty="DisplayName"),
		meta=(ForceInlineRow, ShowInnerProperties, ShowTreeView))
	TArray<TObjectPtr<UMounteaAdvancedAttachmentSlot>> AttachmentSlots;

	/**
	 * Event triggered when an attachment is added or removed from the container.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Attachment Container",
		meta = (IsBindableEvent=true))
	FOnAttachmentChanged OnAttachmentChanged;

	/**
	 * Event triggered when the state of a slot changes.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Attachment Container",
		meta = (IsBindableEvent=true))
	FOnSlotStateChanged OnSlotStateChanged;

	/**
	 * Event triggered when the container is cleared of all attachments.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Attachment Container",
		meta = (IsBindableEvent=true))
	FOnContainerCleared OnContainerCleared;

protected:

	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;

protected:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	
#endif
};
