// All rights reserved Dominik Morse (Pavlicek) 2024


#include "Helpers/MounteaInventorySerializationLibrary.h"

#include "Definitions/MounteaInventoryItemBlueprint.h"
#include "Definitions/MounteaInventoryItemsLibrary.h"
#include "Dom/JsonObject.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

FString UMounteaInventorySerializationLibrary::ToJson(const FMounteaInventoryItemRequiredData& ItemRequiredData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("ItemName"), ItemRequiredData.ItemName.ToString());
	JsonObject->SetStringField(TEXT("ItemShortInfo"), ItemRequiredData.ItemShortInfo.ToString());
	JsonObject->SetStringField(TEXT("ItemLongInfo"), ItemRequiredData.ItemLongInfo.ToString());
	JsonObject->SetNumberField(TEXT("ItemFlags"), ItemRequiredData.ItemFlags);
    
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
	return OutputString;
}

FString UMounteaInventorySerializationLibrary::ToJson(const FMounteaInventoryItemOptionalData& ItemOptionalData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetBoolField(TEXT("bHasDurability"), ItemOptionalData.bHasDurability);
	JsonObject->SetNumberField(TEXT("MaxDurability"), ItemOptionalData.MaxDurability);
	JsonObject->SetNumberField(TEXT("BaseDurability"), ItemOptionalData.BaseDurability);
	JsonObject->SetNumberField(TEXT("DurabilityPenalization"), ItemOptionalData.DurabilityPenalization);
	JsonObject->SetNumberField(TEXT("DurabilityToPriceCoefficient"), ItemOptionalData.DurabilityToPriceCoefficient);
	JsonObject->SetBoolField(TEXT("bHasPrice"), ItemOptionalData.bHasPrice);
	JsonObject->SetNumberField(TEXT("BasePrice"), ItemOptionalData.BasePrice);
	JsonObject->SetNumberField(TEXT("SellPriceCoefficient"), ItemOptionalData.SellPriceCoefficient);
	JsonObject->SetNumberField(TEXT("MaxStackSize"), ItemOptionalData.MaxStackSize);
	JsonObject->SetNumberField(TEXT("MaxQuantity"), ItemOptionalData.MaxQuantity);
	JsonObject->SetBoolField(TEXT("bHasWeight"), ItemOptionalData.bHasWeight);
	JsonObject->SetNumberField(TEXT("Weight"), ItemOptionalData.Weight);
    
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
	return OutputString;
}

FString UMounteaInventorySerializationLibrary::ToJson(const struct FMounteaInventoryItemsLibraryRow& ItemBlueprintRow)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("MaxDurability"), ItemBlueprintRow.InitialQuantity);
	JsonObject->SetStringField(TEXT("ItemName"), ItemBlueprintRow.InventoryItemBlueprintName);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
	return OutputString;
}

FString UMounteaInventorySerializationLibrary::ToJson(const UMounteaInventoryItemBlueprint* ItemBlueprint)
{
	if (!IsValid(ItemBlueprint))
	{
		return TEXT("");
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	
	JsonObject->SetStringField(TEXT("BlueprintGuid"), ItemBlueprint->GetBlueprintGuid().ToString(EGuidFormats::DigitsWithHyphensLower));
	
	TArray<FGameplayTag> tagsArray;
	ItemBlueprint->GetDefaultTags().GetGameplayTagArray(tagsArray);
	TArray<TSharedPtr<FJsonValue>> TagsArray;
	for (const FGameplayTag& Tag : tagsArray)
	{
		TagsArray.Add(MakeShareable(new FJsonValueString(Tag.ToString())));
	}
	JsonObject->SetArrayField(TEXT("DefaultTags"), TagsArray);

	JsonObject->SetStringField(TEXT("PrimaryData"), ToJson(ItemBlueprint->GetPrimaryData()));
	JsonObject->SetStringField(TEXT("SecondaryData"), ToJson(ItemBlueprint->GetSecondaryData()));

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}


FString UMounteaInventorySerializationLibrary::ToJson(const class UMounteaInventoryItemsLibrary* ItemBlueprintsLibrary)
{
	if (!IsValid(ItemBlueprintsLibrary))
	{
		return TEXT("");
	}
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> JsonArray;
    
	for (UMounteaInventoryItemBlueprint* Blueprint : ItemBlueprintsLibrary->GetInventoryItemBlueprints())
	{
		JsonArray.Add(MakeShareable(new FJsonValueString(ToJson(Blueprint))));
	}
    
	JsonObject->SetArrayField(TEXT("ItemBlueprints"), JsonArray);
    
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
	return OutputString;
}
