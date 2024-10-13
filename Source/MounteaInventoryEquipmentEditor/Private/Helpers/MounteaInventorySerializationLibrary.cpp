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

	JsonObject->SetStringField(TEXT("ItemName"), ItemRequiredData.ItemName.IsEmpty()
													 ? TEXT("Default Item Name")
													 : ItemRequiredData.ItemName.ToString());
	JsonObject->SetStringField(TEXT("ItemShortInfo"), ItemRequiredData.ItemShortInfo.IsEmpty()
														  ? TEXT("No short info")
														  : ItemRequiredData.ItemShortInfo.ToString());
	JsonObject->SetStringField(TEXT("ItemLongInfo"), ItemRequiredData.ItemLongInfo.IsEmpty()
														 ? TEXT("No long info")
														 : ItemRequiredData.ItemLongInfo.ToString());
	JsonObject->SetNumberField(TEXT("ItemFlags"), ItemRequiredData.ItemFlags ? ItemRequiredData.ItemFlags : 0);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

FString UMounteaInventorySerializationLibrary::ToJson(const FMounteaInventoryItemOptionalData& ItemOptionalData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetBoolField(TEXT("bHasDurability"), ItemOptionalData.bHasDurability);
	JsonObject->SetNumberField(
		TEXT("MaxDurability"), ItemOptionalData.MaxDurability > 0 ? ItemOptionalData.MaxDurability : 10.0f);
	JsonObject->SetNumberField(
		TEXT("BaseDurability"), ItemOptionalData.BaseDurability > 0 ? ItemOptionalData.BaseDurability : 10.0f);
	JsonObject->SetNumberField(TEXT("DurabilityPenalization"), ItemOptionalData.DurabilityPenalization > 0
																   ? ItemOptionalData.DurabilityPenalization
																   : 1.0f);
	JsonObject->SetNumberField(TEXT("DurabilityToPriceCoefficient"), ItemOptionalData.DurabilityToPriceCoefficient > 0
																		 ? ItemOptionalData.DurabilityToPriceCoefficient
																		 : 1.0f);

	JsonObject->SetBoolField(TEXT("bHasPrice"), ItemOptionalData.bHasPrice);
	JsonObject->SetNumberField(TEXT("BasePrice"), ItemOptionalData.BasePrice > 0 ? ItemOptionalData.BasePrice : 10.0f);
	JsonObject->SetNumberField(
		TEXT("SellPriceCoefficient"),
		ItemOptionalData.SellPriceCoefficient > 0 ? ItemOptionalData.SellPriceCoefficient : 1.0f);
	JsonObject->SetNumberField(
		TEXT("MaxStackSize"), ItemOptionalData.MaxStackSize > 0 ? ItemOptionalData.MaxStackSize : 30);
	JsonObject->
		SetNumberField(TEXT("MaxQuantity"), ItemOptionalData.MaxQuantity > 0 ? ItemOptionalData.MaxQuantity : 0);
	JsonObject->SetBoolField(TEXT("bHasWeight"), ItemOptionalData.bHasWeight);
	JsonObject->SetNumberField(TEXT("Weight"), ItemOptionalData.Weight > 0 ? ItemOptionalData.Weight : 0.1f);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

FString UMounteaInventorySerializationLibrary::ToJson(const FMounteaInventoryItemsLibraryRow& ItemBlueprintRow)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetNumberField(TEXT("InitialQuantity"), ItemBlueprintRow.InitialQuantity);
	JsonObject->SetStringField(TEXT("InventoryItemBlueprint"), ToJson(ItemBlueprintRow.InventoryItemBlueprint.LoadSynchronous()));

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

FString UMounteaInventorySerializationLibrary::ToJson(const UMounteaInventoryItemBlueprint* ItemBlueprint)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> TagsArray;

	if (ItemBlueprint)
	{
		TArray<FGameplayTag> tagsArray;
		ItemBlueprint->GetDefaultTags().GetGameplayTagArray(tagsArray);
        
		for (const FGameplayTag& Tag : tagsArray)
		{
			TagsArray.Add(MakeShareable(new FJsonValueString(Tag.ToString())));
		}
	}

	JsonObject->SetStringField(TEXT("BlueprintGuid"), ItemBlueprint ? ItemBlueprint->GetBlueprintGuid().ToString(EGuidFormats::DigitsWithHyphensLower) : FGuid().ToString(EGuidFormats::DigitsWithHyphensLower));
    
	JsonObject->SetStringField(TEXT("PrimaryData"), ItemBlueprint ? ToJson(ItemBlueprint->GetPrimaryData()) : ToJson(FMounteaInventoryItemRequiredData()));
	JsonObject->SetStringField(TEXT("SecondaryData"), ItemBlueprint ? ToJson(ItemBlueprint->GetSecondaryData()) : ToJson(FMounteaInventoryItemOptionalData()));

	JsonObject->SetArrayField(TEXT("DefaultTags"), TagsArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

FString UMounteaInventorySerializationLibrary::ToJson(const UMounteaInventoryItemsLibrary* ItemBlueprintsLibrary)
{
	if (!IsValid(ItemBlueprintsLibrary))
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		JsonObject->SetArrayField(TEXT("ItemBlueprintsRows"), TArray<TSharedPtr<FJsonValue>>());

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

		return OutputString;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	for (const auto& Row : ItemBlueprintsLibrary->GetInventoryItemBlueprintRows())
	{
		if (Row.InventoryItemBlueprint)
		{
			JsonArray.Add(MakeShareable(new FJsonValueString(ToJson(Row))));
		}
		else
		{
			TSharedPtr<FJsonObject> EmptyRowObject = MakeShareable(new FJsonObject);
			EmptyRowObject->SetStringField(TEXT("InventoryItemBlueprintGuid"), FGuid().ToString(EGuidFormats::DigitsWithHyphensLower));
			EmptyRowObject->SetNumberField(TEXT("InitialQuantity"), 0);
			JsonArray.Add(MakeShareable(new FJsonValueObject(EmptyRowObject)));
		}
	}

	JsonObject->SetArrayField(TEXT("ItemBlueprintsRows"), JsonArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

FMounteaInventoryItemRequiredData UMounteaInventorySerializationLibrary::RequiredDataFromJson(const TSharedPtr<FJsonObject>& JsonObject)
{
	FMounteaInventoryItemRequiredData resultItemData = FMounteaInventoryItemRequiredData();
	if (!JsonObject.IsValid()) return resultItemData;

	resultItemData.ItemName = FText::FromString(JsonObject->GetStringField(TEXT("ItemName")));
	resultItemData.ItemShortInfo = FText::FromString(JsonObject->GetStringField(TEXT("ItemShortInfo")));
	resultItemData.ItemLongInfo = FText::FromString(JsonObject->GetStringField(TEXT("ItemLongInfo")));
	resultItemData.ItemFlags = JsonObject->GetNumberField(TEXT("ItemFlags"));

	return resultItemData;
}

FMounteaInventoryItemOptionalData UMounteaInventorySerializationLibrary::OptionalDataFromJson(const TSharedPtr<FJsonObject>& JsonObject)
{
	FMounteaInventoryItemOptionalData resultItemData = FMounteaInventoryItemOptionalData();
	if (!JsonObject.IsValid()) return resultItemData;

	resultItemData.bHasDurability = JsonObject->GetBoolField(TEXT("bHasDurability"));
	resultItemData.MaxDurability = JsonObject->GetNumberField(TEXT("MaxDurability"));
	resultItemData.BaseDurability = JsonObject->GetNumberField(TEXT("BaseDurability"));
	resultItemData.DurabilityPenalization = JsonObject->GetNumberField(TEXT("DurabilityPenalization"));
	resultItemData.DurabilityToPriceCoefficient = JsonObject->GetNumberField(TEXT("DurabilityToPriceCoefficient"));

	resultItemData.bHasPrice = JsonObject->GetBoolField(TEXT("bHasPrice"));
	resultItemData.BasePrice = JsonObject->GetNumberField(TEXT("BasePrice"));
	resultItemData.SellPriceCoefficient = JsonObject->GetNumberField(TEXT("SellPriceCoefficient"));
	resultItemData.MaxStackSize = JsonObject->GetNumberField(TEXT("MaxStackSize"));
	resultItemData.MaxQuantity = JsonObject->GetNumberField(TEXT("MaxQuantity"));
	resultItemData.bHasWeight = JsonObject->GetBoolField(TEXT("bHasWeight"));
	resultItemData.Weight = JsonObject->GetNumberField(TEXT("Weight"));

	return resultItemData;
}

UMounteaInventoryItemBlueprint* UMounteaInventorySerializationLibrary::ItemBlueprintFromJson(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (!JsonObject.IsValid()) return nullptr;

	UMounteaInventoryItemBlueprint* resultBlueprint = NewObject<UMounteaInventoryItemBlueprint>();
	resultBlueprint->SetBlueprintGuid(FGuid::NewGuid());
	
	const TArray<TSharedPtr<FJsonValue>>* TagsJsonArray;
	FGameplayTagContainer TagContainer;
	if (JsonObject->TryGetArrayField(TEXT("DefaultTags"), TagsJsonArray))
	{
		for (const TSharedPtr<FJsonValue>& TagValue : *TagsJsonArray)
		{
			const FGameplayTag newGameplayTag = FGameplayTag::RequestGameplayTag(FName(*TagValue->AsString()));
			TagContainer.AddTag(newGameplayTag);
		}
		resultBlueprint->SetDefaultTags(TagContainer);
	}

	resultBlueprint->SetPrimaryData(RequiredDataFromJson(JsonObject->GetObjectField(TEXT("PrimaryData"))));
	resultBlueprint->SetSecondaryData(OptionalDataFromJson(JsonObject->GetObjectField(TEXT("SecondaryData"))));

	return resultBlueprint;
}

FMounteaInventoryItemsLibraryRow UMounteaInventorySerializationLibrary::ItemBlueprintRowFromJson(const TSharedPtr<FJsonObject>& JsonObject)
{
	FMounteaInventoryItemsLibraryRow resultRow = FMounteaInventoryItemsLibraryRow();
	if (!JsonObject.IsValid()) return resultRow;

	resultRow.InitialQuantity = JsonObject->GetIntegerField(TEXT("InitialQuantity"));
    
	FString BlueprintJson = JsonObject->GetStringField(TEXT("InventoryItemBlueprint"));
	TSharedPtr<FJsonObject> BlueprintJsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BlueprintJson);
    
	if (FJsonSerializer::Deserialize(Reader, BlueprintJsonObject))
	{
		UMounteaInventoryItemBlueprint* Blueprint = NewObject<UMounteaInventoryItemBlueprint>();
		resultRow.InventoryItemBlueprint = ItemBlueprintFromJson(BlueprintJsonObject);
	}

	return resultRow;
}

UMounteaInventoryItemsLibrary* UMounteaInventorySerializationLibrary::ItemBlueprintsLibraryFromJson(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (!JsonObject.IsValid()) return nullptr;

	UMounteaInventoryItemsLibrary* resultLibrary = NewObject<UMounteaInventoryItemsLibrary>();

	const TArray<TSharedPtr<FJsonValue>>* RowsJsonArray;
	if (JsonObject->TryGetArrayField(TEXT("ItemBlueprintsRows"), RowsJsonArray))
	{
		for (const TSharedPtr<FJsonValue>& RowValue : *RowsJsonArray)
		{
			FMounteaInventoryItemsLibraryRow Row = ItemBlueprintRowFromJson(RowValue->AsObject());
			resultLibrary->GetInventoryItemBlueprintRows().Add(Row);
		}
	}

	return resultLibrary;
}
