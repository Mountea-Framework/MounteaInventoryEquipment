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
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "MounteaAdvancedAttachmentContainerInterface.generated.h"

class UMounteaAttachableComponent;
class UMounteaAdvancedAttachmentSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttachmentChanged, const FName&, SlotId, UObject*, NewAttachment, UObject*, OldAttachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotStateChanged, const FName&, SlotId, bool, bIsEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainerCleared);

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedAttachmentContainerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedAttachmentContainerInterface defines the contract for managing attachment slots and operations.
 * Container interfaces provide slot management, attachment validation, event broadcasting, and comprehensive
 * attachment system operations for equipment containers and inventory attachment systems.
 *
 * @see [Container Interface](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see IMounteaAdvancedAttachmentAttachableInterface
 * @see UMounteaAttachmentContainerComponent
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedAttachmentContainerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	FName GetDefaultAttachmentTarget() const;
	virtual FName GetDefaultAttachmentTarget_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	USceneComponent* GetAttachmentTargetComponent() const;
	virtual USceneComponent* GetAttachmentTargetComponent_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	void SetDefaultAttachmentTargetComponent(USceneComponent* NewTarget);
	virtual void SetDefaultAttachmentTargetComponent_Implementation(USceneComponent* NewTarget) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool IsValidSlot(const FName& SlotId) const;
	virtual bool IsValidSlot_Implementation(const FName& SlotId) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	UMounteaAdvancedAttachmentSlot* GetSlot(const FName& SlotId) const;
	virtual UMounteaAdvancedAttachmentSlot* GetSlot_Implementation(const FName& SlotId) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool IsSlotOccupied(const FName& SlotId) const;
	virtual bool IsSlotOccupied_Implementation(const FName& SlotId) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool DisableSlot(const FName& SlotId);
	virtual bool DisableSlot_Implementation(const FName& SlotId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool TryDetach(const FName& SlotId);
	virtual bool TryDetach_Implementation(const FName& SlotId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool TryAttach(const FName& SlotId, UObject* Attachment);
	virtual bool TryAttach_Implementation(const FName& SlotId, UObject* Attachment) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool ForceAttach(const FName& SlotId, UObject* Attachment);
	virtual bool ForceAttach_Implementation(const FName& SlotId, UObject* Attachment) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool ForceDetach(const FName& SlotId);
	virtual bool ForceDetach_Implementation(const FName& SlotId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	FName FindFirstFreeSlotWithTags(const FGameplayTagContainer& RequiredTags) const;
	virtual FName FindFirstFreeSlotWithTags_Implementation(const FGameplayTagContainer& RequiredTags) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	FName GetSlotIdForAttachable(const UMounteaAttachableComponent* Attachable) const;
	virtual FName GetSlotIdForAttachable_Implementation(const UMounteaAttachableComponent* Attachable) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	FName GetFirstEmptySlot() const;
	virtual FName GetFirstEmptySlot_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	void ClearAll();
	virtual void ClearAll_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	TArray<UMounteaAdvancedAttachmentSlot*> GetAttachmentSlots() const;
	virtual TArray<UMounteaAdvancedAttachmentSlot*> GetAttachmentSlots_Implementation() const = 0;

	virtual FOnAttachmentChanged& GetOnAttachmentChangedEventHandle() = 0;
	virtual FOnSlotStateChanged& GetOnSlotStateChangedEventHandle() = 0;
	virtual FOnContainerCleared& GetOnContainerClearedEventHandle() = 0;
};
