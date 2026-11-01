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

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Helpers/MounteaJsonDefinitionDependencyScanner.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Slate/SMounteaJsonDefinitionDependencyList.h"
#include "Helpers/MounteaJsonDefinitionDeletionCheck.h"

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaJsonDefinitionDeletionCheckTest,
	"Mountea.InventorySystemEditor.JsonDefinition.DeletionCheck",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaJsonDefinitionDeletionCheckTest::RunTest(const FString& Parameters)
{
	UMounteaAdvancedInventoryGlobalConfig* config = NewObject<UMounteaAdvancedInventoryGlobalConfig>(GetTransientPackage());
	FMounteaJsonObjectDefinition& parent = config->JsonObjectDefinitions.Add(TEXT("Parent"));
	FMounteaJsonObjectDefinitionInclude include;
	include.DefinitionKey = TEXT("Child");
	parent.IncludedDefinitions.Add(include);
	config->JsonObjectDefinitions.Add(TEXT("Child"));
	config->JsonObjectDefinitions.Add(TEXT("Lonely"));

	const FMounteaJsonDefinitionDeletionReport childReport = FMounteaJsonDefinitionDeletionCheck::Check(*config, TEXT("Child"), {});
	TestTrue(TEXT("Included key blocks deletion"), childReport.HasBlockers());
	TestEqual(TEXT("Includer reported"), childReport.IncludedBy.Num(), 1);

	const FMounteaJsonDefinitionDeletionReport lonelyReport = FMounteaJsonDefinitionDeletionCheck::Check(*config, TEXT("Lonely"), {});
	TestFalse(TEXT("Unused key can be deleted"), lonelyReport.HasBlockers());

	TArray<FMounteaJsonDefinitionUsage> usages;
	for (int32 index = 0; index < 12; index++)
	{
		usages.Add({ TEXT("C++"), FString::Printf(TEXT("File%d.cpp"), index), 1 });
	}

	const FMounteaJsonDefinitionDeletionReport usedReport = FMounteaJsonDefinitionDeletionCheck::Check(*config, TEXT("Lonely"), usages);
	TestTrue(TEXT("Usages block deletion"), usedReport.HasBlockers());
	TestTrue(TEXT("Describe lists at most 10 entries"), FMounteaJsonDefinitionDeletionCheck::Describe(TEXT("Lonely"), usedReport).ToString().Contains(TEXT("and 2 more")));
	return true;
}

#endif
