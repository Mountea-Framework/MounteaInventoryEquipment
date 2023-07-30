// All rights reserved Dominik Pavlicek 2023.


#include "Setup/MounteaInventoryConfig.h"

#include "Net/UnrealNetwork.h"

UMounteaInventoryConfig::UMounteaInventoryConfig()
{
	RepKey = 0;
}

void UMounteaInventoryConfig::RemoveFlag(const FGameplayTag& Flag)
{
	RepKey++;
	
	InventoryFlags.RemoveTag(Flag);
}

void UMounteaInventoryConfig::AddFlag(const FGameplayTag& Flag)
{
	RepKey++;
	
	InventoryFlags.AddTag(Flag);
}

void UMounteaInventoryConfig::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryConfig, InventoryFlags);
}