// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaEquipmentBaseEnums.h"
#include "MounteaEquipmentBaseDataTypes.generated.h"

enum class EAttachmentSlotType : uint8;
enum class EAttachmentSlotState : uint8;
class UMounteaAttachableComponent;

/**
 * Represents a single attachment slot in an equipment system.
 * This structure defines the properties of an attachment slot, including its ID, tags, display name, State, type, and target name.
 */
USTRUCT(BlueprintType)
struct FAttachmentSlot
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Attachment")
	TObjectPtr<UMounteaAttachableComponent> Attachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FGameplayTagContainer SlotTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Attachment")
	EAttachmentSlotState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	EAttachmentSlotType SlotType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FName TargetName;

public:

	FORCEINLINE bool IsValid() const
	{
		return !DisplayName.IsEmpty();
	}

	FORCEINLINE bool IsEmpty() const
	{
		return State == EAttachmentSlotState::EASS_Empty && Attachment == nullptr;
	}

	FORCEINLINE bool IsOccupied() const
	{
		return State == EAttachmentSlotState::EASS_Occupied && Attachment != nullptr;
	}

	FORCEINLINE void DisableSlot()
	{
		if (!IsEmpty())
			Detach();
		State = EAttachmentSlotState::EASS_Locked;
	}

	FORCEINLINE bool IsLocked() const
	{
		return State == EAttachmentSlotState::EASS_Locked;
	}

	FORCEINLINE bool CanAttach() const
	{
		return IsValid() && IsEmpty() && !IsLocked();
	}

	FORCEINLINE bool Attach(UMounteaAttachableComponent* NewAttachment)
	{
		if (!CanAttach() || !NewAttachment)
			return false;

		Attachment = NewAttachment;
		State = EAttachmentSlotState::EASS_Occupied;
		return true;
	}

	FORCEINLINE bool Detach()
	{
		if (!IsOccupied())
			return false;

		Attachment = nullptr;
		State = EAttachmentSlotState::EASS_Empty;
		return true;
	}

	FORCEINLINE bool HasTag(const FGameplayTag& Tag) const
	{
		return SlotTags.HasTag(Tag);
	}

	FORCEINLINE bool MatchesTags(const FGameplayTagContainer& Tags, bool bRequireAll) const
	{
		return bRequireAll ? SlotTags.HasAll(Tags) : SlotTags.HasAny(Tags);
	}
};
