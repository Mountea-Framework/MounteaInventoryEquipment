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


#include "Statics/MounteaInventorySystemStatics.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

bool UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(const UWorld* WorldContext)
{
	return !UKismetSystemLibrary::IsDedicatedServer(WorldContext);
}

UObject* UMounteaInventorySystemStatics::GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter,
	bool& bResult)
{
	bResult = false;

	if (ClassFilter == nullptr) return nullptr;
	if (Object == nullptr) return nullptr;
	if (!Object->IsA(ClassFilter)) return nullptr;

	bResult = true;
	return Object;
}

UMounteaAdvancedInventorySettingsConfig* UMounteaInventorySystemStatics::GetMounteaAdvancedInventoryConfig()
{
	auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	return settings ? settings->InventorySettingsConfig.LoadSynchronous() : nullptr;
}

FText UMounteaInventorySystemStatics::ReplaceRegexInText(const FString& Regex, const FText& Replacement, const FText& SourceText)
{
	const FString sourceString = SourceText.ToString();

	const FString escapedRegex = EscapeRegexSpecialChars(Regex);

	const FRegexPattern regexPattern(escapedRegex);
	FRegexMatcher regexMatcher(regexPattern, sourceString);

	if (!regexMatcher.FindNext()) 
		return SourceText;

	FString formattedString;
	int32 previousPosition = 0;
	FString replacementText = Replacement.ToString();

	formattedString += sourceString.Mid(previousPosition, regexMatcher.GetMatchBeginning() - previousPosition);
	formattedString += replacementText;
	previousPosition = regexMatcher.GetMatchEnding();
	formattedString += sourceString.Mid(previousPosition);

	return FText::FromString(formattedString);
}

FString UMounteaInventorySystemStatics::EscapeRegexSpecialChars(const FString& Input)
{
	FString escapedString = Input;

	// Escape special characters in regex
	// These are the regex special characters: . ^ $ * + ? ( ) [ ] { } | \ /

	const TArray<TPair<FString, FString>> escapePairs = {
		{TEXT("\\"), TEXT("\\\\")},
		{TEXT("$"), TEXT("\\$")},
		{TEXT("{"), TEXT("\\{")},
		{TEXT("}"), TEXT("\\}")},
		{TEXT("["), TEXT("\\[")},
		{TEXT("]"), TEXT("\\]")},
		{TEXT("("), TEXT("\\(")},
		{TEXT(")"), TEXT("\\)")},
		{TEXT("."), TEXT("\\.")},
		{TEXT("^"), TEXT("\\^")},
		{TEXT("|"), TEXT("\\|")},
		{TEXT("?"), TEXT("\\?")},
		{TEXT("+"), TEXT("\\+")},
		{TEXT("*"), TEXT("\\*")},
		{TEXT("/"), TEXT("\\/")}
	};
	
	for (const auto& Pair : escapePairs)
	{
		escapedString = escapedString.Replace(*Pair.Key, *Pair.Value);
	}

	return escapedString;
}


FString UMounteaInventorySystemStatics::ReplaceRegexInString(const FString& Regex, const FString& Replacement, const FString& SourceText)
{
	return ReplaceRegexInText(Regex, FText::FromString(Replacement), FText::FromString(SourceText)).ToString();
}

FGameplayTag UMounteaInventorySystemStatics::GetGameplayTag(const FGameplayTagContainer& Source, const int TagIndex)
{
	return Source.IsValidIndex(TagIndex) ? Source.GetByIndex(TagIndex) : Source.First(); 
}

AActor* UMounteaInventorySystemStatics::GetOwningActor(const UObject* Target)
{
	if (!IsValid(Target))
		return nullptr;

	auto resolveOwner = [&](AActor* InActor) {
		AActor* actorToReturn = InActor;
		if (!IsValid(actorToReturn))
			actorToReturn = Cast<AActor>(Target->GetOuter());
		if (!IsValid(actorToReturn))
			actorToReturn = Target->GetTypedOuter<AActor>();
#if WITH_EDITOR
		if (!IsValid(actorToReturn))
			if (const UBlueprintGeneratedClass* bpClass = Cast<UBlueprintGeneratedClass>(Target->GetOuter()))
				actorToReturn = Cast<AActor>(bpClass->GetDefaultObject());
#endif
		return actorToReturn;
	};

	if (const UActorComponent* component = Cast<UActorComponent>(Target))
		return resolveOwner(component->GetOwner());

	if (const AActor* actor = Cast<AActor>(Target))
		return resolveOwner(actor->GetOwner());

	return resolveOwner(nullptr);
}

TArray<UObject*> UMounteaInventorySystemStatics::GetAssets(const TSubclassOf<UObject> FilterClass)
{
	TArray<UObject*> returnValue;

	if (!FilterClass)
	{
		LOG_WARNING(TEXT("GetAssets: FilterClass is null"))
		return returnValue;
	}

	FAssetRegistryModule& assetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& assetRegistry = assetRegistryModule.Get();

	FARFilter searchFilter;
	searchFilter.bRecursiveClasses = true;
	searchFilter.bRecursivePaths   = true;

	searchFilter.ClassPaths.Add(FilterClass->GetClassPathName());
	
	if (searchFilter.IsEmpty())
		LOG_ERROR(TEXT("GetAssets: searchFilter is empty"));

	TArray<FAssetData> assetDataList;
	assetRegistry.GetAssets(searchFilter, assetDataList);
	
	if (assetDataList.IsEmpty())
		LOG_ERROR(TEXT("GetAssets: assetDataList is empty"));

	for (const FAssetData& assetData : assetDataList)
	{
		if (UObject* loadedAsset = assetData.GetAsset())
			returnValue.Add(loadedAsset);
	}

	return returnValue;
}

bool UMounteaInventorySystemStatics::SetIntPropertyValue(UObject* Target, const FName PropertyName, const int32 Value)
{
	return SetPropertyValueInternal<int32, FIntProperty>(Target, PropertyName, Value);
}

bool UMounteaInventorySystemStatics::SetFloatPropertyValue(UObject* Target, const FName PropertyName, const float Value)
{
	return SetPropertyValueInternal<float, FFloatProperty>(Target, PropertyName, Value);
}

bool UMounteaInventorySystemStatics::SetStringPropertyValue(UObject* Target, const FName PropertyName, const FString& Value)
{
	return SetPropertyValueInternal<FString, FStrProperty>(Target, PropertyName, Value);
}

bool UMounteaInventorySystemStatics::SetBoolPropertyValue(UObject* Target, const FName PropertyName, const bool Value)
{
	return SetPropertyValueInternal<bool, FBoolProperty>(Target, PropertyName, Value);
}

bool UMounteaInventorySystemStatics::SetNamePropertyValue(UObject* Target, const FName PropertyName, const FName Value)
{
	return SetPropertyValueInternal<FName, FNameProperty>(Target, PropertyName, Value);
}

bool UMounteaInventorySystemStatics::SetBytePropertyValue(UObject* Target, const FName PropertyName, const uint8 Value)
{
	return SetPropertyValueInternal<uint8, FByteProperty>(Target, PropertyName, Value);
}