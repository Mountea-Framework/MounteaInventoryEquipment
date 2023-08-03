// All rights reserved Dominik Pavlicek 2023.


#include "Setup/MounteaInventoryConfig.h"

#include "Net/UnrealNetwork.h"

UMounteaInventoryConfig::UMounteaInventoryConfig()
{
	RepKey = 0;
}

void UMounteaInventoryConfig::RemoveFlags(const FGameplayTagContainer& Flags)
{
	for (const FGameplayTag& Itr : Flags)
	{
		RemoveFlag(Itr, true);
	}
	
	RepKey++;
}

void UMounteaInventoryConfig::AddFlags(const FGameplayTagContainer& Flags)
{
	for (const FGameplayTag& Itr : Flags)
	{
		AddFlag(Itr, true);
	}

	RepKey++;
}

void UMounteaInventoryConfig::RemoveFlag(const FGameplayTag& Flag, const bool bSkipNetUpdate)
{
	InventoryFlags.RemoveTag(Flag);

	if (!bSkipNetUpdate) RepKey++;
}

void UMounteaInventoryConfig::AddFlag(const FGameplayTag& Flag, const bool bSkipNetUpdate)
{
	InventoryFlags.AddTag(Flag);

	if (!bSkipNetUpdate) RepKey++;
}

void UMounteaInventoryConfig::ResetFlags()
{
	InventoryFlags = DefaultInventoryFlags;
	
	RepKey++;
}

void UMounteaInventoryConfig::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryConfig, InventoryFlags);
}
