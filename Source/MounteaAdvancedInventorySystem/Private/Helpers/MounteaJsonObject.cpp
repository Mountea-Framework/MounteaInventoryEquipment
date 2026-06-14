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

#include "Helpers/MounteaJsonObject.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

static UObject* ResolveJsonObjectOuter(UObject* Target)
{
	return IsValid(Target) ? Target : GetTransientPackage();
}

static TSharedPtr<FJsonValue> GetFieldValue(const TSharedPtr<FJsonObject>& JsonObject, const FName FieldName)
{
	return JsonObject.IsValid() ? JsonObject->TryGetField(FieldName.ToString()) : nullptr;
}

static UMounteaJsonObject* CreateJsonObjectWrapper(UObject* Target, const TSharedPtr<FJsonObject>& JsonObject)
{
	if (!JsonObject.IsValid())
		return nullptr;

	UMounteaJsonObject* wrapper = NewObject<UMounteaJsonObject>(ResolveJsonObjectOuter(Target));
	wrapper->SetSharedJsonObject(JsonObject);
	return wrapper;
}

UMounteaJsonObject::UMounteaJsonObject()
{
	InitializeJsonObject();
}

void UMounteaJsonObject::InitializeJsonObject()
{
	JsonObject = MakeShared<FJsonObject>();
}

bool UMounteaJsonObject::IsJsonObjectValid() const
{
	return JsonObject.IsValid();
}

TSharedPtr<FJsonObject> UMounteaJsonObject::GetSharedJsonObject() const
{
	return JsonObject;
}

void UMounteaJsonObject::SetSharedJsonObject(const TSharedPtr<FJsonObject>& NewJsonObject)
{
	JsonObject = NewJsonObject.IsValid() ? NewJsonObject : MakeShared<FJsonObject>();
}

bool UMounteaJsonObject::SetObjectFieldRaw(const FName FieldName, const TSharedPtr<FJsonObject>& Value)
{
	if (!JsonObject.IsValid() || FieldName.IsNone() || !Value.IsValid())
		return false;

	JsonObject->SetObjectField(FieldName.ToString(), Value);
	return true;
}

bool UMounteaJsonObject::GetObjectFieldRaw(const FName FieldName, TSharedPtr<FJsonObject>& Value) const
{
	Value.Reset();

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Object)
		return false;

	Value = fieldValue->AsObject();
	return Value.IsValid();
}

bool UMounteaJsonObject::SetStringField(const FName FieldName, const FString& Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	JsonObject->SetStringField(FieldName.ToString(), Value);
	return true;
}

bool UMounteaJsonObject::GetStringField(const FName FieldName, FString& Value) const
{
	Value.Reset();

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::String)
		return false;

	Value = fieldValue->AsString();
	return true;
}

bool UMounteaJsonObject::SetNameField(const FName FieldName, const FName& Value, UMounteaJsonObject*& OutJsonObject)
{
	return SetStringField(FieldName, Value.ToString(), OutJsonObject);
}

bool UMounteaJsonObject::GetNameField(const FName FieldName, FName& Value) const
{
	FString stringValue;
	if (!GetStringField(FieldName, stringValue))
	{
		Value = NAME_None;
		return false;
	}

	Value = FName(*stringValue);
	return true;
}

bool UMounteaJsonObject::SetTextField(const FName FieldName, const FText& Value, UMounteaJsonObject*& OutJsonObject)
{
	return SetStringField(FieldName, Value.ToString(), OutJsonObject);
}

bool UMounteaJsonObject::GetTextField(const FName FieldName, FText& Value) const
{
	FString stringValue;
	if (!GetStringField(FieldName, stringValue))
	{
		Value = FText::GetEmpty();
		return false;
	}

	Value = FText::FromString(stringValue);
	return true;
}

bool UMounteaJsonObject::SetIntField(const FName FieldName, const int32 Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	JsonObject->SetNumberField(FieldName.ToString(), Value);
	return true;
}

bool UMounteaJsonObject::GetIntField(const FName FieldName, int32& Value) const
{
	Value = 0;

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Number)
		return false;

	const double numberValue = fieldValue->AsNumber();
	if (numberValue < static_cast<double>(MIN_int32) || numberValue > static_cast<double>(MAX_int32))
		return false;

	Value = static_cast<int32>(numberValue);
	return true;
}

bool UMounteaJsonObject::SetFloatField(const FName FieldName, const float Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	JsonObject->SetNumberField(FieldName.ToString(), Value);
	return true;
}

bool UMounteaJsonObject::GetFloatField(const FName FieldName, float& Value) const
{
	Value = 0.f;

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Number)
		return false;

	Value = static_cast<float>(fieldValue->AsNumber());
	return true;
}

bool UMounteaJsonObject::SetBoolField(const FName FieldName, const bool Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	JsonObject->SetBoolField(FieldName.ToString(), Value);
	return true;
}

bool UMounteaJsonObject::GetBoolField(const FName FieldName, bool& Value) const
{
	Value = false;

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Boolean)
		return false;

	Value = fieldValue->AsBool();
	return true;
}

bool UMounteaJsonObject::SetObjectField(const FName FieldName, UMounteaJsonObject* Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;
	return IsValid(Value) ? SetObjectFieldRaw(FieldName, Value->GetSharedJsonObject()) : false;
}

bool UMounteaJsonObject::GetObjectField(const FName FieldName, UMounteaJsonObject*& Value) const
{
	Value = nullptr;

	TSharedPtr<FJsonObject> objectValue;
	if (!GetObjectFieldRaw(FieldName, objectValue))
		return false;

	Value = CreateJsonObjectWrapper(const_cast<UMounteaJsonObject*>(this), objectValue);
	return IsValid(Value);
}

bool UMounteaJsonObject::SetNullField(const FName FieldName, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	JsonObject->SetField(FieldName.ToString(), MakeShared<FJsonValueNull>());
	return true;
}

bool UMounteaJsonObject::HasField(const FName FieldName) const
{
	return GetFieldValue(JsonObject, FieldName).IsValid();
}

bool UMounteaJsonObject::RemoveField(const FName FieldName, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone() || !HasField(FieldName))
		return false;

	JsonObject->RemoveField(FieldName.ToString());
	return true;
}

void UMounteaJsonObject::Clear()
{
	InitializeJsonObject();
}

TArray<FString> UMounteaJsonObject::GetFieldNames() const
{
	TArray<FString> returnValue;

	if (!JsonObject.IsValid())
		return returnValue;

	JsonObject->Values.GetKeys(returnValue);
	return returnValue;
}

EJson UMounteaJsonObject::GetFieldType(const FName FieldName) const
{
	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	return fieldValue.IsValid() ? fieldValue->Type : EJson::None;
}

bool UMounteaJsonObject::SetStringArrayField(const FName FieldName, const TArray<FString>& Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	TArray<TSharedPtr<FJsonValue>> arrayValues;
	for (const FString& item : Value)
		arrayValues.Add(MakeShared<FJsonValueString>(item));

	JsonObject->SetArrayField(FieldName.ToString(), arrayValues);
	return true;
}

bool UMounteaJsonObject::GetStringArrayField(const FName FieldName, TArray<FString>& Value) const
{
	Value.Reset();

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Array)
		return false;

	for (const TSharedPtr<FJsonValue>& item : fieldValue->AsArray())
	{
		if (!item.IsValid() || item->Type != EJson::String)
			return false;

		Value.Add(item->AsString());
	}

	return true;
}

bool UMounteaJsonObject::SetNameArrayField(const FName FieldName, const TArray<FName>& Value, UMounteaJsonObject*& OutJsonObject)
{
	TArray<FString> stringValues;
	for (const FName& item : Value)
		stringValues.Add(item.ToString());

	return SetStringArrayField(FieldName, stringValues, OutJsonObject);
}

bool UMounteaJsonObject::GetNameArrayField(const FName FieldName, TArray<FName>& Value) const
{
	Value.Reset();

	TArray<FString> stringValues;
	if (!GetStringArrayField(FieldName, stringValues))
		return false;

	for (const FString& item : stringValues)
		Value.Add(FName(*item));

	return true;
}

bool UMounteaJsonObject::SetIntArrayField(const FName FieldName, const TArray<int32>& Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	TArray<TSharedPtr<FJsonValue>> arrayValues;
	for (const int32 item : Value)
		arrayValues.Add(MakeShared<FJsonValueNumber>(item));

	JsonObject->SetArrayField(FieldName.ToString(), arrayValues);
	return true;
}

bool UMounteaJsonObject::GetIntArrayField(const FName FieldName, TArray<int32>& Value) const
{
	Value.Reset();

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Array)
		return false;

	for (const TSharedPtr<FJsonValue>& item : fieldValue->AsArray())
	{
		if (!item.IsValid() || item->Type != EJson::Number)
			return false;

		const double numberValue = item->AsNumber();
		if (numberValue < static_cast<double>(MIN_int32) || numberValue > static_cast<double>(MAX_int32))
			return false;

		Value.Add(static_cast<int32>(numberValue));
	}

	return true;
}

bool UMounteaJsonObject::SetFloatArrayField(const FName FieldName, const TArray<float>& Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	TArray<TSharedPtr<FJsonValue>> arrayValues;
	for (const float item : Value)
		arrayValues.Add(MakeShared<FJsonValueNumber>(item));

	JsonObject->SetArrayField(FieldName.ToString(), arrayValues);
	return true;
}

bool UMounteaJsonObject::GetFloatArrayField(const FName FieldName, TArray<float>& Value) const
{
	Value.Reset();

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Array)
		return false;

	for (const TSharedPtr<FJsonValue>& item : fieldValue->AsArray())
	{
		if (!item.IsValid() || item->Type != EJson::Number)
			return false;

		Value.Add(static_cast<float>(item->AsNumber()));
	}

	return true;
}

bool UMounteaJsonObject::SetBoolArrayField(const FName FieldName, const TArray<bool>& Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	TArray<TSharedPtr<FJsonValue>> arrayValues;
	for (const bool item : Value)
		arrayValues.Add(MakeShared<FJsonValueBoolean>(item));

	JsonObject->SetArrayField(FieldName.ToString(), arrayValues);
	return true;
}

bool UMounteaJsonObject::GetBoolArrayField(const FName FieldName, TArray<bool>& Value) const
{
	Value.Reset();

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Array)
		return false;

	for (const TSharedPtr<FJsonValue>& item : fieldValue->AsArray())
	{
		if (!item.IsValid() || item->Type != EJson::Boolean)
			return false;

		Value.Add(item->AsBool());
	}

	return true;
}

bool UMounteaJsonObject::SetObjectArrayField(const FName FieldName, const TArray<UMounteaJsonObject*>& Value, UMounteaJsonObject*& OutJsonObject)
{
	OutJsonObject = this;

	if (!JsonObject.IsValid() || FieldName.IsNone())
		return false;

	TArray<TSharedPtr<FJsonValue>> arrayValues;
	for (UMounteaJsonObject* item : Value)
	{
		if (!IsValid(item) || !item->GetSharedJsonObject().IsValid())
			return false;

		arrayValues.Add(MakeShared<FJsonValueObject>(item->GetSharedJsonObject()));
	}

	JsonObject->SetArrayField(FieldName.ToString(), arrayValues);
	return true;
}

bool UMounteaJsonObject::GetObjectArrayField(const FName FieldName, TArray<UMounteaJsonObject*>& Value) const
{
	Value.Reset();

	const TSharedPtr<FJsonValue> fieldValue = GetFieldValue(JsonObject, FieldName);
	if (!fieldValue.IsValid() || fieldValue->Type != EJson::Array)
		return false;

	for (const TSharedPtr<FJsonValue>& item : fieldValue->AsArray())
	{
		if (!item.IsValid() || item->Type != EJson::Object)
			return false;

		UMounteaJsonObject* wrapper = CreateJsonObjectWrapper(const_cast<UMounteaJsonObject*>(this), item->AsObject());
		if (!IsValid(wrapper))
			return false;

		Value.Add(wrapper);
	}

	return true;
}
