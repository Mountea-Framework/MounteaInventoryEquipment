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

#include "Visualiser/MounteaEquipmentComponentVisualizer.h"

#include "Components/SceneComponent.h"
#include "Editor.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Selection.h"
#include "Engine/World.h"
#include "SceneView.h"
#include "SceneManagement.h"
#include "CanvasItem.h"

#include "Camera/CameraComponent.h"
#include "Components/MounteaAttachmentContainerComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Statics/MounteaAttachmentsStatics.h"

namespace
{
	constexpr float SpherePixelRadius = .1f;
	constexpr int32 SphereSegments = 16;
	constexpr float LeaderLineLengthPixels = 60.0f;
	constexpr float LeaderTextPaddingPixels = 6.0f;

	float GetWorldRadiusForScreenSize(const FSceneView* View, const FVector& WorldLocation, const float PixelRadius)
	{
		if (!View || PixelRadius <= 0.0f)
			return PixelRadius;

		FVector2D pixelCenter;
		if (!View->WorldToPixel(WorldLocation, pixelCenter))
			return PixelRadius;

		const FVector2D pixelOffset = pixelCenter + FVector2D(PixelRadius, 0.0f);

		FVector worldOriginCenter;
		FVector worldDirectionCenter;
		FSceneView::DeprojectScreenToWorld(pixelCenter, View->UnconstrainedViewRect, View->ViewMatrices.GetInvViewMatrix(), View->ViewMatrices.GetInvProjectionMatrix(), worldOriginCenter, worldDirectionCenter);

		FVector worldOriginOffset;
		FVector worldDirectionOffset;
		FSceneView::DeprojectScreenToWorld(pixelOffset, View->UnconstrainedViewRect, View->ViewMatrices.GetInvViewMatrix(), View->ViewMatrices.GetInvProjectionMatrix(), worldOriginOffset, worldDirectionOffset);

		const FVector planeNormal = View->GetViewDirection();
		const FPlane depthPlane(WorldLocation, planeNormal);

		const FVector centerOnPlane = FMath::LinePlaneIntersection(worldOriginCenter, worldOriginCenter + worldDirectionCenter * 100000.0f, depthPlane);
		const FVector offsetOnPlane = FMath::LinePlaneIntersection(worldOriginOffset, worldOriginOffset + worldDirectionOffset * 100000.0f, depthPlane);

		return FVector::Dist(centerOnPlane, offsetOnPlane);
	}
}

void FMounteaEquipmentComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (!Component || !PDI || !GEngine || !View || IsCameraPreviewView(View) || !ShouldDrawForComponent(Component))
		return;

	const UMounteaAttachmentContainerComponent* attachableContainerComponent = Cast<UMounteaAttachmentContainerComponent>(Component);
	if (!attachableContainerComponent)
		return;

	TArray<FSlotVisualData> slotsData;
	TArray<FWarningVisualData> warningData;
	GatherSlotData(attachableContainerComponent, slotsData, warningData);

	for (const FSlotVisualData& SlotInfo : slotsData)
	{
		const float worldRadius = GetWorldRadiusForScreenSize(View, SlotInfo.Location, SpherePixelRadius);
		DrawWireSphere(PDI, SlotInfo.Location, SlotInfo.Color.ToFColor(true), worldRadius, SphereSegments, SDPG_Foreground);
	}
}

void FMounteaEquipmentComponentVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (!Component || !View || !Canvas || !GEngine || IsCameraPreviewView(View) || !ShouldDrawForComponent(Component))
		return;

	const UMounteaAttachmentContainerComponent* attachableContainerComponent = Cast<UMounteaAttachmentContainerComponent>(Component);
	if (!attachableContainerComponent)
		return;

	TArray<FSlotVisualData> slotData;
	TArray<FWarningVisualData> warningData;
	GatherSlotData(attachableContainerComponent, slotData, warningData);

	auto drawLabel = [View, Canvas](const FVector& WorldLocation, const FString& Label, const FLinearColor& Color)
	{
		FVector2D screenPosition;
		if (FSceneView::ProjectWorldToScreen(WorldLocation, View->CameraConstrainedViewRect, View->ViewMatrices.GetViewProjectionMatrix(), screenPosition))
		{
			const FIntRect& viewRect = View->CameraConstrainedViewRect;
			const float directionX = (screenPosition.X < viewRect.Min.X + (viewRect.Width() * 0.5f)) ? 1.0f : -1.0f;

			const FVector2D lineStart = screenPosition + FVector2D(directionX * SpherePixelRadius, 0.0f);
			const FVector2D lineEnd = lineStart + FVector2D(directionX * LeaderLineLengthPixels, 0.0f);

			FCanvasLineItem lineItem(lineStart, lineEnd);
			lineItem.SetColor(Color);
			lineItem.LineThickness = 1.0f;
			Canvas->DrawItem(lineItem);

			UFont* font = GEngine->GetSmallFont();
			int32 textHeight = 0;
			int32 textWidth = 0;
			font->GetStringHeightAndWidth(Label, textHeight, textWidth);
			const FVector2D textSize(static_cast<float>(textWidth), static_cast<float>(textHeight));

			FVector2D textPosition = lineEnd + FVector2D(directionX * LeaderTextPaddingPixels, -textSize.Y * 0.5f);
			if (directionX < 0.0f)
				textPosition.X -= textSize.X;

			FCanvasTextItem textItem(textPosition, FText::FromString(Label), font, Color);
			textItem.EnableShadow(FLinearColor::Black);
			Canvas->DrawItem(textItem);
		}
	};

	for (const FSlotVisualData& slotInfo : slotData)
	{
		drawLabel(slotInfo.Location, slotInfo.Label, slotInfo.Color);
	}

	for (const FWarningVisualData& warningInfo : warningData)
	{
		drawLabel(warningInfo.Location, warningInfo.Message, FLinearColor::Red);
	}
}

bool FMounteaEquipmentComponentVisualizer::ShouldDrawForComponent(const UActorComponent* Component)
{
	if (!Component)
		return false;

	if (const UWorld* World = Component->GetWorld())
	{
		if (World->WorldType == EWorldType::EditorPreview)
			return true;
	}

	return IsComponentSelected(Component);
}

bool FMounteaEquipmentComponentVisualizer::IsComponentSelected(const UActorComponent* Component)
{
	if (!Component || !GEditor)
		return false;

	const auto editorWorld = GEditor->GetEditorWorldContext().World();
	if (!editorWorld)
		return false;
	
	if (editorWorld->WorldType != EWorldType::EditorPreview)
		if (USelection* selectedComponent = GEditor->GetSelectedComponents())
			if (selectedComponent->IsSelected(Component))
				return true;

	const AActor* owningActor = Component->GetOwner();
	if (owningActor)
		return GEditor->GetSelectedActors()->IsSelected(owningActor);

	return false;
}

bool FMounteaEquipmentComponentVisualizer::IsCameraPreviewView(const FSceneView* View)
{
	if (!View)
		return false;

	const AActor* viewActor = View->ViewActor.Get();
	if (!viewActor)
		return false;

	return viewActor->FindComponentByClass<UCameraComponent>() != nullptr;
}

void FMounteaEquipmentComponentVisualizer::GatherSlotData(const UMounteaAttachmentContainerComponent* EquipmentComponent, TArray<FSlotVisualData>& OutSlots, TArray<FWarningVisualData>& OutWarnings)
{
	if (!EquipmentComponent)
		return;

	const AActor* componentOwner = EquipmentComponent->GetOwner();
	if (!componentOwner)
		return;

	const TArray<UMounteaAdvancedAttachmentSlot*> attachableSlots = EquipmentComponent->Execute_GetAttachmentSlots(EquipmentComponent);
	if (attachableSlots.Num() == 0)
		return;

	TArray<FString> warning;
	for (int32 Index = 0; Index < attachableSlots.Num(); ++Index)
	{
		const UMounteaAdvancedAttachmentSlot* attachableSlot = attachableSlots[Index];
		if (!attachableSlot)
			continue;

		USceneComponent* targetComponent = ResolveAttachmentTarget(EquipmentComponent, attachableSlot, warning);
		FVector drawLocation = componentOwner->GetActorLocation();

		if (targetComponent)
		{
			drawLocation = targetComponent->GetComponentLocation();

			if (attachableSlot->SlotType == EAttachmentSlotType::EAST_Socket && !attachableSlot->SocketName.IsNone())
			{
				if (targetComponent->DoesSocketExist(attachableSlot->SocketName))
					drawLocation = targetComponent->GetSocketLocation(attachableSlot->SocketName);
				else
					warning.Add(FString::Printf(TEXT("%s -> Missing socket '%s'"), *GetSlotLabel(attachableSlot), *attachableSlot->SocketName.ToString()));
			}
		}
		FSlotVisualData slotInfo;
		slotInfo.Location = drawLocation;
		slotInfo.Label = GetSlotLabel(attachableSlot);
		slotInfo.Color = GetSlotColor(Index);
		OutSlots.Add(MoveTemp(slotInfo));
	}

	if (warning.Num() > 0)
	{
		const FVector baseLocation = componentOwner->GetActorLocation() + FVector(0.0f, 0.0f, 1.0f);
		for (int32 index = 0; index < warning.Num(); ++index)
		{
			FWarningVisualData warningInfo;
			warningInfo.Location = baseLocation + FVector(0.0f, 0.0f, 1.0f * (index + 1));
			warningInfo.Message = warning[index];
			OutWarnings.Add(MoveTemp(warningInfo));
		}
	}
}

USceneComponent* FMounteaEquipmentComponentVisualizer::ResolveAttachmentTarget(const UMounteaAttachmentContainerComponent* EquipmentComponent, const UMounteaAdvancedAttachmentSlot* Slot, TArray<FString>& OutWarnings)
{
	if (!EquipmentComponent || !Slot)
		return nullptr;

	if (Slot->AttachmentTargetComponentOverride)
		return Slot->AttachmentTargetComponentOverride;

	const auto containerTarget = EquipmentComponent->Execute_GetDefaultAttachmentTarget(EquipmentComponent);
	
	const AActor* owningActor = EquipmentComponent->GetOwner();
	const FName targetOverride = Slot->AttachmentTargetOverride.IsNone() ? containerTarget : Slot->AttachmentTargetOverride;
	if (owningActor && !targetOverride.IsNone())
	{
		const TArray<FName> availableNames = UMounteaAttachmentsStatics::GetAvailableComponentNames(owningActor);
		if (availableNames.Contains(targetOverride))
			return UMounteaAttachmentsStatics::GetAvailableComponentByName(owningActor, targetOverride);

		if (availableNames.Num() > 0)
		{
			FString availableList;
			for (int32 index = 0; index < availableNames.Num(); ++index)
			{
				if (index > 0)
					availableList += TEXT(", ");
				availableList += availableNames[index].ToString();
			}
			OutWarnings.Add(FString::Printf(TEXT("%s -> Missing target '%s' (Available: %s)"), *GetSlotLabel(Slot), *targetOverride.ToString(), *availableList));
		}
		else
			OutWarnings.Add(FString::Printf(TEXT("%s -> Missing target '%s'"), *GetSlotLabel(Slot), *targetOverride.ToString()));
	}

	if (USceneComponent* defaultTarget = EquipmentComponent->Execute_GetAttachmentTargetComponent(EquipmentComponent))
		return defaultTarget;

	if (owningActor && owningActor->GetRootComponent())
	{
		OutWarnings.Add(FString::Printf(TEXT("%s -> Using owner root as fallback"), *GetSlotLabel(Slot)));
		return owningActor->GetRootComponent();
	}

	OutWarnings.Add(FString::Printf(TEXT("%s -> No attachment target"), *GetSlotLabel(Slot)));
	return nullptr;
}

FString FMounteaEquipmentComponentVisualizer::GetSlotLabel(const UMounteaAdvancedAttachmentSlot* Slot)
{
	if (!Slot)
		return TEXT("Invalid Slot");

	const FString displayName = Slot->DisplayName.ToString();
	if (!displayName.IsEmpty())
		return displayName;

	if (!Slot->SlotName.IsNone())
		return Slot->SlotName.ToString();

	return TEXT("Unnamed Slot");
}

FLinearColor FMounteaEquipmentComponentVisualizer::GetSlotColor(const int32 Index)
{
	const uint8 randomHue = static_cast<uint8>((Index * 37) % 255);
	return FLinearColor::MakeFromHSV8(randomHue, 200, 255);
}
