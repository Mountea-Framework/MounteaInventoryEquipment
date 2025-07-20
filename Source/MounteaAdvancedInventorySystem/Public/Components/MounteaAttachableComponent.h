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
#include "Components/ActorComponent.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
#include "MounteaAttachableComponent.generated.h"

enum class EAttachmentSlotState : uint8;
class IMounteaAdvancedAttachmentContainerInterface;

/**
 * UMounteaAttachableComponent represents items that can be attached to other actors or components.
 * Attachable components implement the attachment interface with identification, tags, state management,
 * and container association for flexible item attachment within the equipment system.
 *
 * @see [Attachable Components](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see IMounteaAdvancedAttachmentAttachableInterface
 * @see UMounteaAttachmentContainerComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
    AutoExpandCategories=("Mountea","Attachable","Mountea|Attachable"),
    HideCategories=("Cooking","Collision"),
    meta=(BlueprintSpawnableComponent, DisplayName="Mountea Attachable Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAttachableComponent : public UActorComponent, public IMounteaAdvancedAttachmentAttachableInterface
{
	GENERATED_BODY()

public:
	UMounteaAttachableComponent();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mountea|Attachable", AdvancedDisplay,
		meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> AttachedTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FGameplayTagContainer Tags;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	EAttachmentState State;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category="Mountea|Attachable")
	FOnAttachableAttached OnAttachableAttached;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category="Mountea|Attachable")
	FOnAttachableDetached OnAttachableDetached;

public:
	
	virtual TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> GetAttachedTo_Implementation() const override { return AttachedTo; };
	virtual FName GetId_Implementation() const override { return Id; };
	virtual void SetId_Implementation(const FName& NewId) override;
	virtual FText GetDisplayName_Implementation() const override { return DisplayName; };
	virtual void SetDisplayName_Implementation(const FText& NewDisplayName) override;
	virtual FGameplayTagContainer GetTags_Implementation() const override { return Tags; } ;
	virtual void SetTags_Implementation(const FGameplayTagContainer& NewTags) override;
	virtual EAttachmentState GetState_Implementation() const override { return State; } ;
	virtual void SetState_Implementation(const EAttachmentState NewState) override;
	virtual bool IsValidAttachable_Implementation() const override;
	virtual bool CanAttach_Implementation() const override;
	virtual bool AttachToSlot_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId) override;
	virtual bool AttachToContainer_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target) override;
	virtual bool Detach_Implementation() override;
	virtual bool HasTag_Implementation(const FGameplayTag& Tag) const override;
	virtual bool MatchesTags_Implementation(const FGameplayTagContainer& OtherTags, bool bRequireAll) const override;

	virtual FOnAttachableAttached& GetOnAttachableAttachedEventHandle() override { return OnAttachableAttached; };
	virtual FOnAttachableDetached& GetOnAttachableDetachedEventHandle() override { return OnAttachableDetached; };
};