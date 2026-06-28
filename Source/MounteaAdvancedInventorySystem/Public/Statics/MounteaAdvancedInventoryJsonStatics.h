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

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON",
		meta=(MounteaGetter),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		DisplayName="Create Json Object")
	static UMounteaJsonObject* CreateJsonObject(UObject* Target);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON|Serialization",
		meta=(MounteaGetter),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		DisplayName="Parse Json Object From String")
	static UMounteaJsonObject* ParseJsonObjectFromString(UObject* Target, const FString& JsonString);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Serialization",
		meta=(MounteaGetter),
		DisplayName="Json Object To String")
	static FString JsonObjectToString(UMounteaJsonObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Serialization",
		meta=(MounteaGetter),
		DisplayName="Json Object To Pretty String")
	static FString JsonObjectToPrettyString(UMounteaJsonObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON",
		meta=(MounteaValidate),
		DisplayName="Is Valid Json Object")
	static bool IsValidJsonObject(UMounteaJsonObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		DisplayName="Get Global Json Config")
	static UMounteaAdvancedInventoryGlobalConfig* GetGlobalJsonConfig();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		meta=(MounteaJsonDefinition),
		meta=(MounteaJsonDefinitionPin="DefinitionKey"),
		DisplayName="Find Json Object Definition")
	static bool FindJsonObjectDefinition(const FString& DefinitionKey, FMounteaJsonObjectDefinition& Definition);

	static bool ResolveJsonObjectDefinitionByKey(const FString& DefinitionKey, FMounteaJsonObjectDefinition& OutDefinition, TArray<FString>& Errors);
	static bool ResolveJsonObjectDefinitionFieldsByKey(const FString& DefinitionKey, TArray<FMounteaResolvedJsonObjectDefinitionField>& OutFields, TArray<FString>& Errors);
	static FString JoinJsonDefinitionIncludePath(const TArray<FString>& IncludePath);
	static FName MakeJsonDefinitionFieldPinName(const FString& FieldPinPrefix, FName FieldName, int32 OccurrenceIndex, TSet<FName>& UsedPinNames);
	static FString BuildJsonDefinitionFieldPinTooltip(const FMounteaResolvedJsonObjectDefinitionField& FieldData);
	static FString BuildJsonDefinitionIssueText(const FString& DefinitionKey);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		DisplayName="Create Json Object From Definition")
	static UMounteaJsonObject* CreateJsonObjectFromDefinition(UObject* Target, const FMounteaJsonObjectDefinition& Definition);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaGetter),
		meta=(MounteaJsonDefinition),
		meta=(MounteaJsonDefinitionPin="DefinitionKey"),
		meta=(CallableWithoutWorldContext),
		meta=(DefaultToSelf="Target"),
		DisplayName="Create Json Object From Definition Key")
	static UMounteaJsonObject* CreateJsonObjectFromDefinitionKey(UObject* Target, const FString& DefinitionKey);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaValidate),
		DisplayName="Validate Json Object Against Definition")
	static bool ValidateJsonObjectAgainstDefinition(UMounteaJsonObject* Target, const FMounteaJsonObjectDefinition& Definition, TArray<FString>& Errors);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|JSON|Definitions",
		meta=(MounteaValidate),
		meta=(MounteaJsonDefinition),
		meta=(MounteaJsonDefinitionPin="DefinitionKey"),
		DisplayName="Validate Json Object Against Definition Key")
	static bool ValidateJsonObjectAgainstDefinitionKey(UMounteaJsonObject* Target, const FString& DefinitionKey, TArray<FString>& Errors);

#pragma region K2NodeHelpers

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetIntJsonField(UMounteaJsonObject* Target, FName FieldName, int32 Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetInt64JsonField(UMounteaJsonObject* Target, FName FieldName, int64 Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetFloatJsonField(UMounteaJsonObject* Target, FName FieldName, float Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetStringJsonField(UMounteaJsonObject* Target, FName FieldName, const FString& Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetNameJsonField(UMounteaJsonObject* Target, FName FieldName, const FName& Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetTextJsonField(UMounteaJsonObject* Target, FName FieldName, const FText& Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetBoolJsonField(UMounteaJsonObject* Target, FName FieldName, bool Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetByteJsonField(UMounteaJsonObject* Target, FName FieldName, uint8 Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetObjectJsonField(UMounteaJsonObject* Target, FName FieldName, UObject* Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetSoftObjectJsonField(UMounteaJsonObject* Target, FName FieldName, const TSoftObjectPtr<UObject>& Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetClassJsonField(UMounteaJsonObject* Target, FName FieldName, TSubclassOf<UObject> Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool SetSoftClassJsonField(UMounteaJsonObject* Target, FName FieldName, const TSoftClassPtr<UObject>& Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(CustomStructureParam="Value"),
		meta=(DefaultToSelf="Target"))
	static bool SetStructJsonField(UMounteaJsonObject* Target, FName FieldName, const int32& Value, UMounteaJsonObject*& JsonObject);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetIntJsonField(UMounteaJsonObject* Target, FName FieldName, int32& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetInt64JsonField(UMounteaJsonObject* Target, FName FieldName, int64& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetFloatJsonField(UMounteaJsonObject* Target, FName FieldName, float& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetStringJsonField(UMounteaJsonObject* Target, FName FieldName, FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetNameJsonField(UMounteaJsonObject* Target, FName FieldName, FName& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetTextJsonField(UMounteaJsonObject* Target, FName FieldName, FText& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetBoolJsonField(UMounteaJsonObject* Target, FName FieldName, bool& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetByteJsonField(UMounteaJsonObject* Target, FName FieldName, uint8& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetObjectJsonField(UMounteaJsonObject* Target, FName FieldName, UObject*& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetSoftObjectJsonField(UMounteaJsonObject* Target, FName FieldName, TSoftObjectPtr<UObject>& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetClassJsonField(UMounteaJsonObject* Target, FName FieldName, TSubclassOf<UObject>& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields",
		meta=(DefaultToSelf="Target"))
	static bool GetSoftClassJsonField(UMounteaJsonObject* Target, FName FieldName, TSoftClassPtr<UObject>& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="Mountea|Inventory & Equipment|JSON|Fields", CustomThunk,
		meta=(CustomStructureParam="Value"),
		meta=(DefaultToSelf="Target"))
	static bool GetStructJsonField(UMounteaJsonObject* Target, FName FieldName, int32& Value);

	DECLARE_FUNCTION(execSetStructJsonField);
	DECLARE_FUNCTION(execGetStructJsonField);

#pragma endregion
};
