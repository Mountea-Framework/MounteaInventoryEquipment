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

#include "Statics/MounteaAdvancedInventoryLoadoutEditorStatics.h"

#include "Helpers/MounteaInventoryImportHelpers.h"
#include "Helpers/MounteaInventoryZipHelper.h"

#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"
#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventorySystemStatics.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Dom/JsonObject.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryLoadoutEditorStatics::ImportLoadoutsFromFilePath(
	const FString& FilePath,
	const FString& TargetFolder,
	TArray<UMounteaAdvancedInventoryLoadoutConfig*>& OutLoadouts,
	FString& OutErrorMessage)
{
	OutLoadouts.Reset();
	OutErrorMessage.Reset();
	TArray<TArray<uint8>> singleZips;

	if (!FMounteaInventoryZipHelper::ReadEntityArchives(FilePath, TEXT("mntealoadout"), TEXT("mntealoadouts"), singleZips, OutErrorMessage))
		return false;

	// ── Phase 1 & 2: Read and validate all ZIPs before writing anything ──────

	struct FParsedLoadout
	{
		FString LoadoutJson;
		TArray<FString> BundledItemJsons;
		TSharedPtr<FJsonObject> ParsedJson;
		TSet<FGuid> BundledGuids;
	};

	TArray<FParsedLoadout> parsed;
	parsed.Reserve(singleZips.Num());

	const TMap<FGuid, UMounteaInventoryItemTemplate*> projectItems =
		FMounteaInventoryImportHelpers::BuildProjectItemGuidMap();

	TSet<FGuid> loadoutGuids;

	for (const TArray<uint8>& zipBlob : singleZips)
	{
		FParsedLoadout entry;

		FString parseError;

		if (!ParseLoadoutFromZip(zipBlob, entry.LoadoutJson, entry.BundledItemJsons, parseError))
		{
			OutErrorMessage += parseError + TEXT("\n");
			LOG_ERROR(TEXT("[Loadout Import] %s"), *parseError);
			return false;
		}

		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(entry.LoadoutJson);

		if (!FJsonSerializer::Deserialize(reader, entry.ParsedJson) || !entry.ParsedJson.IsValid())
		{
			OutErrorMessage += TEXT("Failed to parse loadout JSON\n");
			LOG_ERROR(TEXT("[Loadout Import] Failed to parse loadout JSON"));
			return false;
		}

		const FGuid parsedGuid = ExtractGuidFromLoadoutJson(entry.LoadoutJson);

		if (!parsedGuid.IsValid() || loadoutGuids.Contains(parsedGuid))
		{
			OutErrorMessage = TEXT("Invalid or duplicate loadout GUID in archive");
			return false;
		}

		loadoutGuids.Add(parsedGuid);
		entry.BundledGuids = FMounteaInventoryImportHelpers::ParseBundledGuids(entry.BundledItemJsons);

		// Build combined set: bundled + project
		TSet<FGuid> knownGuids = entry.BundledGuids;

		for (const auto& kv : projectItems)
			knownGuids.Add(kv.Key);

		FString validationError;

		if (!ValidateLoadout(entry.ParsedJson, knownGuids, validationError))
		{
			OutErrorMessage += validationError + TEXT("\n");
			LOG_ERROR(TEXT("[Loadout Import] %s"), *validationError);
			return false;
		}

		parsed.Add(MoveTemp(entry));
	}

	// Persist validated dependencies and entities. Surface the first write failure.
	TArray<UMounteaAdvancedInventoryLoadoutConfig*> existing = LoadAllExistingLoadouts();
	TMap<FGuid, UMounteaInventoryItemTemplate*> itemsByGuid = projectItems;
	const FString folder = TargetFolder.IsEmpty() ? TEXT("/Game/") : TargetFolder;

	for (FParsedLoadout& entry : parsed)
	{
		if (!FMounteaInventoryImportHelpers::ImportBundledItems(entry.BundledItemJsons, folder, FilePath, itemsByGuid, OutErrorMessage))
			return false;

		const FGuid guid = ExtractGuidFromLoadoutJson(entry.LoadoutJson);
		UMounteaAdvancedInventoryLoadoutConfig* asset = FindLoadoutByGuid(existing, guid);

		if (asset)
		{
			if (!UpdateExistingLoadout(asset, entry.ParsedJson, itemsByGuid, FilePath, OutErrorMessage))
				return false;
		}
		else
		{
			FString name = ExtractNameFromLoadoutJson(entry.LoadoutJson);

			if (name.IsEmpty())
				name = guid.ToString(EGuidFormats::DigitsWithHyphens);
			asset = CreateLoadoutAsset(folder, name, OutErrorMessage);

			if (!asset || !DeserializeLoadoutFromJson(entry.ParsedJson, asset, itemsByGuid, OutErrorMessage))
				return false;

			asset->SetSourceFilePath(FilePath);

			if (!SaveLoadoutAsset(asset, asset->GetOutermost()->GetName()))
			{
				OutErrorMessage = TEXT("Failed to save imported loadout asset");
				return false;
			}

			existing.Add(asset);
		}

		OutLoadouts.Add(asset);
	}

	OutErrorMessage.Reset();
	return true;
}

// ---------------------------------------------------------------------------
// ZIP parsing
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryLoadoutEditorStatics::ParseLoadoutFromZip(
	const TArray<uint8>& ZipBytes,
	FString& OutLoadoutJson,
	TArray<FString>& OutBundledItemJsons,
	FString& OutErrorMessage)
{
	FString stationTaxonomy;
	return FMounteaInventoryZipHelper::ExtractEntityArchive(ZipBytes, TEXT("loadout.json"), OutLoadoutJson,
		OutBundledItemJsons, stationTaxonomy, OutErrorMessage);
}

// ---------------------------------------------------------------------------
// Validation
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryLoadoutEditorStatics::ValidateLoadout(
	const TSharedPtr<FJsonObject>& LoadoutJson,
	const TSet<FGuid>& KnownGuids,
	FString& OutErrorMessage)
{
	OutErrorMessage.Reset();
	auto fail = [&OutErrorMessage](const TCHAR* Message)
	{
		OutErrorMessage = Message;
		return false;
	};
	FString guidText;
	FGuid guid;

	if (!LoadoutJson.IsValid() || !LoadoutJson->TryGetStringField(TEXT("guid"), guidText) ||
		!FGuid::Parse(guidText, guid) || !guid.IsValid())
		return fail(TEXT("Loadout requires a valid nonzero guid"));

	const TArray<TSharedPtr<FJsonValue>>* items;

	if (!LoadoutJson->TryGetArrayField(TEXT("items"), items))
		return fail(TEXT("Loadout requires an items array (which may be empty)"));

	TSet<FName> occupiedSlots;

	for (const TSharedPtr<FJsonValue>& value : *items)
	{
		const TSharedPtr<FJsonObject>* row;
		const TSharedPtr<FJsonObject>* ref;
		FString refText;
		FGuid itemGuid;

		if (!value.IsValid() || !value->TryGetObject(row) || !(*row)->TryGetObjectField(TEXT("ref"), ref) ||
			!(*ref)->TryGetStringField(TEXT("guid"), refText) || !FGuid::Parse(refText, itemGuid) ||
			!itemGuid.IsValid() || !KnownGuids.Contains(itemGuid))
			return fail(TEXT("Every loadout item requires a known ref.guid"));

		double quantity;

		if (!(*row)->TryGetNumberField(TEXT("qty"), quantity) || !FMath::IsFinite(quantity) ||
			quantity < 1 || quantity > MAX_int32 || quantity != static_cast<int32>(quantity))
			return fail(TEXT("Loadout qty must be a positive int32 integer"));

		double durability = 1.0;

		if ((*row)->HasField(TEXT("durability")) && (!(*row)->TryGetNumberField(TEXT("durability"), durability) ||
			!FMath::IsFinite(durability) || durability < 0.0 || durability > 1.0))
			return fail(TEXT("Loadout durability must be a finite fraction from zero to one"));

		bool autoEquip = false;

		if ((*row)->HasField(TEXT("autoEquip")) && !(*row)->TryGetBoolField(TEXT("autoEquip"), autoEquip))
			return fail(TEXT("Loadout autoEquip must be boolean"));

		FString slot;

		if ((*row)->HasField(TEXT("slot")) && (!(*row)->TryGetStringField(TEXT("slot"), slot) || slot.Len() >= NAME_SIZE))
			return fail(TEXT("Loadout slot must be a valid name string"));

		if (autoEquip && !FName(*slot).IsNone())
		{
			const FName slotName(*slot);

			if (occupiedSlots.Contains(slotName))
				return fail(TEXT("Multiple loadout rows cannot automatically equip the same slot"));

			occupiedSlots.Add(slotName);
		}

		bool random = false;

		if ((*row)->HasField(TEXT("useRandomQuantity")) && !(*row)->TryGetBoolField(TEXT("useRandomQuantity"), random))
			return fail(TEXT("useRandomQuantity must be boolean"));

		if ((*row)->HasField(TEXT("randomRange")))
		{
			const TSharedPtr<FJsonObject>* range;
			double minimum, maximum;

			if (!(*row)->TryGetObjectField(TEXT("randomRange"), range) ||
				!(*range)->TryGetNumberField(TEXT("min"), minimum) || !(*range)->TryGetNumberField(TEXT("max"), maximum) ||
				!FMath::IsFinite(minimum) || !FMath::IsFinite(maximum) || minimum < 1 || maximum < minimum || maximum > MAX_int32 ||
				minimum != static_cast<int32>(minimum) || maximum != static_cast<int32>(maximum))
				return fail(TEXT("randomRange requires positive integer min and max, with min <= max"));
		}
		else if (random)
			return fail(TEXT("Random quantity requires randomRange"));
	}

	if (LoadoutJson->HasField(TEXT("slots")))
		return fail(TEXT("Loadout slots map is unsupported; assign autoEquip and slot on each items row"));

	return true;
}

// ---------------------------------------------------------------------------
// Asset creation & serialization
// ---------------------------------------------------------------------------

UMounteaAdvancedInventoryLoadoutConfig* UMounteaAdvancedInventoryLoadoutEditorStatics::CreateLoadoutAsset(
	const FString& TargetFolder,
	const FString& AssetName,
	FString& OutErrorMessage)
{
	FString packageName, uniqueName;
	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get().CreateUniqueAssetName(
		FPaths::Combine(TargetFolder, FMounteaInventoryImportHelpers::SanitizeAssetName(AssetName)), TEXT(""), packageName, uniqueName);
	UPackage* newPackage = CreatePackage(*packageName);

	if (!newPackage)
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to create package: %s"), *packageName);
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
		return nullptr;
	}

	UMounteaAdvancedInventoryLoadoutConfig* loadout = NewObject<UMounteaAdvancedInventoryLoadoutConfig>(
		newPackage,
		UMounteaAdvancedInventoryLoadoutConfig::StaticClass(),
		*uniqueName,
		RF_Public | RF_Standalone
	);

	if (!loadout)
	{
		OutErrorMessage = TEXT("Failed to create loadout config object");
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
	}

	return loadout;
}

bool UMounteaAdvancedInventoryLoadoutEditorStatics::DeserializeLoadoutFromJson(
	const TSharedPtr<FJsonObject>& JsonObject,
	UMounteaAdvancedInventoryLoadoutConfig* Loadout,
	const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
	FString& OutErrorMessage)
{
	if (!JsonObject.IsValid() || !IsValid(Loadout))
	{
		OutErrorMessage = TEXT("Invalid JSON or loadout object");
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
		return false;
	}

	TSet<FGuid> knownGuids;

	for (const auto& pair : ItemsByGuid)
	{
		if (IsValid(pair.Value))
			knownGuids.Add(pair.Key);
	}

	if (!ValidateLoadout(JsonObject, knownGuids, OutErrorMessage))
		return false;

	// Validation completes before touching the existing asset. Each row owns a separate instance,
	// even when multiple rows reference the same item template.
	TArray<TObjectPtr<UMounteaAdvancedInventoryLoadoutItem>> newItems;
	const TArray<TSharedPtr<FJsonValue>>* items;
	JsonObject->TryGetArrayField(TEXT("items"), items);

	for (const auto& value : *items)
	{
		const TSharedPtr<FJsonObject> row = value->AsObject();
		const TSharedPtr<FJsonObject> ref = row->GetObjectField(TEXT("ref"));
		FGuid itemGuid;
		FGuid::Parse(ref->GetStringField(TEXT("guid")), itemGuid);
		UMounteaInventoryItemTemplate* itemTemplate = ItemsByGuid.FindChecked(itemGuid);
		auto* item = NewObject<UMounteaAdvancedInventoryLoadoutItem>(Loadout);
		item->Guid = itemGuid;
		item->ItemTemplate = itemTemplate;
		item->BaseQuantity = static_cast<int32>(row->GetNumberField(TEXT("qty")));
		double durability = 1.0;
		row->TryGetNumberField(TEXT("durability"), durability);
		const bool durable = itemTemplate->bHasDurability &&
			UMounteaInventorySystemStatics::HasFlag(itemTemplate->ItemFlags, EInventoryItemFlags::EIIF_Durable);
		item->BaseDurability = durable ? FMath::RoundToInt(durability * itemTemplate->MaxDurability) : -1;
		bool autoEquip = false, random = false;
		row->TryGetBoolField(TEXT("autoEquip"), autoEquip);
		row->TryGetBoolField(TEXT("useRandomQuantity"), random);
		item->bAutomaticallyEquip = autoEquip;
		item->bUseRandomQuantity = random;
		FString slot;
		row->TryGetStringField(TEXT("slot"), slot);
		item->EquipmentSlot = FName(*slot);
		const TSharedPtr<FJsonObject>* range;

		if (row->TryGetObjectField(TEXT("randomRange"), range))
			item->RandomRange = FIntPoint(static_cast<int32>((*range)->GetNumberField(TEXT("min"))), static_cast<int32>((*range)->GetNumberField(TEXT("max"))));
#if WITH_EDITORONLY_DATA
		item->DisplayName = FName(*itemTemplate->DisplayName.ToString());
#endif
		newItems.Add(item);
	}

	FGuid::Parse(JsonObject->GetStringField(TEXT("guid")), Loadout->Guid);
	Loadout->Items = MoveTemp(newItems);
	return true;
}

bool UMounteaAdvancedInventoryLoadoutEditorStatics::SaveLoadoutAsset(
	UMounteaAdvancedInventoryLoadoutConfig* Loadout,
	const FString& PackagePath)
{
	return FMounteaInventoryImportHelpers::SaveNewAssetPackage(Loadout, PackagePath);
}

bool UMounteaAdvancedInventoryLoadoutEditorStatics::UpdateExistingLoadout(
	UMounteaAdvancedInventoryLoadoutConfig* Loadout,
	const TSharedPtr<FJsonObject>& JsonObject,
	const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
	const FString& SourceFilePath,
	FString& OutErrorMessage)
{
	if (!IsValid(Loadout))
	{
		OutErrorMessage = TEXT("Invalid loadout object");
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
		return false;
	}

	if (!DeserializeLoadoutFromJson(JsonObject, Loadout, ItemsByGuid, OutErrorMessage))
		return false;

	Loadout->SetSourceFilePath(SourceFilePath);

	return FMounteaInventoryImportHelpers::SaveExistingAssetPackage(Loadout, OutErrorMessage);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

FGuid UMounteaAdvancedInventoryLoadoutEditorStatics::ExtractGuidFromLoadoutJson(const FString& JsonString)
{
	return FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(JsonString, TEXT("guid"));
}

FString UMounteaAdvancedInventoryLoadoutEditorStatics::ExtractNameFromLoadoutJson(const FString& JsonString)
{
	TSharedPtr<FJsonObject> obj;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(reader, obj) || !obj.IsValid())
		return TEXT("");

	FString name;

	if (obj->TryGetStringField(TEXT("name"), name))
		return FMounteaInventoryImportHelpers::SanitizeAssetName(name);

	return TEXT("");
}

TArray<UMounteaAdvancedInventoryLoadoutConfig*> UMounteaAdvancedInventoryLoadoutEditorStatics::LoadAllExistingLoadouts()
{
	TArray<UMounteaAdvancedInventoryLoadoutConfig*> loadouts;

	FAssetRegistryModule& reg = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> assets;
	reg.Get().GetAssetsByClass(UMounteaAdvancedInventoryLoadoutConfig::StaticClass()->GetClassPathName(), assets, true);

	for (const FAssetData& a : assets)
	{
		if (UMounteaAdvancedInventoryLoadoutConfig* l = Cast<UMounteaAdvancedInventoryLoadoutConfig>(a.GetAsset()))
			loadouts.Add(l);
	}

	return loadouts;
}

UMounteaAdvancedInventoryLoadoutConfig* UMounteaAdvancedInventoryLoadoutEditorStatics::FindLoadoutByGuid(
	const TArray<UMounteaAdvancedInventoryLoadoutConfig*>& Loadouts,
	const FGuid& Guid)
{
	if (!Guid.IsValid())
		return nullptr;

	UMounteaAdvancedInventoryLoadoutConfig* const* found = Loadouts.FindByPredicate(
		[&Guid](const UMounteaAdvancedInventoryLoadoutConfig* Loadout)
		{
			return IsValid(Loadout) && Loadout->Guid == Guid;
		});

	return found ? *found : nullptr;
}


bool UMounteaAdvancedInventoryLoadoutEditorStatics::ExportLoadoutsToFilePath(
	const TArray<UMounteaAdvancedInventoryLoadoutConfig*>& Loadouts,
	const FString& FilePath,
	FString& OutErrorMessage)
{
	OutErrorMessage.Reset();
	TMap<FGuid, TArray<uint8>> archives;

	for (const auto* loadout : Loadouts)
	{
		if (!IsValid(loadout) || !loadout->Guid.IsValid() || archives.Contains(loadout->Guid))
		{
			OutErrorMessage = TEXT("Loadout export requires valid assets with distinct valid GUIDs");
			return false;
		}

		TSharedPtr<FJsonObject> root = MakeShared<FJsonObject>();
		root->SetStringField(TEXT("guid"), loadout->Guid.ToString(EGuidFormats::DigitsWithHyphens));
		root->SetStringField(TEXT("name"), loadout->GetName());
		TArray<TSharedPtr<FJsonValue>> rows;
		TArray<UMounteaInventoryItemTemplate*> templates;
		TSet<FGuid> knownGuids;

		for (const auto& item : loadout->Items)
		{
			if (!IsValid(item.Get()) || !IsValid(item->ItemTemplate.Get()) || !item->ItemTemplate->Guid.IsValid())
			{
				OutErrorMessage = TEXT("Cannot export a loadout with an invalid item template");
				return false;
			}

			UMounteaInventoryItemTemplate* itemTemplate = item->ItemTemplate;

			if (knownGuids.Contains(itemTemplate->Guid) && !templates.Contains(itemTemplate))
			{
				OutErrorMessage = TEXT("Distinct item templates cannot share a GUID");
				return false;
			}

			templates.AddUnique(itemTemplate);
			knownGuids.Add(itemTemplate->Guid);
			TSharedPtr<FJsonObject> row = MakeShared<FJsonObject>();
			TSharedPtr<FJsonObject> ref = MakeShared<FJsonObject>();
			ref->SetStringField(TEXT("guid"), itemTemplate->Guid.ToString(EGuidFormats::DigitsWithHyphens));
			row->SetObjectField(TEXT("ref"), ref);
			row->SetNumberField(TEXT("qty"), item->BaseQuantity);
			const bool durable = itemTemplate->bHasDurability &&
				UMounteaInventorySystemStatics::HasFlag(itemTemplate->ItemFlags, EInventoryItemFlags::EIIF_Durable);
			if (durable && itemTemplate->MaxDurability <= 0)
			{
				OutErrorMessage = TEXT("Durable template requires positive MaxDurability");
				return false;
			}

			row->SetNumberField(TEXT("durability"), durable ? static_cast<double>(item->BaseDurability) / itemTemplate->MaxDurability : 1.0);
			row->SetBoolField(TEXT("autoEquip"), item->bAutomaticallyEquip);
			row->SetStringField(TEXT("slot"), item->EquipmentSlot.IsNone() ? TEXT("") : item->EquipmentSlot.ToString());
			row->SetBoolField(TEXT("useRandomQuantity"), item->bUseRandomQuantity);
			TSharedPtr<FJsonObject> range = MakeShared<FJsonObject>();
			range->SetNumberField(TEXT("min"), item->RandomRange.X);
			range->SetNumberField(TEXT("max"), item->RandomRange.Y);
			row->SetObjectField(TEXT("randomRange"), range);
			rows.Add(MakeShared<FJsonValueObject>(row));
		}

		root->SetArrayField(TEXT("items"), rows);

		if (!ValidateLoadout(root, knownGuids, OutErrorMessage))
			return false;

		FString json;

		if (!FJsonSerializer::Serialize(root.ToSharedRef(), TJsonWriterFactory<>::Create(&json)))
		{
			OutErrorMessage = TEXT("Could not serialize loadout JSON");
			return false;
		}

		TArray<FString> bundledItems;
		TArray<uint8> archive;

		if (!FMounteaInventoryImportHelpers::SerializeBundledItems(templates, bundledItems, OutErrorMessage) ||
			!FMounteaInventoryZipHelper::BuildEntityArchive(TEXT("loadout.json"), json, bundledItems, archive, OutErrorMessage))
			return false;

		archives.Add(loadout->Guid, MoveTemp(archive));
	}

	return FMounteaInventoryZipHelper::WriteEntityArchives(archives, FilePath, TEXT("mntealoadout"), TEXT("mntealoadouts"), OutErrorMessage);
}
