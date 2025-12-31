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

#include "Statics/MounteaInventoryUIStatics.h"

void UMounteaInventoryScrollBox::SetActiveIndex(const int32 NewIndex)
{
	if (ActiveIndex != NewIndex)
	{
		ActiveIndex = FMath::Clamp(NewIndex, 0, GetChildrenCount() - 1);
		CalculateDesiredTransform();
		StartInterpolation();
	}
}

void UMounteaInventoryScrollBox::CalculateDesiredTransform()
{
	TargetTranslation = UMounteaInventoryUIStatics::CalculateCenteredListTranslation(
		this,
		ActiveIndex
	);
}

void UMounteaInventoryScrollBox::StartInterpolation()
{
	if (const UWorld* world = GetWorld())
	{
		world->GetTimerManager().SetTimer(
			InterpolationTimer,
			this,
			&UMounteaInventoryScrollBox::InterpolateToTarget,
			0.016f,
			true
		);
	}
}

void UMounteaInventoryScrollBox::StopInterpolation()
{
	if (const UWorld* world = GetWorld())
		world->GetTimerManager().ClearTimer(InterpolationTimer);
}

void UMounteaInventoryScrollBox::InterpolateToTarget()
{
	const float currentLength = CurrentTranslation.Size();
	const float targetLength = TargetTranslation.Size();
	
	if (FMath::IsNearlyEqual(currentLength, targetLength, 0.1f))
	{
		StopInterpolation();
		return;
	}
	
	CurrentTranslation = FMath::Vector2DInterpTo(
		CurrentTranslation,
		TargetTranslation,
		0.016f,
		InterpolationSpeed
	);
	
	SetRenderTranslation(CurrentTranslation);
}

void UMounteaInventoryScrollBox::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	StopInterpolation();
}