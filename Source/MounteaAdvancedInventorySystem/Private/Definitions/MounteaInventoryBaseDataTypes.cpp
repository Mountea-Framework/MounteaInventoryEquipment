// All rights reserved Dominik Morse 2024


#include "Definitions/MounteaInventoryBaseDataTypes.h"

#include "Definitions/MounteaInventoryBaseEnums.h"

FInventoryRarity::FInventoryRarity() : RarityColor(FColor::Silver)
{
}

FInventoryCategory::FInventoryCategory() : DisplayIcon(nullptr), CategoryFlags(0)
{
}

FInventoryTypeConfig::FInventoryTypeConfig(): ConfigFlags(0)
{
}

bool FInventoryTypeConfig::HasWeightLimit() const
{ return (ConfigFlags & static_cast<uint8>(EInventoryTypeFlags::EITF_HasWeightLimit)) != 0; }

bool FInventoryTypeConfig::HasValueLimit() const
{ return (ConfigFlags & static_cast<uint8>(EInventoryTypeFlags::EITF_HasValueLimit)) != 0; }

bool FInventoryTypeConfig::CanAddItems() const
{ return (ConfigFlags & static_cast<uint8>(EInventoryTypeFlags::EITF_CanAddItems)) != 0; }
