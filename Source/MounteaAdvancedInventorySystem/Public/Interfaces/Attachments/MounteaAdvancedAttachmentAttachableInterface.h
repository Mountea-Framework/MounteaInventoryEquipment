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
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "MounteaAdvancedAttachmentAttachableInterface.generated.h"

enum class EAttachmentState : uint8;
class IMounteaAdvancedAttachmentContainerInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttachableAttached, const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>&, Container, const FName&, SlotId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachableDetached, const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>&, Container);

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedAttachmentAttachableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedAttachmentAttachableInterface defines the contract for objects that can be attached to containers.
 * Attachable interfaces provide identification, state management, tag filtering, and attachment operations
 * for items that participate in the attachment system within equipment and inventory frameworks.
 *
 * @see [Attachable Interface](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see IMounteaAdvancedAttachmentContainerInterface
 * @see UMounteaAttachableComponent
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedAttachmentAttachableInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> GetAttachedTo() const;
	virtual TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> GetAttachedTo_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	FName GetId() const;
	virtual FName GetId_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetId(const FName& NewId);
	virtual void SetId_Implementation(const FName& NewId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	FText GetDisplayName() const;
	virtual FText GetDisplayName_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetDisplayName(const FText& NewDisplayName);
	virtual void SetDisplayName_Implementation(const FText& NewDisplayName) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	FGameplayTagContainer GetTags() const;
	virtual FGameplayTagContainer GetTags_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetTags(const FGameplayTagContainer& NewTags);
	virtual void SetTags_Implementation(const FGameplayTagContainer& NewTags) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	EAttachmentState GetState() const;
	virtual EAttachmentState GetState_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetState(const EAttachmentState NewState);
	virtual void SetState_Implementation(const EAttachmentState NewState) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool IsValidAttachable() const;
	virtual bool IsValidAttachable_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool CanAttach() const;
	virtual bool CanAttach_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool AttachToSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);
	virtual bool AttachToSlot_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool AttachToContainer(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);
	virtual bool AttachToContainer_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool Detach();
	virtual bool Detach_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool HasTag(const FGameplayTag& Tag) const;
	virtual bool HasTag_Implementation(const FGameplayTag& Tag) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool MatchesTags(const FGameplayTagContainer& OtherTags, bool bRequireAll) const;
	virtual bool MatchesTags_Implementation(const FGameplayTagContainer& OtherTags, bool bRequireAll) const = 0;

	virtual FOnAttachableAttached& GetOnAttachableAttachedEventHandle() = 0;
	virtual FOnAttachableDetached& GetOnAttachableDetachedEventHandle() = 0;
};