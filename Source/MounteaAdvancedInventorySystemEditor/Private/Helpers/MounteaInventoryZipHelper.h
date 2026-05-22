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

#pragma once

#include "CoreMinimal.h"

/**
 * Low-level ZIP reading utilities shared by all Mountea import statics.
 * Uses the bundled zip.h / zip.c / miniz.h library (same pattern as MounteaDialogueSystem).
 *
 * All methods are atomic: they either fully succeed or leave nothing behind.
 */
struct FMounteaInventoryZipHelper
{
	/**
	 * Returns true if Data starts with a recognised ZIP magic signature.
	 * Checks PK local-file (0x04034b50), spanned (0x08074b50),
	 * central-directory (0x02014b50), and EOCD (0x06054b50).
	 */
	static bool IsZipFile(const TArray<uint8>& Data);

	/** Load a file from disk into a byte array. */
	static bool LoadFileToBytes(const FString& FilePath, TArray<uint8>& OutBytes);

	/**
	 * Extract all entries from an in-memory ZIP blob.
	 * - JSON / text entries  → OutTextFiles  (key = archive path, value = UTF-8 string)
	 * - Binary entries       → OutBinaryFiles (key = archive path, value = raw bytes)
	 * The split is determined by file extension: *.json → text, everything else → binary.
	 * Writes ZipData to a temp file, opens with zip_open, then deletes the temp on exit.
	 */
	static bool ExtractEntries(
		const TArray<uint8>& ZipData,
		TMap<FString, FString>& OutTextFiles,
		TMap<FString, TArray<uint8>>& OutBinaryFiles);

	/**
	 * Open an outer bundle ZIP and return each inner entry whose archive name ends with
	 * InnerExtension (e.g. "mnteaitem") as a raw byte blob.
	 * Used to expand .mnteaitems → array of .mnteaitem blobs, etc.
	 */
	static bool ExpandBundle(
		const TArray<uint8>& BundleData,
		const FString& InnerExtension,
		TArray<TArray<uint8>>& OutInnerZips);

	/** Convert a raw UTF-8 byte span to FString, skipping a leading BOM if present. */
	static FString BytesToString(const uint8* Bytes, int32 Count);
};
