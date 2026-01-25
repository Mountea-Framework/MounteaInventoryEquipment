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
	static bool SetNamePropertyValue(UObject* Target, FName PropertyName, const FName& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetBytePropertyValue(UObject* Target, FName PropertyName, uint8 Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetGuidPropertyValue(UObject* Target, FName PropertyName, const FGuid& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetVectorPropertyValue(UObject* Target, FName PropertyName, const FVector& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetVector2DPropertyValue(UObject* Target, FName PropertyName, const FVector2D& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetObjectPropertyValue(UObject* Target, const FName PropertyName, UObject* Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetSoftObjectPropertyValue(UObject* Target, FName PropertyName, const TSoftObjectPtr<UObject>& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetClassPropertyValue(UObject* Target, FName PropertyName, const TSubclassOf<UObject> Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool SetSoftClassPropertyValue(UObject* Target, FName PropertyName, const TSoftClassPtr<UObject>& Value);

	
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
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetGuidPropertyValue(UObject* Target, FName PropertyName, FGuid& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetVectorPropertyValue(UObject* Target, FName PropertyName, FVector& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetVector2DPropertyValue(UObject* Target, FName PropertyName, FVector2D& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetObjectPropertyValue(UObject* Target, const FName PropertyName, UObject*& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetSoftObjectPropertyValue(UObject* Target, FName PropertyName, TSoftObjectPtr<UObject>& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetClassPropertyValue(UObject* Target, FName PropertyName, TSubclassOf<UObject>& Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Mountea|Inventory & Equipment|Payloads")
	static bool GetSoftClassPropertyValue(UObject* Target, FName PropertyName, TSoftClassPtr<UObject>& Value);

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
	
	template<typename T>
	static bool SetStructPropertyValueInternal(UObject* Target, const FName PropertyName, const T& Value)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FStructProperty* structProperty = CastField<FStructProperty>(targetProperty);
		if (!structProperty)
			return false;

		// CRITICAL: Validate struct type matches
		if (structProperty->Struct != TBaseStructure<T>::Get())
			return false;

		void* propertyAddress = structProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		*static_cast<T*>(propertyAddress) = Value;
		return true;
	}

	template<typename T>
	static bool GetStructPropertyValueInternal(UObject* Target, const FName PropertyName, T& OutValue)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FStructProperty* structProperty = CastField<FStructProperty>(targetProperty);
		if (!structProperty)
			return false;

		// CRITICAL: Validate struct type matches
		if (structProperty->Struct != TBaseStructure<T>::Get())
			return false;

		const void* propertyAddress = structProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		OutValue = *static_cast<const T*>(propertyAddress);
		return true;
	}
	
	template<typename T>
	static bool SetSoftObjectPropertyValueInternal(UObject* Target, const FName PropertyName, const TSoftObjectPtr<T>& Value)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FSoftObjectProperty* softObjectProperty = CastField<FSoftObjectProperty>(targetProperty);
		if (!softObjectProperty)
			return false;

		// Validate class compatibility
		if (Value.IsValid())
		{
			UObject* loadedObject = Value.LoadSynchronous();
			if (loadedObject && !loadedObject->IsA(softObjectProperty->PropertyClass))
				return false;
		}

		void* propertyAddress = softObjectProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		*static_cast<TSoftObjectPtr<T>*>(propertyAddress) = Value;
		return true;
	}

	template<typename T>
	static bool GetSoftObjectPropertyValueInternal(UObject* Target, const FName PropertyName, TSoftObjectPtr<T>& OutValue)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FSoftObjectProperty* softObjectProperty = CastField<FSoftObjectProperty>(targetProperty);
		if (!softObjectProperty)
			return false;

		const void* propertyAddress = softObjectProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		OutValue = *static_cast<const TSoftObjectPtr<T>*>(propertyAddress);
		return true;
	}
	
	template<typename T>
	static bool SetClassPropertyValueInternal(UObject* Target, const FName PropertyName, const TSubclassOf<T>& Value)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FClassProperty* classProperty = CastField<FClassProperty>(targetProperty);
		if (!classProperty)
			return false;

		// Validate metaclass compatibility
		if (Value && !Value->IsChildOf(classProperty->MetaClass))
			return false;

		void* propertyAddress = classProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		*static_cast<TSubclassOf<T>*>(propertyAddress) = Value;
		return true;
	}

	template<typename T>
	static bool GetClassPropertyValueInternal(UObject* Target, const FName PropertyName, TSubclassOf<T>& OutValue)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FClassProperty* classProperty = CastField<FClassProperty>(targetProperty);
		if (!classProperty)
			return false;

		const void* propertyAddress = classProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		OutValue = *static_cast<const TSubclassOf<T>*>(propertyAddress);
		return true;
	}
	
	template<typename T>
	static bool SetSoftClassPropertyValueInternal(UObject* Target, const FName PropertyName, const TSoftClassPtr<T>& Value)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FSoftClassProperty* softClassProperty = CastField<FSoftClassProperty>(targetProperty);
		if (!softClassProperty)
			return false;

		// Validate metaclass compatibility
		if (Value.IsValid())
		{
			UClass* loadedClass = Value.LoadSynchronous();
			if (loadedClass && !loadedClass->IsChildOf(softClassProperty->MetaClass))
				return false;
		}

		void* propertyAddress = softClassProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		*static_cast<TSoftClassPtr<T>*>(propertyAddress) = Value;
		return true;
	}

	template<typename T>
	static bool GetSoftClassPropertyValueInternal(UObject* Target, const FName PropertyName, TSoftClassPtr<T>& OutValue)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FSoftClassProperty* softClassProperty = CastField<FSoftClassProperty>(targetProperty);
		if (!softClassProperty)
			return false;

		const void* propertyAddress = softClassProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		OutValue = *static_cast<const TSoftClassPtr<T>*>(propertyAddress);
		return true;
	}
	
	static bool SetObjectPropertyValueInternal(UObject* Target, const FName PropertyName, UObject* Value)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FObjectProperty* objectProperty = CastField<FObjectProperty>(targetProperty);
		if (!objectProperty)
			return false;

		// CRITICAL: Validate object class compatibility
		if (Value && !Value->IsA(objectProperty->PropertyClass))
			return false;

		void* propertyAddress = objectProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		objectProperty->SetObjectPropertyValue(propertyAddress, Value);
		return true;
	}
	static bool GetObjectPropertyValueInternal(UObject* Target, const FName PropertyName, UObject*& OutValue)
	{
		if (!IsValid(Target))
			return false;

		UClass* targetClass = Target->GetClass();
		if (!targetClass)
			return false;

		FProperty* targetProperty = targetClass->FindPropertyByName(PropertyName);
		if (!targetProperty)
			return false;

		FObjectProperty* objectProperty = CastField<FObjectProperty>(targetProperty);
		if (!objectProperty)
			return false;

		const void* propertyAddress = objectProperty->ContainerPtrToValuePtr<void>(Target);
		if (!propertyAddress)
			return false;

		OutValue = objectProperty->GetObjectPropertyValue(propertyAddress);
		return true;
	}
};
