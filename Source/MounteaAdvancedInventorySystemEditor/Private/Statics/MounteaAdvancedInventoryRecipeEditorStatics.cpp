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
		return false;
	}

	if (!FMounteaInventoryZipHelper::IsZipFile(fileBytes))
	{
		OutErrorMessage = TEXT("File is not a valid ZIP archive");
		return false;
	}

	TArray<TArray<uint8>> singleZips;
	if (FilePath.EndsWith(TEXT(".mntearecipes")))
	{
		if (!FMounteaInventoryZipHelper::ExpandBundle(fileBytes, TEXT("mntearecipe"), singleZips))
		{
			OutErrorMessage = TEXT("Failed to expand .mntearecipes bundle");
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
		TSharedPtr<FJsonObject> ParsedJson;
		TSet<FGuid> BundledGuids;
	};

	TArray<FParsedRecipe> parsed;
	parsed.Reserve(singleZips.Num());

	const TMap<FGuid, UMounteaInventoryItemTemplate*> projectItems =
		FMounteaInventoryImportHelpers::BuildProjectItemGuidMap();

	for (const TArray<uint8>& zipBlob : singleZips)
	{
		FParsedRecipe entry;

		FString parseError;
		if (!ParseRecipeFromZip(zipBlob, entry.RecipeJson, entry.BundledItemJsons, parseError))
		{
			OutErrorMessage += parseError + TEXT("\n");
			return false;
		}

		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(entry.RecipeJson);
		if (!FJsonSerializer::Deserialize(reader, entry.ParsedJson) || !entry.ParsedJson.IsValid())
		{
			OutErrorMessage += TEXT("Failed to parse recipe JSON\n");
			return false;
		}

		entry.BundledGuids = FMounteaInventoryImportHelpers::ParseBundledGuids(entry.BundledItemJsons);

		TSet<FGuid> knownGuids = entry.BundledGuids;
		for (const auto& kv : projectItems)
			knownGuids.Add(kv.Key);

		FString validationError;
		if (!ValidateRecipe(entry.ParsedJson, knownGuids, validationError))
		{
			OutErrorMessage += validationError + TEXT("\n");
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
		FMounteaInventoryImportHelpers::ImportBundledItems(entry.BundledItemJsons, folderToUse, itemsByGuid);

		// Find or create recipe asset
		const FGuid recipeGuid = ExtractGuidFromRecipeJson(entry.RecipeJson);
		UMounteaRecipeTemplate* recipe = FindRecipeByGuid(existingRecipes, recipeGuid);

		if (recipe)
		{
			FString updateError;
			if (UpdateExistingRecipe(recipe, entry.ParsedJson, itemsByGuid, updateError))
				OutRecipes.Add(recipe);
			else
				OutErrorMessage += updateError + TEXT("\n");
		}
		else
		{
			const FString name = ExtractNameFromRecipeJson(entry.RecipeJson);
			const FString assetName = FString::Printf(TEXT("ImportedRecipe_%s"),
				name.IsEmpty() ? *recipeGuid.ToString(EGuidFormats::Short) : *name);

			FString createError;
			recipe = CreateRecipeAsset(folderToUse, assetName, createError);
			if (!recipe)
			{
				OutErrorMessage += createError + TEXT("\n");
				continue;
			}

			FString deserError;
			if (DeserializeRecipeFromJson(entry.ParsedJson, recipe, itemsByGuid, deserError))
			{
				if (SaveRecipeAsset(recipe, FPaths::Combine(folderToUse, assetName)))
					OutRecipes.Add(recipe);
			}
			else
				OutErrorMessage += deserError + TEXT("\n");
		}
	}

	if (OutRecipes.Num() == 0)
	{
		OutErrorMessage = TEXT("No recipes were successfully imported");
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

bool UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipe(
	const TSharedPtr<FJsonObject>& RecipeJson,
	const TSet<FGuid>& KnownGuids,
	FString& OutErrorMessage)
{
	FString guidStr;
	if (!RecipeJson->TryGetStringField(TEXT("guid"), guidStr) || guidStr.IsEmpty())
	{
		OutErrorMessage = TEXT("Recipe JSON missing 'guid' field");
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
		return nullptr;
	}

	UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(
		newPackage,
		UMounteaRecipeTemplate::StaticClass(),
		*AssetName,
		RF_Public | RF_Standalone
	);

	if (!recipe)
		OutErrorMessage = TEXT("Failed to create recipe template object");

	return recipe;
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::DeserializeRecipeFromJson(
	const TSharedPtr<FJsonObject>& JsonObject,
	UMounteaRecipeTemplate* Recipe,
	const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
	FString& OutErrorMessage)
{
	if (!JsonObject.IsValid() || !IsValid(Recipe))
	{
		OutErrorMessage = TEXT("Invalid JSON or recipe object");
		return false;
	}

	Recipe->RecipeIngredientOptions.Empty();

	// RecipeGuid
	FString guidStr;
	if (JsonObject->TryGetStringField(TEXT("guid"), guidStr))
		FGuid::Parse(guidStr, Recipe->RecipeGuid);

	// Result item + quantity
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
			}
		}

		int32 qty = 1;
		if ((*resultObj)->TryGetNumberField(TEXT("quantity"), qty))
			Recipe->QuantityPerCreation = FMath::Max(1, qty);
	}

	// Crafting requirements
	const TSharedPtr<FJsonObject>* reqsObj;
	if (JsonObject->TryGetObjectField(TEXT("reqs"), reqsObj))
	{
		double craftingTime = 0.0;
		if ((*reqsObj)->TryGetNumberField(TEXT("craftingTime"), craftingTime))
			Recipe->CraftingTime = static_cast<float>(craftingTime);

		FString stationName;
		if ((*reqsObj)->TryGetStringField(TEXT("station"), stationName) && !stationName.IsEmpty())
		{
			const FGameplayTag tag = FGameplayTag::RequestGameplayTag(FName(*stationName), false);
			if (tag.IsValid())
				Recipe->RequiredCraftingPlace = tag;
		}
	}

	// Ingredient groups → RecipeIngredientOptions
	const TArray<TSharedPtr<FJsonValue>>* groupsArray;
	if (JsonObject->TryGetArrayField(TEXT("groups"), groupsArray))
	{
		for (const TSharedPtr<FJsonValue>& groupVal : *groupsArray)
		{
			const TSharedPtr<FJsonObject>* groupObj;
			if (!groupVal->TryGetObject(groupObj))
				continue;

			UMounteaRecipeIngredientsList* ingredientsList =
				NewObject<UMounteaRecipeIngredientsList>(Recipe,
					UMounteaRecipeIngredientsList::StaticClass(),
					NAME_None,
					RF_NoFlags);

			const TArray<TSharedPtr<FJsonValue>>* ingredientsArray;
			if ((*groupObj)->TryGetArrayField(TEXT("ingredients"), ingredientsArray))
			{
				for (const TSharedPtr<FJsonValue>& ingVal : *ingredientsArray)
				{
					const TSharedPtr<FJsonObject>* ingObj;
					if (!ingVal->TryGetObject(ingObj))
						continue;

					const TSharedPtr<FJsonObject>* refObj;
					FString refGuidStr;
					if (!(*ingObj)->TryGetObjectField(TEXT("ref"), refObj) ||
						!(*refObj)->TryGetStringField(TEXT("guid"), refGuidStr))
						continue;

					FGuid ingGuid;
					if (!FGuid::Parse(refGuidStr, ingGuid) || !ingGuid.IsValid())
						continue;

					UMounteaInventoryItemTemplate* const* ingTemplate = ItemsByGuid.Find(ingGuid);
					if (!ingTemplate || !IsValid(*ingTemplate))
						continue;

					UMounteaRecipeIngredient* ingredient =
						NewObject<UMounteaRecipeIngredient>(ingredientsList,
							UMounteaRecipeIngredient::StaticClass(),
							NAME_None,
							RF_NoFlags);

					ingredient->IngredientSource = TSoftObjectPtr<UMounteaInventoryItemTemplate>(*ingTemplate);

					int32 requiredQty = 1;
					if ((*ingObj)->TryGetNumberField(TEXT("quantity"), requiredQty))
						ingredient->RequiredQuantity = FMath::Max(1, requiredQty);

					ingredientsList->RecipeIngredients.Add(ingredient);
				}
			}

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
	FString& OutErrorMessage)
{
	if (!IsValid(Recipe))
	{
		OutErrorMessage = TEXT("Invalid recipe object");
		return false;
	}

	if (!DeserializeRecipeFromJson(JsonObject, Recipe, ItemsByGuid, OutErrorMessage))
		return false;

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
		return name;

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
