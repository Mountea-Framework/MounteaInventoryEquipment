// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventorySystemStatics.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
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
