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

#include "Blueprint/UserWidget.h"
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
	return MakeJsonDefinitionField(FieldName, TEXT("bool"));
}

static FMounteaJsonObjectDefinitionField MakeJsonStructDefinitionField(const FName FieldName, UScriptStruct* StructType)
{
	return MakeJsonDefinitionField(FieldName, TEXT("struct"), StructType);
}

static FMounteaJsonObjectDefinitionField MakeJsonArrayDefinitionField(const FName FieldName, const FName PinCategory, UObject* SubCategoryObject = nullptr)
{
	FMounteaJsonObjectDefinitionField field = MakeJsonDefinitionField(FieldName, PinCategory, SubCategoryObject);
	field.FieldValueType.ContainerType = EPinContainerType::Array;

	return field;
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

#if WITH_EDITOR

void UMounteaAdvancedInventoryGlobalConfig::SetupDefaults()
{
	// ModalBaseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalBaseDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("modalData"), TBaseStructure<FMounteaModalsConfig>::Get()),
			MakeJsonStringDefinitionField(TEXT("modalType"))
		}
	));
	
	// ModalItemDefinition
	JsonObjectDefinitions.Add(TEXT("ModalItemDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get()),
		}
	));

	// ModalSliderDefinition
	JsonObjectDefinitions.Add(TEXT("ModalSliderDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("sliderRange"), TBaseStructure<FIntPoint>::Get())
		}
	));

	// ModalItemQuantityDefinition
	JsonObjectDefinitions.Add(TEXT("ModalItemQuantityDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonSoftObjectPtrDefinitionField(TEXT("inventoryAction"), UMounteaSelectableInventoryItemAction::StaticClass())
		},
		{
			MakeJsonDefinitionInclude(TEXT("ModalBaseDefinition")),
			MakeJsonDefinitionInclude(TEXT("ModalItemDefinition")),
			MakeJsonDefinitionInclude(TEXT("ModalSliderDefinition"))
		}
	));

	
	// ModalResponseBaseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalResponseBaseDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStringDefinitionField(TEXT("modalType")),
			MakeJsonBooleanDefinitionField(TEXT("confirmed"))
		}
	));

	// ModalItemResponseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalItemResponseDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get())
		},
		{
			MakeJsonDefinitionInclude(TEXT("ModalResponseBaseDefinition"))
		}
	));

	// ModalQuantityResponseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalQuantityResponseDefinition"), MakeJsonObjectDefinition(
		{
			MakeJsonIntDefinitionField(TEXT("quantity"))
		},
		{
			MakeJsonDefinitionInclude(TEXT("ModalItemResponseDefinition"))
		}
	));

	// CategorySelectedRequest
	JsonObjectDefinitions.Add(TEXT("CategorySelectedRequest"), MakeJsonObjectDefinition(
		{
			MakeJsonUObjectDefinitionField(TEXT("CategoryWidget")),
			MakeJsonStringDefinitionField(TEXT("CategoryId"))
		}
	));

	// CategorySelectedResponse
	JsonObjectDefinitions.Add(TEXT("CategorySelectedResponse"), MakeJsonObjectDefinition(
		{
			MakeJsonStringDefinitionField(TEXT("CategoryId"))
		}
	));

	// CategoriesCreateRequest
	JsonObjectDefinitions.Add(TEXT("CategoriesCreateRequest"), MakeJsonObjectDefinition(
		{
			MakeJsonDefinitionField(TEXT("CategoryWidgetClass"), TEXT("softclass"), UUserWidget::StaticClass()),
			MakeJsonUObjectDefinitionField(TEXT("RegistrarObject"))
		}
	));

	// ItemsCreateRequest
	JsonObjectDefinitions.Add(TEXT("ItemsCreateRequest"), MakeJsonObjectDefinition(
		{
			MakeJsonDefinitionField(TEXT("ItemWidgetClass"), TEXT("softclass"), UUserWidget::StaticClass()),
			MakeJsonUObjectDefinitionField(TEXT("RegistrarObject")),
			MakeJsonStringDefinitionField(TEXT("categoryId")),
			MakeJsonArrayDefinitionField(TEXT("itemGuids"), TEXT("struct"), TBaseStructure<FGuid>::Get())
		}
	));

	// ItemCreateRequest
	JsonObjectDefinitions.Add(TEXT("ItemCreateRequest"), MakeJsonObjectDefinition(
		{
			MakeJsonUObjectDefinitionField(TEXT("RegistrarObject")),
			MakeJsonStringDefinitionField(TEXT("categoryId")),
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get())
		}
	));

	// ItemSelectedRequest
	JsonObjectDefinitions.Add(TEXT("ItemSelectedRequest"), MakeJsonObjectDefinition(
		{
			MakeJsonUObjectDefinitionField(TEXT("ItemWidget")),
			MakeJsonStructDefinitionField(TEXT("ItemId"), TBaseStructure<FGuid>::Get())
		}
	));

	// ItemSelectedResponse
	JsonObjectDefinitions.Add(TEXT("ItemSelectedResponse"), MakeJsonObjectDefinition(
		{
			MakeJsonStructDefinitionField(TEXT("ItemId"), TBaseStructure<FGuid>::Get())
		}
	));
}

#endif
