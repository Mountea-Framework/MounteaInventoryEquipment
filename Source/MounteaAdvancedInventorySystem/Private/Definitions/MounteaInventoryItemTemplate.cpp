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


#include "Definitions/MounteaInventoryItemTemplate.h"

#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

UMounteaInventoryItemTemplate::UMounteaInventoryItemTemplate():
	Guid(FGuid::NewGuid()),
	ItemCategory(TEXT("Miscellaneous")),
	ItemRarity(TEXT("Common")),
	ItemFlags(0),
	MaxQuantity(1),
	MaxStackSize(99),
	SpawnActor(nullptr),
	ItemMesh(nullptr),
	bHasDurability(false),
	MaxDurability(0),
	BaseDurability(0),
	DurabilityPenalization(0),
	DurabilityToPriceCoefficient(0),
	bHasPrice(false),
	BasePrice(0),
	SellPriceCoefficient(1.f),
	bHasWeight(false),
	Weight(0)
{
}

FString UMounteaInventoryItemTemplate::GetJson() const
{
	return JsonManifest;
}

void UMounteaInventoryItemTemplate::SetJson(const FString& Json)
{
	if (Json.Equals(JsonManifest))
		return;
	
	JsonManifest = Json;
}

bool UMounteaInventoryItemTemplate::CalculateJson()
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
    
    rootObject->SetStringField(TEXT("guid"), Guid.ToString());
    rootObject->SetStringField(TEXT("displayName"), DisplayName.ToString());
    rootObject->SetStringField(TEXT("category"), ItemCategory);
    rootObject->SetStringField(TEXT("subCategory"), ItemSubCategory);
    rootObject->SetStringField(TEXT("rarity"), ItemRarity);
    rootObject->SetNumberField(TEXT("flags"), ItemFlags);
    rootObject->SetNumberField(TEXT("maxQuantity"), MaxQuantity);
    rootObject->SetNumberField(TEXT("maxStackSize"), MaxStackSize);
    
    TArray<TSharedPtr<FJsonValue>> tagsArray;
    for (const FGameplayTag& Tag : Tags)
    {
        tagsArray.Add(MakeShared<FJsonValueString>(Tag.ToString()));
    }
    rootObject->SetArrayField(TEXT("tags"), tagsArray);
    
    rootObject->SetObjectField(TEXT("spawnActor"), CreateAssetLookup(SpawnActor.ToSoftObjectPath()));
    
    TSharedPtr<FJsonObject> descriptionObject = MakeShared<FJsonObject>();
    descriptionObject->SetStringField(TEXT("short"), ItemShortInfo.ToString());
    descriptionObject->SetStringField(TEXT("long"), ItemLongInfo.ToString());
    rootObject->SetObjectField(TEXT("description"), descriptionObject);
    
    TSharedPtr<FJsonObject> visualsObject = MakeShared<FJsonObject>();
    visualsObject->SetObjectField(TEXT("thumbnail"), CreateAssetLookup(ItemThumbnail.ToSoftObjectPath()));
    visualsObject->SetObjectField(TEXT("cover"), CreateAssetLookup(ItemCover.ToSoftObjectPath()));
    visualsObject->SetObjectField(TEXT("mesh"), CreateAssetLookup(ItemMesh ? FSoftObjectPath(ItemMesh->GetPathName()) : FSoftObjectPath()));
    rootObject->SetObjectField(TEXT("visuals"), visualsObject);
    
    TSharedPtr<FJsonObject> durabilityObject = MakeShared<FJsonObject>();
    durabilityObject->SetBoolField(TEXT("enabled"), bHasDurability);
    durabilityObject->SetNumberField(TEXT("max"), MaxDurability);
    durabilityObject->SetNumberField(TEXT("base"), BaseDurability);
    durabilityObject->SetNumberField(TEXT("penalization"), DurabilityPenalization);
    durabilityObject->SetNumberField(TEXT("priceCoefficient"), DurabilityToPriceCoefficient);
    rootObject->SetObjectField(TEXT("durability"), durabilityObject);
    
    TSharedPtr<FJsonObject> economyObject = MakeShared<FJsonObject>();
    economyObject->SetBoolField(TEXT("enabled"), bHasPrice);
    economyObject->SetNumberField(TEXT("basePrice"), BasePrice);
    economyObject->SetNumberField(TEXT("sellCoefficient"), SellPriceCoefficient);
    rootObject->SetObjectField(TEXT("economy"), economyObject);
    
    TSharedPtr<FJsonObject> weightObject = MakeShared<FJsonObject>();
    weightObject->SetBoolField(TEXT("enabled"), bHasWeight);
    weightObject->SetNumberField(TEXT("value"), Weight);
    rootObject->SetObjectField(TEXT("weight"), weightObject);
    
    TArray<TSharedPtr<FJsonValue>> attachmentSlotsArray;
    for (const FGameplayTag& Slot : AttachmentSlots)
    {
        attachmentSlotsArray.Add(MakeShared<FJsonValueString>(Slot.ToString()));
    }
    rootObject->SetArrayField(TEXT("attachmentSlots"), attachmentSlotsArray);
    
    TArray<TSharedPtr<FJsonValue>> specialAffectsArray;
    for (const TSoftClassPtr<UObject>& Affect : ItemSpecialAffects)
    {
        specialAffectsArray.Add(MakeShared<FJsonValueObject>(CreateAssetLookup(Affect.ToSoftObjectPath())));
    }
    rootObject->SetArrayField(TEXT("specialAffects"), specialAffectsArray);
    
    FString outputString;
    TSharedRef<TJsonWriter<>> jsonWriter = TJsonWriterFactory<>::Create(&outputString);
    if (FJsonSerializer::Serialize(rootObject.ToSharedRef(), jsonWriter))
    {
        JsonManifest = outputString;
        return true;
    }
    
    return false;
}

TArray<FString> UMounteaInventoryItemTemplate::GetAllowedCategories()
{
	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
	{
		TArray<FString> returnValues;
		returnValues.Add(TEXT("Miscellaneous"));
		return returnValues;
	}
	return inventorySettings->GetAllowedCategories();
}

TArray<FString> UMounteaInventoryItemTemplate::GetAllowedSubCategories() const
{
	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
		return TArray<FString>();
	
	auto inventorySettingsConfig = inventorySettings->InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(inventorySettingsConfig))
		return TArray<FString>();

	if (inventorySettingsConfig->AllowedCategories.Num() <= 0)
		return TArray<FString>();

	TArray<FString> returnValues;
	if (const auto categoryConfiguration = inventorySettingsConfig->AllowedCategories.Find(ItemCategory))
		categoryConfiguration->SubCategories.GetKeys(returnValues);
	
	return returnValues;
}

TArray<FString> UMounteaInventoryItemTemplate::GetAllowedRarities()
{
	TArray<FString> returnValues;
	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
	{
		returnValues.Add(TEXT("Common"));
		return returnValues;
	}
	return inventorySettings->GetAllowedRarities();
}

#if WITH_EDITOR

void UMounteaInventoryItemTemplate::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto inventorySettings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!inventorySettings)
		return;
	auto inventorySettingsConfig = inventorySettings->InventorySettingsConfig.LoadSynchronous();
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemTemplate, ItemCategory))
	{
		if (inventorySettingsConfig)
		{
			auto categoryConfiguration = inventorySettingsConfig->AllowedCategories.Find(ItemCategory);
			ItemFlags = categoryConfiguration->CategoryData.CategoryFlags;
		}

		ItemSubCategory = TEXT("");
	}
	
	CalculateJson();
}

#endif
