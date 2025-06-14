// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
//#include "MounteaEquipmentBaseDataTypes.generated.h"

enum class EAttachmentSlotType : uint8;
enum class EAttachmentSlotState : uint8;

/**
 * Represents a single attachment slot in an equipment system.
 * This structure defines the properties of an attachment slot, including its ID, tags, display name, state, type, and target name.
 */
USTRUCT(BlueprintType)
struct FAttachmentSlot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	TObjectPtr<UObject> Attachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FGameplayTagContainer SlotTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	EAttachmentSlotState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	EAttachmentSlotType SlotType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FName TargetName;
};

/**
 * Container for attachment slots used in equipment systems.
 * This structure holds multiple attachment slots, each with its own state and properties.
 * It allows for flexible attachment management, enabling items to be equipped, unequipped, or locked in place.
 */
USTRUCT(BlueprintType)
struct FAttachmentContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FName DefaultAttachmentTarget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	EAttachmentSlotState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	TMap<FName, FAttachmentSlot> AttachmentSlots;
};
