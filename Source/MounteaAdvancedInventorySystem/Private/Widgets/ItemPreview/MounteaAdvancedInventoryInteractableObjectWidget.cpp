// MounteaAdvancedInventoryInteractableObjectWidget.cpp


#include "Widgets/ItemPreview/MounteaAdvancedInventoryInteractableObjectWidget.h"

#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"
#include "Components/Image.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Settings/TemplatesConfig/MounteaAdvancedInventoryInteractiveWidgetConfig.h"
#include "Statics/MounteaInventoryStatics.h"

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeConstruct() : bIsMousePressed(true)
{
	Super::NativeConstruct();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::CleanUpPreviewScene()
{
	if( PreviewScene && RendererActor )
	{
		PreviewScene->GetWorld()->DestroyActor( RendererActor );
		RendererActor = nullptr;
	}
	PreviewScene.Reset();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeDestruct()
{
	Super::NativeDestruct();
	CleanUpPreviewScene();
}

bool UMounteaAdvancedInventoryInteractableObjectWidget::InitializeInteractableWidget()
{
	auto templateConfig = UMounteaInventoryStatics::GetTemplateConfig(TEXT("InteractivePreview"));
	if (!templateConfig) return false;

	auto interactiveConfig = Cast<UMounteaAdvancedInventoryInteractiveWidgetConfig>(templateConfig);
	if (!interactiveConfig) return false;

	RendererActorClass = interactiveConfig->RendererActor.LoadSynchronous();
	if (!RendererActorClass)
		RendererActorClass = AMounteaAdvancedInventoryItemPreviewRenderer::StaticClass();

	PreviewRenderTarget = interactiveConfig->DefaultRenderTarget.LoadSynchronous();
	if (!PreviewRenderTarget) return false;

	const auto previewMaterial = interactiveConfig->DefaultRenderTargetMaterial.LoadSynchronous();
	if (!previewMaterial) return false;

	PreviewRenderTarget->ClearColor = FLinearColor(0,0,0,0);

	PreviewMaterialInstance = UMaterialInstanceDynamic::Create(previewMaterial, this);
	PreviewMaterialInstance->SetTextureParameterValue(TEXT("SceneCapture"), PreviewRenderTarget);
	
	PreviewScene = MakeUnique<FPreviewScene>( FPreviewScene::ConstructionValues() );
	UWorld* PreviewWorld = PreviewScene->GetWorld();
	RendererActor = PreviewWorld->SpawnActor<AMounteaAdvancedInventoryItemPreviewRenderer>(RendererActorClass);
	if (!IsValid(RendererActor))
	{
		CleanUpPreviewScene();
		return false;
	}

	PreviewScene->SetLightBrightness( 3.0f );
	PreviewScene->SetSkyBrightness( 1.0f );
	PreviewScene->UpdateCaptureContents();
	
	RendererActor->SetRenderTarget( PreviewRenderTarget );

	PreviewImage->SetBrushFromMaterial(PreviewMaterialInstance);

	if (bIsMousePressed)
		StartPreview();
	
	return true;
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetPreviewMesh(UStaticMesh* StaticMesh) const
{
	if (RendererActor)
		RendererActor->SetStaticMesh(StaticMesh);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetPreviewSkeletalMesh(USkeletalMesh* SkeletalMesh) const
{
	if (RendererActor)
		RendererActor->SetSkeletalMesh(SkeletalMesh);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::ClearPreview() const
{
	if (RendererActor)
		RendererActor->ClearMesh();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::StartPreview()
{
	// TODO: Use a timer to update the preview scene
	// `TimerHandle_PreviewTick` and call function `TickPreview`
	// `TickPreview` should not be in loop, we call `StartPreview` again from `TickPreview`
}

void UMounteaAdvancedInventoryInteractableObjectWidget::PausePreview()
{
	// TODO: Pause timer
}

void UMounteaAdvancedInventoryInteractableObjectWidget::TickPreview()
{
	if (PreviewScene)
		RendererActor->CaptureScene();

	// TODO: clear timer and call StartPreview again
}

// TODO: in all inputs capture `LastInteractionTime` and make sure we don't render the scene
// unless input has been captured for `IdleThreshold` period of time
// basically only capture scenes as long as the user is interacting with the widget and 1 second after than
FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (bIsMousePressed && RendererActor)
	{
		const FVector2D mouseDelta = InMouseEvent.GetCursorDelta() * 0.5f;
		CurrentYaw   += mouseDelta.X;
		CurrentPitch = FMath::Clamp(CurrentPitch - mouseDelta.Y, -80.0f, 80.0f);
		RendererActor->SetCameraRotation(CurrentYaw, CurrentPitch);
		return FReply::Handled();
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsMousePressed = true;
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (bIsMousePressed && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsMousePressed = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseWheel(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	// TODO: Fix non working zoom
	LOG_WARNING(TEXT("WHEEEE"))
	if (RendererActor)
	{
		CurrentZoom = FMath::Clamp(CurrentZoom - InMouseEvent.GetWheelDelta() * 20.0f, 50.0f, 1000.0f);
		RendererActor->SetCameraDistance(CurrentZoom);
		return FReply::Handled();
	}
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}
