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

#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"

#include "Decorations/MounteaSelectableInventoryItemAction.h"
#include "Definitions/MounteaAdvancedInventoryGlobalUIDataTypes.h"

static FMounteaJsonObjectDefinitionField MakeJsonDefinitionField(const FName FieldName, const FName PinCategory, UObject* SubCategoryObject = nullptr)
{
	FMounteaJsonObjectDefinitionField field;
	field.FieldName = FieldName;
	field.FieldValueType.PinCategory = PinCategory;
	field.FieldValueType.PinSubCategoryObject = SubCategoryObject;

	return field;
}

static FMounteaJsonObjectDefinitionField MakeJsonIntDefinitionField(const FName FieldName)
{
	return MakeJsonDefinitionField(FieldName, TEXT("int"));
}

static FMounteaJsonObjectDefinitionField MakeJsonFloatDefinitionField(const FName FieldName)
{
	return MakeJsonDefinitionField(FieldName, TEXT("real"));
}

static FMounteaJsonObjectDefinitionField MakeJsonStringDefinitionField(const FName FieldName)
{
	return MakeJsonDefinitionField(FieldName, TEXT("string"));
}

static FMounteaJsonObjectDefinitionField MakeJsonBooleanDefinitionField(const FName FieldName)
{
	return MakeJsonDefinitionField(FieldName, TEXT("boolean"));
}

static FMounteaJsonObjectDefinitionField MakeJsonStructDefinitionField(const FName FieldName, UScriptStruct* StructType)
{
	return MakeJsonDefinitionField(FieldName, TEXT("struct"), StructType);
}

static FMounteaJsonObjectDefinitionField MakeJsonUObjectDefinitionField(const FName FieldName, UClass* ObjectClass = nullptr)
{
	return MakeJsonDefinitionField(FieldName, TEXT("object"), ObjectClass ? ObjectClass : UObject::StaticClass());
}

static FMounteaJsonObjectDefinitionField MakeJsonSoftObjectPtrDefinitionField(const FName FieldName, UClass* ObjectClass = nullptr)
{
	return MakeJsonDefinitionField(FieldName, TEXT("softobject"), ObjectClass ? ObjectClass : UObject::StaticClass());
}

static FMounteaJsonObjectDefinitionInclude MakeJsonDefinitionInclude(const FString& DefinitionKey)
{
	FMounteaJsonObjectDefinitionInclude include;
	include.DefinitionKey = DefinitionKey;

	return include;
}

static FMounteaJsonObjectDefinition MakeJsonObjectDefinition(
	TArray<FMounteaJsonObjectDefinitionField> Fields,
	TArray<FMounteaJsonObjectDefinitionInclude> IncludedDefinitions = {})
{
	FMounteaJsonObjectDefinition definition;
	definition.Fields = MoveTemp(Fields);
	definition.IncludedDefinitions = MoveTemp(IncludedDefinitions);

	return definition;
}

UMounteaAdvancedInventoryGlobalConfig::UMounteaAdvancedInventoryGlobalConfig()
{
	JsonObjectDefinitions.Add(TEXT("ModalBaseDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("modalData"), TBaseStructure<FMounteaModalsConfig>::Get()),
			MakeJsonStringDefinitionField(TEXT("modalType")),
			MakeJsonBooleanDefinitionField(TEXT("autoClose"))
		}
	));

	JsonObjectDefinitions.Add(TEXT("ModalSliderDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("sliderRange"), TBaseStructure<FVector2D>::Get())
		}
	));

	JsonObjectDefinitions.Add(TEXT("ModalConsumeDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get()),
			MakeJsonSoftObjectPtrDefinitionField(TEXT("instigatorActor"), UMounteaSelectableInventoryItemAction::StaticClass())
		},
		{
			MakeJsonDefinitionInclude(TEXT("ModalBaseDefinition")),
			MakeJsonDefinitionInclude(TEXT("ModalSliderDefinition"))
		}
	));
}
