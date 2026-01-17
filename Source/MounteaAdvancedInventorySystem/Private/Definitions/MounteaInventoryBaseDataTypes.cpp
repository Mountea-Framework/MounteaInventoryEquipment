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


#include "Definitions/MounteaInventoryBaseDataTypes.h"


FInventoryRarity::FInventoryRarity() : RarityColor(FColor::Silver)
{
}

FInventoryCategoryData::FInventoryCategoryData(): DisplayIcon(nullptr), CategoryFlags(0)
{
}

FInventoryCategory::FInventoryCategory() : CategoryData()
{
}

FInventoryTypeConfig::FInventoryTypeConfig(): ConfigFlags(0), AccessFlags(static_cast<uint8>(EInventoryFlags::EIF_Private))
{
}

bool FInventoryTypeConfig::HasWeightLimit() const
{ return (ConfigFlags & static_cast<uint8>(EInventoryTypeFlags::EITF_HasWeightLimit)) != 0; }

bool FInventoryTypeConfig::HasValueLimit() const
{ return (ConfigFlags & static_cast<uint8>(EInventoryTypeFlags::EITF_HasValueLimit)) != 0; }

bool FInventoryTypeConfig::CanAddItems() const
{ return (ConfigFlags & static_cast<uint8>(EInventoryTypeFlags::EITF_CanAddItems)) != 0; }

bool FInventoryTypeConfig::IsPublic() const 
{ 
	return (AccessFlags & static_cast<uint8>(EInventoryFlags::EIF_Public)) != 0; 
}

bool FInventoryTypeConfig::IsTeamShared() const 
{ 
	return (AccessFlags & static_cast<uint8>(EInventoryFlags::EIF_TeamShared)) != 0; 
}

bool FInventoryTypeConfig::IsLootable() const 
{ 
	return (AccessFlags & static_cast<uint8>(EInventoryFlags::EIF_Lootable)) != 0; 
}

bool FInventoryTypeConfig::IsTemporary() const 
{ 
	return (AccessFlags & static_cast<uint8>(EInventoryFlags::EIF_Temporary)) != 0; 
}

bool FInventoryTypeConfig::IsPrivate() const 
{ 
	return (AccessFlags & static_cast<uint8>(EInventoryFlags::EIF_Private)) != 0; 
}

void FInventoryTypeConfig::SetPublic(const bool bValue) 
{ 
	SetFlag(EInventoryFlags::EIF_Public, bValue); 
}

void FInventoryTypeConfig::SetTeamShared(const bool bValue) 
{ 
	SetFlag(EInventoryFlags::EIF_TeamShared, bValue); 
}

void FInventoryTypeConfig::SetLootable(const bool bValue) 
{ 
	SetFlag(EInventoryFlags::EIF_Lootable, bValue); 
}

void FInventoryTypeConfig::SetTemporary(const bool bValue) 
{ 
	SetFlag(EInventoryFlags::EIF_Temporary, bValue); 
}

void FInventoryTypeConfig::SetPrivate(const bool bValue) 
{ 
	SetFlag(EInventoryFlags::EIF_Private, bValue); 
}

void FInventoryTypeConfig::SetFlag(const EInventoryFlags Flag, const bool bValue)
{
	if (bValue)
		AccessFlags |= static_cast<uint8>(Flag);
	else
		AccessFlags &= ~static_cast<uint8>(Flag);
}

bool FInventoryTypeConfig::HasFlag(const EInventoryFlags Flag) const
{
	return (AccessFlags & static_cast<uint8>(Flag)) != 0;
}