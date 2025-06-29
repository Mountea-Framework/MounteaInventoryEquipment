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

public:
	/** Override the default attachment target component by name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableTargetNames", NoResetToDefault))
	FName AttachmentTargetOverride;

	/** Resolved component reference for attachment target override */
	UPROPERTY(BlueprintReadOnly, Category="Settings")
	TObjectPtr<USceneComponent> AttachmentTargetComponentOverride = nullptr;

	/** Socket name to attach to when using socket-based attachment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableSocketNames", NoResetToDefault),
		meta=(EditCondition="SlotType==EAttachmentSlotType::EAST_Socket", EditConditionHides))
	FName SocketName;

protected:
	virtual void BeginPlay_Implementation() override;

public:
	// Editor dropdown helpers
	UFUNCTION()
	TArray<FName> GetAvailableSocketNames() const;
	
	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;

public:
	// State queries
	FORCEINLINE bool IsSlotValid() const
	{
		return ParentContainer.GetObject() != nullptr && !SlotName.IsNone() && !SlotTags.IsEmpty();
	}

	FORCEINLINE bool CanAttach() const
	{
		return IsSlotValid() && IsEmpty() && !IsLocked();
	}

	bool IsEmpty() const;
	bool IsOccupied() const;
	bool IsLocked() const;

public:
	// Attachment operations
	bool Attach(UObject* NewAttachment);
	bool ForceAttach(UObject* NewAttachment);
	bool Detach();
	bool ForceDetach();
	void DisableSlot();

public:
	// Utility functions
	FORCEINLINE bool HasTag(const FGameplayTag& Tag) const
	{
		return SlotTags.HasTag(Tag);
	}

	FORCEINLINE bool MatchesTags(const FGameplayTagContainer& Tags, const bool bRequireAll) const
	{
		return bRequireAll ? SlotTags.HasAll(Tags) : SlotTags.HasAny(Tags);
	}

	FName GetAttachmentSocketName() const;
	static TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> FindAttachableInterface(UObject* Object);
	static bool IsValidForAttachment(const UObject* NewAttachment);

private:
	// Internal helpers
	void TryResolveAttachmentTarget();
	USceneComponent* GetAttachmentTargetComponent() const;
	bool PerformAttachmentLogic(UObject* NewAttachment);
	void HandleAttachableInterface(UObject* NewAttachment);
	void PerformDetachment();
	bool ValidateAttachmentSlot(const USceneComponent* Target) const;
	bool PerformPhysicalAttachment(UObject* Object, USceneComponent* Target) const;

protected:
	// Replication
	virtual void OnRep_State() override;

#if WITH_EDITOR
protected:
	// Editor validation
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};