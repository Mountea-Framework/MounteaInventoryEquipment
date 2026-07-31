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
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"

#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"
#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventorySystemStatics.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/SavePackage.h"

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryLoadoutEditorStatics::ImportLoadoutsFromFilePath(
	const FString& FilePath,
	const FString& TargetFolder,
	TArray<UMounteaAdvancedInventoryLoadoutConfig*>& OutLoadouts,
	FString& OutErrorMessage)
{
	TArray<uint8> fileBytes;
	if (!FMounteaInventoryZipHelper::LoadFileToBytes(FilePath, fileBytes))
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
		return false;
	}

	if (!FMounteaInventoryZipHelper::IsZipFile(fileBytes))
	{
		OutErrorMessage = TEXT("File is not a valid ZIP archive");
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
		return false;
	}

	// Expand bundle or treat as single
	TArray<TArray<uint8>> singleZips;
	if (FilePath.EndsWith(TEXT(".mntealoadouts")))
	{
		if (!FMounteaInventoryZipHelper::ExpandBundle(fileBytes, TEXT("mntealoadout"), singleZips))
		{
			OutErrorMessage = TEXT("Failed to expand .mntealoadouts bundle");
			LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
			return false;
		}
	}
	else
		singleZips.Add(MoveTemp(fileBytes));

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

	// ── Phase 3: Import bundled items, then persist loadouts ─────────────────

	TArray<UMounteaAdvancedInventoryLoadoutConfig*> existingLoadouts = LoadAllExistingLoadouts();

	for (FParsedLoadout& entry : parsed)
	{
		const FString folderToUse = TargetFolder.IsEmpty() ? TEXT("/Game/") : TargetFolder;

		TMap<FGuid, UMounteaInventoryItemTemplate*> itemsByGuid = projectItems;
		FMounteaInventoryImportHelpers::ImportBundledItems(entry.BundledItemJsons, folderToUse, FilePath, itemsByGuid);

		// Find or create loadout asset
		const FGuid loadoutGuid = ExtractGuidFromLoadoutJson(entry.LoadoutJson);
		UMounteaAdvancedInventoryLoadoutConfig* loadout = FindLoadoutByGuid(existingLoadouts, loadoutGuid);

		if (loadout)
		{
			FString updateError;
			if (UpdateExistingLoadout(loadout, entry.ParsedJson, itemsByGuid, FilePath, updateError))
				OutLoadouts.Add(loadout);
			else
			{
				OutErrorMessage += updateError + TEXT("\n");
				LOG_ERROR(TEXT("[Loadout Import] %s"), *updateError);
			}
		}
		else
		{
			const FString name = ExtractNameFromLoadoutJson(entry.LoadoutJson);
			const FString assetName = name.IsEmpty() ? loadoutGuid.ToString(EGuidFormats::Short) : name;

			FString createError;
			loadout = CreateLoadoutAsset(folderToUse, assetName, createError);
			if (!loadout)
			{
				OutErrorMessage += createError + TEXT("\n");
				LOG_ERROR(TEXT("[Loadout Import] %s"), *createError);
				continue;
			}

			FString deserError;
			if (DeserializeLoadoutFromJson(entry.ParsedJson, loadout, itemsByGuid, deserError))
			{
				loadout->SetSourceFilePath(FilePath);
				const FString pkgPath = FPaths::Combine(folderToUse, assetName);
				if (SaveLoadoutAsset(loadout, pkgPath))
					OutLoadouts.Add(loadout);
				else
				{
					OutErrorMessage += FString::Printf(TEXT("Failed to save loadout asset '%s'\n"), *assetName);
					LOG_ERROR(TEXT("[Loadout Import] Failed to save loadout asset '%s'"), *assetName);
				}
			}
			else
			{
				OutErrorMessage += deserError + TEXT("\n");
				LOG_ERROR(TEXT("[Loadout Import] %s"), *deserError);
			}
		}
	}

	if (OutLoadouts.Num() == 0)
	{
		OutErrorMessage = TEXT("No loadouts were successfully imported");
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
		return false;
	}

	OutErrorMessage = FString::Printf(TEXT("Loadout import complete: %d imported"), OutLoadouts.Num());
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
	TMap<FString, FString> textFiles;
	TMap<FString, TArray<uint8>> binaryFiles;

	if (!FMounteaInventoryZipHelper::ExtractEntries(ZipBytes, textFiles, binaryFiles))
	{
		OutErrorMessage = TEXT("Failed to extract ZIP entries");
		return false;
	}

	const FString* loadoutJson = textFiles.Find(TEXT("loadout.json"));
	if (!loadoutJson)
	{
		OutErrorMessage = TEXT("loadout.json not found in ZIP");
		return false;
	}

	OutLoadoutJson = *loadoutJson;

	for (const auto& kv : textFiles)
	{
		if (kv.Key.StartsWith(TEXT("items/")) && kv.Key.EndsWith(TEXT(".json")))
			OutBundledItemJsons.Add(kv.Value);
	}

	return true;
}

// ---------------------------------------------------------------------------
// Validation
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryLoadoutEditorStatics::ValidateLoadout(
	const TSharedPtr<FJsonObject>& LoadoutJson,
	const TSet<FGuid>& KnownGuids,
	FString& OutErrorMessage)
{
	FString guidStr;
	if (!LoadoutJson->TryGetStringField(TEXT("guid"), guidStr) || guidStr.IsEmpty())
	{
		OutErrorMessage = TEXT("Loadout JSON missing 'guid' field");
		LOG_ERROR(TEXT("[Loadout Import] %s"), *OutErrorMessage);
		return false;
	}

	// Validate items[] array
	const TArray<TSharedPtr<FJsonValue>>* itemsArray;
	if (LoadoutJson->TryGetArrayField(TEXT("items"), itemsArray))
	{
		for (const TSharedPtr<FJsonValue>& itemVal : *itemsArray)
		{
			const TSharedPtr<FJsonObject>* itemObj;
			if (!itemVal->TryGetObject(itemObj))
				continue;

			const TSharedPtr<FJsonObject>* refObj;
			if ((*itemObj)->TryGetObjectField(TEXT("ref"), refObj))
			{
				FString refGuid;
				if ((*refObj)->TryGetStringField(TEXT("guid"), refGuid))
				{
					FString checkError;
					if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(
						refGuid, TEXT("items[]"), KnownGuids, checkError))
					{
						OutErrorMessage = checkError;
						LOG_ERROR(TEXT("[Loadout Import] %s"), *checkError);
						return false;
					}
				}
			}
		}
	}

	// Validate slots{} map
	const TSharedPtr<FJsonObject>* slotsObj;
	if (LoadoutJson->TryGetObjectField(TEXT("slots"), slotsObj))
	{
		for (const auto& slotKv : (*slotsObj)->Values)
		{
			const TSharedPtr<FJsonObject>* slotVal;
			if (!slotKv.Value->TryGetObject(slotVal))
				continue;

			FString slotGuid;
			if ((*slotVal)->TryGetStringField(TEXT("guid"), slotGuid))
			{
				FString checkError;
				if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(
					slotGuid, FString::Printf(TEXT("slots[%s]"), *slotKv.Key), KnownGuids, checkError))
				{
					LOG_ERROR(TEXT("[Loadout Import] %s"), *checkError);
					OutErrorMessage = checkError;
					return false;
				}
			}
		}
	}

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
	const FString packageName = FPaths::Combine(TargetFolder, AssetName);
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
		*AssetName,
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

	FString loadoutGuidStr;
	if (JsonObject->TryGetStringField(TEXT("guid"), loadoutGuidStr))
		FGuid::Parse(loadoutGuidStr, Loadout->Guid);

	Loadout->Items.Empty();

	// Build a map from item GUID → created LoadoutItem so slot assignments can find them
	TMap<FGuid, UMounteaAdvancedInventoryLoadoutItem*> loadoutItemsByItemGuid;

	// Process items[]
	const TArray<TSharedPtr<FJsonValue>>* itemsArray;
	if (JsonObject->TryGetArrayField(TEXT("items"), itemsArray))
	{
		for (const TSharedPtr<FJsonValue>& itemVal : *itemsArray)
		{
			const TSharedPtr<FJsonObject>* itemObj;
			if (!itemVal->TryGetObject(itemObj))
				continue;

			const TSharedPtr<FJsonObject>* refObj;
			FString refGuidStr;
			if (!(*itemObj)->TryGetObjectField(TEXT("ref"), refObj) ||
				!(*refObj)->TryGetStringField(TEXT("guid"), refGuidStr))
				continue;

			FGuid itemGuid;
			if (!FGuid::Parse(refGuidStr, itemGuid) || !itemGuid.IsValid())
				continue;

			UMounteaInventoryItemTemplate* const* templatePtr = ItemsByGuid.Find(itemGuid);
			if (!templatePtr || !IsValid(*templatePtr))
			{
				LOG_WARNING(TEXT("[Loadout Import] Item GUID %s not found among imported/known items"), *refGuidStr);
				continue;
			}

			UMounteaAdvancedInventoryLoadoutItem* loadoutItem =
				NewObject<UMounteaAdvancedInventoryLoadoutItem>(Loadout,
					UMounteaAdvancedInventoryLoadoutItem::StaticClass(),
					NAME_None,
					RF_NoFlags);

			loadoutItem->Guid         = itemGuid;
			loadoutItem->ItemTemplate = *templatePtr;

			int32 qty = 1;
			if ((*itemObj)->TryGetNumberField(TEXT("qty"), qty))
				loadoutItem->BaseQuantity = FMath::Max(1, qty);

			// "durability" is a 0.0-1.0 fraction of the item template's MaxDurability; convert to
			// the absolute value BaseQuantity expects, matching the same formula used when a user
			// manually assigns an ItemTemplate in the editor (see PostEditChangeProperty).
			double durabilityFraction = 1.0;
			if ((*itemObj)->TryGetNumberField(TEXT("durability"), durabilityFraction) &&
				(*templatePtr)->bHasDurability &&
				UMounteaInventorySystemStatics::HasFlag((*templatePtr)->ItemFlags, EInventoryItemFlags::EIIF_Durable))
			{
				loadoutItem->BaseDurability = FMath::RoundToInt(durabilityFraction * (*templatePtr)->MaxDurability);
			}

			bool bAutoEquip = false;
			if ((*itemObj)->TryGetBoolField(TEXT("autoEquip"), bAutoEquip) && bAutoEquip)
			{
				loadoutItem->bAutomaticallyEquip = true;

				FString slotName;
				if ((*itemObj)->TryGetStringField(TEXT("slot"), slotName) && !slotName.IsEmpty())
					loadoutItem->EquipmentSlot = FName(*slotName);
			}

#if WITH_EDITORONLY_DATA
			FString displayName;
			if ((*refObj)->TryGetStringField(TEXT("displayName"), displayName))
				loadoutItem->DisplayName = FName(*displayName);
#endif

			Loadout->Items.Add(loadoutItem);
			loadoutItemsByItemGuid.Add(itemGuid, loadoutItem);
		}
	}

	// Process slots{} — find matching LoadoutItem and set equipment slot
	const TSharedPtr<FJsonObject>* slotsObj;
	if (JsonObject->TryGetObjectField(TEXT("slots"), slotsObj))
	{
		for (const auto& slotKv : (*slotsObj)->Values)
		{
			const FString& slotName = slotKv.Key;

			const TSharedPtr<FJsonObject>* slotVal;
			if (!slotKv.Value->TryGetObject(slotVal))
				continue;

			FString slotGuidStr;
			if (!(*slotVal)->TryGetStringField(TEXT("guid"), slotGuidStr))
				continue;

			FGuid slotItemGuid;
			if (!FGuid::Parse(slotGuidStr, slotItemGuid) || !slotItemGuid.IsValid())
				continue;

			if (UMounteaAdvancedInventoryLoadoutItem** existingItem = loadoutItemsByItemGuid.Find(slotItemGuid))
			{
				(*existingItem)->EquipmentSlot       = FName(*slotName);
				(*existingItem)->bAutomaticallyEquip = true;
			}
			else
			{
				// Item only mentioned in slots, not in items[] — create an entry for it
				UMounteaInventoryItemTemplate* const* templatePtr = ItemsByGuid.Find(slotItemGuid);
				if (!templatePtr || !IsValid(*templatePtr))
					continue;

				UMounteaAdvancedInventoryLoadoutItem* loadoutItem =
					NewObject<UMounteaAdvancedInventoryLoadoutItem>(Loadout,
						UMounteaAdvancedInventoryLoadoutItem::StaticClass(),
						NAME_None,
						RF_NoFlags);

				loadoutItem->Guid                = slotItemGuid;
				loadoutItem->ItemTemplate        = *templatePtr;
				loadoutItem->BaseQuantity         = 1;
				loadoutItem->EquipmentSlot        = FName(*slotName);
				loadoutItem->bAutomaticallyEquip  = true;

				Loadout->Items.Add(loadoutItem);
				loadoutItemsByItemGuid.Add(slotItemGuid, loadoutItem);
			}
		}
	}

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
		[&Guid](const UMounteaAdvancedInventoryLoadoutConfig* l)
		{
			return IsValid(l) && l->Guid == Guid;
		});

	return found ? *found : nullptr;
}
