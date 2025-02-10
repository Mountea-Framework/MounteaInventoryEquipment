// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventorySystemStatics.h"

#include "Kismet/KismetSystemLibrary.h"

bool UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(const UWorld* WorldContext)
{
	return !UKismetSystemLibrary::IsDedicatedServer(WorldContext);
}

UMounteaAdvancedInventorySettingsConfig* UMounteaInventorySystemStatics::GetMounteaAdvancedInventoryConfig()
{
	return GetMutableDefault<UMounteaAdvancedInventorySettingsConfig>();
}
