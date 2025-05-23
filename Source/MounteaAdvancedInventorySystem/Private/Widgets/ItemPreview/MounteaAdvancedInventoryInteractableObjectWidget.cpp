// All rights reserved Dominik Morse 2024

#include "Widgets/ItemPreview/MounteaAdvancedInventoryInteractableObjectWidget.h"
#include "Components/Image.h"
#include "Components/Viewport.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"
#include "Blueprint/WidgetTree.h"
#include "Components/NamedSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/TemplatesConfig/MounteaAdvancedInventoryInteractiveWidgetConfig.h"
#include "Statics/MounteaInventoryStatics.h"

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (PreviewSlot)
	{
		PreviewViewport = WidgetTree->ConstructWidget<UViewport>(UViewport::StaticClass(), TEXT("GeneratedPreviewViewport"));
		PreviewSlot->AddChild(PreviewViewport);
	}
	InitializePreviewWorld();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeDestruct()
{
	CleanupPreviewWorld();
	Super::NativeDestruct();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::InitializePreviewWorld()
{
	if (!PreviewViewport) return;
	
	if (!IsValid(PreviewLevelInstance))
	{
		UWorld* previewWorld = PreviewViewport->GetViewportWorld();
		if (!previewWorld)
			return;

		auto templateConfig = UMounteaInventoryStatics::GetTemplateConfig(TEXT("InteractivePreview"));
		if (!templateConfig) return;

		auto interactiveConfig = Cast<UMounteaAdvancedInventoryInteractiveWidgetConfig>(templateConfig);
		if (!interactiveConfig) return;
		auto previewLevel = interactiveConfig->PreviewLevelAsset;
		
		bool bSuccess = false;
		PreviewLevelInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
				GetWorld(),
				previewLevel,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				bSuccess
			);
		
		if (bSuccess && PreviewLevelInstance)
		{
			PreviewLevelInstance->SetShouldBeLoaded(true);
			PreviewLevelInstance->OnLevelShown.AddDynamic(this, &UMounteaAdvancedInventoryInteractableObjectWidget::OnPreviewLevelShown);
		}
	}
}

void UMounteaAdvancedInventoryInteractableObjectWidget::OnPreviewLevelShown()
{
	if (!PreviewLevelInstance)
		return;
	
	PreviewLevelInstance->SetShouldBeVisible(false);
	ULevel* loadedLevel = PreviewLevelInstance->GetLoadedLevel();
	if (!loadedLevel)
		return;

	for (AActor* possibleActor : loadedLevel->Actors)
	{
		if (AMounteaAdvancedInventoryItemPreviewRenderer* R = Cast<AMounteaAdvancedInventoryItemPreviewRenderer>(possibleActor))
		{
			RendererActor = R;
			break;
		}
	}

	if (!RendererActor)
		return;

	SetupRendererActor();
	SetupMaterial();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::CleanupPreviewWorld()
{
	if (PreviewLevelInstance)
	{
		PreviewLevelInstance->SetShouldBeVisible(false);
		PreviewLevelInstance->SetShouldBeLoaded(false);
		PreviewLevelInstance = nullptr;
	}
	RendererActor = nullptr;
	DynamicMaterialInstance = nullptr;
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetupRendererActor() const
{
	if (!RendererActor || !PreviewRenderTarget.IsValid())
		return;

	UTextureRenderTarget2D* RT = PreviewRenderTarget.LoadSynchronous();
	RendererActor->SetRenderTarget(RT);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetupMaterial()
{
	if (!PreviewImage || !PreviewRenderTarget.IsValid() || !PreviewMaterial.IsValid())
		return;

	UTextureRenderTarget2D* RT = PreviewRenderTarget.LoadSynchronous();
	UMaterialInterface* Mat = PreviewMaterial.LoadSynchronous();
	DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Mat, this);
	DynamicMaterialInstance->SetTextureParameterValue(TEXT("Texture"), RT);
	PreviewImage->SetBrushFromMaterial(DynamicMaterialInstance);
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

void UMounteaAdvancedInventoryInteractableObjectWidget::RotatePreview(float DeltaYaw, float DeltaPitch)
{
	CurrentYaw += DeltaYaw;
	CurrentPitch = FMath::Clamp(CurrentPitch + DeltaPitch, -80.0f, 80.0f);
	if (RendererActor)
		RendererActor->SetCameraRotation(CurrentYaw, CurrentPitch);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::ZoomPreview(float ZoomDelta)
{
	CurrentZoom = FMath::Clamp(CurrentZoom + ZoomDelta, 50.0f, 1000.0f);
	if (RendererActor)
		RendererActor->SetCameraDistance(CurrentZoom);
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsMousePressed)
	{
		FVector2D currentMousePosition = InMouseEvent.GetScreenSpacePosition();
		FVector2D mouseDelta = currentMousePosition - LastMousePosition;
		float rotationSensitivity = 0.5f;
		RotatePreview(mouseDelta.X * rotationSensitivity, -mouseDelta.Y * rotationSensitivity);
		LastMousePosition = currentMousePosition;
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsMousePressed = true;
		LastMousePosition = InMouseEvent.GetScreenSpacePosition();
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return FReply::Unhandled();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsMousePressed)
	{
		bIsMousePressed = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	float zoomSensitivity = 20.0f;
	ZoomPreview(-InMouseEvent.GetWheelDelta() * zoomSensitivity);
	return FReply::Handled();
}
