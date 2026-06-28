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

#include "Statics/MounteaAdvancedInventoryJsonStatics.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Helpers/MounteaJsonObject.h"
#include "JsonObjectConverter.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "UObject/StructOnScope.h"

namespace MounteaJsonDefinitionPinTypes
{
	static const FName Boolean(TEXT("bool"));
	static const FName Byte(TEXT("byte"));
	static const FName Class(TEXT("class"));
	static const FName Int(TEXT("int"));
	static const FName Int64(TEXT("int64"));
	static const FName Name(TEXT("name"));
	static const FName Object(TEXT("object"));
	static const FName Real(TEXT("real"));
	static const FName SoftClass(TEXT("softclass"));
	static const FName SoftObject(TEXT("softobject"));
	static const FName String(TEXT("string"));
	static const FName Struct(TEXT("struct"));
	static const FName Text(TEXT("text"));
}

static UObject* ResolveJsonObjectOuter(UObject* Target)
{
	return IsValid(Target) ? Target : GetTransientPackage();
}

static bool SetPathStringField(UMounteaJsonObject* Target, const FName FieldName, const FString& Path, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return UMounteaAdvancedInventoryJsonStatics::IsValidJsonObject(Target) ? Target->SetStringField(FieldName, Path, JsonObject) : false;
}

static bool GetPathStringField(UMounteaJsonObject* Target, const FName FieldName, FString& Path)
{
	Path.Reset();
	return UMounteaAdvancedInventoryJsonStatics::IsValidJsonObject(Target) && Target->GetStringField(FieldName, Path) && !Path.IsEmpty();
}


static UClass* GetDefinitionFieldClass(const FEdGraphPinType& PinType)
{
	return Cast<UClass>(PinType.PinSubCategoryObject.Get());
}

static UScriptStruct* GetDefinitionFieldStruct(const FEdGraphPinType& PinType)
{
	return Cast<UScriptStruct>(PinType.PinSubCategoryObject.Get());
}

static bool IsJsonObjectDefinitionField(const FEdGraphPinType& PinType)
{
	const UClass* fieldClass = GetDefinitionFieldClass(PinType);
	return PinType.PinCategory == MounteaJsonDefinitionPinTypes::Object && fieldClass && fieldClass->IsChildOf(UMounteaJsonObject::StaticClass());
}

static EJson GetJsonTypeForDefinitionField(const FMounteaJsonObjectDefinitionField& Field)
{
	const FEdGraphPinType pinType = Field.FieldValueType;
	if (pinType.ContainerType != EPinContainerType::None)
		return EJson::None;

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Int ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Int64 ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Real ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Byte)
	{
		return EJson::Number;
	}

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Boolean)
		return EJson::Boolean;

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::String ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Name ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Text ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::SoftObject ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Class ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::SoftClass ||
		(pinType.PinCategory == MounteaJsonDefinitionPinTypes::Object && !IsJsonObjectDefinitionField(pinType)))
	{
		return EJson::String;
	}

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Struct || IsJsonObjectDefinitionField(pinType))
		return EJson::Object;

	return EJson::None;
}

static FString GetDefinitionFieldTypeDisplayName(const FMounteaJsonObjectDefinitionField& Field)
{
	const FEdGraphPinType pinType = Field.FieldValueType;
	FString returnValue = pinType.PinCategory.ToString();

	if (const UObject* subCategoryObject = pinType.PinSubCategoryObject.Get())
		returnValue += FString::Printf(TEXT(" (%s)"), *subCategoryObject->GetName());

	return returnValue;
}

static FString GetJsonTypeDisplayName(const EJson JsonType)
{
	switch (JsonType)
	{
		case EJson::None:		return TEXT("None");
		case EJson::Null:		return TEXT("Null");
		case EJson::String:		return TEXT("String");
		case EJson::Number:		return TEXT("Number");
		case EJson::Boolean:		return TEXT("Boolean");
		case EJson::Array:		return TEXT("Array");
		case EJson::Object:		return TEXT("Object");
		default:				return TEXT("Unknown");
	}
}

static void SetDefinitionFieldDefaultValue(UMounteaJsonObject* Target, const FMounteaJsonObjectDefinitionField& Field)
{
	if (!UMounteaAdvancedInventoryJsonStatics::IsValidJsonObject(Target) || Field.FieldName.IsNone())
		return;

	const FEdGraphPinType pinType = Field.FieldValueType;
	const FString fieldName = Field.FieldName.ToString();
	const TSharedPtr<FJsonObject> jsonObject = Target->GetSharedJsonObject();
	if (!jsonObject.IsValid())
		return;

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Int ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Int64 ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Real ||
		pinType.PinCategory == MounteaJsonDefinitionPinTypes::Byte)
	{
		jsonObject->SetNumberField(fieldName, 0.0);
		return;
	}

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Boolean)
	{
		jsonObject->SetBoolField(fieldName, false);
		return;
	}

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Struct || IsJsonObjectDefinitionField(pinType))
	{
		jsonObject->SetObjectField(fieldName, MakeShared<FJsonObject>());
		return;
	}

	jsonObject->SetStringField(fieldName, FString());
}

static bool IsDefinitionFieldRequiredAndEmptyString(const FMounteaJsonObjectDefinitionField& Field, const FString& FieldValue, TArray<FString>& Errors)
{
	if (!Field.bRequired || !FieldValue.IsEmpty())
		return false;

	Errors.Add(FString::Printf(TEXT("Required JSON field '%s' cannot be empty for definition type %s."), *Field.FieldName.ToString(), *GetDefinitionFieldTypeDisplayName(Field)));
	return true;
}

static bool ValidateDefinitionStructField(const FMounteaJsonObjectDefinitionField& Field, const TSharedPtr<FJsonValue>& FieldValue, TArray<FString>& Errors)
{
	const UScriptStruct* structType = GetDefinitionFieldStruct(Field.FieldValueType);
	if (!structType)
		return true;

	const TSharedPtr<FJsonObject> structObject = FieldValue.IsValid() ? FieldValue->AsObject() : nullptr;
	if (!structObject.IsValid())
		return true;

	FStructOnScope structOnScope(structType);
	FText failReason;
	if (FJsonObjectConverter::JsonObjectToUStruct(structObject.ToSharedRef(), structType, structOnScope.GetStructMemory(), 0, 0, false, &failReason))
		return true;

	const FString reason = failReason.IsEmpty() ? TEXT("conversion failed") : failReason.ToString();
	Errors.Add(FString::Printf(TEXT("JSON field '%s' could not be converted to struct '%s': %s."), *Field.FieldName.ToString(), *structType->GetName(), *reason));
	return false;
}

static bool ValidateDefinitionObjectField(const FMounteaJsonObjectDefinitionField& Field, const FString& Path, TArray<FString>& Errors)
{
	if (IsDefinitionFieldRequiredAndEmptyString(Field, Path, Errors))
		return false;

	const UClass* baseClass = GetDefinitionFieldClass(Field.FieldValueType);
	if (!baseClass || Path.IsEmpty())
		return true;

	UObject* loadedObject = StaticLoadObject(UObject::StaticClass(), nullptr, *Path);
	if (!IsValid(loadedObject))
	{
		Errors.Add(FString::Printf(TEXT("JSON field '%s' could not load object path '%s' for base class validation."), *Field.FieldName.ToString(), *Path));
		return false;
	}

	if (loadedObject->IsA(baseClass))
		return true;

	Errors.Add(FString::Printf(TEXT("JSON field '%s' expected object path of type '%s', but loaded '%s'."), *Field.FieldName.ToString(), *baseClass->GetName(), *loadedObject->GetClass()->GetName()));
	return false;
}

static bool ValidateDefinitionClassField(const FMounteaJsonObjectDefinitionField& Field, const FString& Path, TArray<FString>& Errors)
{
	if (IsDefinitionFieldRequiredAndEmptyString(Field, Path, Errors))
		return false;

	const UClass* baseClass = GetDefinitionFieldClass(Field.FieldValueType);
	if (!baseClass || Path.IsEmpty())
		return true;

	UClass* loadedClass = LoadObject<UClass>(nullptr, *Path);
	if (!loadedClass)
	{
		Errors.Add(FString::Printf(TEXT("JSON field '%s' could not load class path '%s' for base class validation."), *Field.FieldName.ToString(), *Path));
		return false;
	}

	if (loadedClass->IsChildOf(baseClass))
		return true;

	Errors.Add(FString::Printf(TEXT("JSON field '%s' expected class path derived from '%s', but loaded '%s'."), *Field.FieldName.ToString(), *baseClass->GetName(), *loadedClass->GetName()));
	return false;
}

static void ValidateDefinitionFieldMetadata(const FMounteaJsonObjectDefinitionField& Field, const TSharedPtr<FJsonValue>& FieldValue, TArray<FString>& Errors)
{
	if (!FieldValue.IsValid())
		return;

	const FEdGraphPinType pinType = Field.FieldValueType;
	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Struct)
	{
		ValidateDefinitionStructField(Field, FieldValue, Errors);
		return;
	}

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Object && !IsJsonObjectDefinitionField(pinType))
	{
		ValidateDefinitionObjectField(Field, FieldValue->AsString(), Errors);
		return;
	}

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::SoftObject)
	{
		ValidateDefinitionObjectField(Field, FieldValue->AsString(), Errors);
		return;
	}

	if (pinType.PinCategory == MounteaJsonDefinitionPinTypes::Class || pinType.PinCategory == MounteaJsonDefinitionPinTypes::SoftClass)
		ValidateDefinitionClassField(Field, FieldValue->AsString(), Errors);
}

static bool AppendDefinitionField(const FMounteaJsonObjectDefinitionField& Field, FMounteaJsonObjectDefinition& OutDefinition, TSet<FName>& FieldNames, TArray<FString>& Errors)
{
	if (Field.FieldName.IsNone())
	{
		Errors.Add(TEXT("Definition contains a field with an empty name."));
		return false;
	}

	if (FieldNames.Contains(Field.FieldName))
	{
		Errors.Add(FString::Printf(TEXT("JSON definition contains duplicate field '%s' after resolving included definitions."), *Field.FieldName.ToString()));
		return false;
	}

	FieldNames.Add(Field.FieldName);
	OutDefinition.Fields.Add(Field);
	return true;
}

static bool ResolveJsonObjectDefinitionInternal(
	const UMounteaAdvancedInventoryGlobalConfig* GlobalConfig,
	const FString& DefinitionKey,
	const FMounteaJsonObjectDefinition& Definition,
	FMounteaJsonObjectDefinition& OutDefinition,
	TSet<FName>& FieldNames,
	TArray<FString>& Errors,
	TSet<FString>& VisitingKeys,
	TSet<FString>& VisitedKeys)
{
	if (!DefinitionKey.IsEmpty())
	{
		if (VisitingKeys.Contains(DefinitionKey))
		{
			Errors.Add(FString::Printf(TEXT("Circular JSON definition include detected at '%s'."), *DefinitionKey));
			return false;
		}

		if (VisitedKeys.Contains(DefinitionKey))
			return true;

		VisitingKeys.Add(DefinitionKey);
	}

	bool bReturnValue = true;
	for (const FMounteaJsonObjectDefinitionInclude& include : Definition.IncludedDefinitions)
	{
		if (!include.IsValid())
		{
			Errors.Add(TEXT("JSON definition contains an empty included definition key."));
			bReturnValue = false;
			continue;
		}

		if (!IsValid(GlobalConfig))
		{
			Errors.Add(FString::Printf(TEXT("Cannot resolve included JSON definition '%s' without a Global Json Config."), *include.DefinitionKey));
			bReturnValue = false;
			continue;
		}

		const FMounteaJsonObjectDefinition* includedDefinition = GlobalConfig->JsonObjectDefinitions.Find(include.DefinitionKey);
		if (!includedDefinition)
		{
			Errors.Add(FString::Printf(TEXT("Could not find included JSON definition '%s'."), *include.DefinitionKey));
			bReturnValue = false;
			continue;
		}

		if (!ResolveJsonObjectDefinitionInternal(GlobalConfig, include.DefinitionKey, *includedDefinition, OutDefinition, FieldNames, Errors, VisitingKeys, VisitedKeys))
			bReturnValue = false;
	}

	for (const FMounteaJsonObjectDefinitionField& field : Definition.Fields)
	{
		if (!AppendDefinitionField(field, OutDefinition, FieldNames, Errors))
			bReturnValue = false;
	}

	if (!DefinitionKey.IsEmpty())
	{
		VisitingKeys.Remove(DefinitionKey);
		VisitedKeys.Add(DefinitionKey);
	}

	return bReturnValue;
}

static bool ResolveJsonObjectDefinitionFieldsInternal(
	const UMounteaAdvancedInventoryGlobalConfig* GlobalConfig,
	const FString& DefinitionKey,
	const FMounteaJsonObjectDefinition& Definition,
	const TArray<FString>& ParentPath,
	TArray<FMounteaResolvedJsonObjectDefinitionField>& OutFields,
	TArray<FString>& Errors,
	TSet<FString>& VisitingKeys,
	TSet<FString>& VisitedKeys)
{
	if (!DefinitionKey.IsEmpty())
	{
		if (VisitingKeys.Contains(DefinitionKey))
		{
			Errors.Add(FString::Printf(TEXT("Circular JSON definition include detected at '%s'."), *DefinitionKey));
			return false;
		}

		if (VisitedKeys.Contains(DefinitionKey))
			return true;

		VisitingKeys.Add(DefinitionKey);
	}

	TArray<FString> definitionPath = ParentPath;
	if (!DefinitionKey.IsEmpty())
		definitionPath.Add(DefinitionKey);

	bool bReturnValue = true;
	for (const FMounteaJsonObjectDefinitionInclude& include : Definition.IncludedDefinitions)
	{
		if (!include.IsValid())
		{
			Errors.Add(TEXT("JSON definition contains an empty included definition key."));
			bReturnValue = false;
			continue;
		}

		if (!IsValid(GlobalConfig))
		{
			Errors.Add(FString::Printf(TEXT("Cannot resolve included JSON definition '%s' without a Global Json Config."), *include.DefinitionKey));
			bReturnValue = false;
			continue;
		}

		const FMounteaJsonObjectDefinition* includedDefinition = GlobalConfig->JsonObjectDefinitions.Find(include.DefinitionKey);
		if (!includedDefinition)
		{
			Errors.Add(FString::Printf(TEXT("Could not find included JSON definition '%s'."), *include.DefinitionKey));
			bReturnValue = false;
			continue;
		}

		if (!ResolveJsonObjectDefinitionFieldsInternal(GlobalConfig, include.DefinitionKey, *includedDefinition, definitionPath, OutFields, Errors, VisitingKeys, VisitedKeys))
			bReturnValue = false;
	}

	for (const FMounteaJsonObjectDefinitionField& field : Definition.Fields)
	{
		if (field.FieldName.IsNone())
		{
			Errors.Add(TEXT("Definition contains a field with an empty name."));
			bReturnValue = false;
			continue;
		}

		FMounteaResolvedJsonObjectDefinitionField& resolvedField = OutFields.AddDefaulted_GetRef();
		resolvedField.Field = field;
		resolvedField.SourceDefinitionKey = DefinitionKey;
		resolvedField.IncludePath = definitionPath;
	}

	if (!DefinitionKey.IsEmpty())
	{
		VisitingKeys.Remove(DefinitionKey);
		VisitedKeys.Add(DefinitionKey);
	}

	return bReturnValue;
}

static void AssignResolvedDefinitionFieldOccurrences(TArray<FMounteaResolvedJsonObjectDefinitionField>& Fields)
{
	TMap<FName, int32> occurrenceCounts;
	for (const FMounteaResolvedJsonObjectDefinitionField& field : Fields)
		occurrenceCounts.FindOrAdd(field.Field.FieldName)++;

	TMap<FName, int32> currentOccurrences;
	for (FMounteaResolvedJsonObjectDefinitionField& field : Fields)
	{
		int32& occurrenceIndex = currentOccurrences.FindOrAdd(field.Field.FieldName);
		occurrenceIndex++;

		field.OccurrenceIndex = occurrenceIndex;
		field.OccurrenceCount = occurrenceCounts.FindRef(field.Field.FieldName);
	}
}

static bool ResolveJsonObjectDefinition(const FString& DefinitionKey, const FMounteaJsonObjectDefinition& Definition, FMounteaJsonObjectDefinition& OutDefinition, TArray<FString>& Errors)
{
	OutDefinition = FMounteaJsonObjectDefinition();

	TSet<FName> fieldNames;
	TSet<FString> visitingKeys;
	TSet<FString> visitedKeys;
	const UMounteaAdvancedInventoryGlobalConfig* globalConfig = UMounteaAdvancedInventoryJsonStatics::GetGlobalJsonConfig();
	return ResolveJsonObjectDefinitionInternal(globalConfig, DefinitionKey, Definition, OutDefinition, fieldNames, Errors, visitingKeys, visitedKeys);
}

UMounteaJsonObject* UMounteaAdvancedInventoryJsonStatics::CreateJsonObject(UObject* Target)
{
	return NewObject<UMounteaJsonObject>(ResolveJsonObjectOuter(Target));
}

UMounteaJsonObject* UMounteaAdvancedInventoryJsonStatics::ParseJsonObjectFromString(UObject* Target, const FString& JsonString)
{
	if (JsonString.IsEmpty())
		return nullptr;

	TSharedPtr<FJsonObject> parsedObject;
	const TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(reader, parsedObject) || !parsedObject.IsValid())
		return nullptr;

	UMounteaJsonObject* returnValue = CreateJsonObject(Target);
	returnValue->SetSharedJsonObject(parsedObject);
	return returnValue;
}

FString UMounteaAdvancedInventoryJsonStatics::JsonObjectToString(UMounteaJsonObject* Target)
{
	if (!IsValidJsonObject(Target))
		return FString();

	FString returnValue;
	const TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&returnValue);
	return FJsonSerializer::Serialize(Target->GetSharedJsonObject().ToSharedRef(), writer) ? returnValue : FString();
}

FString UMounteaAdvancedInventoryJsonStatics::JsonObjectToPrettyString(UMounteaJsonObject* Target)
{
	if (!IsValidJsonObject(Target))
		return FString();

	FString returnValue;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> writer =
		TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&returnValue);
	return FJsonSerializer::Serialize(Target->GetSharedJsonObject().ToSharedRef(), writer) ? returnValue : FString();
}

bool UMounteaAdvancedInventoryJsonStatics::IsValidJsonObject(UMounteaJsonObject* Target)
{
	return IsValid(Target) && Target->IsJsonObjectValid();
}

UMounteaAdvancedInventoryGlobalConfig* UMounteaAdvancedInventoryJsonStatics::GetGlobalJsonConfig()
{
	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	return settings ? settings->GlobalConfig.LoadSynchronous() : nullptr;
}

bool UMounteaAdvancedInventoryJsonStatics::FindJsonObjectDefinition(const FString& DefinitionKey, FMounteaJsonObjectDefinition& Definition)
{
	Definition = FMounteaJsonObjectDefinition();

	const UMounteaAdvancedInventoryGlobalConfig* globalConfig = GetGlobalJsonConfig();
	if (!IsValid(globalConfig) || DefinitionKey.IsEmpty())
		return false;

	if (const FMounteaJsonObjectDefinition* foundDefinition = globalConfig->JsonObjectDefinitions.Find(DefinitionKey))
	{
		Definition = *foundDefinition;
		return true;
	}

	return false;
}

bool UMounteaAdvancedInventoryJsonStatics::ResolveJsonObjectDefinitionByKey(const FString& DefinitionKey, FMounteaJsonObjectDefinition& OutDefinition, TArray<FString>& Errors)
{
	OutDefinition = FMounteaJsonObjectDefinition();
	Errors.Reset();

	FMounteaJsonObjectDefinition definition;
	if (!FindJsonObjectDefinition(DefinitionKey, definition))
	{
		Errors.Add(FString::Printf(TEXT("Could not find JSON object definition '%s'."), *DefinitionKey));
		return false;
	}

	return ResolveJsonObjectDefinition(DefinitionKey, definition, OutDefinition, Errors);
}

bool UMounteaAdvancedInventoryJsonStatics::ResolveJsonObjectDefinitionFieldsByKey(const FString& DefinitionKey, TArray<FMounteaResolvedJsonObjectDefinitionField>& OutFields, TArray<FString>& Errors)
{
	OutFields.Reset();
	Errors.Reset();

	const FString trimmedDefinitionKey = DefinitionKey.TrimStartAndEnd();
	if (trimmedDefinitionKey.IsEmpty() || trimmedDefinitionKey.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return false;

	const UMounteaAdvancedInventoryGlobalConfig* globalConfig = GetGlobalJsonConfig();
	if (!IsValid(globalConfig))
	{
		Errors.Add(TEXT("No Global Json Config is available."));
		return false;
	}

	const FMounteaJsonObjectDefinition* definition = globalConfig->JsonObjectDefinitions.Find(trimmedDefinitionKey);
	if (!definition)
	{
		Errors.Add(FString::Printf(TEXT("Could not find JSON object definition '%s'."), *trimmedDefinitionKey));
		return false;
	}

	TSet<FString> visitingKeys;
	TSet<FString> visitedKeys;
	const TArray<FString> parentPath;
	const bool bReturnValue = ResolveJsonObjectDefinitionFieldsInternal(
		globalConfig,
		trimmedDefinitionKey,
		*definition,
		parentPath,
		OutFields,
		Errors,
		visitingKeys,
		visitedKeys);

	AssignResolvedDefinitionFieldOccurrences(OutFields);
	return bReturnValue || OutFields.Num() > 0;
}

FString UMounteaAdvancedInventoryJsonStatics::JoinJsonDefinitionIncludePath(const TArray<FString>& IncludePath)
{
	return FString::Join(IncludePath, TEXT(" -> "));
}

FName UMounteaAdvancedInventoryJsonStatics::MakeJsonDefinitionFieldPinName(const FString& FieldPinPrefix, const FName FieldName, const int32 OccurrenceIndex, TSet<FName>& UsedPinNames)
{
	const FString basePinName = FieldPinPrefix + FieldName.ToString();
	FString candidatePinName = OccurrenceIndex <= 1
		? basePinName
		: FString::Printf(TEXT("%s_%d"), *basePinName, OccurrenceIndex);

	int32 collisionIndex = OccurrenceIndex <= 1 ? 2 : OccurrenceIndex + 1;
	while (UsedPinNames.Contains(FName(*candidatePinName)))
	{
		candidatePinName = FString::Printf(TEXT("%s_%d"), *basePinName, collisionIndex++);
	}

	const FName pinName(*candidatePinName);
	UsedPinNames.Add(pinName);
	return pinName;
}

FString UMounteaAdvancedInventoryJsonStatics::BuildJsonDefinitionFieldPinTooltip(const FMounteaResolvedJsonObjectDefinitionField& FieldData)
{
	TArray<FString> tooltipLines;
	tooltipLines.Add(FString::Printf(TEXT("JSON field: %s"), *FieldData.Field.FieldName.ToString()));
	tooltipLines.Add(FieldData.Field.bRequired ? TEXT("Status: Required") : TEXT("Status: Optional"));

	if (!FieldData.SourceDefinitionKey.IsEmpty())
		tooltipLines.Add(FString::Printf(TEXT("Origin: %s"), *FieldData.SourceDefinitionKey));

	if (FieldData.IncludePath.Num() > 1)
		tooltipLines.Add(FString::Printf(TEXT("Include path: %s"), *JoinJsonDefinitionIncludePath(FieldData.IncludePath)));

	if (FieldData.IsDuplicate())
	{
		tooltipLines.Add(FString::Printf(
			TEXT("Duplicate warning: occurrence %d of %d. This definition will fail to compile until the duplicate field name is resolved."),
			FieldData.OccurrenceIndex,
			FieldData.OccurrenceCount));
	}

	return FString::Join(tooltipLines, TEXT("\n"));
}

FString UMounteaAdvancedInventoryJsonStatics::BuildJsonDefinitionIssueText(const FString& DefinitionKey)
{
	TArray<FMounteaResolvedJsonObjectDefinitionField> resolvedFields;
	TArray<FString> errors;
	ResolveJsonObjectDefinitionFieldsByKey(DefinitionKey, resolvedFields, errors);

	TArray<FString> warnings;
	for (const FString& error : errors)
	{
		if (!error.IsEmpty())
			warnings.Add(error);
	}

	TSet<FName> reportedDuplicates;
	for (const FMounteaResolvedJsonObjectDefinitionField& fieldData : resolvedFields)
	{
		if (!fieldData.IsDuplicate() || reportedDuplicates.Contains(fieldData.Field.FieldName))
			continue;

		reportedDuplicates.Add(fieldData.Field.FieldName);

		TArray<FString> origins;
		for (const FMounteaResolvedJsonObjectDefinitionField& duplicateData : resolvedFields)
		{
			if (duplicateData.Field.FieldName != fieldData.Field.FieldName)
				continue;

			const FString origin = duplicateData.IncludePath.Num() > 0
				? JoinJsonDefinitionIncludePath(duplicateData.IncludePath)
				: duplicateData.SourceDefinitionKey;
			origins.Add(FString::Printf(TEXT("%d/%d from %s"), duplicateData.OccurrenceIndex, duplicateData.OccurrenceCount, *origin));
		}

		warnings.Add(FString::Printf(
			TEXT("Duplicate JSON field '%s' appears %d times (%s). Runtime compilation will fail until this is resolved."),
			*fieldData.Field.FieldName.ToString(),
			fieldData.OccurrenceCount,
			*FString::Join(origins, TEXT("; "))));
	}

	if (warnings.IsEmpty())
		return FString();

	return FString::Printf(TEXT("NOTE:\n%s"), *FString::Join(warnings, TEXT("\n")));
}

UMounteaJsonObject* UMounteaAdvancedInventoryJsonStatics::CreateJsonObjectFromDefinition(UObject* Target, const FMounteaJsonObjectDefinition& Definition)
{
	FMounteaJsonObjectDefinition resolvedDefinition;
	TArray<FString> errors;
	if (!ResolveJsonObjectDefinition(FString(), Definition, resolvedDefinition, errors))
		return nullptr;

	UMounteaJsonObject* returnValue = CreateJsonObject(Target);
	if (!IsValidJsonObject(returnValue))
		return nullptr;

	for (const FMounteaJsonObjectDefinitionField& field : resolvedDefinition.Fields)
		SetDefinitionFieldDefaultValue(returnValue, field);

	return returnValue;
}

UMounteaJsonObject* UMounteaAdvancedInventoryJsonStatics::CreateJsonObjectFromDefinitionKey(UObject* Target, const FString& DefinitionKey)
{
	FMounteaJsonObjectDefinition definition;
	if (!FindJsonObjectDefinition(DefinitionKey, definition))
		return nullptr;

	FMounteaJsonObjectDefinition resolvedDefinition;
	TArray<FString> errors;
	if (!ResolveJsonObjectDefinition(DefinitionKey, definition, resolvedDefinition, errors))
		return nullptr;

	UMounteaJsonObject* returnValue = CreateJsonObject(Target);
	if (!IsValidJsonObject(returnValue))
		return nullptr;

	for (const FMounteaJsonObjectDefinitionField& field : resolvedDefinition.Fields)
		SetDefinitionFieldDefaultValue(returnValue, field);

	return returnValue;
}

bool UMounteaAdvancedInventoryJsonStatics::ValidateJsonObjectAgainstDefinition(UMounteaJsonObject* Target, const FMounteaJsonObjectDefinition& Definition, TArray<FString>& Errors)
{
	Errors.Reset();

	FMounteaJsonObjectDefinition resolvedDefinition;
	if (!ResolveJsonObjectDefinition(FString(), Definition, resolvedDefinition, Errors))
		return false;

	if (!IsValidJsonObject(Target))
	{
		Errors.Add(TEXT("Target is not a valid Mountea Json Object."));
		return false;
	}

	const TSharedPtr<FJsonObject> jsonObject = Target->GetSharedJsonObject();
	if (!jsonObject.IsValid())
	{
		Errors.Add(TEXT("Target does not contain a valid JSON object."));
		return false;
	}

	for (const FMounteaJsonObjectDefinitionField& field : resolvedDefinition.Fields)
	{
		if (field.FieldName.IsNone())
		{
			Errors.Add(TEXT("Definition contains a field with an empty name."));
			continue;
		}

		const FString fieldName = field.FieldName.ToString();
		const TSharedPtr<FJsonValue> fieldValue = jsonObject->TryGetField(fieldName);
		if (!fieldValue.IsValid())
		{
			if (field.bRequired)
				Errors.Add(FString::Printf(TEXT("Missing required JSON field '%s'."), *fieldName));

			continue;
		}

		const EJson expectedType = GetJsonTypeForDefinitionField(field);
		if (expectedType == EJson::None)
		{
			Errors.Add(FString::Printf(TEXT("Definition field '%s' has unsupported type '%s'."), *fieldName, *GetDefinitionFieldTypeDisplayName(field)));
			continue;
		}

		if (fieldValue->Type != expectedType)
		{
			Errors.Add(FString::Printf(
				TEXT("JSON field '%s' expected %s for definition type %s, but found %s."),
				*fieldName,
				*GetJsonTypeDisplayName(expectedType),
				*GetDefinitionFieldTypeDisplayName(field),
				*GetJsonTypeDisplayName(fieldValue->Type)
			));
			continue;
		}

		ValidateDefinitionFieldMetadata(field, fieldValue, Errors);
	}

	return Errors.Num() == 0;
}

bool UMounteaAdvancedInventoryJsonStatics::ValidateJsonObjectAgainstDefinitionKey(UMounteaJsonObject* Target, const FString& DefinitionKey, TArray<FString>& Errors)
{
	Errors.Reset();

	FMounteaJsonObjectDefinition definition;
	if (!FindJsonObjectDefinition(DefinitionKey, definition))
	{
		Errors.Add(FString::Printf(TEXT("Could not find JSON object definition '%s'."), *DefinitionKey));
		return false;
	}

	FMounteaJsonObjectDefinition resolvedDefinition;
	if (!ResolveJsonObjectDefinition(DefinitionKey, definition, resolvedDefinition, Errors))
		return false;

	return ValidateJsonObjectAgainstDefinition(Target, resolvedDefinition, Errors);
}

bool UMounteaAdvancedInventoryJsonStatics::SetIntJsonField(UMounteaJsonObject* Target, const FName FieldName, const int32 Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return IsValidJsonObject(Target) ? Target->SetIntField(FieldName, Value, JsonObject) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::SetInt64JsonField(UMounteaJsonObject* Target, const FName FieldName, const int64 Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	if (!IsValidJsonObject(Target) || FieldName.IsNone())
		return false;

	Target->GetSharedJsonObject()->SetNumberField(FieldName.ToString(), static_cast<double>(Value));
	return true;
}

bool UMounteaAdvancedInventoryJsonStatics::SetFloatJsonField(UMounteaJsonObject* Target, const FName FieldName, const float Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return IsValidJsonObject(Target) ? Target->SetFloatField(FieldName, Value, JsonObject) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::SetStringJsonField(UMounteaJsonObject* Target, const FName FieldName, const FString& Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return IsValidJsonObject(Target) ? Target->SetStringField(FieldName, Value, JsonObject) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::SetNameJsonField(UMounteaJsonObject* Target, const FName FieldName, const FName& Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return IsValidJsonObject(Target) ? Target->SetNameField(FieldName, Value, JsonObject) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::SetTextJsonField(UMounteaJsonObject* Target, const FName FieldName, const FText& Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return IsValidJsonObject(Target) ? Target->SetTextField(FieldName, Value, JsonObject) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::SetBoolJsonField(UMounteaJsonObject* Target, const FName FieldName, const bool Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return IsValidJsonObject(Target) ? Target->SetBoolField(FieldName, Value, JsonObject) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::SetByteJsonField(UMounteaJsonObject* Target, const FName FieldName, const uint8 Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	return IsValidJsonObject(Target) ? Target->SetIntField(FieldName, Value, JsonObject) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::SetObjectJsonField(UMounteaJsonObject* Target, const FName FieldName, UObject* Value, UMounteaJsonObject*& JsonObject)
{
	JsonObject = Target;
	if (!IsValidJsonObject(Target) || !IsValid(Value))
		return false;

	if (UMounteaJsonObject* jsonValue = Cast<UMounteaJsonObject>(Value))
		return Target->SetObjectField(FieldName, jsonValue, JsonObject);

	return SetPathStringField(Target, FieldName, Value->GetPathName(), JsonObject);
}

bool UMounteaAdvancedInventoryJsonStatics::SetSoftObjectJsonField(UMounteaJsonObject* Target, const FName FieldName, const TSoftObjectPtr<UObject>& Value, UMounteaJsonObject*& JsonObject)
{
	return SetPathStringField(Target, FieldName, Value.ToSoftObjectPath().ToString(), JsonObject);
}

bool UMounteaAdvancedInventoryJsonStatics::SetClassJsonField(UMounteaJsonObject* Target, const FName FieldName, const TSubclassOf<UObject> Value, UMounteaJsonObject*& JsonObject)
{
	return SetPathStringField(Target, FieldName, Value ? Value->GetPathName() : FString(), JsonObject);
}

bool UMounteaAdvancedInventoryJsonStatics::SetSoftClassJsonField(UMounteaJsonObject* Target, const FName FieldName, const TSoftClassPtr<UObject>& Value, UMounteaJsonObject*& JsonObject)
{
	return SetPathStringField(Target, FieldName, Value.ToSoftObjectPath().ToString(), JsonObject);
}

bool UMounteaAdvancedInventoryJsonStatics::SetStructJsonField(UMounteaJsonObject* Target, FName FieldName, const int32& Value, UMounteaJsonObject*& JsonObject)
{
	check(0);
	return false;
}

bool UMounteaAdvancedInventoryJsonStatics::GetIntJsonField(UMounteaJsonObject* Target, const FName FieldName, int32& Value)
{
	return IsValidJsonObject(Target) ? Target->GetIntField(FieldName, Value) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::GetInt64JsonField(UMounteaJsonObject* Target, const FName FieldName, int64& Value)
{
	Value = 0;

	if (!IsValidJsonObject(Target))
		return false;

	const TSharedPtr<FJsonValue> fieldValue = Target->GetSharedJsonObject()->TryGetField(FieldName.ToString());
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Number)
		return false;

	Value = static_cast<int64>(fieldValue->AsNumber());
	return true;
}

bool UMounteaAdvancedInventoryJsonStatics::GetFloatJsonField(UMounteaJsonObject* Target, const FName FieldName, float& Value)
{
	return IsValidJsonObject(Target) ? Target->GetFloatField(FieldName, Value) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::GetStringJsonField(UMounteaJsonObject* Target, const FName FieldName, FString& Value)
{
	return IsValidJsonObject(Target) ? Target->GetStringField(FieldName, Value) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::GetNameJsonField(UMounteaJsonObject* Target, const FName FieldName, FName& Value)
{
	return IsValidJsonObject(Target) ? Target->GetNameField(FieldName, Value) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::GetTextJsonField(UMounteaJsonObject* Target, const FName FieldName, FText& Value)
{
	return IsValidJsonObject(Target) ? Target->GetTextField(FieldName, Value) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::GetBoolJsonField(UMounteaJsonObject* Target, const FName FieldName, bool& Value)
{
	return IsValidJsonObject(Target) ? Target->GetBoolField(FieldName, Value) : false;
}

bool UMounteaAdvancedInventoryJsonStatics::GetByteJsonField(UMounteaJsonObject* Target, const FName FieldName, uint8& Value)
{
	Value = 0;

	int32 intValue = 0;
	if (!IsValidJsonObject(Target) || !Target->GetIntField(FieldName, intValue) || intValue < 0 || intValue > MAX_uint8)
		return false;

	Value = static_cast<uint8>(intValue);
	return true;
}

bool UMounteaAdvancedInventoryJsonStatics::GetObjectJsonField(UMounteaJsonObject* Target, const FName FieldName, UObject*& Value)
{
	Value = nullptr;

	if (!IsValidJsonObject(Target))
		return false;

	UMounteaJsonObject* jsonObject = nullptr;
	if (Target->GetObjectField(FieldName, jsonObject))
	{
		Value = jsonObject;
		return true;
	}

	FString path;
	if (!GetPathStringField(Target, FieldName, path))
		return false;

	Value = StaticLoadObject(UObject::StaticClass(), nullptr, *path);
	return IsValid(Value);
}

bool UMounteaAdvancedInventoryJsonStatics::GetSoftObjectJsonField(UMounteaJsonObject* Target, const FName FieldName, TSoftObjectPtr<UObject>& Value)
{
	Value.Reset();

	FString path;
	if (!GetPathStringField(Target, FieldName, path))
		return false;

	Value = TSoftObjectPtr<UObject>(FSoftObjectPath(path));
	return !Value.IsNull();
}

bool UMounteaAdvancedInventoryJsonStatics::GetClassJsonField(UMounteaJsonObject* Target, const FName FieldName, TSubclassOf<UObject>& Value)
{
	Value = nullptr;

	FString path;
	if (!GetPathStringField(Target, FieldName, path))
		return false;

	UClass* loadedClass = LoadObject<UClass>(nullptr, *path);
	if (!loadedClass)
		return false;

	Value = loadedClass;
	return true;
}

bool UMounteaAdvancedInventoryJsonStatics::GetSoftClassJsonField(UMounteaJsonObject* Target, const FName FieldName, TSoftClassPtr<UObject>& Value)
{
	Value.Reset();

	FString path;
	if (!GetPathStringField(Target, FieldName, path))
		return false;

	Value = TSoftClassPtr<UObject>(FSoftObjectPath(path));
	return !Value.IsNull();
}

bool UMounteaAdvancedInventoryJsonStatics::GetStructJsonField(UMounteaJsonObject* Target, FName FieldName, int32& Value)
{
	check(0);
	return false;
}

DEFINE_FUNCTION(UMounteaAdvancedInventoryJsonStatics::execSetStructJsonField)
{
	P_GET_OBJECT(UMounteaJsonObject, Target);
	P_GET_PROPERTY(FNameProperty, FieldName);

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* valuePtr = Stack.MostRecentPropertyAddress;
	FStructProperty* valueProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_GET_OBJECT_REF(UMounteaJsonObject, JsonObject);

	P_FINISH;

	bool bSuccess = false;
	JsonObject = Target;

	[&]()
	{
		if (!IsValidJsonObject(Target) || !valuePtr || !valueProperty || !valueProperty->Struct || FieldName.IsNone())
			return;

		TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
		if (!FJsonObjectConverter::UStructToJsonObject(valueProperty->Struct, valuePtr, jsonObject, 0, 0))
			return;

		bSuccess = Target->SetObjectFieldRaw(FieldName, jsonObject);
	}();

	*static_cast<bool*>(RESULT_PARAM) = bSuccess;
}

DEFINE_FUNCTION(UMounteaAdvancedInventoryJsonStatics::execGetStructJsonField)
{
	P_GET_OBJECT(UMounteaJsonObject, Target);
	P_GET_PROPERTY(FNameProperty, FieldName);

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* valuePtr = Stack.MostRecentPropertyAddress;
	FStructProperty* valueProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_FINISH;

	bool bSuccess = false;

	[&]()
	{
		if (!IsValidJsonObject(Target) || !valuePtr || !valueProperty || !valueProperty->Struct || FieldName.IsNone())
			return;

		TSharedPtr<FJsonObject> jsonObject;
		if (!Target->GetObjectFieldRaw(FieldName, jsonObject) || !jsonObject.IsValid())
			return;

		bSuccess = FJsonObjectConverter::JsonObjectToUStruct(jsonObject.ToSharedRef(), valueProperty->Struct, valuePtr, 0, 0);
	}();

	*static_cast<bool*>(RESULT_PARAM) = bSuccess;
}
