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

#include "Helpers/MounteaJsonObject.h"

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

#endif
