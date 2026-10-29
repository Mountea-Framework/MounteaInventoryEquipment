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
#include "Definitions/MounteaRecipeTemplate.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	TArray<uint8> JsonBytes(const TSharedRef<FJsonObject>& Object)
	{
		FString text;
		FJsonSerializer::Serialize(Object, TJsonWriterFactory<>::Create(&text));
		FTCHARToUTF8 utf8(*text);
		TArray<uint8> bytes;
		bytes.Append(reinterpret_cast<const uint8*>(utf8.Get()), utf8.Length());
		return bytes;
	}

	bool BuildRecipeArchive(UMounteaRecipeTemplate* Recipe, TArray<uint8>& Bytes, FString& Error)
	{
		if (!UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipeTemplate(Recipe, Error)) return false;
		const auto json = MakeShared<FJsonObject>();
		json->SetStringField(TEXT("guid"), Recipe->RecipeGuid.ToString(EGuidFormats::DigitsWithHyphens));
		json->SetStringField(TEXT("name"), Recipe->GetName());
		json->SetStringField(TEXT("ingredientModel"), TEXT("flat-v1"));
		json->SetNumberField(TEXT("qtyMin"), Recipe->QuantityPerCreation);
		json->SetNumberField(TEXT("qtyMax"), Recipe->QuantityPerCreation);
		const auto result = MakeShared<FJsonObject>();
		UMounteaInventoryItemTemplate* output = Recipe->ResultItem.LoadSynchronous();
		result->SetStringField(TEXT("itemRef"), output->Guid.ToString(EGuidFormats::DigitsWithHyphens));
		result->SetStringField(TEXT("display"), output->DisplayName.ToString());
		json->SetObjectField(TEXT("result"), result);
		const auto reqs = MakeShared<FJsonObject>();
		reqs->SetNumberField(TEXT("duration"), Recipe->CraftingTime);
		reqs->SetStringField(TEXT("station"), Recipe->RequiredCraftingPlace.IsValid() ? Recipe->RequiredCraftingPlace.ToString() : TEXT("None"));
		json->SetObjectField(TEXT("reqs"), reqs);

		TSet<UMounteaInventoryItemTemplate*> items { output };
		TArray<TSharedPtr<FJsonValue>> ingredients;
		TMap<UMounteaInventoryItemTemplate*, int32> indices;
		for (const auto& ingredient : Recipe->RecipeIngredients)
		{
			if (!IsValid(ingredient)) { Error = TEXT("Every ingredient must be a valid object"); return false; }
			auto* item = ingredient->IngredientSource.LoadSynchronous();
			if (const int32* index = indices.Find(item))
			{
				auto entry = ingredients[*index]->AsObject();
				entry->SetNumberField(TEXT("qty"), entry->GetNumberField(TEXT("qty")) + ingredient->RequiredQuantity);
				continue;
			}
			auto ref = MakeShared<FJsonObject>();
			ref->SetStringField(TEXT("guid"), item->Guid.ToString(EGuidFormats::DigitsWithHyphens));
			ref->SetStringField(TEXT("displayName"), item->DisplayName.ToString());
			auto entry = MakeShared<FJsonObject>();
			entry->SetObjectField(TEXT("ref"), ref);
			entry->SetNumberField(TEXT("qty"), ingredient->RequiredQuantity);
			indices.Add(item, ingredients.Num());
			ingredients.Add(MakeShared<FJsonValueObject>(entry));
			items.Add(item);
		}
		json->SetArrayField(TEXT("ingredients"), ingredients);
		TMap<FString, TArray<uint8>> entries;
		entries.Add(TEXT("recipe.json"), JsonBytes(json));
		TSet<FGuid> itemGuids;
		for (auto* item : items)
		{
			if (!item->Guid.IsValid() || itemGuids.Contains(item->Guid))
			{ Error = TEXT("Bundled item GUIDs must be valid and unique"); return false; }
			itemGuids.Add(item->Guid);
			if (!item->CalculateJson() || item->GetJson().IsEmpty()) { Error = TEXT("Unable to serialize a referenced item template"); return false; }
			FTCHARToUTF8 utf8(*item->GetJson());
			TArray<uint8> itemBytes;
			itemBytes.Append(reinterpret_cast<const uint8*>(utf8.Get()), utf8.Length());
			entries.Add(TEXT("items/") + item->Guid.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json"), MoveTemp(itemBytes));
		}
		// The station is emitted as its registered tag, so it round-trips without a display-name lookup.
		if (!FMounteaInventoryZipHelper::BuildArchive(entries, Bytes)) { Error = TEXT("Unable to build recipe archive"); return false; }
		return true;
	}
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipeTemplate(const UMounteaRecipeTemplate* Recipe, FString& Error)
{
	Error.Reset();
	if (!IsValid(Recipe) || !Recipe->RecipeGuid.IsValid()) { Error = TEXT("Recipe must have a valid GUID"); return false; }
	const auto* output = Recipe->ResultItem.LoadSynchronous();
	if (!IsValid(output) || !output->Guid.IsValid() || Recipe->QuantityPerCreation <= 0)
	{ Error = TEXT("Choose a valid result item and a positive output quantity"); return false; }
	if (!FMath::IsFinite(Recipe->CraftingTime) || Recipe->CraftingTime < 0)
	{ Error = TEXT("Crafting time must be finite and nonnegative"); return false; }
	if (Recipe->RecipeIngredients.IsEmpty()) { Error = TEXT("Add at least one mandatory ingredient"); return false; }
	TMap<UMounteaInventoryItemTemplate*, int64> totals;
	for (const auto& ingredient : Recipe->RecipeIngredients)
	{
		if (!IsValid(ingredient)) { Error = TEXT("Every ingredient must be a valid object"); return false; }
		auto* item = ingredient->IngredientSource.LoadSynchronous();
		if (!IsValid(item) || !item->Guid.IsValid() || ingredient->RequiredQuantity <= 0)
		{ Error = TEXT("Every ingredient needs a valid item and a positive quantity"); return false; }
		int64& total = totals.FindOrAdd(item);
		total += ingredient->RequiredQuantity;
		if (total > MAX_int32) { Error = TEXT("Combined ingredient quantity exceeds int32 capacity"); return false; }
	}
	return true;
}

bool UMounteaAdvancedInventoryRecipeEditorStatics::ExportRecipesToFilePath(
	const TArray<UMounteaRecipeTemplate*>& Recipes, const FString& FilePath, FString& Error)
{
	Error.Reset();
	if (Recipes.IsEmpty() || FilePath.IsEmpty()) { Error = TEXT("Choose recipes and an export path"); return false; }
	const bool bundle = Recipes.Num() > 1 || FilePath.EndsWith(TEXT(".mntearecipes"), ESearchCase::IgnoreCase);
	const FString extension = bundle ? TEXT(".mntearecipes") : TEXT(".mntearecipe");
	FString destination = FilePath;
	if (!destination.EndsWith(extension, ESearchCase::IgnoreCase)) destination = FPaths::ChangeExtension(destination, extension.RightChop(1));
	TMap<FString, TArray<uint8>> archives;
	TArray<uint8> output;
	for (auto* recipe : Recipes)
	{
		TArray<uint8> bytes;
		if (!BuildRecipeArchive(recipe, bytes, Error)) return false;
		const FString entry = recipe->RecipeGuid.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".mntearecipe");
		if (archives.Contains(entry)) { Error = TEXT("Export selection contains duplicate recipe GUIDs"); return false; }
		if (!bundle) output = MoveTemp(bytes);
		else archives.Add(entry, MoveTemp(bytes));
	}
	if (bundle && !FMounteaInventoryZipHelper::BuildArchive(archives, output)) { Error = TEXT("Unable to build recipe bundle"); return false; }
	// Prepare the complete export before replacing a destination file.
	const FString temporary = destination + TEXT(".") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".tmp");
	if (!FFileHelper::SaveArrayToFile(output, *temporary)) { Error = TEXT("Unable to write recipe export"); return false; }
	if (!IFileManager::Get().Move(*destination, *temporary, true, true))
	{ IFileManager::Get().Delete(*temporary); Error = TEXT("Unable to replace recipe export destination"); return false; }
	return true;
}
