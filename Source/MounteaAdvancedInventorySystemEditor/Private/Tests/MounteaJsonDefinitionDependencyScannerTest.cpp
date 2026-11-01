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

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Helpers/MounteaJsonDefinitionDependencyScanner.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Slate/SMounteaJsonDefinitionDependencyList.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaJsonDefinitionKeyLiteralCountTest,
	"Mountea.InventorySystemEditor.JsonDefinition.KeyLiteralCount",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaJsonDefinitionKeyLiteralCountTest::RunTest(const FString& Parameters)
{
	const FString content = TEXT("Make(\"Item\"); Make(\"ItemX\"); Make(\"Item\");");

	TestEqual(TEXT("Quoted key counted, prefix matches ignored"), FMounteaJsonDefinitionDependencyScanner::CountKeyLiterals(content, TEXT("Item")), 2);
	TestEqual(TEXT("Missing key counts zero"), FMounteaJsonDefinitionDependencyScanner::CountKeyLiterals(content, TEXT("Recipe")), 0);
	TestEqual(TEXT("Empty key counts zero"), FMounteaJsonDefinitionDependencyScanner::CountKeyLiterals(content, FString()), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaJsonDefinitionSetupDefaultsTest,
	"Mountea.InventorySystemEditor.JsonDefinition.SetupDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaJsonDefinitionSetupDefaultsTest::RunTest(const FString& Parameters)
{
	UMounteaAdvancedInventoryGlobalConfig* config = NewObject<UMounteaAdvancedInventoryGlobalConfig>(GetTransientPackage());
	config->SetupDefaults();

	TestEqual(TEXT("Default definition count"), config->JsonObjectDefinitions.Num(), 21);
	TestFalse(TEXT("Unused key removed"), config->JsonObjectDefinitions.Contains(TEXT("CraftingAction")));

	for (const TPair<FString, FMounteaJsonObjectDefinition>& pair : config->JsonObjectDefinitions)
	{
		TestFalse(*FString::Printf(TEXT("%s has a description"), *pair.Key), pair.Value.Description.IsEmpty());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaJsonDefinitionGroupBlueprintUsagesTest,
	"Mountea.InventorySystemEditor.JsonDefinition.GroupBlueprintUsages",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaJsonDefinitionGroupBlueprintUsagesTest::RunTest(const FString& Parameters)
{
	TArray<FMounteaJsonDefinitionUsage> usages;
	usages.Add({ TEXT("Blueprint"), TEXT("/Game/B/Zed.Zed"), 1 });
	usages.Add({ TEXT("Blueprint"), TEXT("/Game/A/Yak.Yak"), 2 });
	usages.Add({ TEXT("Blueprint"), TEXT("/Game/A/Ant.Ant"), 1 });
	usages.Add({ TEXT("C++"), TEXT("C:/Src/Foo.cpp"), 3 });

	const TArray<FMounteaJsonDependencyGroup> groups = SMounteaJsonDefinitionDependencyList::GroupBlueprintUsages(usages);
	TestEqual(TEXT("C++ excluded, two folders"), groups.Num(), 2);
	if (groups.Num() != 2)
	{
		return true;
	}

	TestEqual(TEXT("Folders sorted"), groups[0].FolderPath, FString(TEXT("/Game/A")));
	TestEqual(TEXT("Usages in folder"), groups[0].Usages.Num(), 2);
	TestEqual(TEXT("Usages sorted by path"), groups[0].Usages[0].Path, FString(TEXT("/Game/A/Ant.Ant")));
	TestEqual(TEXT("Count kept"), groups[0].Usages[1].Count, 2);
	TestEqual(TEXT("Second folder"), groups[1].FolderPath, FString(TEXT("/Game/B")));
	return true;
}

#endif
