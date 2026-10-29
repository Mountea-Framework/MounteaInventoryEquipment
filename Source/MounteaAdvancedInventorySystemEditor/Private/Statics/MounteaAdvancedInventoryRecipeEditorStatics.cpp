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

#include "Statics/MounteaAdvancedInventoryRecipeEditorStatics.h"

#include "Helpers/MounteaInventoryImportHelpers.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"

#include "Definitions/MounteaRecipeTemplate.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Dom/JsonObject.h"
#include "GameplayTagsManager.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/SavePackage.h"

namespace
{
	bool ResolveRecipeStation(const TSharedPtr<FJsonObject>& Json, const TMap<FString, FString>& StationTags,
		FGameplayTag& OutStation, FString& Error)
	{
		OutStation = FGameplayTag();
		if (!Json->HasField(TEXT("reqs")))
			return true;

		const TSharedPtr<FJsonObject>* requirements = nullptr;

		if (!Json->TryGetObjectField(TEXT("reqs"), requirements))
		{
			Error = TEXT("Recipe reqs must be an object");
			return false;
		}

		if (!(*requirements)->HasField(TEXT("station")))
			return true;

		FString station;

		if (!(*requirements)->TryGetStringField(TEXT("station"), station))
		{
			Error = TEXT("Recipe station must be a string");
			return false;
		}

		if (station.IsEmpty() || station.Equals(TEXT("None"), ESearchCase::IgnoreCase))
			return true;

		const FString* mapped = StationTags.Find(station);
		OutStation = FGameplayTag::RequestGameplayTag(FName(mapped ? **mapped : *station), false);
		if (!OutStation.IsValid())
		{
			Error = TEXT("Recipe station must resolve to a registered Gameplay Tag");
			return false;
		}

		return true;
	}
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryRecipeEditorStatics::ImportRecipesFromFilePath(
	const FString& FilePath,
	const FString& TargetFolder,
	TArray<UMounteaRecipeTemplate*>& OutRecipes,
	FString& OutErrorMessage)
{
	OutRecipes.Reset();
	OutErrorMessage.Reset();
	TArray<TArray<uint8>> singleZips;

	if (!FMounteaInventoryZipHelper::ReadEntityArchives(FilePath, TEXT("mntearecipe"), TEXT("mntearecipes"), singleZips, OutErrorMessage))
		return false;

	// ── Phase 1 & 2: Read and validate everything before writing ─────────────

	struct FParsedRecipe
	{
		FString RecipeJson;
		TArray<FString> BundledItemJsons;
		FString StationTaxonomyJson;
		TSharedPtr<FJsonObject> ParsedJson;
		TSet<FGuid> BundledGuids;
		TMap<FString, FString> StationTagByName;
	};

	TArray<FParsedRecipe> parsed;
	parsed.Reserve(singleZips.Num());
	TSet<FGuid> recipeGuids;

	const TMap<FGuid, UMounteaInventoryItemTemplate*> projectItems =
		FMounteaInventoryImportHelpers::BuildProjectItemGuidMap();

	for (const TArray<uint8>& zipBlob : singleZips)
	{
		FParsedRecipe entry;

		FString parseError;

		if (!ParseRecipeFromZip(zipBlob, entry.RecipeJson, entry.BundledItemJsons, entry.StationTaxonomyJson, parseError))
		{
			OutErrorMessage += parseError + TEXT("\n");
			LOG_ERROR(TEXT("[Recipe Import] %s"), *parseError);
			return false;
		}

		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(entry.RecipeJson);

		if (!FJsonSerializer::Deserialize(reader, entry.ParsedJson) || !entry.ParsedJson.IsValid())
		{
			OutErrorMessage += TEXT("Failed to parse recipe JSON\n");
			LOG_ERROR(TEXT("[Recipe Import] Failed to parse recipe JSON"));
			return false;
		}

		entry.StationTagByName = ParseStationTagMap(entry.StationTaxonomyJson);
		FGameplayTag station;

		if (!ResolveRecipeStation(entry.ParsedJson, entry.StationTagByName, station, OutErrorMessage))
			return false;

		entry.BundledGuids = FMounteaInventoryImportHelpers::ParseBundledGuids(entry.BundledItemJsons);

		TSet<FGuid> knownGuids = entry.BundledGuids;

		for (const auto& kv : projectItems)
			knownGuids.Add(kv.Key);

		FString validationError;

		if (!ValidateRecipe(entry.ParsedJson, knownGuids, validationError))
		{
			OutErrorMessage += validationError + TEXT("\n");
			LOG_ERROR(TEXT("[Recipe Import] %s"), *validationError);
			return false;
		}

		const FGuid recipeGuid = ExtractGuidFromRecipeJson(entry.RecipeJson);

		if (recipeGuids.Contains(recipeGuid))
		{
			OutErrorMessage = TEXT("Duplicate recipe GUID in bundle");
			return false;
		}

		recipeGuids.Add(recipeGuid);
		parsed.Add(MoveTemp(entry));
	}

	// Persist validated dependencies and entities. Surface the first write failure.
	TArray<UMounteaRecipeTemplate*> existing = LoadAllExistingRecipes();
	TMap<FGuid, UMounteaInventoryItemTemplate*> itemsByGuid = projectItems;
	const FString folder = TargetFolder.IsEmpty() ? TEXT("/Game/") : TargetFolder;

	for (FParsedRecipe& entry : parsed)
	{
		if (!FMounteaInventoryImportHelpers::ImportBundledItems(entry.BundledItemJsons, folder, FilePath, itemsByGuid, OutErrorMessage))
			return false;

		const FGuid guid = ExtractGuidFromRecipeJson(entry.RecipeJson);
		UMounteaRecipeTemplate* asset = FindRecipeByGuid(existing, guid);

		if (asset)
		{
			if (!UpdateExistingRecipe(asset, entry.ParsedJson, itemsByGuid, entry.StationTagByName, FilePath, OutErrorMessage))
				return false;
		}
		else
		{
			FString name = ExtractNameFromRecipeJson(entry.RecipeJson);

			if (name.IsEmpty())
				name = guid.ToString(EGuidFormats::DigitsWithHyphens);
			asset = CreateRecipeAsset(folder, name, OutErrorMessage);

			if (!asset || !DeserializeRecipeFromJson(entry.ParsedJson, asset, itemsByGuid, entry.StationTagByName, OutErrorMessage))
				return false;

			asset->SetSourceFilePath(FilePath);

			if (!SaveRecipeAsset(asset, asset->GetOutermost()->GetName()))
			{
				OutErrorMessage = TEXT("Failed to save imported recipe asset");
				return false;
			}

			existing.Add(asset);
		}

		OutRecipes.Add(asset);
	}

	OutErrorMessage.Reset();
	return true;
}

// ---------------------------------------------------------------------------
// ZIP parsing
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryRecipeEditorStatics::ParseRecipeFromZip(
	const TArray<uint8>& ZipBytes,
	FString& OutRecipeJson,
	TArray<FString>& OutBundledItemJsons,
	FString& OutStationTaxonomyJson,
	FString& OutErrorMessage)
{
	return FMounteaInventoryZipHelper::ExtractEntityArchive(ZipBytes, TEXT("recipe.json"),
		OutRecipeJson, OutBundledItemJsons, OutStationTaxonomyJson, OutErrorMessage);
}

// ---------------------------------------------------------------------------
// Taxonomy
// ---------------------------------------------------------------------------

TMap<FString, FString> UMounteaAdvancedInventoryRecipeEditorStatics::ParseStationTagMap(const FString& TaxonomyJson)
{
	if (TaxonomyJson.IsEmpty())
		return TMap<FString, FString>();

	TArray<TSharedPtr<FJsonValue>> entries;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(TaxonomyJson);

	if (!FJsonSerializer::Deserialize(reader, entries))
	{
		LOG_WARNING(TEXT("[Recipe Import] Failed to parse taxonomy/crafting-stations.json"));
		return TMap<FString, FString>();
	}

	return ParseStationTagMap(entries);
}

TMap<FString, FString> UMounteaAdvancedInventoryRecipeEditorStatics::ParseStationTagMap(const TArray<TSharedPtr<FJsonValue>>& Entries)
{
	TMap<FString, FString> map;

	for (const TSharedPtr<FJsonValue>& entryVal : Entries)
	{
		const TSharedPtr<FJsonObject>* entryObj;

		if (!entryVal->TryGetObject(entryObj))
			continue;

		FString name, tag;

		if ((*entryObj)->TryGetStringField(TEXT("name"), name) &&
			(*entryObj)->TryGetStringField(TEXT("tag"), tag) &&
			!name.IsEmpty() && !tag.IsEmpty())
			map.Add(name, tag);
	}

	return map;
}

// ---------------------------------------------------------------------------
// Validation
// ---------------------------------------------------------------------------

namespace
{
	bool ReadFlatIngredients(const TSharedPtr<FJsonObject>& Json, TArray<TSharedPtr<FJsonValue>>& Out, FString& Error)
	{
	    if (!Json.IsValid())
	    {
		    Error = TEXT("Invalid recipe JSON");
		    return false;
	    }

	    const TArray<TSharedPtr<FJsonValue>>* ingredients = nullptr;

	    if (Json->HasField(TEXT("groups")))
	    {
		    Error = TEXT("Ingredient groups are not supported; use one ingredients array");
		    return false;
	    }

	    if (!Json->TryGetArrayField(TEXT("ingredients"), ingredients) || ingredients->IsEmpty())
	    {
		    Error = TEXT("A recipe requires a nonempty ingredients array");
		    return false;
	    }

	    Out = *ingredients;
		return true;
	}
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipe(
	const TSharedPtr<FJsonObject>& RecipeJson, const TSet<FGuid>& KnownGuids, FString& OutErrorMessage)
{
	OutErrorMessage.Reset();
	FString guidString;
	FGuid guid;

	if (!RecipeJson.IsValid() || !RecipeJson->TryGetStringField(TEXT("guid"), guidString) || !FGuid::Parse(guidString, guid) || !guid.IsValid())
	{
		OutErrorMessage = TEXT("Recipe requires a valid nonzero guid");
		return false;
	}

	const TSharedPtr<FJsonObject>* result = nullptr;
	FString resultGuid;

	if (!RecipeJson->TryGetObjectField(TEXT("result"), result) || !(*result)->TryGetStringField(TEXT("itemRef"), resultGuid))
	{
		OutErrorMessage = TEXT("Recipe requires result.itemRef");
		return false;
	}

	if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(resultGuid, TEXT("result.itemRef"), KnownGuids, OutErrorMessage))
		return false;

	double yield = 1;

	if (RecipeJson->HasField(TEXT("qtyMax")) &&
		(!RecipeJson->TryGetNumberField(TEXT("qtyMax"), yield) || !FMath::IsFinite(yield) || yield < 1 || yield > MAX_int32 || yield != FMath::FloorToDouble(yield)))
	{
		OutErrorMessage = TEXT("qtyMax must be a positive integer");
		return false;
	}

	const TSharedPtr<FJsonObject>* reqs = nullptr;

	if (RecipeJson->TryGetObjectField(TEXT("reqs"), reqs) && (*reqs)->HasField(TEXT("duration")))
	{
		double duration = 0;

		if (!(*reqs)->TryGetNumberField(TEXT("duration"), duration) || !FMath::IsFinite(duration) || duration < 0 || duration > MAX_flt)
		{
			OutErrorMessage = TEXT("Crafting duration must be a finite nonnegative number");
			return false;
		}
	}

	TArray<TSharedPtr<FJsonValue>> ingredients;

	if (!ReadFlatIngredients(RecipeJson, ingredients, OutErrorMessage))
		return false;

	TMap<FGuid, int64> totals;

	for (int32 index = 0; index < ingredients.Num(); ++index)
	{
		const TSharedPtr<FJsonObject>* ingredient = nullptr;
		const TSharedPtr<FJsonObject>* ref = nullptr;
		FString itemGuid;
		double quantity = 0;

		if (!ingredients[index].IsValid() || !ingredients[index]->TryGetObject(ingredient) ||
			!(*ingredient)->TryGetObjectField(TEXT("ref"), ref) || !(*ref)->TryGetStringField(TEXT("guid"), itemGuid) ||
			!(*ingredient)->TryGetNumberField(TEXT("qty"), quantity) || !FMath::IsFinite(quantity) || quantity <= 0 || quantity > MAX_int32 || quantity != FMath::FloorToDouble(quantity))
		{
			OutErrorMessage = FString::Printf(TEXT("ingredients[%d] requires ref.guid and a positive integer qty"), index);
			return false;
		}

		if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(itemGuid, FString::Printf(TEXT("ingredients[%d].ref.guid"), index), KnownGuids, OutErrorMessage))
			return false;

		FGuid parsed;
		FGuid::Parse(itemGuid, parsed);
		int64& total = totals.FindOrAdd(parsed);
		total += static_cast<int64>(quantity);

		if (total > MAX_int32)
		{
			OutErrorMessage = TEXT("Combined ingredient quantity exceeds int32 capacity");
			return false;
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// Asset creation & serialization
// ---------------------------------------------------------------------------

UMounteaRecipeTemplate* UMounteaAdvancedInventoryRecipeEditorStatics::CreateRecipeAsset(
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
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
		return nullptr;
	}

	UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(
		newPackage,
		UMounteaRecipeTemplate::StaticClass(),
		*uniqueName,
		RF_Public | RF_Standalone
	);

	if (!recipe)
	{
		OutErrorMessage = TEXT("Failed to create recipe template object");
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
	}

	return recipe;
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::DeserializeRecipeFromJson(
	const TSharedPtr<FJsonObject>& JsonObject, UMounteaRecipeTemplate* Recipe,
	const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
	const TMap<FString, FString>& StationTagByName, FString& OutErrorMessage)
{
	TSet<FGuid> knownGuids;

	for (const auto& item : ItemsByGuid)
	{
		if (IsValid(item.Value))
			knownGuids.Add(item.Key);
	}

	if (!IsValid(Recipe) || !ValidateRecipe(JsonObject, knownGuids, OutErrorMessage))
		return false;

	FGameplayTag station;

	if (!ResolveRecipeStation(JsonObject, StationTagByName, station, OutErrorMessage))
		return false;

	TArray<TSharedPtr<FJsonValue>> entries;

	if (!ReadFlatIngredients(JsonObject, entries, OutErrorMessage))
		return false;

	TArray<TObjectPtr<UMounteaRecipeIngredient>> ingredients;
	TMap<FGuid, int32> indexByGuid;

	for (const auto& entry : entries)
	{
		const auto object = entry->AsObject();
		FGuid guid;
		FGuid::Parse(object->GetObjectField(TEXT("ref"))->GetStringField(TEXT("guid")), guid);
		const int32 quantity = static_cast<int32>(object->GetNumberField(TEXT("qty")));

		if (const int32* index = indexByGuid.Find(guid))
			ingredients[*index]->RequiredQuantity += quantity;
		else
		{
			// Preserve a project's chosen subclass and its additional fields on reimport.
			UMounteaRecipeIngredient* previous = nullptr;

			for (UMounteaRecipeIngredient* existing : Recipe->RecipeIngredients)
			{
				if (IsValid(existing) && existing->IngredientSource.LoadSynchronous() == ItemsByGuid.FindChecked(guid))
				{
					previous = existing;
					break;
				}
			}

			UClass* ingredientClass = Recipe->DefaultIngredientClass.Get();

			if (!ingredientClass || ingredientClass->HasAnyClassFlags(CLASS_Abstract))
				ingredientClass = UMounteaRecipeIngredient::StaticClass();
			UMounteaRecipeIngredient* ingredient = previous
				? DuplicateObject<UMounteaRecipeIngredient>(previous, Recipe)
				: NewObject<UMounteaRecipeIngredient>(Recipe, ingredientClass, NAME_None, RF_Transactional);
			ingredient->IngredientSource = ItemsByGuid.FindChecked(guid);
			ingredient->RequiredQuantity = quantity;
			indexByGuid.Add(guid, ingredients.Add(ingredient));
		}
	}
	// Apply only after all parsing/validation has succeeded, so failed reimports preserve the old data.
	FGuid::Parse(JsonObject->GetStringField(TEXT("guid")), Recipe->RecipeGuid);
	FGuid resultGuid;
	FGuid::Parse(JsonObject->GetObjectField(TEXT("result"))->GetStringField(TEXT("itemRef")), resultGuid);
	Recipe->ResultItem = ItemsByGuid.FindChecked(resultGuid);
	int32 yield = 1;
	JsonObject->TryGetNumberField(TEXT("qtyMax"), yield);
	Recipe->QuantityPerCreation = yield;
	Recipe->CraftingTime = 0.f;
	Recipe->RequiredCraftingPlace = station;
	const TSharedPtr<FJsonObject>* reqs = nullptr;

	if (JsonObject->TryGetObjectField(TEXT("reqs"), reqs))
	{
		double duration = 0;
		(*reqs)->TryGetNumberField(TEXT("duration"), duration);
		Recipe->CraftingTime = static_cast<float>(duration);
	}

	Recipe->RecipeIngredients = MoveTemp(ingredients);
	return true;
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::SaveRecipeAsset(
	UMounteaRecipeTemplate* Recipe,
	const FString& PackagePath)
{
	return FMounteaInventoryImportHelpers::SaveNewAssetPackage(Recipe, PackagePath);
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::UpdateExistingRecipe(
	UMounteaRecipeTemplate* Recipe,
	const TSharedPtr<FJsonObject>& JsonObject,
	const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
	const TMap<FString, FString>& StationTagByName,
	const FString& SourceFilePath,
	FString& OutErrorMessage)
{
	if (!IsValid(Recipe))
	{
		OutErrorMessage = TEXT("Invalid recipe object");
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
		return false;
	}

	if (!DeserializeRecipeFromJson(JsonObject, Recipe, ItemsByGuid, StationTagByName, OutErrorMessage))
		return false;

	Recipe->SetSourceFilePath(SourceFilePath);

	return FMounteaInventoryImportHelpers::SaveExistingAssetPackage(Recipe, OutErrorMessage);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

FGuid UMounteaAdvancedInventoryRecipeEditorStatics::ExtractGuidFromRecipeJson(const FString& JsonString)
{
	return FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(JsonString, TEXT("guid"));
}

FString UMounteaAdvancedInventoryRecipeEditorStatics::ExtractNameFromRecipeJson(const FString& JsonString)
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

TArray<UMounteaRecipeTemplate*> UMounteaAdvancedInventoryRecipeEditorStatics::LoadAllExistingRecipes()
{
	TArray<UMounteaRecipeTemplate*> recipes;

	FAssetRegistryModule& reg = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> assets;
	reg.Get().GetAssetsByClass(UMounteaRecipeTemplate::StaticClass()->GetClassPathName(), assets, true);

	for (const FAssetData& a : assets)
	{
		if (UMounteaRecipeTemplate* r = Cast<UMounteaRecipeTemplate>(a.GetAsset()))
			recipes.Add(r);
	}

	return recipes;
}

UMounteaRecipeTemplate* UMounteaAdvancedInventoryRecipeEditorStatics::FindRecipeByGuid(
	const TArray<UMounteaRecipeTemplate*>& Recipes,
	const FGuid& Guid)
{
	if (!Guid.IsValid())
		return nullptr;

	UMounteaRecipeTemplate* const* found = Recipes.FindByPredicate(
		[&Guid](const UMounteaRecipeTemplate* Recipe)
		{
			return IsValid(Recipe) && Recipe->RecipeGuid == Guid;
		});

	return found ? *found : nullptr;
}
