// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedEquipmentInterface.generated.h"

struct FAttachmentContainer;
struct FAttachmentSlot;
struct FInventoryItem;
class UMounteaInventoryItemTemplate;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedEquipmentInterface
{
	GENERATED_BODY()

public:
};

