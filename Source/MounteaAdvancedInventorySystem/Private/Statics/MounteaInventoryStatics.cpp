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


#include "Statics/MounteaInventoryStatics.h"

#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseUIEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/ItemActions/MounteaAdvancedInventoryItemActionInterface.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Statics/MounteaInventorySystemStatics.h"

UMounteaAdvancedInventorySettings* UMounteaInventoryStatics::GetInventorySettings()
{
	return GetMutableDefault<UMounteaAdvancedInventorySettings>();
}

UMounteaAdvancedInventorySettingsConfig* UMounteaInventoryStatics::GetInventorySettingsConfig()
{
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return nullptr;
	return settings->InventorySettingsConfig.LoadSynchronous();
}

UPrimaryDataAsset* UMounteaInventoryStatics::GetTemplateConfig(const FString& Key)
{
	const auto config = GetInventorySettingsConfig();
	if (!config) return nullptr;
	const auto templateConfig = config->TemplatesConfig.Find(Key);
	return templateConfig ? templateConfig->LoadSynchronous() : nullptr;
}

FInventoryItem UMounteaInventoryStatics::NewInventoryItem(const FGuid& ItemGuid)
{
	return FInventoryItem(ItemGuid);
}

AActor* UMounteaInventoryStatics::GetOwningActor(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetOwningActor(Target.GetObject()) : nullptr;
}

bool UMounteaInventoryStatics::AddItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItem& Item)
{
	return Target.GetObject() ? Target->Execute_AddItem(Target.GetObject(), Item) : false;
}

bool UMounteaInventoryStatics::RemoveItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid)
{
	return Target.GetObject() ? Target->Execute_RemoveItem(Target.GetObject(), ItemGuid) : false;
}

bool UMounteaInventoryStatics::CanAddItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItem& Item)
{
	return Target.GetObject() ? Target->Execute_CanAddItem(Target.GetObject(), Item) : false;
}

bool UMounteaInventoryStatics::AddItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, const int32 Quantity, const float Durability)
{
	return Target.GetObject() ? Target->Execute_AddItemFromTemplate(Target.GetObject(), Template, Quantity, Durability) : false;
}

bool UMounteaInventoryStatics::RemoveItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, const int32 Quantity)
{
	return Target.GetObject() ? Target->Execute_RemoveItemFromTemplate(Target.GetObject(), Template, Quantity) : false;
}

bool UMounteaInventoryStatics::CanAddItemFromTemplate(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, UMounteaInventoryItemTemplate* Template, const int32 Quantity)
{
	return Target.GetObject() ? Target->Execute_CanAddItemFromTemplate(Target.GetObject(), Template, Quantity) : false;
}

FInventoryItem UMounteaInventoryStatics::FindItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams)
{
	return Target.GetObject() ? Target->Execute_FindItem(Target.GetObject(), SearchParams) : FInventoryItem();
}

int32 UMounteaInventoryStatics::FindItemIndex(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams)
{
	return Target.GetObject() ? Target->Execute_FindItemIndex(Target.GetObject(), SearchParams) : INDEX_NONE;
}

TArray<FInventoryItem> UMounteaInventoryStatics::FindItems(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryItemSearchParams& SearchParams)
{
	return Target.GetObject() ? Target->Execute_FindItems(Target.GetObject(), SearchParams) : TArray<FInventoryItem>();
}

TArray<FInventoryItem> UMounteaInventoryStatics::GetAllItems(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetAllItems(Target.GetObject()) : TArray<FInventoryItem>();
}

void UMounteaInventoryStatics::ClearInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_ClearInventory(Target.GetObject());
}

bool UMounteaInventoryStatics::HasItem(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target,
	const FInventoryItemSearchParams& SearchParams)
{
	return IsValid(Target.GetObject()) ? Target->Execute_HasItem(Target.GetObject(), SearchParams) : false;
}

bool UMounteaInventoryStatics::IncreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, const int32 Amount)
{
	return Target.GetObject() ? Target->Execute_IncreaseItemQuantity(Target.GetObject(), ItemGuid, Amount) : false;
}

bool UMounteaInventoryStatics::DecreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, const int32 Amount)
{
	return Target.GetObject() ? Target->Execute_DecreaseItemQuantity(Target.GetObject(), ItemGuid, Amount) : false;
}

bool UMounteaInventoryStatics::ModifyItemDurability(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, const float DeltaDurability)
{
	return Target.GetObject() ? Target->Execute_ModifyItemDurability(Target.GetObject(), ItemGuid, DeltaDurability) : false;
}

void UMounteaInventoryStatics::ProcessInventoryNotification(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FInventoryNotificationData& Notification)
{
	if (Target.GetObject())
		Target->Execute_ProcessInventoryNotification(Target.GetObject(), Notification);
}

FString UMounteaInventoryStatics::InventoryItemToString(const FInventoryItem& Item)
{
	return Item.ToString();
}

FInventoryCategory UMounteaInventoryStatics::GetInventoryCategory(const FInventoryItem& Item)
{
	if (!Item.Template) return FInventoryCategory();
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return FInventoryCategory();
	const auto inventorySettingsConfig = settings->InventorySettingsConfig.LoadSynchronous();
	if (!inventorySettingsConfig) return FInventoryCategory();
	const auto categoryKey = Item.Template->ItemCategory;
	return inventorySettingsConfig->AllowedCategories.Contains(categoryKey) ? inventorySettingsConfig->AllowedCategories.FindChecked(categoryKey) : FInventoryCategory();
}

FString UMounteaInventoryStatics::GetInventoryCategoryKey(const FInventoryItem& Item)
{
	if (!Item.Template) return TEXT("");
	return Item.Template->ItemCategory;
}

FInventoryRarity UMounteaInventoryStatics::GetInventoryRarity(const FInventoryItem& Item)
{
	if (!Item.Template) return FInventoryRarity();
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return FInventoryRarity();
	const auto inventorySettingsConfig = settings->InventorySettingsConfig.LoadSynchronous();
	if (!inventorySettingsConfig) return FInventoryRarity();
	const auto rarityKey = Item.Template->ItemRarity;
	return inventorySettingsConfig->AllowedRarities.Contains(rarityKey) ? inventorySettingsConfig->AllowedRarities.FindChecked(rarityKey) : FInventoryRarity();
}

FString UMounteaInventoryStatics::GetInventoryRarityKey(const FInventoryItem& Item)
{
	if (!Item.Template) return TEXT("");
	return Item.Template->ItemCategory;
}

FText UMounteaInventoryStatics::GetInventoryItemName(const FInventoryItem& Item)
{
	return Item.GetItemName();
}

FText UMounteaInventoryStatics::GetInventoryItemShortInfo(const FInventoryItem& Item)
{
	return Item.GetItemShortInfo();
}

FText UMounteaInventoryStatics::GetInventoryItemLongInfo(const FInventoryItem& Item)
{
	return Item.GetItemLongInfo();
}

UTexture2D* UMounteaInventoryStatics::GetInventoryItemCover(const FInventoryItem& Item)
{
	return Item.GetCover();
}

bool UMounteaInventoryStatics::IsInventoryItemValid(const FInventoryItem& Item)
{
	return Item.IsItemValid();
}

TArray<TSoftClassPtr<UObject>> UMounteaInventoryStatics::GetItemActions(const FInventoryItem& Item)
{
	if (!Item.IsItemValid()) return {};
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return {};
	const auto inventorySettingsConfig = settings->InventorySettingsConfig.LoadSynchronous();
	if (!inventorySettingsConfig) return {};
    
	const auto itemCategory = Item.GetTemplate()->ItemCategory;
	const auto itemSubCategory = Item.GetTemplate()->ItemSubCategory;
    
	const auto categoryDefinition = inventorySettingsConfig->AllowedCategories.FindRef(itemCategory);
	if (categoryDefinition.CategoryData.AllowedActions.Num() <= 0)
	{
		LOG_WARNING(TEXT("No actions defined for category '%s'!"), *itemCategory);
		return {};
	}
    
	TSet<TSoftClassPtr<UObject>> uniqueActions = categoryDefinition.CategoryData.AllowedActions;
	if (!itemSubCategory.IsEmpty())
		uniqueActions.Append(categoryDefinition.SubCategories.FindRef(itemSubCategory).AllowedActions);

	TArray<TSoftClassPtr<UObject>> validActions = uniqueActions.Array();
	validActions.RemoveAll([](const TSoftClassPtr<UObject>& actionClass)
	{
		return actionClass.IsNull() || !actionClass.ToSoftObjectPath().IsValid();
	});

	return validActions;
}

EInventoryItemActionCallback UMounteaInventoryStatics::GetItemActionFlags(const UObject* Target)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[GetItemActionFlags] Target is invalid!"))
		return EInventoryItemActionCallback::EIIAC_None;
	}
	if (!Target->Implements<UMounteaAdvancedInventoryItemActionInterface>())
	{
		LOG_ERROR(TEXT("[GetItemActionFlags] Target does not implement IMounteaInventoryItemActionInterface!"))
		return EInventoryItemActionCallback::EIIAC_None;
	}
	return IMounteaAdvancedInventoryItemActionInterface::Execute_GetInventoryItemActionCallback(Target);
}

bool UMounteaInventoryStatics::ItemAction_HasActionFlag(UObject* Target, const EInventoryItemActionCallback FlagToCheck)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[HasActionFlag] Target is invalid!"))
		return false;
	}
	if (!Target->Implements<UMounteaAdvancedInventoryItemActionInterface>())
	{
		LOG_ERROR(TEXT("[HasActionFlag] Target does not implement IMounteaInventoryItemActionInterface!"))
		return false;
	}
	
	return (static_cast<uint8>(IMounteaAdvancedInventoryItemActionInterface::Execute_GetInventoryItemActionCallback(Target))
		& static_cast<uint8>(FlagToCheck)) != 0;
}

void UMounteaInventoryStatics::ItemAction_AddActionFlag(UObject* Target, EInventoryItemActionCallback FlagToAdd)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[AddActionFlag] Target is invalid!"))
		return;
	}
	if (!Target->Implements<UMounteaAdvancedInventoryItemActionInterface>())
	{
		LOG_ERROR(TEXT("[AddActionFlag] Target does not implement IMounteaInventoryItemActionInterface!"))
		return;
	}

	IMounteaAdvancedInventoryItemActionInterface::Execute_AddActionFlag(Target, FlagToAdd);	
}

void UMounteaInventoryStatics::ItemAction_RemoveActionFlag(UObject* Target, const EInventoryItemActionCallback FlagToRemove)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[RemoveActionFlag] Target is invalid!"))
		return;
	}
	if (!Target->Implements<UMounteaAdvancedInventoryItemActionInterface>())
	{
		LOG_ERROR(TEXT("[RemoveActionFlag] Target does not implement IMounteaInventoryItemActionInterface!"))
		return;
	}

	IMounteaAdvancedInventoryItemActionInterface::Execute_RemoveActionFlag(Target, FlagToRemove);
}

void UMounteaInventoryStatics::ItemAction_ClearAllActionFlags(UObject* Target)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[ClearAllActionFlags] Target is invalid!"))
		return;
	}
	if (!Target->Implements<UMounteaAdvancedInventoryItemActionInterface>())
	{
		LOG_ERROR(TEXT("[ClearAllActionFlags] Target does not implement IMounteaInventoryItemActionInterface!"))
		return;
	}

	IMounteaAdvancedInventoryItemActionInterface::Execute_ClearAllActionFlags(Target);
}

FInventoryNotificationData UMounteaInventoryStatics::CreateNotificationData(
	const FString& Type,
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& SourceInventory,
	const FGuid& ItemGuid,
	const int32 QuantityDelta
)
{
	const UMounteaAdvancedInventorySettingsConfig* Config = GetDefault<UMounteaAdvancedInventorySettings>()->InventorySettingsConfig.LoadSynchronous();
	if (!Config) return FInventoryNotificationData();

	const FInventoryNotificationConfig* NotifConfig = Config->NotificationConfigs.Find(Type);
	if (!NotifConfig) return FInventoryNotificationData();

	auto inventoryItem = SourceInventory->Execute_FindItem(SourceInventory.GetObject(), FInventoryItemSearchParams(ItemGuid));
	/* if (!inventoryItem.IsItemValid()) return FInventoryNotificationData();*/ // TODO: How to process failed Item?

	FText notificationText = NotifConfig->MessageTemplate;
	notificationText = UMounteaInventorySystemStatics::ReplaceRegexInText(TEXT("${quantity}"), FText::AsNumber(FMath::Abs(QuantityDelta)), notificationText);
	notificationText = UMounteaInventorySystemStatics::ReplaceRegexInText(TEXT("${itemName}"), inventoryItem.GetItemName(), notificationText);

	
	return FInventoryNotificationData(
		Type,
		NotifConfig->NotificationCategory,
		NotifConfig->MessageTitle,
		notificationText,
		ItemGuid,
		SourceInventory,
		FMath::Abs(QuantityDelta),
		NotifConfig->DefaultDuration,
		*NotifConfig,
		nullptr
	);
}
