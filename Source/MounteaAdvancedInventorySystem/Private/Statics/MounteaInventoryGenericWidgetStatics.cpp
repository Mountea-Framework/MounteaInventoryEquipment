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


#include "Statics/MounteaInventoryGenericWidgetStatics.h"

#include "GameplayTagContainer.h"
#include "Components/Widget.h"

bool UMounteaInventoryGenericWidgetStatics::IsValidGenericWidget(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaInventoryGenericWidgetInterface>();
}

void UMounteaInventoryGenericWidgetStatics::ProcessInventoryWidgetCommand(UObject* GenericWidget, const FString& Command, UObject* OptionalPayload)
{
	if (IsValidGenericWidget(GenericWidget))
		return IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(GenericWidget, Command, OptionalPayload);
}

void UMounteaInventoryGenericWidgetStatics::ConsumeUIInput(UWidget* Target, const FGameplayTag InputTag, 
	const FMounteaWidgetInputPayload& Payload, const float DeltaTime)
{
	if (IsValidGenericWidget(Target))
		IMounteaInventoryGenericWidgetInterface::Execute_ConsumeUIInput(Target, InputTag, Payload, DeltaTime);
}

void UMounteaInventoryGenericWidgetStatics::RefreshWidget(UWidget* Target)
{
	if (IsValidGenericWidget(Target))
		IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(Target);
}

void UMounteaInventoryGenericWidgetStatics::ApplyTheme(UWidget* Target)
{
	if (IsValidGenericWidget(Target))
		IMounteaInventoryGenericWidgetInterface::Execute_ApplyTheme(Target);
}

void UMounteaInventoryGenericWidgetStatics::SetWidgetTag(UWidget* Target, const FGameplayTag& WidgetTag)
{
	if (IsValidGenericWidget(Target))
		IMounteaInventoryGenericWidgetInterface::Execute_SetWidgetTag(Target, WidgetTag);
}

FGameplayTag UMounteaInventoryGenericWidgetStatics::GetWidgetTag(UWidget* Target)
{
	return IsValidGenericWidget(Target) ? IMounteaInventoryGenericWidgetInterface::Execute_GetWidgetTag(Target) : FGameplayTag();
}
