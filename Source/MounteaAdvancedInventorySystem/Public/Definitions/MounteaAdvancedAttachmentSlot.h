// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaEquipmentBaseDataTypes.h"
#include "MounteaEquipmentBaseEnums.h"
#include "UObject/Object.h"
#include "MounteaAdvancedAttachmentSlot.generated.h"

class IMounteaAdvancedAttachmentContainerInterface;
class UMounteaAttachableComponent;
enum class EAttachmentSlotState : uint8;
enum class EAttachmentSlotType : uint8;

/**
 * Represents a single attachment slot in an equipment system.
 * Defines the properties of an attachment slot, including its ID, tags, display name, State, type, and target name.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable,
	AutoExpandCategories="Mountea",
	EditInlineNew)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedAttachmentSlot : public UObject
{
	GENERATED_BODY()

public:

	UMounteaAdvancedAttachmentSlot();
	explicit UMounteaAdvancedAttachmentSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& NewParentContainer);

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FGameplayTagContainer SlotTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	EAttachmentSlotState State;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	EAttachmentSlotType SlotType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FName TargetComponentOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(EditCondition="SlotType==EAttachmentSlotType::EAST_Socket"))
	FName TargetName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Debug",
		meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> ParentContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Debug",
		meta=(DisplayThumbnail=false))
	TObjectPtr<UMounteaAttachableComponent> Attachment;

public:

	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;

	FORCEINLINE bool IsSlotValid() const
	{
		return !DisplayName.IsEmpty();
	}

	bool IsEmpty() const;

	bool IsOccupied() const;

	void DisableSlot();

	bool IsLocked() const;
	

	FORCEINLINE bool CanAttach() const
	{
		return IsSlotValid() && IsEmpty() && !IsLocked();
	}

	bool Attach(UMounteaAttachableComponent* NewAttachment);

	bool Detach();

	FORCEINLINE bool HasTag(const FGameplayTag& Tag) const
	{
		return SlotTags.HasTag(Tag);
	}

	FORCEINLINE bool MatchesTags(const FGameplayTagContainer& Tags, const bool bRequireAll) const
	{
		return bRequireAll ? SlotTags.HasAll(Tags) : SlotTags.HasAny(Tags);
	}
};
