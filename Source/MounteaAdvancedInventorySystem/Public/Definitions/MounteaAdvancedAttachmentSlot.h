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
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableTargetNames"),
		meta=(NoResetToDefault))
	FName AttachmentTargetOverride;

	/** Resolved component reference for attachment target override */
	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Settings")
	TObjectPtr<USceneComponent> AttachmentTargetComponentOverride = nullptr;

	/** Socket name to attach to when using socket-based attachment */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Settings",
		meta=(GetOptions="GetAvailableSocketNames"),
		meta=(NoResetToDefault),
		meta=(EditCondition="SlotType==EAttachmentSlotType::EAST_Socket", EditConditionHides))
	FName SocketName;

protected:
	virtual void BeginPlay_Implementation() override;

public:
	
	UFUNCTION()
	TArray<FName> GetAvailableSocketNames() const;
	
	UFUNCTION()
	TArray<FName> GetAvailableTargetNames() const;	

public:
	
	virtual bool Attach(UObject* NewAttachment) override;
	virtual bool ForceAttach(UObject* NewAttachment) override;
	virtual bool Detach() override;
	virtual bool ForceDetach() override;

public:
	
	FName GetAttachmentSocketName() const;
	static TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> FindAttachableInterface(UObject* Object);

private:
	
	// Internal helpers
	void TryResolveAttachmentTarget();
	USceneComponent* GetAttachmentTargetComponent() const;
	bool PerformAttachmentLogic(UObject* NewAttachment);
	void HandleAttachableInterface(UObject* NewAttachment) const;
	void PerformDetachment() const;
	bool ValidateAttachmentSlot(const USceneComponent* Target) const;
	bool PerformPhysicalAttachment(UObject* Object, USceneComponent* Target) const;

protected:
	
	virtual void OnRep_State() override;

#if WITH_EDITOR
public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};