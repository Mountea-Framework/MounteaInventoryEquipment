// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaAttachmentsStatics.generated.h"

struct FGameplayTagContainer;
class UMounteaAttachableComponent;
class UMounteaAdvancedAttachmentSlot;
class IMounteaAdvancedAttachmentContainerInterface;

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
	 * Returns an array of socket names available on the specified component of the target actor.
	 * 
	 * @param Target  Actor to get socket names from
	 * @param ComponentName  Name of the component to get socket names from
	 * @return  Array of socket names available on the specified component of the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<FName> GetAvailableSocketNames(const AActor* Target, const FName& ComponentName);

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static FName GetDefaultAttachmentTarget(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);

	/**
	 * Checks if the specified slot ID is valid.
	 * 
	 * @param Target  Target to validate the slot on
	 * @param SlotId  The slot ID to validate
	 * @return  True if the slot is valid, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool IsValidSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Gets the attachment slot by ID.
	 * 
	 * @param Target  Target to get the slot from
	 * @param SlotId  The slot ID to retrieve
	 * @return  The attachment slot, or null if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedAttachmentSlot* GetSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Checks if the specified slot is occupied.
	 * 
	 * @param Target  Target to check the slot on
	 * @param SlotId  The slot ID to check
	 * @return  True if the slot is occupied, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Validate", ExpandBoolAsExecs="ReturnValue"))
	static bool IsSlotOccupied(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Disables the specified slot.
	 * 
	 * @param Target  Target to disable the slot on
	 * @param SlotId  The slot ID to disable
	 * @return  True if the slot was successfully disabled, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
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
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool TryAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UObject* Attachment);

	/**
	 * Tries to detach from the specified slot.
	 * 
	 * @param Target  Target to detach from
	 * @param SlotId  The slot ID to detach from
	 * @return  True if detachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
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
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool ForceAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UObject* Attachment);

	/**
	 * Forces detachment from the specified slot.
	 * 
	 * @param Target  Target to detach from
	 * @param SlotId  The slot ID to detach from
	 * @return  True if detachment was successful, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Setter", ExpandBoolAsExecs="ReturnValue"))
	static bool ForceDetach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId);

	/**
	 * Finds the first free slot that matches the required tags.
	 * 
	 * @param Target       Target to search for slots on
	 * @param RequiredTags Tags that the slot must match
	 * @return  The slot ID of the first free matching slot, or NAME_None if none found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static FName FindFirstFreeSlotWithTags(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FGameplayTagContainer& RequiredTags);

	/**
	 * Gets the slot ID for the specified attachable component.
	 * 
	 * @param Target     Target to search for the attachable on
	 * @param Attachable The attachable component to find
	 * @return  The slot ID where the attachable is located, or NAME_None if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static FName GetSlotIdForAttachable(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const UMounteaAttachableComponent* Attachable);

	/**
	 * Clears all attachments from all slots.
	 * 
	 * @param Target  Target to clear all attachments from
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Setter"))
	static void ClearAll(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);

	/**
	 * Gets the first emtpy slot ID.
	 * 
	 * @param Target  Target to clear all attachments from
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static FName GetFirstEmptySlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target);
};
