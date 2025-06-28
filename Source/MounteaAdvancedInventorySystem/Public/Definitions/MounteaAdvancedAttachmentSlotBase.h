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
UCLASS(Abstract, ClassGroup=(Mountea), AutoExpandCategories="Mountea",
	EditInlineNew)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedAttachmentSlotBase : public UObject
{
	GENERATED_BODY()

public:

	UMounteaAdvancedAttachmentSlotBase();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableSlotNames"),
		meta=(DisplayPriority=-1, NoResetToDefault))
	FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	FGameplayTagContainer SlotTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	EAttachmentSlotState State;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	EAttachmentSlotType SlotType;

protected:

	UFUNCTION()
	virtual TArray<FName> GetAvailableSlotNames() const;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
