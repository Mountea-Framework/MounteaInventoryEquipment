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

UObject* UMounteaInventorySystemStatics::GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter, bool& bResult)
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
	return settings ? settings->AdvancedInventorySettingsConfig.LoadSynchronous() : nullptr;
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

bool UMounteaInventorySystemStatics::IsTagsContainerEmpty(const FGameplayTagContainer& Source)
{
	return Source.IsEmpty();
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

#pragma region K2NodeHelpers

bool UMounteaInventorySystemStatics::SetIntPropertyValue(UObject* Target, const FName PropertyName, const int32 Value)
{
	return Target ? SetPropertyValueInternal<int32, FIntProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetFloatPropertyValue(UObject* Target, const FName PropertyName, const float Value)
{
	return Target ? SetPropertyValueInternal<float, FFloatProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetStringPropertyValue(UObject* Target, const FName PropertyName, const FString& Value)
{
	return Target ? SetPropertyValueInternal<FString, FStrProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetBoolPropertyValue(UObject* Target, const FName PropertyName, const bool Value)
{
	return Target ? SetPropertyValueInternal<bool, FBoolProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetNamePropertyValue(UObject* Target, const FName PropertyName, const FName& Value)
{
	return Target ? SetPropertyValueInternal<FName, FNameProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetBytePropertyValue(UObject* Target, const FName PropertyName, const uint8 Value)
{
	return Target ? SetPropertyValueInternal<uint8, FByteProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetObjectPropertyValue(UObject* Target, const FName PropertyName, UObject* Value)
{
	return Target && Value ? SetObjectPropertyValueInternal(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetSoftObjectPropertyValue(UObject* Target, const FName PropertyName, const TSoftObjectPtr<UObject>& Value)
{
	return Target ? SetSoftObjectPropertyValueInternal<UObject>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetClassPropertyValue(UObject* Target, const FName PropertyName, const TSubclassOf<UObject> Value)
{
	return Target ? SetClassPropertyValueInternal<UObject>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetSoftClassPropertyValue(UObject* Target, const FName PropertyName, const TSoftClassPtr<UObject>& Value)
{
	return Target ? SetSoftClassPropertyValueInternal<UObject>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::SetGenericStructPropertyValue(UObject* Target, FName PropertyName, const int32& Value)
{
	check(0);
	return false;
}

bool UMounteaInventorySystemStatics::GetIntPropertyValue(UObject* Target, const FName PropertyName, int32& Value)
{
	return Target ? GetPropertyValueInternal<int32, FIntProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetInt64PropertyValue(UObject* Target, const FName PropertyName, int64& Value)
{
	return Target ? GetPropertyValueInternal<int64, FInt64Property>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetFloatPropertyValue(UObject* Target, const FName PropertyName, float& Value)
{
	return Target ? GetPropertyValueInternal<float, FFloatProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetStringPropertyValue(UObject* Target, const FName PropertyName, FString& Value)
{
	return Target ? GetPropertyValueInternal<FString, FStrProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetBoolPropertyValue(UObject* Target, const FName PropertyName, bool& Value)
{
	return Target ? GetPropertyValueInternal<bool, FBoolProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetNamePropertyValue(UObject* Target, const FName PropertyName, FName& Value)
{
	return Target ? GetPropertyValueInternal<FName, FNameProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetBytePropertyValue(UObject* Target, const FName PropertyName, uint8& Value)
{
	return Target ? GetPropertyValueInternal<uint8, FByteProperty>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetObjectPropertyValue(UObject* Target, const FName PropertyName, UObject*& Value)
{
	return Target && Value ? GetObjectPropertyValueInternal(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetSoftObjectPropertyValue(UObject* Target, const FName PropertyName, TSoftObjectPtr<UObject>& Value)
{
	return Target ? GetSoftObjectPropertyValueInternal<UObject>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetClassPropertyValue(UObject* Target, const FName PropertyName, TSubclassOf<UObject>& Value)
{
	return Target ? GetClassPropertyValueInternal<UObject>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetSoftClassPropertyValue(UObject* Target, const FName PropertyName, TSoftClassPtr<UObject>& Value)
{
	return Target ? GetSoftClassPropertyValueInternal<UObject>(Target, PropertyName, Value) : false;
}

bool UMounteaInventorySystemStatics::GetGenericStructPropertyValue(UObject* Target, FName PropertyName, int32& Value)
{
	check(0);
	return false;
}

DEFINE_FUNCTION(UMounteaInventorySystemStatics::execSetGenericStructPropertyValue)
{
	P_GET_OBJECT(UObject, Target);
	P_GET_PROPERTY(FNameProperty, PropertyName);

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* valuePtr = Stack.MostRecentPropertyAddress;
	FStructProperty* valueProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_FINISH;

	bool bSuccess = false;

	[&]()
	{
		if (!IsValid(Target) || !valuePtr || !valueProperty || !valueProperty->Struct)
			return;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		FStructProperty* structProperty = CastField<FStructProperty>(targetProperty);
		if (!structProperty || structProperty->Struct != valueProperty->Struct)
			return;

		void* propertyAddress = structProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return;

		structProperty->Struct->CopyScriptStruct(propertyAddress, valuePtr);
		bSuccess = true;
	}();

	*static_cast<bool*>(RESULT_PARAM) = bSuccess;
}

DEFINE_FUNCTION(UMounteaInventorySystemStatics::execGetGenericStructPropertyValue)
{
	P_GET_OBJECT(UObject, Target);
	P_GET_PROPERTY(FNameProperty, PropertyName);

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* valuePtr = Stack.MostRecentPropertyAddress;
	FStructProperty* valueProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_FINISH;

	bool bSuccess = false;

	[&]()
	{
		if (!IsValid(Target) || !valuePtr || !valueProperty || !valueProperty->Struct)
			return;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		FStructProperty* structProperty = CastField<FStructProperty>(targetProperty);
		if (!structProperty || structProperty->Struct != valueProperty->Struct)
			return;

		const void* propertyAddress = structProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return;

		structProperty->Struct->CopyScriptStruct(valuePtr, propertyAddress);
		bSuccess = true;
	}();

	*static_cast<bool*>(RESULT_PARAM) = bSuccess;
}

#pragma endregion