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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableSlotNames", DisplayPriority=-1, NoResetToDefault))
	FName SlotName;

	/** Gameplay tags that define what can attach to this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	FGameplayTagContainer SlotTags;

	/** Human-readable name for this slot displayed in UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	FText DisplayName;

	/** Current state of the attachment slot (Empty, Occupied, Locked) */
	UPROPERTY(ReplicatedUsing=OnRep_State, VisibleAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	EAttachmentSlotState State;
	
	/** Type of attachment this slot supports (Socket, Component) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(DisplayPriority=-1, NoResetToDefault))
	EAttachmentSlotType SlotType;

	/** Reference to the container that owns this slot */
	UPROPERTY(BlueprintReadOnly, Category="Debug", AdvancedDisplay,
		meta=(DisplayThumbnail=false, NoResetToDefault))
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> ParentContainer;

	/** Currently attached object (replicated to clients) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Debug", AdvancedDisplay,
		meta=(DisplayThumbnail=false, NoResetToDefault))
	TObjectPtr<UObject> Attachment;

	/** Local cache of last attachment for detachment operations */
	UPROPERTY()
	TObjectPtr<UObject> LastAttachment;

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
	// UObject interface
	virtual UWorld* GetWorld() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;

protected:
	// Replication
	UFUNCTION()
	virtual void OnRep_State();

protected:
	// Editor helpers
	UFUNCTION()
	virtual TArray<FName> GetAvailableSlotNames() const;

#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};