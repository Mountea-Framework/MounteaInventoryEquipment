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

	static bool CanExecuteCosmeticEvents(const UWorld* WorldContext);
	
public:

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
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Object By Class")
	static UObject* GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter, bool& bResult);
	
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
	static FString EscapeRegexSpecialChars(const FString& Input);

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

	/**
	 * Retrieves an invalid FGuid.
	 *
	 * This function provides a static method to return an invalid/uninitialized FGuid.
	 *
	 * @return An invalid FGuid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Guid",
		meta=(CustomTag="MounteaK2Getter"))
	static FGuid GetInvalidGuid() { return FGuid(); };

	/**
	 * Retrieves the owning actor of a given component.
	 *
	 * This function returns the actor that owns the specified component.
	 * If the component is valid, it retrieves the actor from the component's GetOwner method.
	 *
	 * @param Target The object for which to retrieve the owning actor.
	 * @return The actor that owns the specified component, or nullptr if the component is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Owner",
		meta=(CustomTag="MounteaK2Getter"))
	static AActor* GetOwningActor(const UObject* Target);

	/**
	 * Loads all assets of given class.
	 * 
	 * @param FilterClass Class to filter by.
	 * @return All assets of given class if any exist.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Assets", 
		meta = (DeterminesOutputType = "FilterClass"),
		meta=(CustomTag="MounteaK2Getter"))
	static TArray<UObject*> GetAssets(const TSubclassOf<UObject> FilterClass);
	
#pragma region K2NodeHelpers
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetIntPropertyValue(UObject* Target, FName PropertyName, int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly,Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetFloatPropertyValue(UObject* Target, FName PropertyName, float Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetStringPropertyValue(UObject* Target, FName PropertyName, const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetBoolPropertyValue(UObject* Target, FName PropertyName, bool Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetNamePropertyValue(UObject* Target, FName PropertyName, FName Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetBytePropertyValue(UObject* Target, FName PropertyName, uint8 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetIntPropertyValue(UObject* Target, FName PropertyName, int32& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetInt64PropertyValue(UObject* Target, FName PropertyName, int64& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetFloatPropertyValue(UObject* Target, FName PropertyName, float& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetStringPropertyValue(UObject* Target, FName PropertyName, FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetBoolPropertyValue(UObject* Target, FName PropertyName, bool& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetNamePropertyValue(UObject* Target, FName PropertyName, FName& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetBytePropertyValue(UObject* Target, FName PropertyName, uint8& Value);

#pragma endregion
	
public:
	
	template<typename TEnum>
	static constexpr bool HasFlag(const uint8 value, TEnum Flag)
	{
		static_assert(std::is_enum<TEnum>::value, "TEnum must be an enum type.");
		return (value & static_cast<uint8>(Flag)) != 0;
	}
	
	template<typename T, typename PropertyType>
	static bool SetPropertyValueInternal(UObject* Target, const FName PropertyName, const T Value)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		PropertyType* typedProperty = CastField<PropertyType>(targetProperty);
		if (!typedProperty)
			return false;

		void* propertyAddress = typedProperty->template ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		typedProperty->SetPropertyValue(propertyAddress, Value);
		return true;
	}
	
	template<typename T, typename PropertyType>
	static bool GetPropertyValueInternal(UObject* Target, const FName PropertyName, T& OutValue)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		PropertyType* typedProperty = CastField<PropertyType>(targetProperty);
		if (!typedProperty)
			return false;

		const void* propertyAddress = typedProperty->template ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		OutValue = typedProperty->GetPropertyValue(propertyAddress);
		return true;
	}
};
