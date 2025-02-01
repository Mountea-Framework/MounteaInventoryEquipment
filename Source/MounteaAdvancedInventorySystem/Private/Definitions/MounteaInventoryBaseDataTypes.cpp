// All rights reserved Dominik Morse 2024


#include "Definitions/MounteaInventoryBaseDataTypes.h"


FInventoryRarity::FInventoryRarity() : RarityColor(FColor::Silver)
{
}

FInventoryCategory::FInventoryCategory() : DisplayIcon(nullptr), CategoryFlags(0)
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