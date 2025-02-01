// All rights reserved Dominik Morse 2024


#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Definitions/MounteaInventoryBaseEnums.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventorySettingsConfig"

UMounteaAdvancedInventorySettingsConfig::UMounteaAdvancedInventorySettingsConfig()
{
   ValidateInventoryTypes();
}

#if WITH_EDITOR
void UMounteaAdvancedInventorySettingsConfig::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() != GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventorySettingsConfig, AllowedInventoryTypes))
	{
		return;
	}

	ValidateInventoryTypes();
}

void UMounteaAdvancedInventorySettingsConfig::ValidateInventoryTypes()
{
	TArray RequiredTypes = {
		EInventoryType::EIT_Player,
		EInventoryType::EIT_NPC,
		EInventoryType::EIT_Storage,
		EInventoryType::EIT_Merchant,
		EInventoryType::EIT_Loot,
		EInventoryType::EIT_Specialized
	};

	for (const EInventoryType Type : RequiredTypes)
	{
		if (!AllowedInventoryTypes.Contains(Type))
		{
			AllowedInventoryTypes.Add(Type, GetDefaultConfigForType(Type));
		}
	}
}

FInventoryTypeConfig UMounteaAdvancedInventorySettingsConfig::GetDefaultConfigForType(const EInventoryType Type)
{
	FInventoryTypeConfig Config;
	
	switch(Type)
	{
		case EInventoryType::EIT_Player:
			SetupPlayerConfig(Config);
			break;
		case EInventoryType::EIT_NPC:
			SetupNPCConfig(Config);
			break;
		case EInventoryType::EIT_Storage:
			SetupStorageConfig(Config);
			break;
		case EInventoryType::EIT_Merchant:
			SetupMerchantConfig(Config);
			break;
		case EInventoryType::EIT_Loot:
			SetupLootConfig(Config);
			break;
		case EInventoryType::EIT_Specialized:
			SetupSpecializedConfig(Config);
			break;
		default:
			break;
	}
	
	return Config;
}

void UMounteaAdvancedInventorySettingsConfig::SetupPlayerConfig(FInventoryTypeConfig& Config)
{
	Config.InventoryDisplayName = LOCTEXT("PlayerInventory", "Character Inventory");
	Config.ConfigFlags = static_cast<uint8>(EInventoryTypeFlags::EITF_HasWeightLimit | 
										  EInventoryTypeFlags::EITF_CanAddItems |
										  EInventoryTypeFlags::EITF_CanRemoveItems |
										  EInventoryTypeFlags::EITF_AllowStacking |
										  EInventoryTypeFlags::EITF_AutoStack |
										  EInventoryTypeFlags::EITF_Persistent);
	Config.AccessFlags = static_cast<uint8>(EInventoryFlags::EIF_Private);
	Config.SlotsRange = FIntPoint(30, 50);
	Config.StartingSlots = 30;
	Config.MaxWeight = 75.0f;
	Config.WeightThresholds = FVector4(0.3f, 0.5f, 0.7f, 0.9f);
	Config.WeightSpeedMultipliers = FVector4(1.0f, 0.8f, 0.6f, 0.4f);
}

void UMounteaAdvancedInventorySettingsConfig::SetupNPCConfig(FInventoryTypeConfig& Config)
{
	Config.InventoryDisplayName = LOCTEXT("NPCInventory", "Character Inventory");
	Config.ConfigFlags = static_cast<uint8>(EInventoryTypeFlags::EITF_HasWeightLimit | 
										  EInventoryTypeFlags::EITF_CanAddItems |
										  EInventoryTypeFlags::EITF_CanRemoveItems);
	Config.AccessFlags = static_cast<uint8>(EInventoryFlags::EIF_Private | EInventoryFlags::EIF_Lootable);
	Config.SlotsRange = FIntPoint(15, 30);
	Config.StartingSlots = 15;
	Config.MaxWeight = 50.0f;
}

void UMounteaAdvancedInventorySettingsConfig::SetupStorageConfig(FInventoryTypeConfig& Config)
{
	Config.InventoryDisplayName = LOCTEXT("StorageInventory", "Storage");
	Config.ConfigFlags = static_cast<uint8>(EInventoryTypeFlags::EITF_HasWeightLimit | 
										  EInventoryTypeFlags::EITF_CanAddItems |
										  EInventoryTypeFlags::EITF_CanRemoveItems |
										  EInventoryTypeFlags::EITF_AllowStacking |
										  EInventoryTypeFlags::EITF_Persistent);
	Config.AccessFlags = static_cast<uint8>(EInventoryFlags::EIF_Private);
	Config.SlotsRange = FIntPoint(50, 200);
	Config.StartingSlots = 50;
	Config.MaxWeight = 300.0f;
}

void UMounteaAdvancedInventorySettingsConfig::SetupMerchantConfig(FInventoryTypeConfig& Config)
{
	Config.InventoryDisplayName = LOCTEXT("MerchantInventory", "Merchant");
	Config.ConfigFlags = static_cast<uint8>(EInventoryTypeFlags::EITF_HasValueLimit | 
										  EInventoryTypeFlags::EITF_CanAddItems |
										  EInventoryTypeFlags::EITF_CanRemoveItems |
										  EInventoryTypeFlags::EITF_AllowStacking);
	Config.AccessFlags = static_cast<uint8>(EInventoryFlags::EIF_Private);
	Config.SlotsRange = FIntPoint(50, 50);
	Config.StartingSlots = 50;
	Config.MaxValue = 5000.0f;
}

void UMounteaAdvancedInventorySettingsConfig::SetupLootConfig(FInventoryTypeConfig& Config)
{
	Config.InventoryDisplayName = LOCTEXT("LootInventory", "Loot");
	Config.ConfigFlags = static_cast<uint8>(EInventoryTypeFlags::EITF_CanRemoveItems |
										  EInventoryTypeFlags::EITF_AllowStacking);
	Config.AccessFlags = static_cast<uint8>(EInventoryFlags::EIF_Public | 
										  EInventoryFlags::EIF_Lootable | 
										  EInventoryFlags::EIF_Temporary);
	Config.SlotsRange = FIntPoint(1, 30);
	Config.StartingSlots = 10;
}

void UMounteaAdvancedInventorySettingsConfig::SetupSpecializedConfig(FInventoryTypeConfig& Config)
{
	Config.InventoryDisplayName = LOCTEXT("SpecializedInventory", "Special");
	Config.ConfigFlags = static_cast<uint8>(EInventoryTypeFlags::EITF_CanAddItems | 
										  EInventoryTypeFlags::EITF_CanRemoveItems);
	Config.AccessFlags = static_cast<uint8>(EInventoryFlags::EIF_Private);
	Config.SlotsRange = FIntPoint(20, 20);
	Config.StartingSlots = 20;
}
#endif

#undef LOCTEXT_NAMESPACE
