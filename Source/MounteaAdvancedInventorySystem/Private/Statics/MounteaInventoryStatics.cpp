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

#include "Decorations/MounteaCallbackInventoryItemAction.h"
#include "Decorations/MounteaInventoryItemAction.h"
#include "Decorations/MounteaSelectableInventoryItemAction.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseUIEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Settings/TemplatesConfig/MounteaAdvancedInventoryPayloadsConfig.h"
#include "Statics/MounteaInventorySystemStatics.h"

#pragma region ItemActions

bool UMounteaInventoryStatics::InitializeItemAction(UMounteaInventoryItemAction* Target,
	const FInventoryItem& TargetItem, const TScriptInterface<IMounteaAdvancedInventoryInterface>& OwningInventory,
	UObject* ContextPayload)
{
	return IsValid(Target) ? Target->InitializeItemAction(TargetItem, OwningInventory, ContextPayload) : false;
}

bool UMounteaInventoryStatics::ExecuteInventoryAction(UMounteaInventoryItemAction* Target, const FInventoryItem& TargetItem)
{
	return IsValid(Target) ? Target->ExecuteInventoryAction(TargetItem) : false;
}

FGameplayTagContainer UMounteaInventoryStatics::GetItemActionTags(UMounteaInventoryItemAction* Target)
{
	return IsValid(Target) ? Target->GetItemActionTags() : FGameplayTagContainer();
}

#pragma endregion 

FInventoryCategoryData UMounteaInventoryStatics::GetInventoryCategoryData(const FString& CategoryName, const FString ParentCategory, bool& bResult)
{
	bResult = false;
	if (CategoryName.IsEmpty()) return FInventoryCategoryData();
	
	const auto inventoryConfig = GetInventorySettingsConfig();
	if (!inventoryConfig) return FInventoryCategoryData();
	
	if (ParentCategory.IsEmpty())
	{
		bResult = inventoryConfig->AllowedCategories.Contains(CategoryName);
		if (bResult)
		{
			auto inventoryCategory = *inventoryConfig->AllowedCategories.Find(CategoryName);
			return inventoryCategory.CategoryData;
		}
		return FInventoryCategoryData();
	}
	
	if (!inventoryConfig->AllowedCategories.Contains(ParentCategory)) return FInventoryCategoryData();

	const FInventoryCategory parentCategory = *inventoryConfig->AllowedCategories.Find(ParentCategory);
		
	bResult = parentCategory.SubCategories.Contains(CategoryName);
	if (bResult)
	{
		auto inventoryCategory = *parentCategory.SubCategories.Find(CategoryName);
		return inventoryCategory;
	}
	return FInventoryCategoryData();
}

UMounteaAdvancedInventorySettings* UMounteaInventoryStatics::GetInventorySettings()
{
	return GetMutableDefault<UMounteaAdvancedInventorySettings>();
}

UMounteaAdvancedInventorySettingsConfig* UMounteaInventoryStatics::GetInventorySettingsConfig()
{
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return nullptr;
	return settings->AdvancedInventorySettingsConfig.LoadSynchronous();
}

UPrimaryDataAsset* UMounteaInventoryStatics::GetTemplateConfig(const FString& Key)
{
	const auto config = GetInventorySettingsConfig();
	if (!config) return nullptr;
	const auto templateConfig = config->TemplatesConfig.Find(Key);
	return templateConfig ? templateConfig->LoadSynchronous() : nullptr;
}

UMounteaAdvancedInventoryPayloadsConfig* UMounteaInventoryStatics::GetPayloadsConfig()
{
	return Cast<UMounteaAdvancedInventoryPayloadsConfig>(GetTemplateConfig(TEXT("Payloads")));
}

FPayloadConfig UMounteaInventoryStatics::FindPayloadConfig(const FGameplayTag Creator, const FGameplayTag Receiver, bool& bSuccess)
{
	const auto payloadConfig = GetPayloadsConfig();
	if (!IsValid(payloadConfig))
	{
		bSuccess = false;
		return {};
	}

	FPayloadConfig searchKey;
	searchKey.PayloadCreator = Creator;
	searchKey.PayloadReceiver = Receiver;

	if (const FPayloadConfig* foundConfig = payloadConfig->PayloadConfigs.Find(searchKey))
	{
		bSuccess = true;
		return *foundConfig;
	}

	bSuccess = false;
	return {};
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

FString UMounteaInventoryStatics::InventoryToString(const TScriptInterface<IMounteaAdvancedInventoryInterface>& Target)
{
	if (!Target.GetObject() || !Target.GetObject()->Implements<UMounteaAdvancedInventoryInterface>())
		return FString();
	
	TArray<FInventoryItem> allItems = GetAllItems(Target);
	AActor* inventoryOwner = GetOwningActor(Target);
	
	FString returnValue = FString::Printf(TEXT("Inventory [Owner: %s, Items: %d]\n"),
		inventoryOwner ? *inventoryOwner->GetName() : TEXT("None"),
		allItems.Num()
	);
	
	for (int32 i = 0; i < allItems.Num(); ++i)
	{
		returnValue += FString::Printf(TEXT("[%d] %s\n"), i, *InventoryItemToString(allItems[i]));
	}
	
	return returnValue;
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
	const auto advancedInventorySettingsConfig = settings->AdvancedInventorySettingsConfig.LoadSynchronous();
	if (!advancedInventorySettingsConfig) return FInventoryCategory();
	const auto categoryKey = Item.Template->ItemCategory;
	return advancedInventorySettingsConfig->AllowedCategories.Contains(categoryKey) ? advancedInventorySettingsConfig->AllowedCategories.FindChecked(categoryKey) : FInventoryCategory();
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
	const auto advancedInventorySettingsConfig = settings->AdvancedInventorySettingsConfig.LoadSynchronous();
	if (!advancedInventorySettingsConfig) return FInventoryRarity();
	const auto rarityKey = Item.Template->ItemRarity;
	return advancedInventorySettingsConfig->AllowedRarities.Contains(rarityKey) ? advancedInventorySettingsConfig->AllowedRarities.FindChecked(rarityKey) : FInventoryRarity();
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

TArray<UMounteaCallbackInventoryItemAction*> UMounteaInventoryStatics::GetItemActions(const FInventoryItem& Item)
{	
	const auto itemTemplate = Item.GetTemplate();
	if (!itemTemplate) return {};
    
	TArray<UMounteaCallbackInventoryItemAction*> validActions;
	const TArray<UMounteaSelectableInventoryItemAction*> uniqueActions = GetDisplayableItemActions(Item);

	Algo::TransformIf(
		uniqueActions,
		validActions,
		[](const UMounteaSelectableInventoryItemAction* A)
		{
			return A && A->GetCallbackItemAction() != nullptr;
		},
		[](const UMounteaSelectableInventoryItemAction* A)
		{
			return A->GetCallbackItemAction();
		}
	);

	return validActions;
}

TArray<UMounteaSelectableInventoryItemAction*> UMounteaInventoryStatics::GetDisplayableItemActions(const FInventoryItem& Item)
{	
	const auto itemTemplate = Item.GetTemplate();
	if (!itemTemplate) return {};
	
	const TArray<UMounteaSelectableInventoryItemAction*> uniqueActions = itemTemplate->ItemActions;
	TArray<UMounteaSelectableInventoryItemAction*> validActions = uniqueActions;
	
	validActions.RemoveAll([Item](const UMounteaSelectableInventoryItemAction* actionData)
	{
		return actionData && actionData->IsAllowed(Item) != true && actionData->IsActionVisible(Item) != true;
	});
	
	validActions.Sort([](const UMounteaSelectableInventoryItemAction& A, const UMounteaSelectableInventoryItemAction& B)
	{
		if (!IsValid(&A)) return false;
		if (!IsValid(&B)) return true;
		return A.GetActionPriority() > B.GetActionPriority();
	});

	return validActions;
}

FGameplayTagContainer UMounteaInventoryStatics::GetItemCustomData(const FInventoryItem& Item)
{
	if (!Item.IsItemValid()) return {};
	return Item.GetCustomData();
}

TArray<FInventoryItem> UMounteaInventoryStatics::SortInventoryItems(const TArray<FInventoryItem>& Items, const TArray<FInventorySortCriteria>& SortingCriteria)
{
	if (Items.Num() < 1 || SortingCriteria.Num() == 0)
		return TArray<FInventoryItem>();
	
	if (Items.Num() == 1)
		return Items;

	TArray<FInventoryItem> returnValue = Items;
	enum class ESortKey : uint8 { Name, Value, Weight, Rarity, Unknown };
	
	auto getSortKey = [](const FString& Key) -> ESortKey
	{
		if (Key.Equals(TEXT("Name"), ESearchCase::IgnoreCase)) return ESortKey::Name;
		if (Key.Equals(TEXT("Value"), ESearchCase::IgnoreCase)) return ESortKey::Value;
		if (Key.Equals(TEXT("Weight"), ESearchCase::IgnoreCase)) return ESortKey::Weight;
		if (Key.Equals(TEXT("Rarity"), ESearchCase::IgnoreCase)) return ESortKey::Rarity;
		return ESortKey::Unknown;
	};
	
	TArray<FInventorySortCriteria> sortedCriteria = SortingCriteria;
	sortedCriteria.Sort([](const FInventorySortCriteria& A, const FInventorySortCriteria& B)
	{
		return A.SortPriority > B.SortPriority;
	});
	
	for (const FInventorySortCriteria& Criteria : sortedCriteria)
	{
		const ESortKey sortKey = getSortKey(Criteria.SortingKey);
		
		Algo::StableSort(returnValue, [sortKey](const FInventoryItem& A, const FInventoryItem& B)
		{
			switch (sortKey)
			{
				case ESortKey::Name:
				return GetInventoryItemName(A).ToString().Compare(GetInventoryItemName(B).ToString()) < 0;
				
				case ESortKey::Value:
					if (!A.Template || !B.Template) return false;
					return A.Template->BasePrice * A.GetQuantity() < B.Template->BasePrice * B.GetQuantity();
					
				case ESortKey::Weight:
					if (!A.Template || !B.Template) return false;
					return A.Template->Weight * A.GetQuantity() < B.Template->Weight * B.GetQuantity();
					
				case ESortKey::Rarity:
					{
						FInventoryRarity RarityA = GetInventoryRarity(A);
						FInventoryRarity RarityB = GetInventoryRarity(B);
						return RarityA.RarityPriority < RarityB.RarityPriority;
					}
				
				default:
					return false;
			}
		});
	}
	
	return returnValue;
}

FString UMounteaInventoryStatics::ItemTemplate_GetItemTemplateJson(UMounteaInventoryItemTemplate* ItemTemplate)
{
	if (!ItemTemplate)
		return TEXT("");
	
	ItemTemplate_CalculateItemTemplateJson(ItemTemplate);
	
	return ItemTemplate->GetJson();
}

bool UMounteaInventoryStatics::ItemTemplate_CalculateItemTemplateJson(UMounteaInventoryItemTemplate* ItemTemplate)
{
	auto CreateAssetLookup = [](const FSoftObjectPath& SoftPath) -> TSharedPtr<FJsonObject>
    {
        TSharedPtr<FJsonObject> lookupObject = MakeShared<FJsonObject>();
        if (SoftPath.IsValid())
        {
            lookupObject->SetStringField(TEXT("name"), SoftPath.GetAssetName());
            lookupObject->SetStringField(TEXT("path"), SoftPath.ToString());
        }
        else
        {
            lookupObject->SetStringField(TEXT("name"), TEXT(""));
            lookupObject->SetStringField(TEXT("path"), TEXT(""));
        }
        return lookupObject;
    };
    
    TSharedPtr<FJsonObject> rootObject = MakeShared<FJsonObject>();
    
    rootObject->SetStringField(TEXT("guid"), ItemTemplate->Guid.ToString());
    rootObject->SetStringField(TEXT("displayName"), ItemTemplate->DisplayName.ToString());
    rootObject->SetStringField(TEXT("category"), ItemTemplate->ItemCategory);
    rootObject->SetStringField(TEXT("subCategory"), ItemTemplate->ItemSubCategory);
    rootObject->SetStringField(TEXT("rarity"), ItemTemplate->ItemRarity);
    rootObject->SetNumberField(TEXT("flags"), ItemTemplate->ItemFlags);
    rootObject->SetNumberField(TEXT("maxQuantity"), ItemTemplate->MaxQuantity);
    rootObject->SetNumberField(TEXT("maxStackSize"),ItemTemplate->MaxStackSize);
    
    TArray<TSharedPtr<FJsonValue>> tagsArray;
    for (const FGameplayTag& Tag : ItemTemplate->Tags)
    {
        tagsArray.Add(MakeShared<FJsonValueString>(Tag.ToString()));
    }
    rootObject->SetArrayField(TEXT("tags"), tagsArray);
    
    rootObject->SetObjectField(TEXT("spawnActor"), CreateAssetLookup(ItemTemplate->SpawnActor.ToSoftObjectPath()));
    
    TSharedPtr<FJsonObject> descriptionObject = MakeShared<FJsonObject>();
    descriptionObject->SetStringField(TEXT("short"), ItemTemplate->ItemShortInfo.ToString());
    descriptionObject->SetStringField(TEXT("long"), ItemTemplate->ItemLongInfo.ToString());
    rootObject->SetObjectField(TEXT("description"), descriptionObject);
    
    TSharedPtr<FJsonObject> visualsObject = MakeShared<FJsonObject>();
    visualsObject->SetObjectField(TEXT("thumbnail"), CreateAssetLookup(ItemTemplate->ItemThumbnail.ToSoftObjectPath()));
    visualsObject->SetObjectField(TEXT("cover"), CreateAssetLookup(ItemTemplate->ItemCover.ToSoftObjectPath()));
    visualsObject->SetObjectField(TEXT("mesh"), CreateAssetLookup(ItemTemplate->ItemMesh ? FSoftObjectPath(ItemTemplate->ItemMesh->GetPathName()) : FSoftObjectPath()));
    rootObject->SetObjectField(TEXT("visuals"), visualsObject);
    
    TSharedPtr<FJsonObject> durabilityObject = MakeShared<FJsonObject>();
    durabilityObject->SetBoolField(TEXT("enabled"), ItemTemplate->bHasDurability);
    durabilityObject->SetNumberField(TEXT("max"), ItemTemplate->MaxDurability);
    durabilityObject->SetNumberField(TEXT("base"), ItemTemplate->BaseDurability);
    durabilityObject->SetNumberField(TEXT("penalization"), ItemTemplate->DurabilityPenalization);
    durabilityObject->SetNumberField(TEXT("priceCoefficient"), ItemTemplate->DurabilityToPriceCoefficient);
    rootObject->SetObjectField(TEXT("durability"), durabilityObject);
    
    TSharedPtr<FJsonObject> economyObject = MakeShared<FJsonObject>();
    economyObject->SetBoolField(TEXT("enabled"), ItemTemplate->bHasPrice);
    economyObject->SetNumberField(TEXT("basePrice"), ItemTemplate->BasePrice);
    economyObject->SetNumberField(TEXT("sellCoefficient"), ItemTemplate->SellPriceCoefficient);
    rootObject->SetObjectField(TEXT("economy"), economyObject);
    
    TSharedPtr<FJsonObject> weightObject = MakeShared<FJsonObject>();
    weightObject->SetBoolField(TEXT("enabled"), ItemTemplate->bHasWeight);
    weightObject->SetNumberField(TEXT("value"), ItemTemplate->Weight);
    rootObject->SetObjectField(TEXT("weight"), weightObject);
    
    TArray<TSharedPtr<FJsonValue>> attachmentSlotsArray;
    for (const FGameplayTag& Slot : ItemTemplate->AttachmentSlots)
    {
        attachmentSlotsArray.Add(MakeShared<FJsonValueString>(Slot.ToString()));
    }
    rootObject->SetArrayField(TEXT("attachmentSlots"), attachmentSlotsArray);
    
    TArray<TSharedPtr<FJsonValue>> specialAffectsArray;
    for (const TSoftClassPtr<UObject>& Affect : ItemTemplate->ItemSpecialAffects)
    {
        specialAffectsArray.Add(MakeShared<FJsonValueObject>(CreateAssetLookup(Affect.ToSoftObjectPath())));
    }
    rootObject->SetArrayField(TEXT("specialAffects"), specialAffectsArray);
    
    FString outputString;
    TSharedRef<TJsonWriter<>> jsonWriter = TJsonWriterFactory<>::Create(&outputString);
    if (FJsonSerializer::Serialize(rootObject.ToSharedRef(), jsonWriter))
    {
    	ItemTemplate->SetJson(outputString);
        return true;
    }
    
    return false;
}

EInventoryItemActionCallback UMounteaInventoryStatics::GetItemActionFlags(const UMounteaSelectableInventoryItemAction* Target)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[GetItemActionFlags] Target is invalid!"))
		return EInventoryItemActionCallback::EIIAC_None;
	}
	return Target->GetInventoryItemActionCallback();
}

bool UMounteaInventoryStatics::ItemAction_HasActionFlag(UMounteaSelectableInventoryItemAction* Target, const EInventoryItemActionCallback FlagToCheck)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[HasActionFlag] Target is invalid!"))
		return false;
	}	
	return (static_cast<uint8>(Target->GetInventoryItemActionCallback())
		& static_cast<uint8>(FlagToCheck)) != 0;
}

void UMounteaInventoryStatics::ItemAction_AddActionFlag(UMounteaSelectableInventoryItemAction* Target, EInventoryItemActionCallback FlagToAdd)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[AddActionFlag] Target is invalid!"))
		return;
	}
	Target->AddActionFlag(FlagToAdd);	
}

void UMounteaInventoryStatics::ItemAction_RemoveActionFlag(UMounteaSelectableInventoryItemAction* Target, const EInventoryItemActionCallback FlagToRemove)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[RemoveActionFlag] Target is invalid!"))
		return;
	}
	Target->RemoveActionFlag(FlagToRemove);
}

void UMounteaInventoryStatics::ItemAction_ClearAllActionFlags(UMounteaSelectableInventoryItemAction* Target)
{
	if (!IsValid(Target))
	{
		LOG_ERROR(TEXT("[ClearAllActionFlags] Target is invalid!"))
		return;
	}
	Target->ClearAllActionFlags();
}

void UMounteaInventoryStatics::CleanupInventoryAction(UMounteaInventoryItemAction* Target)
{
	if (IsValid(Target))
		Target->CleanupInventoryAction();
}

FInventoryNotificationData UMounteaInventoryStatics::CreateNotificationData(
	const FString& Type,
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& SourceInventory,
	const FGuid& ItemGuid,
	const int32 QuantityDelta
)
{
	const UMounteaAdvancedInventorySettingsConfig* Config = GetDefault<UMounteaAdvancedInventorySettings>()->AdvancedInventorySettingsConfig.LoadSynchronous();
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
