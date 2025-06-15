// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "MounteaAttachmentContainerComponent.generated.h"

enum class EAttachmentSlotState : uint8;
class UMounteaAdvancedAttachmentSlot;

/**
 * Component that holds an attachment container for equipment systems.
 * This component allows actors to manage attachments dynamically at runtime.
 * It can be used to equip, unequip, and manage attachments on actors.
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

public:

	virtual AActor* GetOwningActor_Implementation() const override;
	virtual FName GetDefaultAttachmentTarget_Implementation() const override { return DefaultAttachmentTarget; }
	virtual bool IsValidSlot_Implementation(const FName& SlotId) const override;
	virtual UMounteaAdvancedAttachmentSlot* GetSlot_Implementation(const FName& SlotId) const override;
	virtual bool IsSlotOccupied_Implementation(const FName& SlotId) const override;
	virtual bool DisableSlot_Implementation(const FName& SlotId) override;
	virtual bool TryAttach_Implementation(const FName& SlotId, UMounteaAttachableComponent* Attachment) override;
	virtual bool TryDetach_Implementation(const FName& SlotId) override;
	virtual bool ForceAttach_Implementation(const FName& SlotId, UMounteaAttachableComponent* Attachment) override;
	virtual bool ForceDetach_Implementation(const FName& SlotId) override;
	virtual FName FindFirstFreeSlotWithTags_Implementation(const FGameplayTagContainer& RequiredTags) const override;
	virtual FName GetSlotIdForAttachable_Implementation(const UMounteaAttachableComponent* Attachable) const override;
	virtual void ClearAll_Implementation() override;
	void ApplyParentContainer();

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment",
		meta=(GetOptions="GetAvailableTargetNames"))
	FName DefaultAttachmentTarget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	EAttachmentSlotState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment",
		Instanced,
		meta=(ForceInlineRow), meta=(TitleProperty="DisplayName"), meta=(ShowInnerProperties))
	TMap<FName, TObjectPtr<UMounteaAdvancedAttachmentSlot>> AttachmentSlots;

protected:

	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;

protected:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
};
