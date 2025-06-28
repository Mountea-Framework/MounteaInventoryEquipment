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
	virtual bool TryAttach_Implementation(const FName& SlotId, UObject* Attachment) override;
	virtual bool TryDetach_Implementation(const FName& SlotId) override;
	virtual bool ForceAttach_Implementation(const FName& SlotId, UObject* Attachment) override;
	virtual bool ForceDetach_Implementation(const FName& SlotId) override;
	virtual FName FindFirstFreeSlotWithTags_Implementation(const FGameplayTagContainer& RequiredTags) const override;
	virtual FName GetSlotIdForAttachable_Implementation(const UMounteaAttachableComponent* Attachable) const override;
	virtual FName GetFirstEmptySlot_Implementation() const override;
	virtual void ClearAll_Implementation() override;
	void ApplyParentContainer();

public:
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Attachment",
		meta=(GetOptions="GetAvailableTargetNames"))
	FName DefaultAttachmentTarget;
	
	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category="Attachment")
	EAttachmentSlotState State;
	
	// Does not support runtime addition/removal of slots.
	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category="Attachment",
		Instanced,
		meta=(TitleProperty="DisplayName"),
		meta=(ForceInlineRow, ShowInnerProperties, ShowTreeView))
	TArray<TObjectPtr<UMounteaAdvancedAttachmentSlot>> AttachmentSlots;

protected:

	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	
#endif
};
