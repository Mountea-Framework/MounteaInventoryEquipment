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

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Helpers/MounteaInventoryZipHelper.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace
{
	TArray<uint8> ArchiveTestBytes(const FString& Text)
	{
		FTCHARToUTF8 utf8(*Text);
		TArray<uint8> bytes;
		bytes.Append(reinterpret_cast<const uint8*>(utf8.Get()), utf8.Length());
		return bytes;
	}

	FString ArchiveTestJson(const FGuid& Guid, const FString& Name = TEXT("Example"))
	{
		return FString::Printf(TEXT("{\"guid\":\"%s\",\"displayName\":\"%s\"}"),
			*Guid.ToString(EGuidFormats::DigitsWithHyphens), *Name);
	}

	struct FArchiveTestDirectory
	{
		FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() /
			TEXT("Automation/MounteaUnifiedArchive") / FGuid::NewGuid().ToString(EGuidFormats::Digits));
		FArchiveTestDirectory() { IFileManager::Get().MakeDirectory(*Path, true); }
		~FArchiveTestDirectory() { IFileManager::Get().DeleteDirectory(*Path, false, true); }
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaUnifiedArchiveTest,
	"Mountea.InventorySystemEditor.UnifiedArchive.TransportContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaUnifiedArchiveTest::RunTest(const FString& Parameters)
{
	FArchiveTestDirectory directory;
	struct FEntityFormat { const TCHAR* Root; const TCHAR* Single; const TCHAR* Bundle; };
	const FEntityFormat formats[] = {
		{TEXT("item.json"), TEXT("mnteaitem"), TEXT("mnteaitems")},
		{TEXT("recipe.json"), TEXT("mntearecipe"), TEXT("mntearecipes")},
		{TEXT("loadout.json"), TEXT("mntealoadout"), TEXT("mntealoadouts")}
	};
	for (const FEntityFormat& format : formats)
	{
		const FString context = format.Single;
		FGuid first = FGuid::NewGuid();
		first.A |= 0xA0000000;
		const FGuid second = FGuid::NewGuid(), dependency = FGuid::NewGuid();
		const FString firstJson = ArchiveTestJson(first), secondJson = ArchiveTestJson(second);
		const FString dependencyJson = ArchiveTestJson(dependency);
		const FString dependencyPath = TEXT("items/") + dependency.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json");
		const FString firstName = first.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".") + format.Single;
		const FString secondName = second.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".") + format.Single;
		FString error, rootJson, taxonomy;
		TArray<FString> dependencies;
		TArray<uint8> firstArchive, secondArchive;
		if (!TestTrue(context + TEXT(" builds first archive"), FMounteaInventoryZipHelper::BuildEntityArchive(
			format.Root, firstJson, {dependencyJson}, firstArchive, error))) continue;
		if (!TestTrue(context + TEXT(" builds second archive"), FMounteaInventoryZipHelper::BuildEntityArchive(
			format.Root, secondJson, {dependencyJson}, secondArchive, error))) continue;
		TMap<FString, FString> text;
		TMap<FString, TArray<uint8>> binary;
		TestTrue(context + TEXT(" extracts physical entries"), FMounteaInventoryZipHelper::ExtractEntries(firstArchive, text, binary));
		TestTrue(context + TEXT(" uses canonical root filename"), text.Contains(format.Root));
		TestTrue(context + TEXT(" uses dependency GUID filename"), text.Contains(dependencyPath));
		TestTrue(context + TEXT(" extracts entity"), FMounteaInventoryZipHelper::ExtractEntityArchive(
			firstArchive, format.Root, rootJson, dependencies, taxonomy, error));
		TestEqual(context + TEXT(" preserves root GUID payload"), rootJson, firstJson);
		TestEqual(context + TEXT(" preserves dependency"), dependencies.Num(), 1);

		const FString singlePath = directory.Path / (context + TEXT(".") + format.Single);
		TMap<FGuid, TArray<uint8>> archives;
		archives.Add(first, firstArchive);
		TestTrue(context + TEXT(" writes single"), FMounteaInventoryZipHelper::WriteEntityArchives(
			archives, singlePath, format.Single, format.Bundle, error));
		TArray<TArray<uint8>> read;
		TestTrue(context + TEXT(" reads single"), FMounteaInventoryZipHelper::ReadEntityArchives(
			singlePath, format.Single, format.Bundle, read, error));
		TestEqual(context + TEXT(" single count"), read.Num(), 1);

		const FString bundlePath = directory.Path / (context + TEXT(".") + format.Bundle);
		archives.Add(second, secondArchive);
		const FString automaticPath = directory.Path / (context + TEXT("_automatic"));
		TestTrue(context + TEXT(" chooses bundle extension by count"), FMounteaInventoryZipHelper::WriteEntityArchives(
			archives, automaticPath, format.Single, format.Bundle, error));
		TestTrue(context + TEXT(" automatic bundle destination exists"), IFileManager::Get().FileExists(*(automaticPath + TEXT(".") + format.Bundle)));
		TestTrue(context + TEXT(" writes bundle"), FMounteaInventoryZipHelper::WriteEntityArchives(
			archives, bundlePath, format.Single, format.Bundle, error));
		TArray<uint8> bundle;
		TestTrue(context + TEXT(" loads written bundle"), FFileHelper::LoadFileToArray(bundle, *bundlePath));
		TestTrue(context + TEXT(" extracts outer bundle"), FMounteaInventoryZipHelper::ExtractEntries(bundle, text, binary));
		TestTrue(context + TEXT(" first inner GUID filename"), binary.Contains(firstName));
		TestTrue(context + TEXT(" second inner GUID filename"), binary.Contains(secondName));
		TestTrue(context + TEXT(" reads bundle"), FMounteaInventoryZipHelper::ReadEntityArchives(
			bundlePath, format.Single, format.Bundle, read, error));
		TestEqual(context + TEXT(" bundle count"), read.Num(), 2);
		for (const TArray<uint8>& inner : read)
		{
			TestTrue(context + TEXT(" same entity parser handles bundled archive"), FMounteaInventoryZipHelper::ExtractEntityArchive(
				inner, format.Root, rootJson, dependencies, taxonomy, error));
			TestTrue(context + TEXT(" bundled payload matches entity GUID"), rootJson == firstJson || rootJson == secondJson);
		}

		// Optional Inventour media and taxonomy never require UE assets to be created.
		TMap<FString, TArray<uint8>> entries;
		entries.Add(format.Root, ArchiveTestBytes(firstJson));
		entries.Add(TEXT("assets/"), TArray<uint8>());
		entries.Add(TEXT("items/"), TArray<uint8>());
		entries.Add(TEXT("taxonomy/"), TArray<uint8>());
		entries.Add(TEXT("assets/icon.png"), TArray<uint8>{1, 2, 3});
		entries.Add(TEXT("assets/metadata.json"), ArchiveTestBytes(TEXT("{}")));
		entries.Add(TEXT("taxonomy/categories.json"), ArchiveTestBytes(TEXT("[]")));
		entries.Add(TEXT("taxonomy/crafting-stations.json"), ArchiveTestBytes(TEXT("[]")));
		TArray<uint8> optionalArchive;
		TestTrue(context + TEXT(" builds optional entries"), FMounteaInventoryZipHelper::BuildArchive(entries, optionalArchive));
		TestTrue(context + TEXT(" accepts optional entries"), FMounteaInventoryZipHelper::ExtractEntityArchive(
			optionalArchive, format.Root, rootJson, dependencies, taxonomy, error));
		TestEqual(context + TEXT(" station taxonomy only consumed for recipes"), taxonomy,
			FString(format.Root) == TEXT("recipe.json") ? FString(TEXT("[]")) : FString());

		// Reject invalid input without replacing any caller output or destination file.
		const TArray<uint8> sentinel = {9, 8, 7};
		read = {sentinel};
		TestTrue(context + TEXT(" stages raw JSON fixture"), FFileHelper::SaveArrayToFile(ArchiveTestBytes(firstJson), *singlePath));
		TestFalse(context + TEXT(" rejects raw JSON"), FMounteaInventoryZipHelper::ReadEntityArchives(
			singlePath, format.Single, format.Bundle, read, error));
		TestTrue(context + TEXT(" failed read preserves output"), read.Num() == 1 && read[0] == sentinel);
		TestTrue(context + TEXT(" stages wrong extension fixture"), FFileHelper::SaveArrayToFile(firstArchive, *(directory.Path / TEXT("wrong.json"))));
		TestFalse(context + TEXT(" rejects wrong extension"), FMounteaInventoryZipHelper::ReadEntityArchives(
			directory.Path / TEXT("wrong.json"), format.Single, format.Bundle, read, error));
		TestTrue(context + TEXT(" stages existing destination"), FFileHelper::SaveArrayToFile(sentinel, *singlePath));
		TMap<FGuid, TArray<uint8>> invalidExport;
		invalidExport.Add(second, firstArchive);
		TestFalse(context + TEXT(" rejects mismatched export GUID"), FMounteaInventoryZipHelper::WriteEntityArchives(
			invalidExport, singlePath, format.Single, format.Bundle, error));
		TArray<uint8> preserved;
		TestTrue(context + TEXT(" reloads destination"), FFileHelper::LoadFileToArray(preserved, *singlePath));
		TestTrue(context + TEXT(" failed export preserves destination"), preserved == sentinel);

		entries.Reset();
		entries.Add(format.Root, ArchiveTestBytes(firstJson));
		entries.Add(TEXT("items/") + second.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json"), ArchiveTestBytes(dependencyJson));
		TArray<uint8> malformed;
		TestTrue(context + TEXT(" builds dependency mismatch fixture"), FMounteaInventoryZipHelper::BuildArchive(entries, malformed));
		rootJson = TEXT("unchanged");
		TestFalse(context + TEXT(" rejects dependency filename mismatch"), FMounteaInventoryZipHelper::ExtractEntityArchive(
			malformed, format.Root, rootJson, dependencies, taxonomy, error));
		TestEqual(context + TEXT(" failed extraction preserves output"), rootJson, FString(TEXT("unchanged")));
		TestFalse(context + TEXT(" rejects duplicate dependency GUID"), FMounteaInventoryZipHelper::BuildEntityArchive(
			format.Root, firstJson, {dependencyJson, dependencyJson}, malformed, error));
		TestFalse(context + TEXT(" rejects zero root GUID"), FMounteaInventoryZipHelper::BuildEntityArchive(
			format.Root, ArchiveTestJson(FGuid()), {}, malformed, error));

		// Semantic equality ignores formatting and field order for shared dependencies.
		TArray<uint8> equivalent;
		const FString reordered = FString::Printf(TEXT("{ \"displayName\": \"Example\", \"guid\": \"%s\" }"),
			*dependency.ToString(EGuidFormats::DigitsWithHyphens));
		TestTrue(context + TEXT(" builds equivalent dependency fixture"), FMounteaInventoryZipHelper::BuildEntityArchive(
			format.Root, secondJson, {reordered}, equivalent, error));
		entries.Reset();
		entries.Add(firstName, firstArchive);
		entries.Add(secondName, equivalent);
		TestTrue(context + TEXT(" builds equivalent dependency bundle"), FMounteaInventoryZipHelper::BuildArchive(entries, malformed));
		TestTrue(context + TEXT(" accepts equivalent dependency definitions"), FMounteaInventoryZipHelper::ExpandBundle(malformed, format.Single, read));
		read = {sentinel};

		// Outer archive validation runs before any import caller can change assets.
		TArray<uint8> conflicting;
		TestTrue(context + TEXT(" builds conflicting dependency fixture"), FMounteaInventoryZipHelper::BuildEntityArchive(
			format.Root, secondJson, {ArchiveTestJson(dependency, TEXT("Different"))}, conflicting, error));
		entries.Reset();
		entries.Add(firstName, firstArchive);
		entries.Add(secondName, conflicting);
		TestTrue(context + TEXT(" builds conflicting bundle"), FMounteaInventoryZipHelper::BuildArchive(entries, malformed));
		TestFalse(context + TEXT(" rejects conflicting dependency definitions"), FMounteaInventoryZipHelper::ExpandBundle(malformed, format.Single, read));
		TMap<FGuid, TArray<uint8>> conflictingExports;
		conflictingExports.Add(first, firstArchive); conflictingExports.Add(second, conflicting);
		TestFalse(context + TEXT(" rejects conflicting dependency export"), FMounteaInventoryZipHelper::WriteEntityArchives(
			conflictingExports, singlePath, format.Single, format.Bundle, error));
		preserved.Reset();
		TestTrue(context + TEXT(" reloads failed conflict destination"), FFileHelper::LoadFileToArray(preserved, *singlePath));
		TestTrue(context + TEXT(" conflicting export preserves existing file"), preserved == sentinel);
		entries.Reset();
		entries.Add(firstName, firstArchive);
		entries.Add(secondName, firstArchive);
		TestTrue(context + TEXT(" builds repeated entity identity fixture"), FMounteaInventoryZipHelper::BuildArchive(entries, malformed));
		TestFalse(context + TEXT(" rejects duplicate entity GUID payloads"), FMounteaInventoryZipHelper::ExpandBundle(malformed, format.Single, read));
		entries.Reset();
		entries.Add(firstName, firstArchive);
		entries.Add(TEXT("unexpected.json"), ArchiveTestBytes(TEXT("{}")));
		TestTrue(context + TEXT(" builds mixed bundle"), FMounteaInventoryZipHelper::BuildArchive(entries, malformed));
		TestFalse(context + TEXT(" rejects mixed bundle"), FMounteaInventoryZipHelper::ExpandBundle(malformed, format.Single, read));
		entries.Reset();
		entries.Add(firstName, firstArchive);
		entries.Add(secondName, TArray<uint8>{0x50, 0x4b, 0x03, 0x04});
		TestTrue(context + TEXT(" builds corrupt inner archive fixture"), FMounteaInventoryZipHelper::BuildArchive(entries, malformed));
		TestFalse(context + TEXT(" rejects corrupt inner archive"), FMounteaInventoryZipHelper::ExpandBundle(malformed, format.Single, read));
		const TArray<uint8> emptyZip = {0x50, 0x4b, 0x05, 0x06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		TestFalse(context + TEXT(" rejects empty bundle"), FMounteaInventoryZipHelper::ExpandBundle(emptyZip, format.Single, read));
		TestFalse(context + TEXT(" rejects corrupt bundle"), FMounteaInventoryZipHelper::ExpandBundle({0x50, 0x4b, 0x03, 0x04}, format.Single, read));
		TestTrue(context + TEXT(" failed bundle validation preserves output"), read.Num() == 1 && read[0] == sentinel);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
