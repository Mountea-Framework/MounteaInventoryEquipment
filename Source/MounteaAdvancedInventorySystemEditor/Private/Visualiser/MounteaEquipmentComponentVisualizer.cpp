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
#include "SceneView.h"
#include "SceneManagement.h"
#include "CanvasItem.h"

#include "Components/MounteaAttachmentContainerComponent.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Statics/MounteaAttachmentsStatics.h"

void FMounteaEquipmentComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (!Component || !PDI || !GEngine || !IsComponentSelected(Component))
		return;

	const UMounteaAttachmentContainerComponent* attachableContainerComponent = Cast<UMounteaAttachmentContainerComponent>(Component);
	if (!attachableContainerComponent)
		return;

	TArray<FSlotVisualData> slotsData;
	TArray<FWarningVisualData> warningData;
	GatherSlotData(attachableContainerComponent, slotsData, warningData);

	for (const FSlotVisualData& SlotInfo : slotsData)
	{
		PDI->DrawPoint(SlotInfo.Location, SlotInfo.Color, 8.0f, SDPG_Foreground);
	}
}

void FMounteaEquipmentComponentVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (!Component || !View || !Canvas || !GEngine || !IsComponentSelected(Component))
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
			FCanvasTextItem textItem(screenPosition, FText::FromString(Label), GEngine->GetSmallFont(), Color);
			textItem.EnableShadow(FLinearColor::Black);
			Canvas->DrawItem(textItem);
		}
	};

	for (const FSlotVisualData& SlotInfo : slotData)
	{
		drawLabel(SlotInfo.Location + FVector(0.0f, 0.0f, 10.0f), SlotInfo.Label, SlotInfo.Color);
	}

	for (const FWarningVisualData& WarningInfo : warningData)
	{
		drawLabel(WarningInfo.Location, WarningInfo.Message, FLinearColor::Red);
	}
}

bool FMounteaEquipmentComponentVisualizer::IsComponentSelected(const UActorComponent* Component)
{
	if (!Component || !GEditor)
		return false;

	if (USelection* selectedComponent = GEditor->GetSelectedComponents())
	{
		if (selectedComponent->IsSelected(Component))
			return true;
	}

	const AActor* owningActor = Component->GetOwner();
	if (owningActor)
		return GEditor->GetSelectedActors()->IsSelected(owningActor);

	return false;
}

void FMounteaEquipmentComponentVisualizer::GatherSlotData(const UMounteaAttachmentContainerComponent* EquipmentComponent, TArray<FSlotVisualData>& OutSlots, TArray<FWarningVisualData>& OutWarnings) const
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
		const FVector baseLocation = componentOwner->GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
		for (int32 index = 0; index < warning.Num(); ++index)
		{
			FWarningVisualData warningInfo;
			warningInfo.Location = baseLocation + FVector(0.0f, 0.0f, 15.0f * (index + 1));
			warningInfo.Message = warning[index];
			OutWarnings.Add(MoveTemp(warningInfo));
		}
	}
}

USceneComponent* FMounteaEquipmentComponentVisualizer::ResolveAttachmentTarget(const UMounteaAttachmentContainerComponent* EquipmentComponent, const UMounteaAdvancedAttachmentSlot* Slot, TArray<FString>& OutWarnings) const
{
	if (!EquipmentComponent || !Slot)
		return nullptr;

	if (Slot->AttachmentTargetComponentOverride)
		return Slot->AttachmentTargetComponentOverride;

	const AActor* owningActor = EquipmentComponent->GetOwner();
	const FName targetOverride = Slot->AttachmentTargetOverride;
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
