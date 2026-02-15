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
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "MounteaAdvancedAttachmentSlotBase.generated.h"

enum class EAttachmentSlotState : uint8;
enum class EAttachmentSlotType : uint8;
class IMounteaAdvancedAttachmentContainerInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotBeginPlaySignature);

/**
 * UMounteaAdvancedAttachmentSlotBase is the base class for attachment slots in equipment systems.
 * Attachment slots define locations where equipment can be attached to actors, with support for
 * gameplay tag filtering, state management, and network replication.
 *
 * @see [Attachment System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see UMounteaAdvancedAttachmentSlot
 * @see IMounteaAdvancedAttachmentContainerInterface
 */
UCLASS(Abstract, ClassGroup=(Mountea), AutoExpandCategories="Mountea", EditInlineNew)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedAttachmentSlotBase : public UObject
{
	GENERATED_BODY()

public:
	UMounteaAdvancedAttachmentSlotBase();

public:
	
	/** Unique identifier for this attachment slot */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableSlotNames"),
		meta=(DisplayPriority=-1),
		meta=(NoResetToDefault))
	FName SlotName;

	/** Gameplay tags that define what can attach to this slot */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1),
		meta=(NoResetToDefault))
	FGameplayTagContainer SlotTags;

	/** Human-readable name for this slot displayed in UI */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1),
		meta=(NoResetToDefault))
	FText DisplayName;

	/** Current state of the attachment slot (Empty, Occupied, Locked) */
	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_State, VisibleAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1),
		meta=(NoResetToDefault))
	EAttachmentSlotState State;
	
	/** Type of attachment this slot supports (Socket, Component) */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1),
		meta=(NoResetToDefault))
	EAttachmentSlotType SlotType;

	/** Reference to the container that owns this slot */
	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Debug", AdvancedDisplay,
		meta=(DisplayThumbnail=false),
		meta=(NoResetToDefault))
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> ParentContainer;
	
	/** Local cache of last attachment for detachment operations */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Debug", AdvancedDisplay,
		meta=(DisplayThumbnail=false),
		meta=(NoResetToDefault))
	TObjectPtr<UObject> LastAttachment;
	
	/** Currently attached object (replicated to clients) */
	UPROPERTY(SaveGame, Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Information",
		meta=(DisplayThumbnail=false),
		meta=(NoResetToDefault))
	TObjectPtr<UObject> Attachment;

protected:
	
	/** Event broadcasted when slot begins play */
	UPROPERTY(BlueprintAssignable, DisplayName="Begin Play")
	FSlotBeginPlaySignature SlotBeginPlay;

public:
	// Lifecycle
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachment")
	void BeginPlay();
	virtual void BeginPlay_Implementation();

	virtual void InitializeAttachmentSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Parent);
	virtual AActor* GetOwningActor() const;

public:
	
	virtual UWorld* GetWorld() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	
public:
	
	virtual bool Attach(UObject* NewAttachment);
	virtual bool ForceAttach(UObject* NewAttachment);
	virtual bool Detach();
	virtual bool ForceDetach();
	virtual void DisableSlot();
	
	FORCEINLINE virtual bool IsSlotValid() const
	{
		return ParentContainer.GetObject() != nullptr && (!SlotName.IsNone() || !SlotTags.IsEmpty());
	}
	FORCEINLINE virtual bool CanAttach() const
	{
		return IsSlotValid() && !IsLocked();
	}
	FORCEINLINE virtual bool CanDetach() const
	{
		return IsSlotValid() && IsOccupied();
	}
	FORCEINLINE virtual bool CanAttachAttachable(const UObject* NewAttachment) const;
	FORCEINLINE virtual bool HasTag(const FGameplayTag& Tag) const
	{
		return SlotTags.HasTag(Tag);
	}
	FORCEINLINE virtual bool MatchesTags(const FGameplayTagContainer& Tags, const bool bRequireAll) const
	{
		return bRequireAll ? SlotTags.HasAll(Tags) : SlotTags.HasAny(Tags);
	}
	
	virtual bool IsEmpty() const;
	virtual bool IsOccupied() const;
	virtual bool IsLocked() const;

protected:
	
	UFUNCTION()
	virtual void OnRep_State();

protected:
	
	UFUNCTION()
	virtual TArray<FName> GetAvailableSlotNames() const;

#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};