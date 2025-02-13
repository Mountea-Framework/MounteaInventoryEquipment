// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventorySystemStatics.h"

#include "Kismet/KismetSystemLibrary.h"

bool UMounteaInventorySystemStatics::CanExecuteCosmeticEvents(const UWorld* WorldContext)
{
	return !UKismetSystemLibrary::IsDedicatedServer(WorldContext);
}

UMounteaAdvancedInventorySettingsConfig* UMounteaInventorySystemStatics::GetMounteaAdvancedInventoryConfig()
{
	return GetMutableDefault<UMounteaAdvancedInventorySettingsConfig>();
}

FText UMounteaInventorySystemStatics::ReplaceRegexInText(const FString& Regex, const FText& Replacement, const FText& SourceText)
{
	FString SourceString = SourceText.ToString();
	FRegexPattern RegexPattern(Regex);
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
