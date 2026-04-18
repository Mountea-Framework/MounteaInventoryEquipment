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
	
	/**
	 * Returns the container this attachable is currently attached to.
	 *
	 * @return Owning attachment container, or null interface when detached.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> GetAttachedTo() const;
	virtual TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> GetAttachedTo_Implementation() const = 0;

	/**
	 * Returns the unique identifier of this attachable object.
	 *
	 * @return Attachable identifier.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	FName GetId() const;
	virtual FName GetId_Implementation() const = 0;

	/**
	 * Sets the unique identifier of this attachable object.
	 *
	 * @param NewId New attachable identifier.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetId(const FName& NewId);
	virtual void SetId_Implementation(const FName& NewId) = 0;

	/**
	 * Returns the user-facing display name of this attachable object.
	 *
	 * @return Localized display name.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	FText GetDisplayName() const;
	virtual FText GetDisplayName_Implementation() const = 0;

	/**
	 * Sets the user-facing display name of this attachable object.
	 *
	 * @param NewDisplayName New localized display name.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetDisplayName(const FText& NewDisplayName);
	virtual void SetDisplayName_Implementation(const FText& NewDisplayName) = 0;

	/**
	 * Returns gameplay tags associated with this attachable object.
	 *
	 * @return Tag container used for slot/tag compatibility checks.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	FGameplayTagContainer GetTags() const;
	virtual FGameplayTagContainer GetTags_Implementation() const = 0;

	/**
	 * Replaces gameplay tags associated with this attachable object.
	 *
	 * @param NewTags New tag container for compatibility/filtering logic.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetTags(const FGameplayTagContainer& NewTags);
	virtual void SetTags_Implementation(const FGameplayTagContainer& NewTags) = 0;

	/**
	 * Returns current runtime attachment state.
	 *
	 * @return Current EAttachmentState value.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	EAttachmentState GetState() const;
	virtual EAttachmentState GetState_Implementation() const = 0;

	/**
	 * Sets current runtime attachment state.
	 *
	 * @param NewState New EAttachmentState value.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	void SetState(const EAttachmentState NewState);
	virtual void SetState_Implementation(const EAttachmentState NewState) = 0;
	
	/**
	 * Validates whether this object is currently usable as an attachable.
	 *
	 * @return True when attachable data/state is valid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool IsValidAttachable() const;
	virtual bool IsValidAttachable_Implementation() const = 0;
	
	/**
	 * Checks whether this attachable can currently be attached.
	 *
	 * @return True if attach operation is currently allowed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool CanAttach() const;
	virtual bool CanAttach_Implementation() const = 0;

	/**
	 * Attempts to attach this object to a specific container slot.
	 *
	 * @param Target Target container interface.
	 * @param SlotId Destination slot identifier.
	 * @return True if attachment succeeded.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool AttachToSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);
	virtual bool AttachToSlot_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId) = 0;

	/**
	 * Attempts to attach this object to a container using container-defined slot resolution.
	 *
	 * @param Target Target container interface.
	 * @return True if attachment succeeded.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool AttachToContainer(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);
	virtual bool AttachToContainer_Implementation(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target) = 0;

	/**
	 * Detaches this object from its current container.
	 *
	 * @return True if detachment succeeded.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool Detach();
	virtual bool Detach_Implementation() = 0;

	/**
	 * Checks whether this attachable contains the requested gameplay tag.
	 *
	 * @param Tag Tag to test.
	 * @return True when tag is present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool HasTag(const FGameplayTag& Tag) const;
	virtual bool HasTag_Implementation(const FGameplayTag& Tag) const = 0;

	/**
	 * Compares attachable tags against another tag container.
	 *
	 * @param OtherTags Tag container to compare with.
	 * @param bRequireAll If true, requires all tags; otherwise any match is sufficient.
	 * @return True when comparison rules pass.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Attachable")
	bool MatchesTags(const FGameplayTagContainer& OtherTags, bool bRequireAll) const;
	virtual bool MatchesTags_Implementation(const FGameplayTagContainer& OtherTags, bool bRequireAll) const = 0;

	/**
	 * Returns multicast event handle fired when this attachable is attached.
	 *
	 * @return Reference to attach event delegate.
	 */
	virtual FOnAttachableAttached& GetOnAttachableAttachedEventHandle() = 0;
	
	/**
	 * Returns multicast event handle fired when this attachable is detached.
	 *
	 * @return Reference to detach event delegate.
	 */
	virtual FOnAttachableDetached& GetOnAttachableDetachedEventHandle() = 0;
};
