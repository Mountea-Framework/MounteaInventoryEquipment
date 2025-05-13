// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventorySystemStatics.h"

#include "Kismet/KismetSystemLibrary.h"
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
	const FString SourceString = SourceText.ToString();
	const FRegexPattern RegexPattern(Regex);
	FRegexMatcher RegexMatcher(RegexPattern, SourceString);

	if (!RegexMatcher.FindNext()) 
	{
		return SourceText;
	}

	FString FormattedString;
	int32 PreviousPosition = 0;
	FString ReplacementText = Replacement.ToString();

	do
	{
		FormattedString += SourceString.Mid(PreviousPosition, RegexMatcher.GetMatchBeginning() - PreviousPosition);
		FormattedString += ReplacementText;
		PreviousPosition = RegexMatcher.GetMatchEnding();
	} 
	while (RegexMatcher.FindNext());

	FormattedString += SourceString.Mid(PreviousPosition);

	return FText::FromString(FormattedString);
}

FString UMounteaInventorySystemStatics::ReplaceRegexInString(const FString& Regex, const FString& Replacement, const FString& SourceText)
{
	return ReplaceRegexInText(Regex, FText::FromString(Replacement), FText::FromString(SourceText)).ToString();
}

FGameplayTag UMounteaInventorySystemStatics::GetGameplayTag(const FGameplayTagContainer& Source, const int TagIndex)
{
	return Source.IsValidIndex(TagIndex) ? Source.GetByIndex(TagIndex) : Source.First(); 
}
