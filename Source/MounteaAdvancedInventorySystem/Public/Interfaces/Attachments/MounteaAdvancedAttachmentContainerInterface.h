// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "MounteaAdvancedAttachmentContainerInterface.generated.h"

class UMounteaAttachableComponent;
class UMounteaAdvancedAttachmentSlot;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedAttachmentContainerInterface : public UInterface
{
	GENERATED_BODY()
};

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
	bool TryAttach(const FName& SlotId, UMounteaAttachableComponent* Attachment);
	virtual bool TryAttach_Implementation(const FName& SlotId, UMounteaAttachableComponent* Attachment) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool TryDetach(const FName& SlotId);
	virtual bool TryDetach_Implementation(const FName& SlotId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	bool ForceAttach(const FName& SlotId, UMounteaAttachableComponent* Attachment);
	virtual bool ForceAttach_Implementation(const FName& SlotId, UMounteaAttachableComponent* Attachment) = 0;

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
	void ClearAll();
	virtual void ClearAll_Implementation() = 0;
};
