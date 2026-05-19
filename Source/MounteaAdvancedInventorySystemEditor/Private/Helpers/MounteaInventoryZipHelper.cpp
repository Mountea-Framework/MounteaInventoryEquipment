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

THIRD_PARTY_INCLUDES_START
#include "zip.h"
THIRD_PARTY_INCLUDES_END

bool FMounteaInventoryZipHelper::IsZipFile(const TArray<uint8>& Data)
{
	if (Data.Num() < 4)
		return false;

	// ZIP signatures: local-file header, spanned, central-directory, EOCD
	constexpr uint32 SigLocalFile  = 0x04034b50;
	constexpr uint32 SigSpanned    = 0x08074b50;
	constexpr uint32 SigCentral    = 0x02014b50;
	constexpr uint32 SigEOCD       = 0x06054b50;

	const uint32 sig = static_cast<uint32>(Data[0])
		| (static_cast<uint32>(Data[1]) << 8)
		| (static_cast<uint32>(Data[2]) << 16)
		| (static_cast<uint32>(Data[3]) << 24);

	return sig == SigLocalFile || sig == SigSpanned || sig == SigCentral || sig == SigEOCD;
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

bool FMounteaInventoryZipHelper::ExtractEntries(
	const TArray<uint8>& ZipData,
	TMap<FString, FString>& OutTextFiles,
	TMap<FString, TArray<uint8>>& OutBinaryFiles)
{
	const FString tempPath = FPaths::CreateTempFilename(
		FPlatformProcess::UserTempDir(), TEXT("MounteaInventory"), TEXT(".zip"));

	if (!FFileHelper::SaveArrayToFile(ZipData, *tempPath))
		return false;

	struct zip_t* zip = zip_open(TCHAR_TO_UTF8(*tempPath), 0, 'r');
	if (!zip)
	{
		IFileManager::Get().Delete(*tempPath);
		return false;
	}

	const int total = zip_entries_total(zip);
	for (int i = 0; i < total; ++i)
	{
		zip_entry_openbyindex(zip, i);

		const char* rawName = zip_entry_name(zip);
		const int   size    = static_cast<int>(zip_entry_size(zip));

		if (rawName && size > 0)
		{
			const FString entryName = UTF8_TO_TCHAR(rawName);

			TArray<uint8> buf;
			buf.SetNumUninitialized(size);

			if (zip_entry_noallocread(zip, buf.GetData(), size) != -1)
			{
				if (entryName.EndsWith(TEXT(".json")))
					OutTextFiles.Add(entryName, BytesToString(buf.GetData(), size));
				else
					OutBinaryFiles.Add(entryName, MoveTemp(buf));
			}
		}

		zip_entry_close(zip);
	}

	zip_close(zip);
	IFileManager::Get().Delete(*tempPath);

	return true;
}

bool FMounteaInventoryZipHelper::ExpandBundle(
	const TArray<uint8>& BundleData,
	const FString& InnerExtension,
	TArray<TArray<uint8>>& OutInnerZips)
{
	const FString tempPath = FPaths::CreateTempFilename(
		FPlatformProcess::UserTempDir(), TEXT("MounteaBundle"), TEXT(".zip"));

	if (!FFileHelper::SaveArrayToFile(BundleData, *tempPath))
		return false;

	struct zip_t* zip = zip_open(TCHAR_TO_UTF8(*tempPath), 0, 'r');
	if (!zip)
	{
		IFileManager::Get().Delete(*tempPath);
		return false;
	}

	const FString dotExt = TEXT(".") + InnerExtension;
	const int total = zip_entries_total(zip);

	for (int i = 0; i < total; ++i)
	{
		zip_entry_openbyindex(zip, i);

		const char* rawName = zip_entry_name(zip);
		const int   size    = static_cast<int>(zip_entry_size(zip));

		if (rawName && size > 0)
		{
			const FString entryName = UTF8_TO_TCHAR(rawName);
			if (entryName.EndsWith(dotExt))
			{
				TArray<uint8> buf;
				buf.SetNumUninitialized(size);
				if (zip_entry_noallocread(zip, buf.GetData(), size) != -1)
					OutInnerZips.Add(MoveTemp(buf));
			}
		}

		zip_entry_close(zip);
	}

	zip_close(zip);
	IFileManager::Get().Delete(*tempPath);

	return OutInnerZips.Num() > 0;
}
