// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/MounteaInventoryComponent.h"
#include "Components/SceneComponent.h"
#include "Helpers/MounteaJsonObject.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Statics/MounteaAdvancedInventoryJsonStatics.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaJsonObjectRoundTripTest, "Mountea.InventorySystem.JsonObject.RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaJsonObjectRoundTripTest::RunTest(const FString& Parameters)
{
	UMounteaJsonObject* Json = NewObject<UMounteaJsonObject>(GetTransientPackage());
	UMounteaJsonObject* Out = nullptr;

	const FGuid Guid = FGuid::NewGuid();
	TestTrue(TEXT("Set string"), Json->SetStringField(TEXT("S"), TEXT("abc"), Out));
	TestTrue(TEXT("Set int"), Json->SetIntField(TEXT("I"), 42, Out));
	TestTrue(TEXT("Set bool"), Json->SetBoolField(TEXT("B"), true, Out));
	TestTrue(TEXT("Set guid"), Json->SetGuidField(TEXT("G"), Guid, Out));

	FString S; int32 I = 0; bool B = false; FGuid G;
	TestTrue(TEXT("Get string"), Json->GetStringField(TEXT("S"), S));
	TestEqual(TEXT("String value"), S, FString(TEXT("abc")));
	TestTrue(TEXT("Get int"), Json->GetIntField(TEXT("I"), I));
	TestEqual(TEXT("Int value"), I, 42);
	TestTrue(TEXT("Get bool"), Json->GetBoolField(TEXT("B"), B));
	TestTrue(TEXT("Bool value"), B);
	TestTrue(TEXT("Get guid"), Json->GetGuidField(TEXT("G"), G));
	TestEqual(TEXT("Guid value"), G, Guid);

	TestFalse(TEXT("Missing field not readable"), Json->GetStringField(TEXT("Nope"), S));
	TestFalse(TEXT("Wrong type not readable"), Json->GetIntField(TEXT("S"), I));

	TestTrue(TEXT("Remove field"), Json->RemoveField(TEXT("S"), Out));
	TestFalse(TEXT("Removed field gone"), Json->HasField(TEXT("S")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaJsonObjectInterfaceFieldTest, "Mountea.InventorySystem.JsonObject.InterfaceField",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaJsonObjectInterfaceFieldTest::RunTest(const FString& Parameters)
{
	FEdGraphPinType interfacePin;
	interfacePin.PinCategory = FName(TEXT("interface"));
	interfacePin.PinSubCategoryObject = UMounteaAdvancedInventoryInterface::StaticClass();
	TestTrue(TEXT("Interface pin supported"), UMounteaAdvancedInventoryJsonStatics::IsSupportedJsonDefinitionFieldPinType(interfacePin));

	FEdGraphPinType mapPin = interfacePin;
	mapPin.ContainerType = EPinContainerType::Map;
	TestFalse(TEXT("Map pin unsupported"), UMounteaAdvancedInventoryJsonStatics::IsSupportedJsonDefinitionFieldPinType(mapPin));

	FMounteaJsonObjectDefinitionField field;
	field.FieldName = TEXT("Owner");
	field.FieldValueType = interfacePin;
	FMounteaJsonObjectDefinition definition;
	definition.Fields.Add(field);

	UMounteaJsonObject* json = NewObject<UMounteaJsonObject>(GetTransientPackage());
	UMounteaJsonObject* out = nullptr;
	UObject* implementer = NewObject<UMounteaInventoryComponent>(GetTransientPackage());
	TArray<FString> errors;

	TestTrue(TEXT("Store implementer"), UMounteaAdvancedInventoryJsonStatics::SetObjectJsonField(json, TEXT("Owner"), implementer, out));
	UObject* readBack = nullptr;
	TestTrue(TEXT("Read implementer"), UMounteaAdvancedInventoryJsonStatics::GetObjectJsonField(json, TEXT("Owner"), readBack));
	TestEqual(TEXT("Same object"), readBack, implementer);
	TestTrue(TEXT("Implementer validates"), UMounteaAdvancedInventoryJsonStatics::ValidateJsonObjectAgainstDefinition(json, definition, errors));

	UObject* nonImplementer = NewObject<USceneComponent>(GetTransientPackage());
	TestTrue(TEXT("Store non-implementer"), UMounteaAdvancedInventoryJsonStatics::SetObjectJsonField(json, TEXT("Owner"), nonImplementer, out));
	TestFalse(TEXT("Non-implementer rejected"), UMounteaAdvancedInventoryJsonStatics::ValidateJsonObjectAgainstDefinition(json, definition, errors));
	return true;
}

#endif
