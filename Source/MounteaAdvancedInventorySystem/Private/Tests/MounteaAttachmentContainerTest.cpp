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

#include "Components/MounteaAttachmentContainerComponent.h"
#include "Components/SceneComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Tests/MounteaInventoryTestFixtures.h"

namespace
{
	using MounteaInventoryTest::FInventoryWorldFixture;

	// Guarantees a registered root so the actor can be an attachment target or an attachable.
	void EnsureRoot(AActor* Actor)
	{
		if (Actor->GetRootComponent())
			return;
		USceneComponent* Root = NewObject<USceneComponent>(Actor, TEXT("TestRoot"));
		Actor->SetRootComponent(Root);
		Root->RegisterComponent();
	}

	// Container with one component-type slot ("SlotA"), target set directly (BeginPlay resolution is not relied on).
	UMounteaAttachmentContainerComponent* MakeContainer(FInventoryWorldFixture& Fixture, AActor*& OutOwner)
	{
		OutOwner = Fixture.SpawnActor();
		EnsureRoot(OutOwner);
		// Target must be set before registration; the container logs when it registers with no target.
		UMounteaAttachmentContainerComponent* Container = NewObject<UMounteaAttachmentContainerComponent>(OutOwner);
		Container->DefaultAttachmentTargetComponent = OutOwner->GetRootComponent();
		Container->DefaultAttachmentTarget = OutOwner->GetRootComponent()->GetFName(); // BeginPlay resolves the target by name
		OutOwner->AddInstanceComponent(Container);
		Container->RegisterComponent();

		UMounteaAdvancedAttachmentSlot* Slot = NewObject<UMounteaAdvancedAttachmentSlot>(Container);
		Slot->SlotName = TEXT("SlotA");
		Slot->SlotType = EAttachmentSlotType::EAST_Component; // socket type would require a real socket
		Slot->InitializeAttachmentSlot(Container);
		Container->AttachmentSlots.Add(Slot);
		return Container;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaAttachmentIsValidSlotTest, "Mountea.InventorySystem.Attachments.IsValidSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaAttachmentIsValidSlotTest::RunTest(const FString& Parameters)
{
	FInventoryWorldFixture Fixture;
	if (!Fixture.Setup(*this))
		return false;

	AActor* Owner = nullptr;
	UMounteaAttachmentContainerComponent* Container = MakeContainer(Fixture, Owner);

	TestTrue(TEXT("Configured slot is valid"), Container->Execute_IsValidSlot(Container, TEXT("SlotA")));
	TestFalse(TEXT("Unknown slot is invalid"), Container->Execute_IsValidSlot(Container, TEXT("Missing")));

	Fixture.Teardown(*this);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaAttachmentAttachDetachTest, "Mountea.InventorySystem.Attachments.AttachDetach",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaAttachmentAttachDetachTest::RunTest(const FString& Parameters)
{
	FInventoryWorldFixture Fixture;
	if (!Fixture.Setup(*this))
		return false;

	AActor* Owner = nullptr;
	UMounteaAttachmentContainerComponent* Container = MakeContainer(Fixture, Owner);
	AActor* Attachable = Fixture.SpawnActor();
	EnsureRoot(Attachable);
	EnsureRoot(Attachable);

	TestFalse(TEXT("Slot starts empty"), Container->Execute_IsSlotOccupied(Container, TEXT("SlotA")));
	TestTrue(TEXT("TryAttach to empty slot succeeds"), Container->Execute_TryAttach(Container, TEXT("SlotA"), Attachable));
	TestTrue(TEXT("Slot reports occupied"), Container->Execute_IsSlotOccupied(Container, TEXT("SlotA")));

	const UMounteaAdvancedAttachmentSlot* Slot = Container->Execute_GetSlot(Container, TEXT("SlotA"));
	if (TestNotNull(TEXT("GetSlot returns slot"), Slot))
		TestEqual(TEXT("Slot holds the attachment"), Slot->Attachment.Get(), static_cast<UObject*>(Attachable));

	TestTrue(TEXT("TryDetach succeeds"), Container->Execute_TryDetach(Container, TEXT("SlotA")));
	TestFalse(TEXT("Slot is empty after detach"), Container->Execute_IsSlotOccupied(Container, TEXT("SlotA")));

	Fixture.Teardown(*this);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaAttachmentRejectTest, "Mountea.InventorySystem.Attachments.RejectInvalidAttach",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaAttachmentRejectTest::RunTest(const FString& Parameters)
{
	FInventoryWorldFixture Fixture;
	if (!Fixture.Setup(*this))
		return false;

	AActor* Owner = nullptr;
	UMounteaAttachmentContainerComponent* Container = MakeContainer(Fixture, Owner);
	AActor* Attachable = Fixture.SpawnActor();
	EnsureRoot(Attachable);
	EnsureRoot(Attachable);

	// Occupied slots are NOT rejected by the code (the old attachment is replaced); only locked slots, unknown ids and null are.
	TestFalse(TEXT("Null attachment is rejected"), Container->Execute_TryAttach(Container, TEXT("SlotA"), nullptr));
	TestFalse(TEXT("Unknown slot is rejected"), Container->Execute_TryAttach(Container, TEXT("Missing"), Attachable));

	TestTrue(TEXT("DisableSlot succeeds"), Container->Execute_DisableSlot(Container, TEXT("SlotA")));
	TestFalse(TEXT("Locked slot rejects attach"), Container->Execute_TryAttach(Container, TEXT("SlotA"), Attachable));
	TestFalse(TEXT("Locked slot stays unoccupied"), Container->Execute_IsSlotOccupied(Container, TEXT("SlotA")));

	Fixture.Teardown(*this);
	return true;
}

#endif
