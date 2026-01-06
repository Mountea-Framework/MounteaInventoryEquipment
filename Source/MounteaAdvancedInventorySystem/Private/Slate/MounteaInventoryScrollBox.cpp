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


#include "Slate/MounteaInventoryScrollBox.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Statics/MounteaInventoryUIStatics.h"

void UMounteaInventoryScrollBox::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(true);
}

int32 UMounteaInventoryScrollBox::GetChildrenCount() const
{
	return VerticalBox ? VerticalBox->GetChildrenCount() : INDEX_NONE;
}

void UMounteaInventoryScrollBox::AddChild(UWidget* Content)
{
	if (!Content || !VerticalBox)
		return;
	VerticalBox->AddChild(Content);
}

void UMounteaInventoryScrollBox::ResetChildren()
{
	if (VerticalBox)
	{
		ActiveIndex = INDEX_NONE;
		VerticalBox->ClearChildren();
	}
	
	UnlockScroll();
}

TSharedRef<SWidget> UMounteaInventoryScrollBox::RebuildWidget()
{
	if (WidgetTree && !VerticalBox)
	{
		VerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("VerticalBox")
			);
		WidgetTree->RootWidget = VerticalBox;
	}
	
	return Super::RebuildWidget();
}

void UMounteaInventoryScrollBox::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	InterpolateToTarget(InDeltaTime);
}

FReply UMounteaInventoryScrollBox::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bCaptureInput)
		return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	
	if (bUseScrollDelay && bScrollLocked)
		return FReply::Handled();

	if (bUseScrollDelay)
	{
		bScrollLocked = true;

		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().SetTimer(
				ScrollDelayTimerHandle,
				this,
				&UMounteaInventoryScrollBox::UnlockScroll,
				ScrollDelay,
				false
			);
		}
	}

	const float wheelDelta = InMouseEvent.GetWheelDelta();
	BroadcastIndexChange(wheelDelta > 0 ? -1 : 1);

	return FReply::Handled();
}


FReply UMounteaInventoryScrollBox::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bCaptureInput)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	
	const FKey key = InKeyEvent.GetKey();

	// Keys that should move the selection
	const bool bUpKey =
		(key == EKeys::Up || key == EKeys::W || key == EKeys::Gamepad_DPad_Up);
	const bool bDownKey =
		(key == EKeys::Down || key == EKeys::S || key == EKeys::Gamepad_DPad_Down);

	if (!bUpKey && !bDownKey)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	// Delay
	if (bUseScrollDelay && bScrollLocked)
		return FReply::Handled();

	if (bUseScrollDelay)
	{
		bScrollLocked = true;

		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().SetTimer(
				ScrollDelayTimerHandle,
				this,
				&UMounteaInventoryScrollBox::UnlockScroll,
				ScrollDelay,
				false
			);
		}
	}

	const int32 deltaValue = bUpKey ? -1 : 1;
	BroadcastIndexChange(deltaValue);

	return FReply::Handled();
}


FReply UMounteaInventoryScrollBox::NativeOnAnalogValueChanged(const FGeometry& InGeometry,
	const FAnalogInputEvent& InAnalogEvent)
{
	if (!bCaptureInput)
		return Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);
	
	const FKey key = InAnalogEvent.GetKey();
	const float value = InAnalogEvent.GetAnalogValue();

	if (key != EKeys::Gamepad_LeftY || FMath::Abs(value) <= 0.5f)
		return Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);
	
	if (bUseScrollDelay && bScrollLocked)
		return FReply::Handled();

	if (bUseScrollDelay)
	{
		bScrollLocked = true;

		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().SetTimer(
				ScrollDelayTimerHandle,
				this,
				&UMounteaInventoryScrollBox::UnlockScroll,
				ScrollDelay,
				false
			);
		}
	}

	BroadcastIndexChange(value > 0 ? -1 : 1);

	return FReply::Handled();
}


void UMounteaInventoryScrollBox::SetActiveIndex(const int32 NewIndex)
{
	if (!VerticalBox)
		return;
	
	const int32 maxIndex = VerticalBox->GetChildrenCount() - 1;
	ActiveIndex = FMath::Clamp(NewIndex, 0, maxIndex);
	CalculateDesiredTransform();
}

void UMounteaInventoryScrollBox::BroadcastIndexChange(const int32 Delta) const
{
	int32 newIndex = ActiveIndex + Delta;
	
	if (bWrapAround)
	{
		if (newIndex < 0)
			newIndex = VerticalBox->GetChildrenCount() - 1 ;
		else if (newIndex > VerticalBox->GetChildrenCount() - 1)
			newIndex = 0;
	}
	else
		newIndex = FMath::Clamp(newIndex, 0, VerticalBox->GetChildrenCount() - 1);
	
	if (newIndex != ActiveIndex)
		OnNewIndexCalculated.Broadcast(newIndex);
}

void UMounteaInventoryScrollBox::CalculateDesiredTransform()
{
	if (!VerticalBox)
		return;
	
	TargetTranslation = UMounteaInventoryUIStatics::CalculateCenteredListTranslation(
		VerticalBox,
		ActiveIndex
	);
}

void UMounteaInventoryScrollBox::InterpolateToTarget(const float DeltaTime)
{
	if (!VerticalBox)
		return;
	
	const float currentLength = CurrentTranslation.Size();
	const float targetLength = TargetTranslation.Size();
	
	if (FMath::IsNearlyEqual(currentLength, targetLength, 0.1f))
		return;
	
	CurrentTranslation = FMath::Vector2DInterpTo(CurrentTranslation, TargetTranslation, DeltaTime, InterpolationSpeed);
	VerticalBox->SetRenderTranslation(CurrentTranslation);
}

void UMounteaInventoryScrollBox::UnlockScroll()
{
	bScrollLocked = false;
}
