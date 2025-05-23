// All rights reserved Dominik Morse 2024

#include "Widgets/ItemPreview/MounteaAdvancedInventoryInteractableObjectWidget.h"

#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"
#include "Components/Image.h"
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/TemplatesConfig/MounteaAdvancedInventoryInteractiveWidgetConfig.h"
#include "Statics/MounteaInventoryStatics.h"

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializePreviewLevel();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeDestruct()
{
	CleanupPreviewLevel();
	Super::NativeDestruct();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::InitializePreviewLevel()
{
	auto templateConfig = UMounteaInventoryStatics::GetTemplateConfig(TEXT("InteractivePreview"));
	if (!templateConfig) return;

	auto interactiveConfig = Cast<UMounteaAdvancedInventoryInteractiveWidgetConfig>(templateConfig);
	if (!interactiveConfig) return;
	PreviewWorld = interactiveConfig->PreviewLevelAsset.LoadSynchronous();
	
	if (!PreviewWorld|| LevelInstance)
		return;

	FVector location = FVector::ZeroVector;
	FRotator rotation = FRotator::ZeroRotator;
	if (PreviewParentActor)
	{
		location = PreviewParentActor->GetActorLocation();
		rotation = PreviewParentActor->GetActorRotation();
	}

	bool loadSucceeded = false;
	LevelInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		GetWorld(),
		PreviewWorld,
		location,
		rotation,
		loadSucceeded
	);
	if (!LevelInstance || !loadSucceeded)
		return;

	LevelInstance->SetShouldBeLoaded(true);
	LevelInstance->SetShouldBeVisible(true);
	LevelInstance->OnLevelLoaded.AddDynamic(this, &UMounteaAdvancedInventoryInteractableObjectWidget::OnLevelLoaded);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::OnLevelLoaded()
{
	if (!LevelInstance)
		return;

	ULevel* loadedLevel = LevelInstance->GetLoadedLevel();
	if (!loadedLevel)
		return;

	for (AActor* actor : loadedLevel->Actors)
	{
		if (AMounteaAdvancedInventoryItemPreviewRenderer* rend = Cast<AMounteaAdvancedInventoryItemPreviewRenderer>(actor))
		{
			RendererActor = rend;
			break;
		}
	}
	if (!RendererActor)
		return;
	
	ClearPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::CleanupPreviewLevel()
{
	if (RendererActor)
	{
		RendererActor->Destroy();
		RendererActor = nullptr;
	}
	if (LevelInstance)
	{
		UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), PreviewWorld, FLatentActionInfo(), false);
		LevelInstance->SetShouldBeLoaded(false);
		LevelInstance->SetShouldBeVisible(false);
		LevelInstance = nullptr;
	}
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetPreviewMesh(UStaticMesh* StaticMesh)
{
	if (RendererActor)
		RendererActor->SetStaticMesh(StaticMesh);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetPreviewSkeletalMesh(USkeletalMesh* SkeletalMesh)
{
	if (RendererActor)
		RendererActor->SetSkeletalMesh(SkeletalMesh);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::ClearPreview()
{
	if (RendererActor)
		RendererActor->ClearMesh();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (bIsMousePressed && RendererActor)
	{
		FVector2D currentPos = InMouseEvent.GetScreenSpacePosition();
		FVector2D delta	  = currentPos - LastMousePosition;
		float sensitivity	= 0.5f;
		CurrentYaw   += delta.X * sensitivity;
		CurrentPitch = FMath::Clamp(CurrentPitch - delta.Y * sensitivity, -80.0f, 80.0f);
		RendererActor->SetCameraRotation(CurrentYaw, CurrentPitch);
		LastMousePosition = currentPos;
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsMousePressed	= true;
		LastMousePosition = InMouseEvent.GetScreenSpacePosition();
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return FReply::Unhandled();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsMousePressed)
	{
		bIsMousePressed = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseWheel(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	CurrentZoom = FMath::Clamp(CurrentZoom - InMouseEvent.GetWheelDelta() * 20.0f, 50.0f, 1000.0f);
	if (RendererActor)
		RendererActor->SetCameraDistance(CurrentZoom);
	return FReply::Handled();
}
