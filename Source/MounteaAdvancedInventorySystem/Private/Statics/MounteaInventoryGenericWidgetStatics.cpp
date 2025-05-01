// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryGenericWidgetStatics.h"

void UMounteaInventoryGenericWidgetStatics::ProcessInventoryWidgetCommand(UObject* GenericWidget,
	const FString& Command, UObject* OptionalPayload)
{
	if (!IsValid(GenericWidget))
		return;

	if (GenericWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		return IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(GenericWidget, Command, OptionalPayload);
}
