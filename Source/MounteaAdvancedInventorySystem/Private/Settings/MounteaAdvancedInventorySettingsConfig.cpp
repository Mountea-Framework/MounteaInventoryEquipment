// All rights reserved Dominik Morse 2024


#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventorySettingsConfig"

UMounteaAdvancedInventorySettingsConfig::UMounteaAdvancedInventorySettingsConfig()
{
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

TArray<FString> UMounteaAdvancedInventorySettingsConfig::GetNotificationTypes() const
{
	TArray<FString> returnValue;
	returnValue.Append(NotificationTypes.Array());

	// Dynamically get all notification types
	returnValue.Append(MounteaInventoryNotificationBaseTypes::GetAllNotificationTypes());

	return returnValue;
}

#if WITH_EDITOR
void UMounteaAdvancedInventorySettingsConfig::SetDefaultValues()
{
	// Setup base inventory types
	ValidateInventoryTypes();
	
	// Setup item categories
	FInventoryCategory WeaponCategory;
	WeaponCategory.CategoryDisplayName = LOCTEXT("WeaponCategory", "Weapons");
	WeaponCategory.CategoryPriority = 0;
	WeaponCategory.CategoryFlags = static_cast<uint8>(EInventoryItemFlags::EIIF_Durable | EInventoryItemFlags::EIIF_Dropable);
	AllowedCategories.Add("Weapons", WeaponCategory);
	
	FInventoryCategory ArmorCategory;
	ArmorCategory.CategoryDisplayName = LOCTEXT("ArmorCategory", "Armor");
	ArmorCategory.CategoryPriority = 1;
	ArmorCategory.CategoryFlags = static_cast<uint8>(EInventoryItemFlags::EIIF_Durable | EInventoryItemFlags::EIIF_Dropable);
	AllowedCategories.Add("Armors", ArmorCategory);
	
	FInventoryCategory ConsumableCategory;
	ConsumableCategory.CategoryDisplayName = LOCTEXT("ConsumableCategory", "Consumables");
	ConsumableCategory.CategoryPriority = 2;
	ConsumableCategory.CategoryFlags = static_cast<uint8>(EInventoryItemFlags::EIIF_Consumable | EInventoryItemFlags::EIIF_Stackable);
	AllowedCategories.Add("Consumables", ConsumableCategory);
	
	FInventoryCategory MaterialCategory;
	MaterialCategory.CategoryDisplayName = LOCTEXT("MaterialCategory", "Materials");
	MaterialCategory.CategoryPriority = 3;
	MaterialCategory.CategoryFlags = static_cast<uint8>(EInventoryItemFlags::EIIF_Craftable | EInventoryItemFlags::EIIF_Stackable);
	AllowedCategories.Add("Materials", MaterialCategory);

	FInventoryCategory QuestCategory;
	QuestCategory.CategoryDisplayName = LOCTEXT("QuestCategory", "Quest Items");
	QuestCategory.CategoryPriority = 4;
	QuestCategory.CategoryFlags = static_cast<uint8>(EInventoryItemFlags::EIIF_QuestItem);
	AllowedCategories.Add("Quest Items", QuestCategory);

	FInventoryCategory KeysCategory;
	KeysCategory.CategoryDisplayName = LOCTEXT("KeysCategory", "Keys");
	KeysCategory.CategoryPriority = 4;
	KeysCategory.CategoryFlags = static_cast<uint8>(EInventoryItemFlags::EIIF_QuestItem);
	AllowedCategories.Add("Keys", KeysCategory);
	
	// Setup item rarities
	FInventoryRarity CommonRarity;
	CommonRarity.RarityDisplayName = LOCTEXT("CommonRarity", "Common");
	CommonRarity.RarityColor = FLinearColor(0.5f, 0.5f, 0.5f);
	CommonRarity.BasePriceMultiplier = 1.0f;
	AllowedRarities.Add("Common", CommonRarity);
	
	FInventoryRarity UncommonRarity;
	UncommonRarity.RarityDisplayName = LOCTEXT("UncommonRarity", "Uncommon");
	UncommonRarity.RarityColor = FLinearColor(0.2f, 0.8f, 0.2f);
	UncommonRarity.BasePriceMultiplier = 2.0f;
	AllowedRarities.Add("Uncommon", UncommonRarity);
	
	FInventoryRarity RareRarity;
	RareRarity.RarityDisplayName = LOCTEXT("RareRarity", "Rare");
	RareRarity.RarityColor = FLinearColor(0.2f, 0.2f, 1.0f);
	RareRarity.BasePriceMultiplier = 4.0f;
	AllowedRarities.Add("Rare", RareRarity);
	
	FInventoryRarity EpicRarity;
	EpicRarity.RarityDisplayName = LOCTEXT("EpicRarity", "Epic");
	EpicRarity.RarityColor = FLinearColor(0.6f, 0.2f, 0.8f);
	EpicRarity.BasePriceMultiplier = 8.0f;
	AllowedRarities.Add("Epic", EpicRarity);
	
	FInventoryRarity LegendaryRarity;
	LegendaryRarity.RarityDisplayName = LOCTEXT("LegendaryRarity", "Legendary");
	LegendaryRarity.RarityColor = FLinearColor(1.0f, 0.5f, 0.0f);
	LegendaryRarity.BasePriceMultiplier = 16.0f;
	AllowedRarities.Add("Legendary", LegendaryRarity);
}

void UMounteaAdvancedInventorySettingsConfig::SetDefaultNotificationConfig()
{
	// Setup notification configs
	{
		FInventoryNotificationConfig InventoryLimitConfig;
		InventoryLimitConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Warning;
		InventoryLimitConfig.MessageTemplate = LOCTEXT("InventoryLimit", "Inventory Limit Reached");
		InventoryLimitConfig.bShowProgressBar = false;
		InventoryLimitConfig.bCanBeClosed = true;
		InventoryLimitConfig.bHasDuration = true;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::InventoryLimitReached, InventoryLimitConfig);

		FInventoryNotificationConfig QuantityLimitConfig;
		QuantityLimitConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Warning;
		QuantityLimitConfig.MessageTemplate = LOCTEXT("QuantityLimit", "${itemName} Max Quantity");
		QuantityLimitConfig.bShowProgressBar = false;
		QuantityLimitConfig.bCanBeClosed = true;
		QuantityLimitConfig.bHasDuration = true;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::QuantityLimitReached, QuantityLimitConfig);

		FInventoryNotificationConfig NotFoundConfig;
		NotFoundConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Error;
		NotFoundConfig.MessageTemplate = LOCTEXT("ItemNotFound", "${itemName} Not Found");
		NotFoundConfig.bShowProgressBar = true;
		NotFoundConfig.bCanBeClosed = true;
		NotFoundConfig.bHasDuration = true;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::ItemNotFound, NotFoundConfig);

		FInventoryNotificationConfig ItemAddedConfig;
		ItemAddedConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Info;
		ItemAddedConfig.MessageTemplate = LOCTEXT("ItemAdded", "$(quantity)x ${itemName}");
		ItemAddedConfig.bShowProgressBar = false;
		ItemAddedConfig.bCanBeClosed = true;
		ItemAddedConfig.bHasDuration = true;
		ItemAddedConfig.DefaultDuration = 2.0f;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::ItemAdded, ItemAddedConfig);

		FInventoryNotificationConfig ItemRemovedConfig;
		ItemRemovedConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Info;
		ItemRemovedConfig.MessageTemplate = LOCTEXT("ItemRemoved", "Removed ${itemName}");
		ItemRemovedConfig.bShowProgressBar = false;
		ItemRemovedConfig.bCanBeClosed = true;
		ItemRemovedConfig.bHasDuration = true;
		ItemRemovedConfig.DefaultDuration = 2.0f;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::ItemRemoved, ItemRemovedConfig);

		FInventoryNotificationConfig PartiallyAddedConfig;
		PartiallyAddedConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Warning;
		PartiallyAddedConfig.MessageTemplate = LOCTEXT("ItemPartiallyAdded", "$(quantity)x ${itemName}");
		PartiallyAddedConfig.bShowProgressBar = false;
		PartiallyAddedConfig.bCanBeClosed = true;
		PartiallyAddedConfig.bHasDuration = true;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::ItemPartiallyAdded, PartiallyAddedConfig);

		FInventoryNotificationConfig PartiallyRemovedConfig;
		PartiallyRemovedConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Info;
		PartiallyRemovedConfig.MessageTemplate = LOCTEXT("ItemPartiallyRemoved", "Removed $(quantity)x ${itemName}");
		PartiallyRemovedConfig.bShowProgressBar = false;
		PartiallyRemovedConfig.bCanBeClosed = true;
		PartiallyRemovedConfig.bHasDuration = true;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::ItemPartiallyRemoved, PartiallyRemovedConfig);

		FInventoryNotificationConfig NotUpdatedConfig;
		NotUpdatedConfig.NotificationCategory = EInventoryNotificationCategory::EINC_Error;
		NotUpdatedConfig.MessageTemplate = LOCTEXT("ItemNotUpdated", "Operation Failed!");
		NotUpdatedConfig.bShowProgressBar = false;
		NotUpdatedConfig.bCanBeClosed = true;
		NotUpdatedConfig.bHasDuration = true;
		NotificationConfigs.Add(MounteaInventoryNotificationBaseTypes::ItemNotUpdated, NotUpdatedConfig);
	}
}

void UMounteaAdvancedInventorySettingsConfig::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() != GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventorySettingsConfig, AllowedInventoryTypes))
	{
		return;
	}

	ValidateInventoryTypes();
}
#endif

#undef LOCTEXT_NAMESPACE
