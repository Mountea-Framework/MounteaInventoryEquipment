// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventorySystemStatics.generated.h"

class UMounteaAdvancedInventorySettingsConfig;
/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventorySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/*************************************************************/
	/************************ INTERNAL ***********************/
	/*************************************************************/
	static bool CanExecuteCosmeticEvents(const UWorld* WorldContext);
	
	/*************************************************************/
	/******************* BLUEPRINTABLE *******************/
	/*************************************************************/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Configuration", meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventorySettingsConfig* GetMounteaAdvancedInventoryConfig();

	/**
	 * Replaces text in a source string using a regular expression.
	 * 
	 * @param Regex The regular expression pattern to search for in the source text.
	 * @param Replacement The text to replace the matched pattern.
	 * @param SourceText The original text where the replacement will occur.
	 * @return The modified text with the replacements applied.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Text", meta=(CustomTag="MounteaK2Getter"))
	static FText ReplaceRegexInText(const FString& Regex, const FText& Replacement, const FText& SourceText);

	/**
	 * Replaces string in a source string using a regular expression.
	 * 
	 * @param Regex The regular expression pattern to search for in the source string.
	 * @param Replacement The string to replace the matched pattern.
	 * @param SourceText The original string where the replacement will occur.
	 * @return The modified string with the replacements applied.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Text", meta=(CustomTag="MounteaK2Getter"))
	static FString ReplaceRegexInString(const FString& Regex, const FString& Replacement, const FString& SourceText);
};
