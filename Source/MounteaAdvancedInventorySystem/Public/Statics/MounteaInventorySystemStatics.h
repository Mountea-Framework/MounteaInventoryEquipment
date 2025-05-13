// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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

	/**
	 * Retrieves the given object if it is of the specified class type.
	 *
	 * This function checks whether the provided `Object` is of the class type specified by `ClassFilter`.
	 * If the `Object` is of the specified class or a subclass thereof, it returns the `Object` of the specified class and sets `bResult` to `true`.
	 * Otherwise, it returns `nullptr` and sets `bResult` to `false`.
	 *
	 * @param Object		The object to check and potentially retrieve.
	 * @param ClassFilter	The class type to check against. Only objects of this class or subclasses will be returned.
	 * @param bResult		(Out) `true` if the object is of the specified class type; `false` otherwise.
	 * @return					The casted `Object` if it is of the specified class type; otherwise, `nullptr`.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Helpers",
		meta = (ClassFilter = "Object"),
		meta=(DeterminesOutputType = "ClassFilter"),
		meta=(CustomTag="MounteaK2Getter"))
	static UObject* GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter, bool& bResult);
	
	/*************************************************************/
	/******************* BLUEPRINTABLE *******************/
	/*************************************************************/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|Configuration",
		meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventorySettingsConfig* GetMounteaAdvancedInventoryConfig();

	/**
	 * Replaces text in a source string using a regular expression.
	 * 
	 * @param Regex The regular expression pattern to search for in the source text.
	 * @param Replacement The text to replace the matched pattern.
	 * @param SourceText The original text where the replacement will occur.
	 * @return The modified text with the replacements applied.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Text",
		meta=(CustomTag="MounteaK2Getter"))
	static FText ReplaceRegexInText(const FString& Regex, const FText& Replacement, const FText& SourceText);

	/**
	 * Replaces string in a source string using a regular expression.
	 * 
	 * @param Regex The regular expression pattern to search for in the source string.
	 * @param Replacement The string to replace the matched pattern.
	 * @param SourceText The original string where the replacement will occur.
	 * @return The modified string with the replacements applied.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Text",
		meta=(CustomTag="MounteaK2Getter"))
	static FString ReplaceRegexInString(const FString& Regex, const FString& Replacement, const FString& SourceText);

	/**
	 * 
	 * @param Source
	 * @param TagIndex 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|GameplayTags",
		meta=(CustomTag="MounteaK2Getter"))
	static FGameplayTag GetGameplayTag(const FGameplayTagContainer& Source, const int TagIndex = 0);
	
	template<typename TEnum>
	static constexpr bool HasFlag(uint8 value, TEnum flag)
	{
		static_assert(std::is_enum<TEnum>::value, "TEnum must be an enum type.");
		return (value & static_cast<uint8>(flag)) != 0;
	}
};
