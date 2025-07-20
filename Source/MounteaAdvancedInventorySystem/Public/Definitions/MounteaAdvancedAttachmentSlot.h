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
#include "MounteaAdvancedAttachmentSlotBase.h"
#include "MounteaEquipmentBaseEnums.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
#include "UObject/Object.h"

#include "MounteaAdvancedAttachmentSlot.generated.h"

class UMounteaAttachableComponent;
enum class EAttachmentSlotState : uint8;
enum class EAttachmentSlotType : uint8;

/**
 * UMounteaAdvancedAttachmentSlot is a concrete attachment slot implementation for equipment systems.
 * Attachment slots define specific locations where equipment can be attached to actors, supporting
 * socket-based and component-based attachment methods with gameplay tag filtering and network replication.
 *
 * @see [Attachment System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see UMounteaAdvancedAttachmentSlotBase
 * @see IMounteaAdvancedAttachmentContainerInterface
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
public:
	// Editor validation
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};