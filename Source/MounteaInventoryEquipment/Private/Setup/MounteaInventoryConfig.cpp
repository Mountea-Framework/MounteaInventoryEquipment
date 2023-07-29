// All rights reserved Dominik Pavlicek 2023.


#include "Setup/MounteaInventoryConfig.h"

#include "Net/UnrealNetwork.h"

UMounteaInventoryConfig::UMounteaInventoryConfig()
{
	RepKey = 0;
}

void UMounteaInventoryConfig::RemoveFlag(const uint8 Flag)
{
	RepKey++;
	
	InventoryFlags |= static_cast<uint8>(Flag);
}

void UMounteaInventoryConfig::AddFlag(const uint8 Flag)
{
	RepKey++;
	
	InventoryFlags &= ~static_cast<uint8>(Flag);
}

void UMounteaInventoryConfig::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryConfig, InventoryFlags);
}