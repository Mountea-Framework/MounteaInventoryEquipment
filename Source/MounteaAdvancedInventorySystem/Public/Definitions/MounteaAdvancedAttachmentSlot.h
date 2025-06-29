// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "MounteaAdvancedAttachmentSlotBase.h"
#include "MounteaEquipmentBaseEnums.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
#include "UObject/Object.h"

#include "MounteaAdvancedAttachmentSlot.generated.h"

class UMounteaAttachableComponent;
enum class EAttachmentSlotState : uint8;
enum class EAttachmentSlotType : uint8;

/**
 * Represents a single attachment slot in an equipment system.
 * Defines the properties of an attachment slot, including its ID, tags, display name, State, type, and target name.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, EditInlineNew)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedAttachmentSlot : public UMounteaAdvancedAttachmentSlotBase
{
	GENERATED_BODY()

public:

	UMounteaAdvancedAttachmentSlot();

protected:

	virtual void BeginPlay_Implementation() override;
	void TryResolveAttachmentTarget();

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableTargetNames"),
		meta=(NoResetToDefault))
	FName AttachmentTargetOverride;

	UPROPERTY(BlueprintReadOnly, Category="Settings")
	TObjectPtr<USceneComponent> AttachmentTargetComponentOverride = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableSocketNames"),
		meta=(EditCondition="SlotType==EAttachmentSlotType::EAST_Socket"),
		meta=(EditConditionHides),
		meta=(NoResetToDefault))
	FName SocketName;

public:

	UFUNCTION()
	TArray<FName> GetAvailableSocketNames() const;
	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;

	FORCEINLINE bool IsSlotValid() const
	{
		return  ParentContainer.GetObject() != nullptr
		&& !SlotName.IsNone()
		&& !SlotTags.IsEmpty();
	}

	bool IsEmpty() const;

	bool IsOccupied() const;

	void DisableSlot();

	bool IsLocked() const;
	

	FORCEINLINE bool CanAttach() const
	{
		return IsSlotValid() && IsEmpty() && !IsLocked();
	}

	bool Attach(UObject* NewAttachment);
	bool ForceAttach(UObject* NewAttachment);
	static TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> FindAttachableInterface(UObject* Object);
	static bool IsValidForAttachment(const UObject* NewAttachment);
	bool ValidateAttachmentSlot(const USceneComponent* Target) const;
	bool PerformPhysicalAttachment(UObject* Object, USceneComponent* Target) const;
	FName GetAttachmentSocketName() const;

	bool Detach();
	bool ForceDetach();
	void PerformPhysicalDetachment() const;

	FORCEINLINE bool HasTag(const FGameplayTag& Tag) const
	{
		return SlotTags.HasTag(Tag);
	}

	FORCEINLINE bool MatchesTags(const FGameplayTagContainer& Tags, const bool bRequireAll) const
	{
		return bRequireAll ? SlotTags.HasAll(Tags) : SlotTags.HasAny(Tags);
	}

	virtual void OnRep_State() override;

#if WITH_EDITOR

	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	
#endif
	
};
