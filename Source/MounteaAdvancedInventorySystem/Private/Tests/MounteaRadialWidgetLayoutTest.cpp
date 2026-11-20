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

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Input/HittestGrid.h"
#include "Layout/Geometry.h"
#include "Rendering/DrawElements.h"
#include "Statics/MounteaInventoryBaseUIStatics.h"
#include "Styling/WidgetStyle.h"
#include "Types/PaintArgs.h"
#include "Widgets/MounteaAdvancedInventoryBaseWidget.h"

#include <limits>

namespace MounteaRadialWidgetLayoutTest
{
	UUserWidget* AddChild(UCanvasPanel* Canvas)
	{
		auto* child = NewObject<UMounteaAdvancedInventoryBaseWidget>();
		Canvas->AddChildToCanvas(child);
		return child;
	}

	bool TestPosition(FAutomationTestBase& Test, const TCHAR* Description,
		const TMap<int32, FMounteaTransformedWidgetPositionData>& Layout, int32 Key, const FVector2D& Expected)
	{
		const auto* entry = Layout.Find(Key);

		if (!Test.TestNotNull(Description, entry))
			return false;

		return Test.TestTrue(Description, entry->Position.Equals(Expected, 0.001));
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRadialWidgetGeometryTest,
	"Mountea.Inventory.UI.Layout.Radial.GeometryAndOrdering",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaRadialWidgetGeometryTest::RunTest(const FString& Parameters)
{
	using namespace MounteaRadialWidgetLayoutTest;
	auto* canvas = NewObject<UCanvasPanel>();
	auto* first = AddChild(canvas);
	auto* middle = AddChild(canvas);
	auto* last = AddChild(canvas);
	TMap<int32, UUserWidget*> children;
	children.Add(90, last);
	children.Add(-7, first);
	children.Add(12, middle);
	const double diagonal = 100.0 / FMath::Sqrt(2.0);
	const auto below = UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, children, 100.f);
	TestEqual(TEXT("Three children produce three transforms"), below.Num(), 3);
	TestPosition(*this, TEXT("Lowest key starts 45 degrees before Below"), below, -7, FVector2D(diagonal, diagonal));
	TestPosition(*this, TEXT("Middle key lies directly Below"), below, 12, FVector2D(0.0, 100.0));
	TestPosition(*this, TEXT("Highest key ends 45 degrees after Below"), below, 90, FVector2D(-diagonal, diagonal));

	for (const auto& pair : below)
	{
		TestEqual(TEXT("Transform retains its widget"), pair.Value.Widget.Get(), children.FindRef(pair.Key));
		TestEqual(TEXT("Calculated widgets stay upright"), pair.Value.RotationAngle, 0.f);
		TestTrue(TEXT("Every widget center is on the requested ring"), FMath::IsNearlyEqual(pair.Value.Position.Size(), 100.0, 0.001));
	}

	const auto above = UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, children, 100.f, EMounteaRadialArcDirection::Above);
	TestPosition(*this, TEXT("Above starts in the upper left"), above, -7, FVector2D(-diagonal, -diagonal));
	TestPosition(*this, TEXT("Above midpoint"), above, 12, FVector2D(0.0, -100.0));
	const auto left = UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, children, 100.f, EMounteaRadialArcDirection::Left);
	TestPosition(*this, TEXT("Left starts in the lower left"), left, -7, FVector2D(-diagonal, diagonal));
	TestPosition(*this, TEXT("Left midpoint"), left, 12, FVector2D(-100.0, 0.0));
	const auto right = UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, children, 100.f, EMounteaRadialArcDirection::Right);
	TestPosition(*this, TEXT("Right starts in the upper right"), right, -7, FVector2D(diagonal, -diagonal));
	TestPosition(*this, TEXT("Right midpoint"), right, 12, FVector2D(100.0, 0.0));

	TMap<int32, UUserWidget*> single;
	single.Add(12, middle);
	TestPosition(*this, TEXT("Single child uses the direction midpoint"), UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, single, 100.f), 12, FVector2D(0.0, 100.0));

	for (const int32 count : {2, 5, 7, 8, 9, 10, 12})
	{
		TMap<int32, UUserWidget*> ringChildren;

		for (int32 index = count - 1; index >= 0; --index)
		{
			ringChildren.Add(index, AddChild(canvas));
		}

		const auto ring = UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, ringChildren, 100.f);
		TestEqual(TEXT("All ring children receive transforms"), ring.Num(), count);
		const double expectedDot = count <= 8 ? FMath::Sqrt(0.5) : FMath::Cos(2.0 * PI / count);

		for (int32 index = 0; index < count - 1; ++index)
		{
			const auto* current = ring.Find(index);
			const auto* next = ring.Find(index + 1);

			if (current && next)
				TestTrue(TEXT("Adjacent spacing uses 45 degrees or the full-circle step"), FMath::IsNearlyEqual(FVector2D::DotProduct(current->Position, next->Position) / 10000.0, expectedDot, 0.0001));
		}

		if (count >= 8 && ring.Contains(0) && ring.Contains(count - 1))
		{
			TestFalse(TEXT("Full circles have distinct first and last positions"), ring[0].Position.Equals(ring[count - 1].Position, 0.001));
			TestTrue(TEXT("Closing gap matches the other full-circle gaps"), FMath::IsNearlyEqual(FVector2D::DotProduct(ring[0].Position, ring[count - 1].Position) / 10000.0, expectedDot, 0.0001));
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRadialWidgetFilteringTest,
	"Mountea.Inventory.UI.Layout.Radial.FilteringAndInvalidInput",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaRadialWidgetFilteringTest::RunTest(const FString& Parameters)
{
	using namespace MounteaRadialWidgetLayoutTest;
	auto* canvas = NewObject<UCanvasPanel>();
	auto* otherCanvas = NewObject<UCanvasPanel>();
	auto* first = AddChild(canvas);
	auto* second = AddChild(canvas);
	auto* detached = NewObject<UMounteaAdvancedInventoryBaseWidget>();
	auto* foreign = AddChild(otherCanvas);
	auto* slot = CastChecked<UCanvasPanelSlot>(first->Slot);
	slot->SetPosition(FVector2D(11.0, 22.0));
	first->SetRenderTransformAngle(31.f);
	first->SetVisibility(ESlateVisibility::Hidden);
	second->SetVisibility(ESlateVisibility::Collapsed);
	TMap<int32, UUserWidget*> children;
	children.Add(50, first);
	children.Add(20, second);
	children.Add(10, first);
	children.Add(-3, nullptr);
	children.Add(-2, detached);
	children.Add(-1, foreign);
	const auto layout = UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, children, 100.f);
	TestEqual(TEXT("Only distinct direct children participate"), layout.Num(), 2);
	TestTrue(TEXT("A duplicate widget retains its lowest key"), layout.Contains(10));
	TestFalse(TEXT("The higher duplicate key is omitted"), layout.Contains(50));
	TestTrue(TEXT("Visibility does not remove supplied members"), layout.Contains(20));
	const double x = 100.0 * FMath::Sin(PI / 8.0);
	const double y = 100.0 * FMath::Cos(PI / 8.0);
	TestPosition(*this, TEXT("Spacing counts eligible unique children only"), layout, 10, FVector2D(x, y));
	TestEqual(TEXT("Calculation leaves slot position unchanged"), slot->GetPosition(), FVector2D(11.0, 22.0));
	TestEqual(TEXT("Calculation leaves widget rotation unchanged"), first->GetRenderTransformAngle(), 31.f);
	TestEqual(TEXT("Calculation does not attach detached widgets"), detached->GetParent(), static_cast<UPanelWidget*>(nullptr));
	TestEqual(TEXT("Calculation does not reparent foreign widgets"), foreign->GetParent(), static_cast<UPanelWidget*>(otherCanvas));

	const TMap<int32, UUserWidget*> empty;
	TestTrue(TEXT("Empty input calculates an empty result"), UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, empty, 100.f).IsEmpty());
	TestFalse(TEXT("Empty input applies nothing"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, empty, 100.f));
	TestTrue(TEXT("Null canvas calculates an empty result"), UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(nullptr, children, 100.f).IsEmpty());
	TestFalse(TEXT("Null canvas applies nothing"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(nullptr, children, 100.f));

	for (const float radius : {0.f, -100.f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
	{
		TestTrue(TEXT("Invalid radius calculates an empty result"), UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, children, radius).IsEmpty());
		TestFalse(TEXT("Invalid radius applies nothing"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, children, radius));
	}

	TestEqual(TEXT("Invalid calls leave the original slot intact"), slot->GetPosition(), FVector2D(11.0, 22.0));
	TestEqual(TEXT("Invalid calls leave original rotation intact"), first->GetRenderTransformAngle(), 31.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRadialWidgetApplyTest,
	"Mountea.Inventory.UI.Layout.Radial.ApplyRepeatAndReflow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaRadialWidgetApplyTest::RunTest(const FString& Parameters)
{
	using namespace MounteaRadialWidgetLayoutTest;
	auto* canvas = NewObject<UCanvasPanel>();
	TMap<int32, UUserWidget*> children;
	children.Add(3, AddChild(canvas));
	children.Add(1, AddChild(canvas));
	children.Add(2, AddChild(canvas));

	for (const auto& pair : children)
	{
		auto* slot = CastChecked<UCanvasPanelSlot>(pair.Value->Slot);
		slot->SetAnchors(FAnchors(0.f));
		slot->SetAlignment(FVector2D::ZeroVector);
		slot->SetPosition(FVector2D(200.0, 300.0));
		slot->SetAutoSize(false);
		pair.Value->SetRenderTransformAngle(45.f);
		pair.Value->SetRenderTranslation(FVector2D(7.0, 9.0));
		pair.Value->SetRenderTransformPivot(FVector2D(0.2, 0.3));
	}

	TestTrue(TEXT("Valid radial layout applies before the first arranged frame"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, children, 100.f));
	const auto expected = UMounteaInventoryBaseUIStatics::CalculateRadialWidgetLayout(canvas, children, 100.f);

	for (const auto& pair : children)
	{
		auto* slot = CastChecked<UCanvasPanelSlot>(pair.Value->Slot);
		TestEqual(TEXT("Minimum anchor is Canvas center"), slot->GetAnchors().Minimum, FVector2D(0.5));
		TestEqual(TEXT("Maximum anchor is Canvas center"), slot->GetAnchors().Maximum, FVector2D(0.5));
		TestEqual(TEXT("Slot alignment is centered"), slot->GetAlignment(), FVector2D(0.5));
		TestEqual(TEXT("Authored render pivot is preserved"), pair.Value->GetRenderTransformPivot(), FVector2D(0.2, 0.3));
		TestTrue(TEXT("Auto-size is enabled by default"), slot->GetAutoSize());
		TestTrue(TEXT("Applied position matches calculation"), slot->GetPosition().Equals(expected.FindChecked(pair.Key).Position, 0.001));
		TestEqual(TEXT("Application removes prior widget rotation"), pair.Value->GetRenderTransformAngle(), 0.f);
		TestEqual(TEXT("Authored render translation is preserved"), pair.Value->GetRenderTransform().Translation, FVector2D(7.0, 9.0));
	}

	TestTrue(TEXT("Repeated application succeeds"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, children, 100.f));

	for (const auto& pair : children)
	{
		TestTrue(TEXT("Repeating does not accumulate position"), CastChecked<UCanvasPanelSlot>(pair.Value->Slot)->GetPosition().Equals(expected.FindChecked(pair.Key).Position, 0.001));
	}

	auto* removed = children.FindChecked(2);
	canvas->RemoveChild(removed);
	TestTrue(TEXT("A stale map can reflow after a child is removed"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, children, 100.f));
	const double x = 100.0 * FMath::Sin(PI / 8.0);
	const double y = 100.0 * FMath::Cos(PI / 8.0);
	TestTrue(TEXT("Remaining first child uses the two-item spacing"), CastChecked<UCanvasPanelSlot>(children.FindChecked(1)->Slot)->GetPosition().Equals(FVector2D(x, y), 0.001));
	TestTrue(TEXT("Remaining last child closes the two-item arc"), CastChecked<UCanvasPanelSlot>(children.FindChecked(3)->Slot)->GetPosition().Equals(FVector2D(-x, y), 0.001));
	TestEqual(TEXT("Reflow preserves canvas membership"), canvas->GetChildrenCount(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRadialWidgetManualSizeTest,
	"Mountea.Inventory.UI.Layout.Radial.ManualSizing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaRadialWidgetManualSizeTest::RunTest(const FString& Parameters)
{
	using namespace MounteaRadialWidgetLayoutTest;
	auto* canvas = NewObject<UCanvasPanel>();
	auto* fixed = AddChild(canvas);
	auto* fixedSlot = CastChecked<UCanvasPanelSlot>(fixed->Slot);
	fixedSlot->SetSize(FVector2D(80.0, 35.0));
	fixedSlot->SetAutoSize(false);
	TMap<int32, UUserWidget*> children;
	children.Add(0, fixed);
	TestTrue(TEXT("Fixed manual size applies without geometry"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, children, 100.f, EMounteaRadialArcDirection::Below, false));
	TestEqual(TEXT("Fixed slot extent is preserved"), fixedSlot->GetSize(), FVector2D(80.0, 35.0));
	TestFalse(TEXT("Manual sizing leaves auto-size disabled"), fixedSlot->GetAutoSize());
	TestTrue(TEXT("Repeating manual layout succeeds"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, children, 100.f, EMounteaRadialArcDirection::Below, false));
	TestEqual(TEXT("Repeating preserves manual size"), fixedSlot->GetSize(), FVector2D(80.0, 35.0));

	auto* stretched = AddChild(canvas);
	auto* stretchedSlot = CastChecked<UCanvasPanelSlot>(stretched->Slot);
	stretchedSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	stretchedSlot->SetOffsets(FMargin(10.f, 20.f, 30.f, 40.f));
	stretchedSlot->SetAlignment(FVector2D(0.2, 0.3));
	stretchedSlot->SetAutoSize(false);
	stretched->SetRenderTransformAngle(17.f);
	TMap<int32, UUserWidget*> stretchedOnly;
	stretchedOnly.Add(1, stretched);
	TestFalse(TEXT("Manual stretched sizing waits for arranged Canvas geometry"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, stretchedOnly, 100.f, EMounteaRadialArcDirection::Below, false));
	TestEqual(TEXT("Deferred stretch retains minimum anchors"), stretchedSlot->GetAnchors().Minimum, FVector2D::ZeroVector);
	TestEqual(TEXT("Deferred stretch retains maximum anchors"), stretchedSlot->GetAnchors().Maximum, FVector2D(1.0));
	TestTrue(TEXT("Deferred stretch retains its margins"), stretchedSlot->GetOffsets() == FMargin(10.f, 20.f, 30.f, 40.f));
	TestEqual(TEXT("Deferred stretch retains alignment"), stretchedSlot->GetAlignment(), FVector2D(0.2, 0.3));
	TestEqual(TEXT("Deferred stretch retains rotation"), stretched->GetRenderTransformAngle(), 17.f);
	children.Add(1, stretched);
	TestTrue(TEXT("A deferred stretched child does not prevent fixed children from applying"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, children, 100.f, EMounteaRadialArcDirection::Below, false));
	TestTrue(TEXT("Size-to-content can place a stretched child without arranged geometry"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(canvas, stretchedOnly, 100.f));
	TestTrue(TEXT("Size-to-content enables auto-size on the former stretched slot"), stretchedSlot->GetAutoSize());
	TestEqual(TEXT("Size-to-content centers the former stretched slot anchors"), stretchedSlot->GetAnchors().Maximum, FVector2D(0.5));

	// Paint an empty Canvas into a rectangular local geometry to populate its real Slate cache.
	// No window, renderer submission, or mutation of private cached state is needed.
	auto* arrangedCanvas = NewObject<UCanvasPanel>();
	const auto slateCanvas = arrangedCanvas->TakeWidget();
	const FVector2D canvasSize(600.0, 240.0);
	FHittestGrid hitTestGrid;
	hitTestGrid.SetHittestArea(FVector2D::ZeroVector, canvasSize);
	const FPaintArgs paintArgs(nullptr, hitTestGrid, FVector2D::ZeroVector, 0.0, 0.f);
	FSlateWindowElementList drawElements(nullptr);
	slateCanvas->SlatePrepass(1.f);
	slateCanvas->Paint(paintArgs, FGeometry::MakeRoot(canvasSize, FSlateLayoutTransform()),
		FSlateRect(0.f, 0.f, 600.f, 240.f), drawElements, 0, FWidgetStyle(), true);
	TestTrue(TEXT("Rectangular Canvas geometry is cached"), FVector2D(arrangedCanvas->GetCachedGeometry().GetLocalSize()).Equals(canvasSize, 0.001));

	TMap<int32, UUserWidget*> arrangedChildren;
	TArray<UCanvasPanelSlot*> arrangedSlots;
	const FAnchors authoredAnchors[] =
	{
		FAnchors(0.f, 0.f, 1.f, 1.f),
		FAnchors(0.25f, 0.f, 0.75f, 0.f),
		FAnchors(0.f, 0.25f, 0.f, 0.75f)
	};
	const FMargin authoredOffsets[] =
	{
		FMargin(10.f, 20.f, 30.f, 40.f),
		FMargin(10.f, 20.f, 30.f, 80.f),
		FMargin(10.f, 20.f, 70.f, 40.f)
	};
	const FVector2D expectedSizes[] =
	{
		FVector2D(560.0, 180.0),
		FVector2D(260.0, 80.0),
		FVector2D(70.0, 60.0)
	};

	for (int32 index = 0; index < 3; ++index)
	{
		auto* child = AddChild(arrangedCanvas);
		auto* slot = CastChecked<UCanvasPanelSlot>(child->Slot);
		slot->SetAnchors(authoredAnchors[index]);
		slot->SetOffsets(authoredOffsets[index]);
		slot->SetAutoSize(false);
		arrangedChildren.Add(index, child);
		arrangedSlots.Add(slot);
	}

	for (int32 application = 0; application < 2; ++application)
	{
		TestTrue(TEXT("Manual stretched layout applies with rectangular geometry"), UMounteaInventoryBaseUIStatics::ApplyRadialWidgetLayout(arrangedCanvas, arrangedChildren, 100.f, EMounteaRadialArcDirection::Below, false));

		for (int32 index = 0; index < arrangedSlots.Num(); ++index)
		{
			TestTrue(TEXT("Manual stretch resolves its extent and remains stable on repeat"), arrangedSlots[index]->GetSize().Equals(expectedSizes[index], 0.001));
			TestEqual(TEXT("Resolved stretch uses fixed center anchors"), arrangedSlots[index]->GetAnchors().Minimum, FVector2D(0.5));
			TestEqual(TEXT("Resolved stretch no longer has trailing margin anchors"), arrangedSlots[index]->GetAnchors().Maximum, FVector2D(0.5));
			TestFalse(TEXT("Resolved manual stretch remains manually sized"), arrangedSlots[index]->GetAutoSize());
		}
	}

	return true;
}

#endif
