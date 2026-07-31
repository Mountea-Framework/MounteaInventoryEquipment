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
#include "Definitions/MounteaRecipeIngredientsList.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

#include "AssetRegistry/AssetRegistryModule.h"
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
				if (SaveRecipeAsset(recipe, FPaths::Combine(folderToUse, assetName)))
					OutRecipes.Add(recipe);
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

bool UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipe(
	const TSharedPtr<FJsonObject>& RecipeJson,
	const TSet<FGuid>& KnownGuids,
	FString& OutErrorMessage)
{
	FString guidStr;
	if (!RecipeJson->TryGetStringField(TEXT("guid"), guidStr) || guidStr.IsEmpty())
	{
		OutErrorMessage = TEXT("Recipe JSON missing 'guid' field");
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
		return false;
	}

	// Validate result item
	const TSharedPtr<FJsonObject>* resultObj;
	if (RecipeJson->TryGetObjectField(TEXT("result"), resultObj))
	{
		FString itemRef;
		if ((*resultObj)->TryGetStringField(TEXT("itemRef"), itemRef) && !itemRef.IsEmpty())
		{
			FString checkError;
			if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(
				itemRef, TEXT("result.itemRef"), KnownGuids, checkError))
			{
				OutErrorMessage = checkError;
				LOG_ERROR(TEXT("[Recipe Import] %s"), *checkError);
				return false;
			}
		}
	}

	// Validate ingredient groups
	const TArray<TSharedPtr<FJsonValue>>* groupsArray;
	if (RecipeJson->TryGetArrayField(TEXT("groups"), groupsArray))
	{
		for (int32 gi = 0; gi < groupsArray->Num(); ++gi)
		{
			const TSharedPtr<FJsonObject>* groupObj;
			if (!(*groupsArray)[gi]->TryGetObject(groupObj))
				continue;

			const TArray<TSharedPtr<FJsonValue>>* ingredientsArray;
			if (!(*groupObj)->TryGetArrayField(TEXT("ingredients"), ingredientsArray))
				continue;

			for (int32 ii = 0; ii < ingredientsArray->Num(); ++ii)
			{
				const TSharedPtr<FJsonObject>* ingObj;
				if (!(*ingredientsArray)[ii]->TryGetObject(ingObj))
					continue;

				const TSharedPtr<FJsonObject>* refObj;
				FString refGuid;
				if ((*ingObj)->TryGetObjectField(TEXT("ref"), refObj) &&
					(*refObj)->TryGetStringField(TEXT("guid"), refGuid))
				{
					FString checkError;
					if (!FMounteaInventoryImportHelpers::CheckGuidInKnownSet(
						refGuid,
						FString::Printf(TEXT("groups[%d].ingredients[%d]"), gi, ii),
						KnownGuids,
						checkError))
					{
						OutErrorMessage = checkError;
						LOG_ERROR(TEXT("[Recipe Import] %s"), *checkError);
						return false;
					}
				}
			}
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
	const FString packageName = FPaths::Combine(TargetFolder, AssetName);
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
		*AssetName,
		RF_Public | RF_Standalone
	);

	if (!recipe)
	{
		OutErrorMessage = TEXT("Failed to create recipe template object");
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
	}

	return recipe;
}

namespace MounteaRecipeImportPrivate
{
	struct FIngredientRef
	{
		FGuid Guid;
		int32 Quantity = 1;
	};

	/** Safety cap on generated ingredient-option combinations (guards against pathological oneOf-group counts). */
	constexpr int32 MaxIngredientOptionCombinations = 64;
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::DeserializeRecipeFromJson(
	const TSharedPtr<FJsonObject>& JsonObject,
	UMounteaRecipeTemplate* Recipe,
	const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
	const TMap<FString, FString>& StationTagByName,
	FString& OutErrorMessage)
{
	using namespace MounteaRecipeImportPrivate;

	if (!JsonObject.IsValid() || !IsValid(Recipe))
	{
		OutErrorMessage = TEXT("Invalid JSON or recipe object");
		LOG_ERROR(TEXT("[Recipe Import] %s"), *OutErrorMessage);
		return false;
	}

	Recipe->RecipeIngredientOptions.Empty();

	// RecipeGuid
	FString guidStr;
	if (JsonObject->TryGetStringField(TEXT("guid"), guidStr))
		FGuid::Parse(guidStr, Recipe->RecipeGuid);

	// Result item
	const TSharedPtr<FJsonObject>* resultObj;
	if (JsonObject->TryGetObjectField(TEXT("result"), resultObj))
	{
		FString itemRef;
		if ((*resultObj)->TryGetStringField(TEXT("itemRef"), itemRef))
		{
			FGuid itemGuid;
			if (FGuid::Parse(itemRef, itemGuid))
			{
				if (UMounteaInventoryItemTemplate* const* t = ItemsByGuid.Find(itemGuid))
					Recipe->ResultItem = TSoftObjectPtr<UMounteaInventoryItemTemplate>(*t);
				else
					LOG_WARNING(TEXT("[Recipe Import] result.itemRef %s not found among imported/known items"), *itemRef);
			}
		}
	}

	// Result quantity: qtyMin/qtyMax live at the recipe root, not nested under "result".
	// The current data model only stores a single deterministic yield, so qtyMax is used.
	int32 qtyMax = 1;
	JsonObject->TryGetNumberField(TEXT("qtyMax"), qtyMax);
	Recipe->QuantityPerCreation = FMath::Max(1, qtyMax);

	// Crafting requirements
	const TSharedPtr<FJsonObject>* reqsObj;
	if (JsonObject->TryGetObjectField(TEXT("reqs"), reqsObj))
	{
		double duration = 0.0;
		if ((*reqsObj)->TryGetNumberField(TEXT("duration"), duration))
			Recipe->CraftingTime = static_cast<float>(duration);

		FString stationName;
		if ((*reqsObj)->TryGetStringField(TEXT("station"), stationName) &&
			!stationName.IsEmpty() && !stationName.Equals(TEXT("None"), ESearchCase::IgnoreCase))
		{
			// `station` is a display name (e.g. "Forge"); resolve through the bundled taxonomy to the real tag.
			const FString* mappedTag = StationTagByName.Find(stationName);
			const FString tagName = mappedTag ? *mappedTag : stationName;

			const FGameplayTag tag = FGameplayTag::RequestGameplayTag(FName(*tagName), false);
			if (tag.IsValid())
				Recipe->RequiredCraftingPlace = tag;
			else
				LOG_WARNING(TEXT("[Recipe Import] Crafting station '%s' has no taxonomy mapping and is not a valid registered gameplay tag; RequiredCraftingPlace left unset"), *stationName);
		}
	}

	// Ingredient groups → RecipeIngredientOptions.
	// Real recipes use groups[] with `required`/`oneOf` flags, which the current runtime model
	// (RecipeIngredientOptions = OR-alternatives, each internally AND-only) can't represent directly.
	// So: merge all required, non-oneOf groups into one shared base AND-set, and cartesian-expand
	// one option per combination of choices across all required, oneOf groups.
	TArray<FIngredientRef> baseIngredients;
	TArray<TArray<FIngredientRef>> choiceGroups;

	const TArray<TSharedPtr<FJsonValue>>* groupsArray;
	if (JsonObject->TryGetArrayField(TEXT("groups"), groupsArray))
	{
		for (int32 gi = 0; gi < groupsArray->Num(); ++gi)
		{
			const TSharedPtr<FJsonObject>* groupObj;
			if (!(*groupsArray)[gi]->TryGetObject(groupObj))
				continue;

			bool bRequired = true;
			(*groupObj)->TryGetBoolField(TEXT("required"), bRequired);
			if (!bRequired)
			{
				FString groupTitle;
				(*groupObj)->TryGetStringField(TEXT("title"), groupTitle);
				LOG_WARNING(TEXT("[Recipe Import] Skipping optional ingredient group '%s' (required=false) - not supported by the current recipe data model"), *groupTitle);
				continue;
			}

			bool bOneOf = false;
			(*groupObj)->TryGetBoolField(TEXT("oneOf"), bOneOf);

			const TArray<TSharedPtr<FJsonValue>>* ingredientsArray;
			if (!(*groupObj)->TryGetArrayField(TEXT("ingredients"), ingredientsArray))
				continue;

			TArray<FIngredientRef> groupIngredients;
			for (int32 ii = 0; ii < ingredientsArray->Num(); ++ii)
			{
				const TSharedPtr<FJsonObject>* ingObj;
				if (!(*ingredientsArray)[ii]->TryGetObject(ingObj))
					continue;

				const TSharedPtr<FJsonObject>* refObj;
				FString refGuidStr;
				if (!(*ingObj)->TryGetObjectField(TEXT("ref"), refObj) ||
					!(*refObj)->TryGetStringField(TEXT("guid"), refGuidStr))
					continue;

				FIngredientRef ingredientRef;
				if (!FGuid::Parse(refGuidStr, ingredientRef.Guid) || !ingredientRef.Guid.IsValid())
					continue;

				int32 qty = 1;
				(*ingObj)->TryGetNumberField(TEXT("qty"), qty);
				ingredientRef.Quantity = FMath::Max(1, qty);

				groupIngredients.Add(ingredientRef);
			}

			if (bOneOf)
			{
				if (groupIngredients.Num() > 0)
					choiceGroups.Add(MoveTemp(groupIngredients));
			}
			else
				baseIngredients.Append(groupIngredients);
		}
	}

	if (baseIngredients.Num() > 0 || choiceGroups.Num() > 0)
	{
		// Cartesian product across oneOf choice groups (empty choiceGroups -> single base-only combination).
		TArray<TArray<FIngredientRef>> combinations;
		combinations.Add(TArray<FIngredientRef>());

		bool bTruncated = false;
		for (const TArray<FIngredientRef>& choices : choiceGroups)
		{
			TArray<TArray<FIngredientRef>> expanded;
			expanded.Reserve(combinations.Num() * choices.Num());

			for (const TArray<FIngredientRef>& existing : combinations)
			{
				for (const FIngredientRef& choice : choices)
				{
					if (expanded.Num() >= MaxIngredientOptionCombinations)
					{
						bTruncated = true;
						break;
					}
					TArray<FIngredientRef> combo = existing;
					combo.Add(choice);
					expanded.Add(MoveTemp(combo));
				}
				if (bTruncated)
					break;
			}

			combinations = MoveTemp(expanded);
			if (bTruncated)
				break;
		}

		if (bTruncated)
			LOG_WARNING(TEXT("[Recipe Import] Recipe '%s' generates more than %d ingredient option combinations; truncating"), *guidStr, MaxIngredientOptionCombinations);

		for (const TArray<FIngredientRef>& combo : combinations)
		{
			UMounteaRecipeIngredientsList* ingredientsList =
				NewObject<UMounteaRecipeIngredientsList>(Recipe,
					UMounteaRecipeIngredientsList::StaticClass(),
					NAME_None,
					RF_NoFlags);

			auto addIngredient = [&](const FIngredientRef& Ref)
			{
				UMounteaInventoryItemTemplate* const* ingTemplate = ItemsByGuid.Find(Ref.Guid);
				if (!ingTemplate || !IsValid(*ingTemplate))
				{
					LOG_WARNING(TEXT("[Recipe Import] Ingredient GUID %s not found among imported/known items"), *Ref.Guid.ToString());
					return;
				}

				UMounteaRecipeIngredient* ingredient =
					NewObject<UMounteaRecipeIngredient>(ingredientsList,
						UMounteaRecipeIngredient::StaticClass(),
						NAME_None,
						RF_NoFlags);

				ingredient->IngredientSource = TSoftObjectPtr<UMounteaInventoryItemTemplate>(*ingTemplate);
				ingredient->RequiredQuantity = Ref.Quantity;
				ingredientsList->RecipeIngredients.Add(ingredient);
			};

			for (const FIngredientRef& ref : baseIngredients)
				addIngredient(ref);
			for (const FIngredientRef& ref : combo)
				addIngredient(ref);

			Recipe->RecipeIngredientOptions.Add(ingredientsList);
		}
	}

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
