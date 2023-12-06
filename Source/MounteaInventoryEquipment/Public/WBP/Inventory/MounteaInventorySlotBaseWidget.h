// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/UI/MounteaInventorySlotWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaInventorySlotBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventorySlotBaseWidget : public UMounteaBaseUserWidget, public IMounteaInventorySlotWBPInterface
{
	GENERATED_BODY()

public:

	virtual bool IsSlotEmpty_Implementation() const override;

protected:
	
	virtual FEventReply ResolveDrop_Implementation(UUserWidget* DroppedWidget, UObject* Payload) override;
};
