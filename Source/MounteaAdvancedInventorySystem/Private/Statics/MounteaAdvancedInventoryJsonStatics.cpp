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

static EJson GetJsonTypeForDefinitionField(const EMounteaJsonDefinitionFieldType FieldType)
{
	switch (FieldType)
	{
		case EMounteaJsonDefinitionFieldType::Int:
		case EMounteaJsonDefinitionFieldType::Int64:
		case EMounteaJsonDefinitionFieldType::Float:
		case EMounteaJsonDefinitionFieldType::Byte:
			return EJson::Number;

		case EMounteaJsonDefinitionFieldType::Bool:
			return EJson::Boolean;

		case EMounteaJsonDefinitionFieldType::String:
		case EMounteaJsonDefinitionFieldType::Name:
		case EMounteaJsonDefinitionFieldType::Text:
		case EMounteaJsonDefinitionFieldType::Object:
		case EMounteaJsonDefinitionFieldType::SoftObject:
		case EMounteaJsonDefinitionFieldType::Class:
		case EMounteaJsonDefinitionFieldType::SoftClass:
			return EJson::String;

		case EMounteaJsonDefinitionFieldType::JsonObject:
		case EMounteaJsonDefinitionFieldType::Struct:
			return EJson::Object;

		default:
			return EJson::None;
	}
}

static FString GetDefinitionFieldTypeDisplayName(const EMounteaJsonDefinitionFieldType FieldType)
{
	const UEnum* enumPtr = StaticEnum<EMounteaJsonDefinitionFieldType>();
	return enumPtr ? enumPtr->GetDisplayNameTextByValue(static_cast<int64>(FieldType)).ToString() : TEXT("Unknown");
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

	const FString fieldName = Field.FieldName.ToString();
	const TSharedPtr<FJsonObject> jsonObject = Target->GetSharedJsonObject();
	if (!jsonObject.IsValid())
		return;

	switch (Field.FieldType)
	{
		case EMounteaJsonDefinitionFieldType::Int:
		case EMounteaJsonDefinitionFieldType::Int64:
		case EMounteaJsonDefinitionFieldType::Float:
		case EMounteaJsonDefinitionFieldType::Byte:
			jsonObject->SetNumberField(fieldName, 0.0);
			break;

		case EMounteaJsonDefinitionFieldType::Bool:
			jsonObject->SetBoolField(fieldName, false);
			break;

		case EMounteaJsonDefinitionFieldType::String:
		case EMounteaJsonDefinitionFieldType::Name:
		case EMounteaJsonDefinitionFieldType::Text:
		case EMounteaJsonDefinitionFieldType::Object:
		case EMounteaJsonDefinitionFieldType::SoftObject:
		case EMounteaJsonDefinitionFieldType::Class:
		case EMounteaJsonDefinitionFieldType::SoftClass:
			jsonObject->SetStringField(fieldName, FString());
			break;

		case EMounteaJsonDefinitionFieldType::JsonObject:
		case EMounteaJsonDefinitionFieldType::Struct:
			jsonObject->SetObjectField(fieldName, MakeShared<FJsonObject>());
			break;

		default:
			break;
	}
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

UMounteaJsonObject* UMounteaAdvancedInventoryJsonStatics::CreateJsonObjectFromDefinition(UObject* Target, const FMounteaJsonObjectDefinition& Definition)
{
	UMounteaJsonObject* returnValue = CreateJsonObject(Target);
	if (!IsValidJsonObject(returnValue))
		return nullptr;

	for (const FMounteaJsonObjectDefinitionField& field : Definition.Fields)
		SetDefinitionFieldDefaultValue(returnValue, field);

	return returnValue;
}

UMounteaJsonObject* UMounteaAdvancedInventoryJsonStatics::CreateJsonObjectFromDefinitionKey(UObject* Target, const FString& DefinitionKey)
{
	FMounteaJsonObjectDefinition definition;
	return FindJsonObjectDefinition(DefinitionKey, definition) ? CreateJsonObjectFromDefinition(Target, definition) : nullptr;
}

bool UMounteaAdvancedInventoryJsonStatics::ValidateJsonObjectAgainstDefinition(UMounteaJsonObject* Target, const FMounteaJsonObjectDefinition& Definition, TArray<FString>& Errors)
{
	Errors.Reset();

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

	for (const FMounteaJsonObjectDefinitionField& field : Definition.Fields)
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

		const EJson expectedType = GetJsonTypeForDefinitionField(field.FieldType);
		if (expectedType == EJson::None)
		{
			Errors.Add(FString::Printf(TEXT("Definition field '%s' has unsupported type '%s'."), *fieldName, *GetDefinitionFieldTypeDisplayName(field.FieldType)));
			continue;
		}

		if (fieldValue->Type != expectedType)
		{
			Errors.Add(FString::Printf(
				TEXT("JSON field '%s' expected %s for definition type %s, but found %s."),
				*fieldName,
				*GetJsonTypeDisplayName(expectedType),
				*GetDefinitionFieldTypeDisplayName(field.FieldType),
				*GetJsonTypeDisplayName(fieldValue->Type)
			));
		}
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

	return ValidateJsonObjectAgainstDefinition(Target, definition, Errors);
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
