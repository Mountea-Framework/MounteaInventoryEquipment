// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "MounteaAdvancedAttachmentSlotBase.generated.h"

enum class EAttachmentSlotState : uint8;
enum class EAttachmentSlotType : uint8;

/**
 * Represents a single attachment slot in an equipment system.
 * Defines the properties of an attachment slot, including its ID, tags, display name, State, type, and target name.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable,
	AutoExpandCategories="Mountea",
	EditInlineNew)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedAttachmentSlotBase : public UObject
{
	GENERATED_BODY()

public:

	UMounteaAdvancedAttachmentSlotBase();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FGameplayTagContainer SlotTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	EAttachmentSlotState State;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	EAttachmentSlotType SlotType;
};
