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

#include "Helpers/MounteaInventoryZipHelper.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

THIRD_PARTY_INCLUDES_START
#include "zip.h"
THIRD_PARTY_INCLUDES_END

bool FMounteaInventoryZipHelper::IsZipFile(const TArray<uint8>& Data)
{
	if (Data.Num() < 4)
		return false;

	// ZIP signatures: local-file header, spanned, central-directory, EOCD
	constexpr uint32 sigLocalFile  = 0x04034b50;
	constexpr uint32 sigSpanned    = 0x08074b50;
	constexpr uint32 sigCentral    = 0x02014b50;
	constexpr uint32 sigEOCD       = 0x06054b50;

	const uint32 sig = static_cast<uint32>(Data[0])
		| (static_cast<uint32>(Data[1]) << 8)
		| (static_cast<uint32>(Data[2]) << 16)
		| (static_cast<uint32>(Data[3]) << 24);

	return sig == sigLocalFile || sig == sigSpanned || sig == sigCentral || sig == sigEOCD;
}

bool FMounteaInventoryZipHelper::LoadFileToBytes(const FString& FilePath, TArray<uint8>& OutBytes)
{
	return FFileHelper::LoadFileToArray(OutBytes, *FilePath);
}

FString FMounteaInventoryZipHelper::BytesToString(const uint8* Bytes, int32 Count)
{
	int32 offset = 0;

	if (Count >= 3 && Bytes[0] == 0xEF && Bytes[1] == 0xBB && Bytes[2] == 0xBF)
		offset = 3;

	return FString(FUTF8ToTCHAR(reinterpret_cast<const UTF8CHAR*>(Bytes + offset), Count - offset));
}

bool FMounteaInventoryZipHelper::ExtractEntries(const TArray<uint8>& ZipData,
	TMap<FString, FString>& OutTextFiles, TMap<FString, TArray<uint8>>& OutBinaryFiles)
{
	if (!IsZipFile(ZipData))
		return false;

	const FString tempPath = FPaths::CreateTempFilename(FPlatformProcess::UserTempDir(), TEXT("MounteaInventory"), TEXT(".zip"));

	if (!FFileHelper::SaveArrayToFile(ZipData, *tempPath))
	{
		IFileManager::Get().Delete(*tempPath);
		return false;
	}

	zip_t* archive = zip_open(TCHAR_TO_UTF8(*tempPath), 0, 'r');

	if (!archive)
	{
		IFileManager::Get().Delete(*tempPath);
		return false;
	}

	TMap<FString, FString> textFiles;
	TMap<FString, TArray<uint8>> binaryFiles;
	TSet<FString> names;
	const int total = zip_entries_total(archive);
	bool success = total > 0;

	for (int i = 0; success && i < total; ++i)
	{
		if (zip_entry_openbyindex(archive, i) < 0)
		{
			success = false;
			break;
		}

		const char* rawName = zip_entry_name(archive);
		const FString name = rawName ? UTF8_TO_TCHAR(rawName) : TEXT("");
		const auto size = zip_entry_size(archive);
		// Web ZIP writers commonly emit explicit directory entries.
		if (zip_entry_isdir(archive) == 1)
		{
			if (name.StartsWith(TEXT("/")) || name.Contains(TEXT("..")) || name.Contains(TEXT("\\")) || name.Contains(TEXT(":")))
				success = false;
			if (zip_entry_close(archive) < 0)
				success = false;
			continue;
		}

		if (name.IsEmpty() || name.StartsWith(TEXT("/")) || name.Contains(TEXT("..")) || name.Contains(TEXT("\\")) ||
			name.Contains(TEXT(":")) || names.Contains(name.ToLower()) || zip_entry_isdir(archive) != 0 || size == 0 || size > MAX_int32)
			success = false;
		else
		{
			names.Add(name.ToLower());
			TArray<uint8> bytes;
			bytes.SetNumUninitialized(static_cast<int32>(size));

			if (zip_entry_noallocread(archive, bytes.GetData(), bytes.Num()) != static_cast<int64>(size))
				success = false;
			else if (name.EndsWith(TEXT(".json")))
				textFiles.Add(name, BytesToString(bytes.GetData(), bytes.Num()));
			else
				binaryFiles.Add(name, MoveTemp(bytes));
		}

		if (zip_entry_close(archive) < 0)
			success = false;
	}

	zip_close(archive);
	IFileManager::Get().Delete(*tempPath);
	success = success && (!textFiles.IsEmpty() || !binaryFiles.IsEmpty());

	if (success)
	{
		OutTextFiles = MoveTemp(textFiles);
		OutBinaryFiles = MoveTemp(binaryFiles);
	}

	return success;
}

bool FMounteaInventoryZipHelper::ExpandBundle(const TArray<uint8>& BundleData, const FString& InnerExtension,
	TArray<TArray<uint8>>& OutInnerZips)
{
	TMap<FString, FString> textFiles;
	TMap<FString, TArray<uint8>> binaryFiles;

	if (!ExtractEntries(BundleData, textFiles, binaryFiles) || !textFiles.IsEmpty() || binaryFiles.IsEmpty())
		return false;

	TArray<TArray<uint8>> result;
	TSet<FGuid> guids;
	TMap<FGuid, TSharedPtr<FJsonValue>> itemDefinitions;
	TArray<FString> names;
	binaryFiles.GetKeys(names);
	names.Sort();

	for (const FString& name : names)
	{
		FGuid guid;

		if (FPaths::GetExtension(name) != InnerExtension || name.Contains(TEXT("/")) ||
			!FGuid::ParseExact(FPaths::GetBaseFilename(name), EGuidFormats::DigitsWithHyphens, guid) || !guid.IsValid() || guids.Contains(guid))
			return false;

		const TArray<uint8>& bytes = binaryFiles.FindChecked(name);
		const FString root = InnerExtension == TEXT("mnteaitem") ? TEXT("item.json") :
			InnerExtension == TEXT("mntearecipe") ? TEXT("recipe.json") : TEXT("loadout.json");
		FString json, taxonomy, error;
		TArray<FString> items;

		if (!ExtractEntityArchive(bytes, root, json, items, taxonomy, error))
			return false;

		if (root == TEXT("item.json"))
			items.Add(json);
		for (const FString& itemJson : items)
		{
			TSharedPtr<FJsonValue> definition;

			if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(itemJson), definition) || !definition.IsValid())
				return false;

			FGuid itemGuid;
			FString guidText;

			if (!definition->AsObject()->TryGetStringField(TEXT("guid"), guidText) || !FGuid::Parse(guidText, itemGuid))
				return false;

			if (const TSharedPtr<FJsonValue>* previous = itemDefinitions.Find(itemGuid))
			{
				if (!FJsonValue::CompareEqual(**previous, *definition))
					return false;
			}
			else
				itemDefinitions.Add(itemGuid, definition);
		}

		TSharedPtr<FJsonObject> object;
		FString jsonGuid;
		FGuid parsedGuid;

		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), object) || !object.IsValid() || !object->TryGetStringField(TEXT("guid"), jsonGuid) ||
			!FGuid::Parse(jsonGuid, parsedGuid) || parsedGuid != guid)
			return false;

		guids.Add(guid);
		result.Add(bytes);
	}

	OutInnerZips = MoveTemp(result);
	return true;
}

bool FMounteaInventoryZipHelper::BuildArchive(const TMap<FString, TArray<uint8>>& Entries, TArray<uint8>& OutBytes)
{
	if (Entries.IsEmpty())
		return false;

	const FString tempPath = FPaths::CreateTempFilename(FPlatformProcess::UserTempDir(), TEXT("MounteaInventory"), TEXT(".zip"));
	zip_t* archive = zip_open(TCHAR_TO_UTF8(*tempPath), 6, 'w');

	if (!archive)
	{
		IFileManager::Get().Delete(*tempPath);
		return false;
	}

	bool success = true;
	TArray<FString> names;
	Entries.GetKeys(names);
	names.Sort();

	for (const FString& name : names)
	{
		if (name.IsEmpty() || name.StartsWith(TEXT("/")) || name.Contains(TEXT("..")) || name.Contains(TEXT("\\")) ||
			zip_entry_open(archive, TCHAR_TO_UTF8(*name)) < 0)
		{
			success = false;
			break;
		}

		const auto& bytes = Entries.FindChecked(name);
		const int writeResult = zip_entry_write(archive, bytes.GetData(), bytes.Num());
		const int closeResult = zip_entry_close(archive);

		if (writeResult < 0 || closeResult < 0)
		{
			success = false;
			break;
		}
	}

	zip_close(archive);
	TArray<uint8> result;
	success = success && FFileHelper::LoadFileToArray(result, *tempPath);
	IFileManager::Get().Delete(*tempPath);

	if (success)
		OutBytes = MoveTemp(result);
	return success;
}

namespace
{
	bool ParseEntityGuid(const FString& Json, FGuid& OutGuid)
	{
		TSharedPtr<FJsonObject> object;
		FString value;
		return FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), object) && object.IsValid() &&
			object->TryGetStringField(TEXT("guid"), value) && FGuid::Parse(value, OutGuid) && OutGuid.IsValid();
	}

	TArray<uint8> JsonBytes(const FString& Json)
	{
		FTCHARToUTF8 utf8(*Json);
		TArray<uint8> bytes;
		bytes.Append(reinterpret_cast<const uint8*>(utf8.Get()), utf8.Length());
		return bytes;
	}
}

bool FMounteaInventoryZipHelper::ReadEntityArchives(const FString& FilePath, const FString& SingleExtension,
	const FString& BundleExtension, TArray<TArray<uint8>>& OutArchives, FString& OutError)
{
	OutError.Reset();
	const FString extension = FPaths::GetExtension(FilePath).ToLower();
	TArray<uint8> bytes;

	if (extension != SingleExtension && extension != BundleExtension)
	{
		OutError = TEXT("Unsupported entity file extension");
		return false;
	}

	if (!LoadFileToBytes(FilePath, bytes) || !IsZipFile(bytes))
	{
		OutError = TEXT("Entity files must contain a readable ZIP archive");
		return false;
	}

	TArray<TArray<uint8>> archives;

	if (extension == BundleExtension)
	{
		if (!ExpandBundle(bytes, SingleExtension, archives))
		{
			OutError = TEXT("Invalid, empty, or mixed entity bundle");
			return false;
		}
	}
	else
	{
		const FString root = SingleExtension == TEXT("mnteaitem") ? TEXT("item.json") :
			SingleExtension == TEXT("mntearecipe") ? TEXT("recipe.json") : TEXT("loadout.json");
		FString json, taxonomy;
		TArray<FString> items;

		if (!ExtractEntityArchive(bytes, root, json, items, taxonomy, OutError))
			return false;

		archives.Add(MoveTemp(bytes));
	}

	OutArchives = MoveTemp(archives);
	return true;
}

bool FMounteaInventoryZipHelper::ExtractEntityArchive(const TArray<uint8>& Bytes, const FString& RootJsonFile,
	FString& OutJson, TArray<FString>& OutBundledItems, FString& OutStationTaxonomy, FString& OutError)
{
	OutError.Reset();
	TMap<FString, FString> text;
	TMap<FString, TArray<uint8>> binary;

	if (!ExtractEntries(Bytes, text, binary))
	{
		OutError = TEXT("Invalid entity ZIP entries");
		return false;
	}

	for (const auto& entry : binary)
	{
		if (!entry.Key.StartsWith(TEXT("assets/")))
		{
			OutError = TEXT("Unexpected binary entry outside assets/");
			return false;
		}
	}

	if (RootJsonFile != TEXT("item.json") && RootJsonFile != TEXT("recipe.json") && RootJsonFile != TEXT("loadout.json"))
	{
		OutError = TEXT("Unsupported entity root JSON file");
		return false;
	}

	const FString* root = text.Find(RootJsonFile);
	FGuid guid;

	if (!root || !ParseEntityGuid(*root, guid))
	{
		OutError = TEXT("Missing root JSON or invalid entity GUID");
		return false;
	}

	TArray<FString> items;
	TSet<FGuid> itemGuids;
	FString taxonomy;

	for (const auto& entry : text)
	{
		if (entry.Key == RootJsonFile || entry.Key.StartsWith(TEXT("assets/")))
			continue;

		if (entry.Key.StartsWith(TEXT("taxonomy/")))
		{
			TSharedPtr<FJsonValue> value;

			if (FPaths::GetExtension(entry.Key) != TEXT("json") || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(entry.Value), value))
			{
				OutError = TEXT("Invalid taxonomy JSON");
				return false;
			}

			if (entry.Key == TEXT("taxonomy/crafting-stations.json") && RootJsonFile == TEXT("recipe.json"))
				taxonomy = entry.Value;
			continue;
		}

		FGuid fileGuid, itemGuid;

		if (!entry.Key.StartsWith(TEXT("items/")) || FPaths::GetPath(entry.Key) != TEXT("items") || FPaths::GetExtension(entry.Key) != TEXT("json") ||
			!FGuid::ParseExact(FPaths::GetBaseFilename(entry.Key), EGuidFormats::DigitsWithHyphens, fileGuid) || !fileGuid.IsValid() ||
			!ParseEntityGuid(entry.Value, itemGuid) || fileGuid != itemGuid || itemGuids.Contains(itemGuid) || (RootJsonFile == TEXT("item.json") && itemGuid == guid))
		{
			OutError = TEXT("Unexpected entry or invalid bundled item GUID");
			return false;
		}

		itemGuids.Add(itemGuid);
		items.Add(entry.Value);
	}

	OutJson = *root;
	OutBundledItems = MoveTemp(items);
	OutStationTaxonomy = MoveTemp(taxonomy);
	return true;
}

bool FMounteaInventoryZipHelper::BuildEntityArchive(const FString& RootJsonFile, const FString& EntityJson,
	const TArray<FString>& BundledItemJsons, TArray<uint8>& OutBytes, FString& OutError)
{
	OutError.Reset();
	FGuid guid;

	if ((RootJsonFile != TEXT("item.json") && RootJsonFile != TEXT("recipe.json") && RootJsonFile != TEXT("loadout.json")) || !ParseEntityGuid(EntityJson, guid))
	{
		OutError = TEXT("Invalid root JSON or entity GUID");
		return false;
	}

	const FGuid rootGuid = guid;
	TMap<FString, TArray<uint8>> entries;
	entries.Add(RootJsonFile, JsonBytes(EntityJson));

	for (const FString& json : BundledItemJsons)
	{
		if (!ParseEntityGuid(json, guid) || (RootJsonFile == TEXT("item.json") && guid == rootGuid))
		{
			OutError = TEXT("Invalid or duplicate bundled item JSON or GUID");
			return false;
		}

		const FString name = TEXT("items/") + guid.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json");

		if (entries.Contains(name))
		{
			OutError = TEXT("Duplicate bundled item GUID");
			return false;
		}

		entries.Add(name, JsonBytes(json));
	}

	if (!BuildArchive(entries, OutBytes))
	{
		OutError = TEXT("Failed to build entity ZIP");
		return false;
	}

	return true;
}

bool FMounteaInventoryZipHelper::WriteEntityArchives(const TMap<FGuid, TArray<uint8>>& Archives, const FString& FilePath,
	const FString& SingleExtension, const FString& BundleExtension, FString& OutError)
{
	OutError.Reset();

	if (Archives.IsEmpty() || FilePath.IsEmpty())
	{
		OutError = TEXT("No entities or destination to export");
		return false;
	}

	FString destination = FilePath;
	const FString extension = FPaths::GetExtension(destination).ToLower();

	if (!extension.IsEmpty() && extension != SingleExtension && extension != BundleExtension)
	{
		OutError = TEXT("Unsupported export extension");
		return false;
	}

	const bool bundle = Archives.Num() > 1 || extension == BundleExtension;

	if (extension.IsEmpty())
		destination += TEXT(".") + (bundle ? BundleExtension : SingleExtension);
	else if (bundle && extension == SingleExtension)
		destination = FPaths::ChangeExtension(destination, BundleExtension);
	TMap<FString, TArray<uint8>> entries;

	for (const auto& entry : Archives)
	{
		const FString root = SingleExtension == TEXT("mnteaitem") ? TEXT("item.json") :
			SingleExtension == TEXT("mntearecipe") ? TEXT("recipe.json") : TEXT("loadout.json");
		FString json, taxonomy;
		TArray<FString> items;
		FGuid guid;

		if (!entry.Key.IsValid() || !ExtractEntityArchive(entry.Value, root, json, items, taxonomy, OutError) || !ParseEntityGuid(json, guid) || guid != entry.Key)
		{
			if (OutError.IsEmpty())
				OutError = TEXT("Invalid entity archive or GUID");
			return false;
		}

		entries.Add(entry.Key.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".") + SingleExtension, entry.Value);
	}

	TArray<uint8> bytes;

	if (bundle)
	{
		if (!BuildArchive(entries, bytes))
		{
			OutError = TEXT("Failed to build entity bundle");
			return false;
		}

		TArray<TArray<uint8>> validated;

		if (!ExpandBundle(bytes, SingleExtension, validated))
		{
			OutError = TEXT("Bundle contains invalid or conflicting entity definitions");
			return false;
		}
	}
	else
		bytes = Archives.CreateConstIterator().Value();
	const FString temp = FPaths::CreateTempFilename(*FPaths::GetPath(FPaths::ConvertRelativePathToFull(destination)), TEXT("MounteaExport"), TEXT(".tmp"));

	if (!FFileHelper::SaveArrayToFile(bytes, *temp))
	{
		OutError = TEXT("Failed to stage export file");
		IFileManager::Get().Delete(*temp);
		return false;
	}

	if (!IFileManager::Get().Move(*destination, *temp, true, true))
	{
		OutError = TEXT("Failed to replace export file");
		IFileManager::Get().Delete(*temp);
		return false;
	}

	return true;
}
