// MounteaAdvancedInventoryInteractableObjectWidget.cpp


#include "Widgets/ItemPreview/MounteaAdvancedInventoryInteractableObjectWidget.h"

#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"
#include "Components/Image.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Settings/TemplatesConfig/MounteaAdvancedInventoryInteractiveWidgetConfig.h"
#include "Statics/MounteaInventoryStatics.h"

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;
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
	if (!PreviewScene.IsValid() || !IsValid(RendererActor))
	{
		LOG_ERROR(TEXT("Preview scene or renderer actor is invalid!"));
		return;
	}
	
	if (const UWorld* contextWorld = GetWorld())
	{
		if (FMath::IsNearlyZero(LastInteractionTime))
			LastInteractionTime = GetWorld()->GetTimeSeconds();
		
		if (!contextWorld->GetTimerManager().IsTimerActive(TimerHandle_PreviewTick) && !FMath::IsNearlyEqual(PreviewTickFrequency, 0.f, 0.001f))
		{
			const float Interval = 1.f / PreviewTickFrequency;
			contextWorld->GetTimerManager().SetTimer(
				TimerHandle_PreviewTick,
				this,
				&UMounteaAdvancedInventoryInteractableObjectWidget::TickPreview,
				Interval,
				true
			);
		}
	}
}

void UMounteaAdvancedInventoryInteractableObjectWidget::PausePreview()
{
	if (const UWorld* contextWorld = GetWorld())
		contextWorld->GetTimerManager().ClearTimer(TimerHandle_PreviewTick);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::TickPreview()
{
	const float currentTime = GetWorld()->GetTimeSeconds();
	if ((currentTime - LastInteractionTime) > IdleThreshold)
	{
		PausePreview();
		return;
	}
	
	PreviewScene->UpdateCaptureContents();
	RendererActor->CaptureScene();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::UpdateLastInteractionAndStartPreview()
{
	if (const UWorld* contextWorld = GetWorld())
	{
		LastInteractionTime = contextWorld->GetTimeSeconds();
		StartPreview();
	}
}

void UMounteaAdvancedInventoryInteractableObjectWidget::ResetCameraToDefaults()
{
	CurrentYaw   = 0.0f;
	CurrentPitch = 0.0f;
	CurrentZoom  = 1.0f;
	CurrentCameraHeight = 0.0f;

	if (RendererActor)
	{
		RendererActor->ResetToDefaults();
		RendererActor->CaptureScene();
	}
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	const FVector2D currentMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const FVector2D mouseDelta = currentMousePos - LastMousePosition;
    
	if (bIsMousePressed && RendererActor)
	{
		const float rotationSensitivity = 0.5f * CurrentZoom;
        
		CurrentYaw += mouseDelta.X * rotationSensitivity;
		CurrentPitch = FMath::Clamp(CurrentPitch - mouseDelta.Y * rotationSensitivity, -YawLimits, YawLimits);
        
		RendererActor->SetCameraRotation(CurrentYaw, CurrentPitch);
		UpdateLastInteractionAndStartPreview();
        
		LastMousePosition = currentMousePos;
		return FReply::Handled();
	}
	else if (bIsMiddleMousePressed && RendererActor)
	{
		const float heightSensitivity = CameraHeightSensitivity * CurrentZoom;
		const float heightIncrement = (mouseDelta.Y > 0) ? heightSensitivity : -heightSensitivity;
        
		CurrentCameraHeight = FMath::Clamp(CurrentCameraHeight + heightIncrement, -HeightLimit, HeightLimit);
		RendererActor->SetCameraHeight(CurrentCameraHeight);
        
		UpdateLastInteractionAndStartPreview();
        
		LastMousePosition = currentMousePos;
		return FReply::Handled();
	}
    
	LastMousePosition = currentMousePos;
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsMousePressed = true;
		UpdateLastInteractionAndStartPreview();
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		bIsMiddleMousePressed = true;
		UpdateLastInteractionAndStartPreview();
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
		UpdateLastInteractionAndStartPreview();
		return FReply::Handled().ReleaseMouseCapture();
	}
	else if (bIsMiddleMousePressed && InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		bIsMiddleMousePressed = false;
		UpdateLastInteractionAndStartPreview();
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseWheel(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (RendererActor)
	{
		const float zoomFactor = 1.0f + InMouseEvent.GetWheelDelta() * 0.1f;
		CurrentZoom = FMath::Clamp(CurrentZoom * zoomFactor, ScaleLimits.X, ScaleLimits.Y);
		RendererActor->SetCameraDistance(CurrentZoom);
		UpdateLastInteractionAndStartPreview();
		return FReply::Handled();
	}
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	PausePreview();
}
