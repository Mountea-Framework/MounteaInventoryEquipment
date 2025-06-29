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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaAttachmentsStatics.generated.h"

enum class EAttachmentSlotState : uint8;
enum class EAttachmentState : uint8;
struct FGameplayTag;
struct FGameplayTagContainer;
class UMounteaAttachableComponent;
class UMounteaAdvancedAttachmentSlot;
class IMounteaAdvancedAttachmentContainerInterface;
class IMounteaAdvancedAttachmentAttachableInterface;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAttachmentsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * Returns an array of mesh components available on the target actor.
	 * 
	 * @param Target  Actor to get components from
	 * @return  Array of mesh components available on the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<USceneComponent*> GetAvailableComponents(const AActor* Target);

	/**
	 * Returns an array of names of mesh components available on the target actor.
	 * 
	 * @param Target  Actor to get component names from
	 * @return  Array of names of mesh components available on the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<FName> GetAvailableComponentNames(const AActor* Target);

	/**
	 * Returns first found mesh component available on the target actor by name.
	 * 
	 * @param Target  Actor to get component names from
	 * @param ComponentName Name of component to search for.
	 * @return  Mesh component available on the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static USceneComponent* GetAvailableComponentByName(const AActor* Target, const FName& ComponentName);
	
	/**
	 * Returns an array of socket names available on the specified component of the target actor.
	 * 
	 * @param Target  Actor to get socket names from
	 * @param ComponentName  Name of the component to get socket names from
	 * @return  Array of socket names available on the specified component of the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<FName> GetAvailableSocketNames(const AActor* Target, const FName& ComponentName);

#pragma region AttachmentContainer
	
	/**
	 * Returns an array of socket names available on the target actor.
	 * 
	 * @param Target  Target to get the owning actor from
	 * @return  The owning actor of the target attachment container interface.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|AttachmentContainer",
		meta=(CustomTag="MounteaK2Getter"))
	static AActor* GetOwningActor(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);

	/**
	 * Returns the default attachment target name.
	 * 
	 * @param Target  Target to get the default attachment target from
	 * @return  The default attachment target name
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Getter"))
	static FName GetDefaultAttachmentTarget(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);

	/**
	 * Checks if the specified slot ID is valid.
	 * 
	 * @param Target  Target to validate the slot on
	 * @param SlotId  The slot ID to validate
	 * @return  True if the slot is valid, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool IsValidSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Gets the attachment slot by ID.
	 * 
	 * @param Target  Target to get the slot from
	 * @param SlotId  The slot ID to retrieve
	 * @return  The attachment slot, or null if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedAttachmentSlot* GetSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Checks if the specified slot is occupied.
	 * 
	 * @param Target  Target to check the slot on
	 * @param SlotId  The slot ID to check
	 * @return  True if the slot is occupied, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool IsSlotOccupied(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Disables the specified slot.
	 * 
	 * @param Target  Target to disable the slot on
	 * @param SlotId  The slot ID to disable
	 * @return  True if the slot was successfully disabled, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool DisableSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Tries to attach an attachment to the specified slot.
	 * 
	 * @param Target     Target to attach to
	 * @param SlotId     The slot ID to attach to
	 * @param Attachment The attachable object to attach. If implements Attachable interface, it will be used as such.
	 * @return  True if attachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool TryAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UObject* Attachment);

	/**
	 * Tries to detach from the specified slot.
	 * 
	 * @param Target  Target to detach from
	 * @param SlotId  The slot ID to detach from
	 * @return  True if detachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool TryDetach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Forces attachment to the specified slot.
	 * 
	 * @param Target     Target to attach to
	 * @param SlotId     The slot ID to attach to
	 * @param Attachment The attachable object to attach. If implements Attachable interface, it will be used as such.
	 * @return  True if attachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool ForceAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UObject* Attachment);

	/**
	 * Forces detachment from the specified slot.
	 * 
	 * @param Target  Target to detach from
	 * @param SlotId  The slot ID to detach from
	 * @return  True if detachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool ForceDetach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Finds the first free slot that matches the required tags.
	 * 
	 * @param Target       Target to search for slots on
	 * @param RequiredTags Tags that the slot must match
	 * @return  The slot ID of the first free matching slot, or NAME_None if none found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Getter"))
	static FName FindFirstFreeSlotWithTags(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FGameplayTagContainer& RequiredTags);

	/**
	 * Gets the slot ID for the specified attachable component.
	 * 
	 * @param Target     Target to search for the attachable on
	 * @param Attachable The attachable component to find
	 * @return  The slot ID where the attachable is located, or NAME_None if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Getter"))
	static FName GetSlotIdForAttachable(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const UMounteaAttachableComponent* Attachable);

	/**
	 * Clears all attachments from all slots.
	 * 
	 * @param Target  Target to clear all attachments from
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Setter"))
	static void ClearAll(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);

	/**
	 * Gets the first emtpy slot ID.
	 * 
	 * @param Target  Target to clear all attachments from
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Getter"))
	static FName GetFirstEmptySlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);

	/**
	 * Gets all Slots in the Attachment Container.
	 * 
	 * @param Target  Target to clear all attachments from
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|AttachmentContainer",
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<UMounteaAdvancedAttachmentSlot*> GetAttachmentSlots(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);

#pragma endregion

#pragma region Attachable

	/**
	 * Gets the container this attachable is attached to.
	 * 
	 * @param Target  Target to get the attached container from
	 * @return  The container this attachable is attached to
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Getter"))
	static TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> GetAttachedTo(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Gets the ID of the attachable.
	 * 
	 * @param Target  Target to get the ID from
	 * @return  The ID of the attachable
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Getter"))
	static FName GetId(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Sets the ID of the attachable.
	 * 
	 * @param Target  Target to set the ID on
	 * @param NewId   The new ID to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetId(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FName& NewId);

	/**
	 * Gets the display name of the attachable.
	 * 
	 * @param Target  Target to get the display name from
	 * @return  The display name of the attachable
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Getter"))
	static FText GetDisplayName(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Sets the display name of the attachable.
	 * 
	 * @param Target          Target to set the display name on
	 * @param NewDisplayName  The new display name to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetDisplayName(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FText& NewDisplayName);

	/**
	 * Gets the tags of the attachable.
	 * 
	 * @param Target  Target to get the tags from
	 * @return  The tags of the attachable
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Getter"))
	static FGameplayTagContainer GetTags(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Sets the tags of the attachable.
	 * 
	 * @param Target   Target to set the tags on
	 * @param NewTags  The new tags to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetTags(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FGameplayTagContainer& NewTags);

	/**
	 * Gets the state of the attachable.
	 * 
	 * @param Target  Target to get the state from
	 * @return  The state of the attachable
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Getter"))
	static EAttachmentState GetState(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Sets the state of the attachable.
	 * 
	 * @param Target    Target to set the state on
	 * @param NewState  The new state to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetState(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const EAttachmentState NewState);

	/**
	 * Checks if the attachable is valid.
	 * 
	 * @param Target  Target to validate
	 * @return  True if the attachable is valid, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool IsValidAttachable(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Checks if the attachable can attach.
	 * 
	 * @param Target  Target to check
	 * @return  True if the attachable can attach, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool CanAttach(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Attaches the attachable to a specific slot.
	 * 
	 * @param Target          Target attachable to attach
	 * @param Container       Container to attach to
	 * @param SlotId          The slot ID to attach to
	 * @return  True if attachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool AttachToSlot(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Container, const FName& SlotId);

	/**
	 * Attaches the attachable to any available slot in the container.
	 * 
	 * @param Target     Target attachable to attach
	 * @param Container  Container to attach to
	 * @return  True if attachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool AttachToContainer(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Container);

	/**
	 * Detaches the attachable from its current container.
	 * 
	 * @param Target  Target attachable to detach
	 * @return  True if detachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool Detach(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target);

	/**
	 * Checks if the attachable has a specific tag.
	 * 
	 * @param Target  Target to check
	 * @param Tag     The tag to check for
	 * @return  True if the attachable has the tag, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool HasTag(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FGameplayTag& Tag);

	/**
	 * Checks if the attachable matches the given tags.
	 * 
	 * @param Target       Target to check
	 * @param OtherTags    Tags to match against
	 * @param bRequireAll  Whether all tags must match or just any
	 * @return  True if the tags match, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Attachable",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool MatchesTags(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FGameplayTagContainer& OtherTags, bool bRequireAll);
	
#pragma endregion 
};
