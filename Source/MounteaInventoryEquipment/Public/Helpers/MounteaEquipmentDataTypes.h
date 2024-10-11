// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "MounteaEquipmentDataTypes.generated.h"

// Enum for specifying the state of equipment slot
UENUM(BlueprintType)
enum class EEquipmentSlotState : uint8
{
	ESS_Disabled	UMETA(DisplayName="Disabled",	Tooltip="Slot is disabled.\nWhen slot is disabled, then equipment is automatically detached and no equipment can be equipped.\nSlot can be enabled again."),
	ESS_Blocked		UMETA(DisplayName="Blocked",	Tooltip="Slot is blocked by another Slot.\nWhen slot is blocked, no equipment can be equipped.\nSlot can be enabled again."),
	ESS_Occupied	UMETA(DisplayName="Occupied",	Tooltip="Slot is occupied by Item.\nOccupied is Active.\nWhen slot is occupied then switching to different item is enabled."),
	ESS_Empty		UMETA(Displayname="Empty",		Tooltip="Slot is empty.\nEmpty slot can recieve Item."),

	ESS_Default		UMETA(hidden)
};

// Enum for specifying the method of attachment for an equipment item.
UENUM(BlueprintType)
enum class EEquipmentItemAttachmentMethod : uint8
{
	EIAM_None		UMETA(DisplayName="None", Tooltip="No attachment method is specified.\nItem will not be attached."),
	EIAM_Socket		UMETA(DisplayName="Socket", Tooltip="Item is attached using a socket.\nThis method attaches the item to a specific socket on the character or object."),
	EIAM_Component	UMETA(DisplayName="Component", Tooltip="Item is attached to a component.\nThis method attaches the item to a specific component of the character or object."),

	EIAM_Default	UMETA(hidden)
};

// Data structure that holds information about how an equipment item will be attached.
USTRUCT(BlueprintType)
struct FEquipmentAttachmentData
{
	GENERATED_BODY()

public:

	// Defines how the equipment item will be attached to its owner.
	UPROPERTY(Category="Attachment", EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Defines how the equipment item will be attached to its owner."))
	EEquipmentItemAttachmentMethod AttachmentMethod;

	/**
	 * The name or tag of the component in the owner's parent that the item will attach to.
	 * Only applicable if AttachmentMethod is set to Component or Socket.
	*/
	UPROPERTY(Category="Attachment", EditAnywhere, BlueprintReadWrite, meta=(EditCondition="AttachmentMethod==EEquipmentItemAttachmentMethod::EIAM_Component||AttachmentMethod==EEquipmentItemAttachmentMethod::EIAM_Socket", ToolTip="The name or tag of the component in the owner's parent that the item will attach to.\nOnly applicable if AttachmentMethod is set to Component or Socket."))
	FName AttachmentComponent;

	/**
	 * The name of the socket from the specified AttachmentMesh that the item will attach to.
	 * Only applicable if AttachmentMethod is set to Socket.
	*/
	UPROPERTY(Category="Attachment", EditAnywhere, BlueprintReadWrite, meta=(EditCondition="AttachmentMethod==EEquipmentItemAttachmentMethod::EIAM_Socket", ToolTip="The name of the socket from the specified AttachmentMesh that the item will attach to.\nOnly applicable if AttachmentMethod is set to Socket."))
	FName AttachmentSocket;
};

