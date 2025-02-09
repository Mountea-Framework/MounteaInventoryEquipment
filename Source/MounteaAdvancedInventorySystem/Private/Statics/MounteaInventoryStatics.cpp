// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryStatics.h"

#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

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

bool UMounteaInventoryStatics::IncreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, int32 Amount)
{
	return Target.GetObject() ? Target->Execute_IncreaseItemQuantity(Target.GetObject(), ItemGuid, Amount) : false;
}

bool UMounteaInventoryStatics::DecreaseItemQuantity(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, int32 Amount)
{
	return Target.GetObject() ? Target->Execute_DecreaseItemQuantity(Target.GetObject(), ItemGuid, Amount) : false;
}

bool UMounteaInventoryStatics::ModifyItemDurability(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target, const FGuid& ItemGuid, float DeltaDurability)
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

FInventoryNotificationData UMounteaInventoryStatics::CreateNotificationData(
	const EInventoryNotificationType Type,
	const EInventoryNotificationCategory Category,
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& SourceInventory,
	const FGuid& ItemGuid,
	const int32 QuantityDelta
)
{
	const UMounteaAdvancedInventorySettingsConfig* Config = GetDefault<UMounteaAdvancedInventorySettings>()->InventorySettingsConfig.LoadSynchronous();
	if (!Config) return FInventoryNotificationData();

	const FInventoryNotificationConfig* NotifConfig = Config->NotificationConfigs.Find(Category);
	if (!NotifConfig) return FInventoryNotificationData();

	return FInventoryNotificationData(
		Type,
		Category,
		NotifConfig->MessageTemplate, // TODO: Compile text together
		ItemGuid,
		SourceInventory,
		FMath::Abs(QuantityDelta),
		0,
		NotifConfig->DefaultDuration
	);
}
