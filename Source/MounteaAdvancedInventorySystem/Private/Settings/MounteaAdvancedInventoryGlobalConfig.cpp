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
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"

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
	const TCHAR* Description,
	TArray<FMounteaJsonObjectDefinitionField> Fields,
	TArray<FMounteaJsonObjectDefinitionInclude> IncludedDefinitions = {})
{
	FMounteaJsonObjectDefinition definition;
	definition.Description = FText::FromString(Description);
	definition.Fields = MoveTemp(Fields);
	definition.IncludedDefinitions = MoveTemp(IncludedDefinitions);

	return definition;
}

#if WITH_EDITOR

void UMounteaAdvancedInventoryGlobalConfig::SetupDefaults()
{
	// ModalBaseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalBaseDefinition"), MakeJsonObjectDefinition(
		TEXT("Shared base for modal requests: the modal data and the modal type."),
		{
			MakeJsonStructDefinitionField(TEXT("modalData"), TBaseStructure<FMounteaModalsConfig>::Get()),
			MakeJsonStringDefinitionField(TEXT("modalType"))
		}
	));

	// ModalItemDefinition
	JsonObjectDefinitions.Add(TEXT("ModalItemDefinition"), MakeJsonObjectDefinition(
		TEXT("Adds the item GUID that a modal refers to."),
		{
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get())
		}
	));

	// ModalSliderDefinition
	JsonObjectDefinitions.Add(TEXT("ModalSliderDefinition"), MakeJsonObjectDefinition(
		TEXT("Adds the integer range used by a quantity slider modal."),
		{
			MakeJsonStructDefinitionField(TEXT("sliderRange"), TBaseStructure<FIntPoint>::Get())
		}
	));

	// ModalItemQuantityDefinition
	JsonObjectDefinitions.Add(TEXT("ModalItemQuantityDefinition"), MakeJsonObjectDefinition(
		TEXT("Request to choose an item quantity. Combines base, item and slider data with the selectable item action."),
		{
			MakeJsonSoftObjectPtrDefinitionField(TEXT("action"), UMounteaSelectableInventoryItemAction::StaticClass())
		},
		{
			MakeJsonDefinitionInclude(TEXT("ModalBaseDefinition")),
			MakeJsonDefinitionInclude(TEXT("ModalItemDefinition")),
			MakeJsonDefinitionInclude(TEXT("ModalSliderDefinition"))
		}
	));

	// ModalResponseBaseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalResponseBaseDefinition"), MakeJsonObjectDefinition(
		TEXT("Shared base for modal responses: the answering modal type and whether it was confirmed."),
		{
			MakeJsonStringDefinitionField(TEXT("modalType")),
			MakeJsonBooleanDefinitionField(TEXT("confirmed"))
		}
	));

	// ModalItemResponseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalItemResponseDefinition"), MakeJsonObjectDefinition(
		TEXT("Modal response for an item: adds the item GUID to the base response."),
		{
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get())
		},
		{
			MakeJsonDefinitionInclude(TEXT("ModalResponseBaseDefinition"))
		}
	));

	// ModalQuantityResponseDefinition
	JsonObjectDefinitions.Add(TEXT("ModalQuantityResponseDefinition"), MakeJsonObjectDefinition(
		TEXT("Modal response for an item quantity: adds the chosen quantity."),
		{
			MakeJsonIntDefinitionField(TEXT("quantity"))
		},
		{
			MakeJsonDefinitionInclude(TEXT("ModalItemResponseDefinition"))
		}
	));

	// CategorySelectedRequest
	JsonObjectDefinitions.Add(TEXT("CategorySelectedRequest"), MakeJsonObjectDefinition(
		TEXT("Request sent when a category is selected: the category widget and its ID."),
		{
			MakeJsonUObjectDefinitionField(TEXT("CategoryWidget"), UObject::StaticClass()),
			MakeJsonStringDefinitionField(TEXT("CategoryId"))
		}
	));

	// CategorySelectedResponse
	JsonObjectDefinitions.Add(TEXT("CategorySelectedResponse"), MakeJsonObjectDefinition(
		TEXT("Response to a category selection: the selected category ID."),
		{
			MakeJsonStringDefinitionField(TEXT("CategoryId"))
		}
	));

	// CategoriesCreateRequest
	JsonObjectDefinitions.Add(TEXT("CategoriesCreateRequest"), MakeJsonObjectDefinition(
		TEXT("Request to create category widgets: the widget class and the registrar object."),
		{
			MakeJsonDefinitionField(TEXT("CategoryWidgetClass"), TEXT("softclass"), UUserWidget::StaticClass()),
			MakeJsonUObjectDefinitionField(TEXT("RegistrarObject"))
		}
	));

	// ItemsCreateRequest
	JsonObjectDefinitions.Add(TEXT("ItemsCreateRequest"), MakeJsonObjectDefinition(
		TEXT("Request to create item widgets for a category: widget class, registrar, category ID and item GUIDs."),
		{
			MakeJsonDefinitionField(TEXT("ItemWidgetClass"), TEXT("softclass"), UUserWidget::StaticClass()),
			MakeJsonUObjectDefinitionField(TEXT("RegistrarObject")),
			MakeJsonStringDefinitionField(TEXT("categoryId")),
			MakeJsonArrayDefinitionField(TEXT("itemGuids"), TEXT("struct"), TBaseStructure<FGuid>::Get())
		}
	));

	// ItemCreateRequest
	JsonObjectDefinitions.Add(TEXT("ItemCreateRequest"), MakeJsonObjectDefinition(
		TEXT("Request to create a single item widget: registrar, category ID and item GUID."),
		{
			MakeJsonUObjectDefinitionField(TEXT("RegistrarObject")),
			MakeJsonStringDefinitionField(TEXT("categoryId")),
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get())
		}
	));

	// ItemSelectedRequest
	JsonObjectDefinitions.Add(TEXT("ItemSelectedRequest"), MakeJsonObjectDefinition(
		TEXT("Request sent when an item is selected: the item widget, its GUID and parent category."),
		{
			MakeJsonUObjectDefinitionField(TEXT("ItemWidget"), UObject::StaticClass()),
			MakeJsonStructDefinitionField(TEXT("ItemId"), TBaseStructure<FGuid>::Get()),
			MakeJsonStringDefinitionField(TEXT("ParentCategory"))
		}
	));

	// ItemSelectedResponse
	JsonObjectDefinitions.Add(TEXT("ItemSelectedResponse"), MakeJsonObjectDefinition(
		TEXT("Response to an item selection: the item GUID and parent category."),
		{
			MakeJsonStructDefinitionField(TEXT("ItemId"), TBaseStructure<FGuid>::Get()),
			MakeJsonStringDefinitionField(TEXT("ParentCategory"))
		}
	));

	// InventoryItemChangePayload
	JsonObjectDefinitions.Add(TEXT("InventoryItemChangePayload"), MakeJsonObjectDefinition(
		TEXT("Payload for an inventory item change: the item GUID."),
		{
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get())
		}
	));

	// InventoryItemQuantityChangePayload
	JsonObjectDefinitions.Add(TEXT("InventoryItemQuantityChangePayload"), MakeJsonObjectDefinition(
		TEXT("Payload for an inventory item quantity change: adds the new quantity."),
		{
			MakeJsonIntDefinitionField(TEXT("quantity"))
		},
		{
			MakeJsonDefinitionInclude(TEXT("InventoryItemChangePayload"))
		}
	));

	// CraftingIngredientPayload
	JsonObjectDefinitions.Add(TEXT("CraftingIngredientPayload"), MakeJsonObjectDefinition(
		TEXT("Crafting ingredient payload: the ingredient template and its required quantities."),
		{
			MakeJsonUObjectDefinitionField(TEXT("ingredientTemplate"), UMounteaInventoryItemTemplate::StaticClass()),
			MakeJsonStructDefinitionField(TEXT("quantities"), TBaseStructure<FIntPoint>::Get())
		}
	));

	// CraftingRecipePayload
	JsonObjectDefinitions.Add(TEXT("CraftingRecipePayload"), MakeJsonObjectDefinition(
		TEXT("Crafting recipe payload: the recipe template and whether the ingredients are sufficient."),
		{
			MakeJsonUObjectDefinitionField(TEXT("recipeTemplate"), UMounteaRecipeTemplate::StaticClass()),
			MakeJsonBooleanDefinitionField(TEXT("hasEnoughIngredients"))
		}
	));

	// ItemAction
	JsonObjectDefinitions.Add(TEXT("ItemAction"), MakeJsonObjectDefinition(
		TEXT("Payload for an item action: the owning inventory and the item GUID."),
		{
			MakeJsonUObjectDefinitionField(TEXT("owningInventory"), UMounteaAdvancedInventoryInterface::StaticClass()),
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get())
		}
	));

	// ItemSelected_ItemActions
	JsonObjectDefinitions.Add(TEXT("ItemSelected_ItemActions"), MakeJsonObjectDefinition(
		TEXT("Payload for showing the actions of a selected item: inventory, item GUID, item actions and action widget class."),
		{
			MakeJsonDefinitionField(TEXT("inventory"), TEXT("interface"), UMounteaAdvancedInventoryInterface::StaticClass()),
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get()),
			MakeJsonArrayDefinitionField(TEXT("itemActions"), TEXT("object"), UMounteaSelectableInventoryItemAction::StaticClass()),
			MakeJsonDefinitionField(TEXT("itemActionWidgetClass"), TEXT("softclass"), UUserWidget::StaticClass())
		}
	));

	// ItemSelected_ItemAction
	JsonObjectDefinitions.Add(TEXT("ItemSelected_ItemAction"), MakeJsonObjectDefinition(
		TEXT("Payload for a single selected item action: inventory, item GUID, action widget class and action."),
		{
			MakeJsonDefinitionField(TEXT("inventory"), TEXT("interface"), UMounteaAdvancedInventoryInterface::StaticClass()),
			MakeJsonStructDefinitionField(TEXT("itemGuid"), TBaseStructure<FGuid>::Get()),
			MakeJsonDefinitionField(TEXT("itemActionWidgetClass"), TEXT("softclass"), UUserWidget::StaticClass()),
			MakeJsonUObjectDefinitionField(TEXT("itemAction"), UMounteaSelectableInventoryItemAction::StaticClass())
		}
	));
}

#endif
