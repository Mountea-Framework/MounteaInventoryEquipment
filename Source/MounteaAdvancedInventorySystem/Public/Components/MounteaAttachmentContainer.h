// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "MounteaAttachmentContainer.generated.h"

/**
 * Component that holds an attachment container for equipment systems.
 * This component allows actors to manage attachments dynamically at runtime.
 * It can be used to equip, unequip, and manage attachments on actors.
 * 
 */
UCLASS(ClassGroup=(Mountea), meta=(BlueprintSpawnableComponent))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAttachmentContainer : public UActorComponent
{
	GENERATED_BODY()

public:

	UMounteaAttachmentContainer();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FAttachmentContainer AttachmentContainer;
};
