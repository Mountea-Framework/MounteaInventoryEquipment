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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "MounteaAdvancedInventoryJsonStatics.generated.h"

class UMounteaAdvancedInventoryGlobalConfig;
class UMounteaJsonObject;

struct MOUNTEAADVANCEDINVENTORYSYSTEM_API FMounteaResolvedJsonObjectDefinitionField
{
	FMounteaJsonObjectDefinitionField Field;
	FString SourceDefinitionKey;
	TArray<FString> IncludePath;
	int32 OccurrenceIndex = 1;
	int32 OccurrenceCount = 1;

	bool IsDuplicate() const
	{
		return OccurrenceCount > 1;
	}
};

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryJsonStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Creates a new JSON object.
	 *
	 * @param Target Context object used for Blueprint invocation.
	 * @return Newly created JSON object.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON",
		meta=(MounteaGetter),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON - Create Json Object"))
	static UMounteaJsonObject* CreateJsonObject(UObject* Target);

	/**
	 * Parses a JSON object from a string.
	 *
	 * @param Target Context object used for Blueprint invocation.
	 * @param JsonString JSON string to parse.
	 * @return Parsed JSON object, or nullptr when parsing fails.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON|Serialization",
		meta=(MounteaGetter),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON - Parse Json Object From String"))
	static UMounteaJsonObject* ParseJsonObjectFromString(UObject* Target, const FString& JsonString);

	/**
	 * Serializes a JSON object to a compact string.
	 *
	 * @param Target JSON object to serialize.
	 * @return Compact JSON string.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Serialization",
		meta=(MounteaGetter),
		meta=(DisplayName="JSON - Json Object To String"))
	static FString JsonObjectToString(UMounteaJsonObject* Target);

	/**
	 * Serializes a JSON object to a formatted string.
	 *
	 * @param Target JSON object to serialize.
	 * @return Pretty-printed JSON string.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Serialization",
		meta=(MounteaGetter),
		meta=(DisplayName="JSON - Json Object To Pretty String"))
	static FString JsonObjectToPrettyString(UMounteaJsonObject* Target);

	/**
	 * Checks whether a JSON object is valid.
	 *
	 * @param Target JSON object to validate.
	 * @return True if the JSON object is valid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON",
		meta=(MounteaValidate),
		meta=(DisplayName="JSON - Is Valid Json Object"))
	static bool IsValidJsonObject(UMounteaJsonObject* Target);

	/**
	 * Returns the global inventory configuration used for JSON definitions.
	 *
	 * @return Global inventory configuration asset, or nullptr when unavailable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		meta=(DisplayName="JSON - Get Global Json Config"))
	static UMounteaAdvancedInventoryGlobalConfig* GetGlobalJsonConfig();

	/**
	 * Finds a JSON object definition by key.
	 *
	 * @param DefinitionKey Definition identifier to search for.
	 * @param Definition Found definition data.
	 * @return True if the definition was found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		meta=(MounteaJsonDefinition),
		meta=(MounteaJsonDefinitionPin="DefinitionKey"),
		meta=(DisplayName="JSON - Find Json Object Definition"))
	static bool FindJsonObjectDefinition(const FString& DefinitionKey, FMounteaJsonObjectDefinition& Definition);

	static bool ResolveJsonObjectDefinitionByKey(const FString& DefinitionKey, FMounteaJsonObjectDefinition& OutDefinition, TArray<FString>& Errors);
	static bool ResolveJsonObjectDefinitionFieldsByKey(const FString& DefinitionKey, TArray<FMounteaResolvedJsonObjectDefinitionField>& OutFields, TArray<FString>& Errors);
	static FString JoinJsonDefinitionIncludePath(const TArray<FString>& IncludePath);
	static FName MakeJsonDefinitionFieldPinName(const FString& FieldPinPrefix, FName FieldName, int32 OccurrenceIndex, TSet<FName>& UsedPinNames);
	static FString BuildJsonDefinitionFieldPinTooltip(const FMounteaResolvedJsonObjectDefinitionField& FieldData);
	static FString BuildJsonDefinitionIssueText(const FString& DefinitionKey);
	static bool IsSupportedJsonDefinitionFieldPinType(const FEdGraphPinType& PinType);

	/**
	 * Creates a JSON object from the supplied definition.
	 *
	 * @param Target Context object used for Blueprint invocation.
	 * @param Definition Definition used to initialize fields.
	 * @return Created JSON object.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON - Create Json Object From Definition"))
	static UMounteaJsonObject* CreateJsonObjectFromDefinition(UObject* Target, const FMounteaJsonObjectDefinition& Definition);

	/**
	 * Creates a JSON object from a registered definition key.
	 *
	 * @param Target Context object used for Blueprint invocation.
	 * @param DefinitionKey Definition identifier to resolve.
	 * @return Created JSON object, or nullptr when the definition is unavailable.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		meta=(MounteaJsonDefinition),
		meta=(MounteaJsonDefinitionPin="DefinitionKey"),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON - Create Json Object From Definition Key"))
	static UMounteaJsonObject* CreateJsonObjectFromDefinitionKey(UObject* Target, const FString& DefinitionKey);

	/**
	 * Validates a JSON object against a supplied definition.
	 *
	 * @param Target JSON object to validate.
	 * @param Definition Definition to validate against.
	 * @param Errors Validation errors, if any.
	 * @return True if the JSON object matches the definition.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaValidate),
		meta=(DisplayName="JSON - Validate Json Object Against Definition"))
	static bool ValidateJsonObjectAgainstDefinition(UMounteaJsonObject* Target, const FMounteaJsonObjectDefinition& Definition, TArray<FString>& Errors);

	/**
	 * Validates a JSON object against a registered definition key.
	 *
	 * @param Target JSON object to validate.
	 * @param DefinitionKey Definition identifier to resolve.
	 * @param Errors Validation errors, if any.
	 * @return True if the JSON object matches the resolved definition.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaValidate),
		meta=(MounteaJsonDefinition),
		meta=(MounteaJsonDefinitionPin="DefinitionKey"),
		meta=(DisplayName="JSON - Validate Json Object Against Definition Key"))
	static bool ValidateJsonObjectAgainstDefinitionKey(UMounteaJsonObject* Target, const FString& DefinitionKey, TArray<FString>& Errors);

#pragma region K2NodeHelpers

	/**
	 * Sets an integer field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Integer value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Int"))
	static bool SetIntJsonField(UMounteaJsonObject* Target, FName FieldName, int32 Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a 64-bit integer field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Integer value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Int64"))
	static bool SetInt64JsonField(UMounteaJsonObject* Target, FName FieldName, int64 Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a floating-point field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Float value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Float"))
	static bool SetFloatJsonField(UMounteaJsonObject* Target, FName FieldName, float Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a string field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value String value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set String"))
	static bool SetStringJsonField(UMounteaJsonObject* Target, FName FieldName, const FString& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a name field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Name value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Name"))
	static bool SetNameJsonField(UMounteaJsonObject* Target, FName FieldName, const FName& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a text field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Text value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Text"))
	static bool SetTextJsonField(UMounteaJsonObject* Target, FName FieldName, const FText& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a boolean field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Boolean value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Bool"))
	static bool SetBoolJsonField(UMounteaJsonObject* Target, FName FieldName, bool Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a byte field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Byte value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Byte"))
	static bool SetByteJsonField(UMounteaJsonObject* Target, FName FieldName, uint8 Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets an object field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Object value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Object"))
	static bool SetObjectJsonField(UMounteaJsonObject* Target, FName FieldName, UObject* Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a soft object field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Soft object value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Soft Object"))
	static bool SetSoftObjectJsonField(UMounteaJsonObject* Target, FName FieldName, const TSoftObjectPtr<UObject>& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a class field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Class value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Class"))
	static bool SetClassJsonField(UMounteaJsonObject* Target, FName FieldName, TSubclassOf<UObject> Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a soft class field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Soft class value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Soft Class"))
	static bool SetSoftClassJsonField(UMounteaJsonObject* Target, FName FieldName, const TSoftClassPtr<UObject>& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a struct field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Struct value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(CustomStructureParam="Value"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Struct"))
	static bool SetStructJsonField(UMounteaJsonObject* Target, FName FieldName, const int32& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets an array field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Array value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(ArrayParm="Value"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Array"))
	static bool SetArrayJsonField(UMounteaJsonObject* Target, FName FieldName, const TArray<int32>& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Sets a set field on a JSON object.
	 *
	 * @param Target JSON object to update.
	 * @param FieldName Name of the field to set.
	 * @param Value Set value to write.
	 * @param JsonObject Updated JSON object reference.
	 * @return True if the field was set successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(SetParam="Value"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Set Set"))
	static bool SetSetJsonField(UMounteaJsonObject* Target, FName FieldName, const TSet<int32>& Value, UMounteaJsonObject*& JsonObject);

	/**
	 * Gets an integer field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved integer value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Int"))
	static bool GetIntJsonField(UMounteaJsonObject* Target, FName FieldName, int32& Value);

	/**
	 * Gets a 64-bit integer field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved integer value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Int64"))
	static bool GetInt64JsonField(UMounteaJsonObject* Target, FName FieldName, int64& Value);

	/**
	 * Gets a floating-point field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved float value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Float"))
	static bool GetFloatJsonField(UMounteaJsonObject* Target, FName FieldName, float& Value);

	/**
	 * Gets a string field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved string value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get String"))
	static bool GetStringJsonField(UMounteaJsonObject* Target, FName FieldName, FString& Value);

	/**
	 * Gets a name field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved name value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Name"))
	static bool GetNameJsonField(UMounteaJsonObject* Target, FName FieldName, FName& Value);

	/**
	 * Gets a text field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved text value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Text"))
	static bool GetTextJsonField(UMounteaJsonObject* Target, FName FieldName, FText& Value);

	/**
	 * Gets a boolean field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved boolean value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Bool"))
	static bool GetBoolJsonField(UMounteaJsonObject* Target, FName FieldName, bool& Value);

	/**
	 * Gets a byte field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved byte value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Byte"))
	static bool GetByteJsonField(UMounteaJsonObject* Target, FName FieldName, uint8& Value);

	/**
	 * Gets an object field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved object value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Object"))
	static bool GetObjectJsonField(UMounteaJsonObject* Target, FName FieldName, UObject*& Value);

	/**
	 * Gets a soft object field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved soft object value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Soft Object"))
	static bool GetSoftObjectJsonField(UMounteaJsonObject* Target, FName FieldName, TSoftObjectPtr<UObject>& Value);

	/**
	 * Gets a class field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved class value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Class"))
	static bool GetClassJsonField(UMounteaJsonObject* Target, FName FieldName, TSubclassOf<UObject>& Value);

	/**
	 * Gets a soft class field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved soft class value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Soft Class"))
	static bool GetSoftClassJsonField(UMounteaJsonObject* Target, FName FieldName, TSoftClassPtr<UObject>& Value);

	/**
	 * Gets a struct field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved struct value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(CustomStructureParam="Value"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Struct"))
	static bool GetStructJsonField(UMounteaJsonObject* Target, FName FieldName, int32& Value);

	/**
	 * Gets an array field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved array value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(ArrayParm="Value"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Array"))
	static bool GetArrayJsonField(UMounteaJsonObject* Target, FName FieldName, TArray<int32>& Value);

	/**
	 * Gets a set field from a JSON object.
	 *
	 * @param Target JSON object to read.
	 * @param FieldName Name of the field to read.
	 * @param Value Retrieved set value.
	 * @return True if the field was read successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(SetParam="Value"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="JSON Field - Get Set"))
	static bool GetSetJsonField(UMounteaJsonObject* Target, FName FieldName, TSet<int32>& Value);

	DECLARE_FUNCTION(execSetStructJsonField);
	DECLARE_FUNCTION(execSetArrayJsonField);
	DECLARE_FUNCTION(execSetSetJsonField);
	DECLARE_FUNCTION(execGetStructJsonField);
	DECLARE_FUNCTION(execGetArrayJsonField);
	DECLARE_FUNCTION(execGetSetJsonField);

#pragma endregion
};
