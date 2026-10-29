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

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool UMounteaAdvancedInventoryRecipeEditorStatics::ImportRecipesFromFilePath(
	const FString& FilePath,
	const FString& TargetFolder,
	TArray<UMounteaRecipeTemplate*>& OutRecipes,
	FString& OutErrorMessage)
{
	TArray<uint8> fileBytes;
	if (!FMounteaInventoryZipHelper::LoadFileToBytes(FilePath, fileBytes))
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
		return false;
	}

	if (!FMounteaInventoryZipHelper::IsZipFile(fileBytes))
	{
		OutErrorMessage = TEXT("File is not a valid ZIP archive");
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
		return false;
	}

	TArray<TArray<uint8>> singleZips;
	if (FilePath.EndsWith(TEXT(".mntearecipes")))
	{
		if (!FMounteaInventoryZipHelper::ExpandBundle(fileBytes, TEXT("mntearecipe"), singleZips))
		{
			OutErrorMessage = TEXT("Failed to expand .mntearecipes bundle");
			LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
			return false;
		}
	}
	else
		singleZips.Add(MoveTemp(fileBytes));

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

		parsed.Add(MoveTemp(entry));
	}

	// ── Phase 3: Import bundled items, then persist recipes ──────────────────

	TArray<UMounteaRecipeTemplate*> existingRecipes = LoadAllExistingRecipes();

	for (FParsedRecipe& entry : parsed)
	{
		const FString folderToUse = TargetFolder.IsEmpty() ? TEXT("/Game/") : TargetFolder;

		TMap<FGuid, UMounteaInventoryItemTemplate*> itemsByGuid = projectItems;
		FMounteaInventoryImportHelpers::ImportBundledItems(entry.BundledItemJsons, folderToUse, FilePath, itemsByGuid);

		// Find or create recipe asset
		const FGuid recipeGuid = ExtractGuidFromRecipeJson(entry.RecipeJson);
		UMounteaRecipeTemplate* recipe = FindRecipeByGuid(existingRecipes, recipeGuid);

		if (recipe)
		{
			FString updateError;
			if (UpdateExistingRecipe(recipe, entry.ParsedJson, itemsByGuid, entry.StationTagByName, FilePath, updateError))
				OutRecipes.Add(recipe);
			else
			{
				OutErrorMessage += updateError + TEXT("\n");
				LOG_ERROR(TEXT("[Recipe Import] %s"), *updateError);
			}
		}
		else
		{
			const FString name = ExtractNameFromRecipeJson(entry.RecipeJson);
			const FString assetName = name.IsEmpty() ? recipeGuid.ToString(EGuidFormats::Short) : name;

			FString createError;
			recipe = CreateRecipeAsset(folderToUse, assetName, createError);
			if (!recipe)
			{
				OutErrorMessage += createError + TEXT("\n");
				LOG_ERROR(TEXT("[Recipe Import] %s"), *createError);
				continue;
			}

			FString deserError;
			if (DeserializeRecipeFromJson(entry.ParsedJson, recipe, itemsByGuid, entry.StationTagByName, deserError))
			{
				recipe->SetSourceFilePath(FilePath);
				if (SaveRecipeAsset(recipe, recipe->GetOutermost()->GetName()))
				{
					OutRecipes.Add(recipe);
					existingRecipes.Add(recipe);
				}
				else
				{
					OutErrorMessage += FString::Printf(TEXT("Failed to save recipe asset '%s'\n"), *assetName);
					LOG_ERROR(TEXT("[Recipe Import] Failed to save recipe asset '%s'"), *assetName);
				}
			}
			else
			{
				OutErrorMessage += deserError + TEXT("\n");
				LOG_ERROR(TEXT("[Recipe Import] %s"), *deserError);
			}
		}
	}

	if (OutRecipes.Num() == 0)
	{
		OutErrorMessage = TEXT("No recipes were successfully imported");
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
		return false;
	}

	OutErrorMessage = FString::Printf(TEXT("Recipe import complete: %d imported"), OutRecipes.Num());
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
	TMap<FString, FString> textFiles;
	TMap<FString, TArray<uint8>> binaryFiles;

	if (!FMounteaInventoryZipHelper::ExtractEntries(ZipBytes, textFiles, binaryFiles))
	{
		OutErrorMessage = TEXT("Failed to extract ZIP entries");
		return false;
	}

	const FString* recipeJson = textFiles.Find(TEXT("recipe.json"));
	if (!recipeJson)
	{
		OutErrorMessage = TEXT("recipe.json not found in ZIP");
		return false;
	}

	OutRecipeJson = *recipeJson;

	// Optional: display-name -> gameplay tag lookup for `reqs.station`. Absence is not an error.
	if (const FString* taxonomyJson = textFiles.Find(TEXT("taxonomy/crafting-stations.json")))
		OutStationTaxonomyJson = *taxonomyJson;

	for (const auto& kv : textFiles)
	{
		if (kv.Key.StartsWith(TEXT("items/")) && kv.Key.EndsWith(TEXT(".json")))
			OutBundledItemJsons.Add(kv.Value);
	}

	return true;
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
	// Transitional reader for Inventour's current format. No alternatives enter the runtime model.
	bool ReadFlatIngredients(const TSharedPtr<FJsonObject>& Json, TArray<TSharedPtr<FJsonValue>>& Out, FString& Error)
	{
		if (!Json.IsValid()) { Error = TEXT("Invalid recipe JSON"); return false; }
		const TArray<TSharedPtr<FJsonValue>>* ingredients = nullptr;
		if (Json->HasField(TEXT("ingredients")))
		{
			if (Json->HasField(TEXT("groups"))) { Error = TEXT("Recipe cannot contain both ingredients and legacy groups"); return false; }
			if (!Json->TryGetArrayField(TEXT("ingredients"), ingredients)) { Error = TEXT("ingredients must be an array"); return false; }
			Out = *ingredients;
		}
		else
		{
			const TArray<TSharedPtr<FJsonValue>>* groups = nullptr;
			if (!Json->TryGetArrayField(TEXT("groups"), groups)) { Error = TEXT("Recipe is missing its ingredients array"); return false; }
			for (const auto& value : *groups)
			{
				const TSharedPtr<FJsonObject>* group = nullptr;
				if (!value.IsValid() || !value->TryGetObject(group)) { Error = TEXT("Invalid legacy ingredient group"); return false; }
				bool required = true, oneOf = false;
				if (((*group)->HasField(TEXT("required")) && !(*group)->TryGetBoolField(TEXT("required"), required)) ||
					((*group)->HasField(TEXT("oneOf")) && !(*group)->TryGetBoolField(TEXT("oneOf"), oneOf)))
				{ Error = TEXT("Legacy group flags must be booleans"); return false; }
				if (!required || oneOf)
				{ Error = TEXT("Alternative or optional ingredients are no longer supported. Convert each crafting method to a separate recipe before importing."); return false; }
				if (!(*group)->TryGetArrayField(TEXT("ingredients"), ingredients)) { Error = TEXT("Legacy group is missing ingredients"); return false; }
				Out.Append(*ingredients);
			}
		}
		if (Out.IsEmpty()) { Error = TEXT("A recipe must have at least one mandatory ingredient"); return false; }
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
	{ OutErrorMessage = TEXT("Recipe requires a valid nonzero guid"); return false; }
	const TSharedPtr<FJsonObject>* result = nullptr;
	FString resultGuid;
	if (!RecipeJson->TryGetObjectField(TEXT("result"), result) || !(*result)->TryGetStringField(TEXT("itemRef"), resultGuid))
	{ OutErrorMessage = TEXT("Recipe requires result.itemRef"); return false; }
	if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(resultGuid, TEXT("result.itemRef"), KnownGuids, OutErrorMessage)) return false;
	double yield = 1;
	if (RecipeJson->HasField(TEXT("qtyMax")) &&
		(!RecipeJson->TryGetNumberField(TEXT("qtyMax"), yield) || !FMath::IsFinite(yield) || yield < 1 || yield > MAX_int32 || yield != FMath::FloorToDouble(yield)))
	{ OutErrorMessage = TEXT("qtyMax must be a positive integer"); return false; }
	const TSharedPtr<FJsonObject>* reqs = nullptr;
	if (RecipeJson->TryGetObjectField(TEXT("reqs"), reqs) && (*reqs)->HasField(TEXT("duration")))
	{
		double duration = 0;
		if (!(*reqs)->TryGetNumberField(TEXT("duration"), duration) || !FMath::IsFinite(duration) || duration < 0 || duration > MAX_flt)
		{ OutErrorMessage = TEXT("Crafting duration must be a finite nonnegative number"); return false; }
	}
	TArray<TSharedPtr<FJsonValue>> ingredients;
	if (!ReadFlatIngredients(RecipeJson, ingredients, OutErrorMessage)) return false;
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
		{ OutErrorMessage = FString::Printf(TEXT("ingredients[%d] requires ref.guid and a positive integer qty"), index); return false; }
		if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(itemGuid, FString::Printf(TEXT("ingredients[%d].ref.guid"), index), KnownGuids, OutErrorMessage)) return false;
		FGuid parsed; FGuid::Parse(itemGuid, parsed);
		int64& total = totals.FindOrAdd(parsed);
		total += static_cast<int64>(quantity);
		if (total > MAX_int32) { OutErrorMessage = TEXT("Combined ingredient quantity exceeds int32 capacity"); return false; }
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
	for (const auto& item : ItemsByGuid) if (IsValid(item.Value)) knownGuids.Add(item.Key);
	if (!IsValid(Recipe) || !ValidateRecipe(JsonObject, knownGuids, OutErrorMessage)) return false;

	TArray<TSharedPtr<FJsonValue>> entries;
	if (!ReadFlatIngredients(JsonObject, entries, OutErrorMessage)) return false;
	TArray<TObjectPtr<UMounteaRecipeIngredient>> ingredients;
	TMap<FGuid, int32> indexByGuid;
	for (const auto& entry : entries)
	{
		const auto object = entry->AsObject();
		FGuid guid; FGuid::Parse(object->GetObjectField(TEXT("ref"))->GetStringField(TEXT("guid")), guid);
		const int32 quantity = static_cast<int32>(object->GetNumberField(TEXT("qty")));
		if (const int32* index = indexByGuid.Find(guid)) ingredients[*index]->RequiredQuantity += quantity;
		else
		{
			// Preserve a project's chosen subclass and its additional fields on reimport.
			UMounteaRecipeIngredient* previous = nullptr;
			for (UMounteaRecipeIngredient* existing : Recipe->RecipeIngredients)
				if (IsValid(existing) && existing->IngredientSource.LoadSynchronous() == ItemsByGuid.FindChecked(guid))
				{ previous = existing; break; }
			UClass* ingredientClass = Recipe->DefaultIngredientClass.Get();
			if (!ingredientClass || ingredientClass->HasAnyClassFlags(CLASS_Abstract)) ingredientClass = UMounteaRecipeIngredient::StaticClass();
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
	FGuid resultGuid; FGuid::Parse(JsonObject->GetObjectField(TEXT("result"))->GetStringField(TEXT("itemRef")), resultGuid);
	Recipe->ResultItem = ItemsByGuid.FindChecked(resultGuid);
	int32 yield = 1; JsonObject->TryGetNumberField(TEXT("qtyMax"), yield);
	Recipe->QuantityPerCreation = yield;
	Recipe->CraftingTime = 0.f;
	Recipe->RequiredCraftingPlace = FGameplayTag();
	const TSharedPtr<FJsonObject>* reqs = nullptr;
	if (JsonObject->TryGetObjectField(TEXT("reqs"), reqs))
	{
		double duration = 0; (*reqs)->TryGetNumberField(TEXT("duration"), duration);
		Recipe->CraftingTime = static_cast<float>(duration);
		FString station;
		if ((*reqs)->TryGetStringField(TEXT("station"), station) && !station.IsEmpty() && !station.Equals(TEXT("None"), ESearchCase::IgnoreCase))
		{
			const FString* mapped = StationTagByName.Find(station);
			Recipe->RequiredCraftingPlace = FGameplayTag::RequestGameplayTag(FName(mapped ? **mapped : *station), false);
		}
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
		[&Guid](const UMounteaRecipeTemplate* r)
		{
			return IsValid(r) && r->RecipeGuid == Guid;
		});

	return found ? *found : nullptr;
}
