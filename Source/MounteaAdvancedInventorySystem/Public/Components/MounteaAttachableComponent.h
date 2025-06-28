// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
#include "MounteaAttachableComponent.generated.h"

enum class EAttachmentSlotState : uint8;
class IMounteaAdvancedAttachmentContainerInterface;

/**
 * Component that represents an attachable item in the Mountea Advanced Inventory System.
 * This component can be used to define items that can be attached to other actors or components.
 * It holds properties such as ID, display name, tags, state, and slot ID.
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mountea")
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> AttachedTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	EAttachmentSlotState State;

public:
	
	virtual TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> GetAttachedTo_Implementation() const override { return AttachedTo; };
	virtual FName GetId_Implementation() const override { return Id; };
	virtual void SetId_Implementation(const FName& NewId) override;
	virtual FText GetDisplayName_Implementation() const override { return DisplayName; };
	virtual void SetDisplayName_Implementation(const FText& NewDisplayName) override;
	virtual FGameplayTagContainer GetTags_Implementation() const override { return Tags; } ;
	virtual void SetTags_Implementation(const FGameplayTagContainer& NewTags) override;
	virtual EAttachmentSlotState GetState_Implementation() const override { return State; } ;
	virtual void SetState_Implementation(const EAttachmentSlotState NewState) override;
	virtual bool IsValidAttachable_Implementation() const override;
	virtual bool IsEmpty_Implementation() const override;
	virtual bool IsOccupied_Implementation() const override;
	virtual bool IsLocked_Implementation() const override;
	virtual bool CanAttach_Implementation() const override;
	virtual bool AttachToSlot_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId) override;
	virtual bool AttachToContainer_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target) override;
	virtual bool Detach_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual bool HasTag_Implementation(const FGameplayTag& Tag) const override;
	virtual bool MatchesTags_Implementation(const FGameplayTagContainer& OtherTags, bool bRequireAll) const override;
};