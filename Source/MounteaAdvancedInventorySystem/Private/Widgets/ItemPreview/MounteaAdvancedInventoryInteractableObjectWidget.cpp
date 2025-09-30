// MounteaAdvancedInventoryInteractableObjectWidget.cpp

#include "Widgets/ItemPreview/MounteaAdvancedInventoryInteractableObjectWidget.h"

#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"
#include "Components/Image.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Settings/TemplatesConfig/MounteaAdvancedInventoryInteractiveWidgetConfig.h"
#include "Statics/MounteaInventoryStatics.h"

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
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

	if (!RendererActorClass)
		RendererActorClass = interactiveConfig->RendererActor.LoadSynchronous();
	if (!RendererActorClass)
		RendererActorClass = AMounteaAdvancedInventoryItemPreviewRenderer::StaticClass();

	PreviewRenderTarget = interactiveConfig->DefaultRenderTarget.LoadSynchronous();
	if (!PreviewRenderTarget) return false;

	const auto previewMaterial = interactiveConfig->DefaultRenderTargetMaterial.LoadSynchronous();
	if (!previewMaterial) return false;

	PreviewRenderTarget->ClearColor = FLinearColor::Transparent;

	PreviewMaterialInstance = UMaterialInstanceDynamic::Create(previewMaterial, this);
	PreviewMaterialInstance->SetTextureParameterValue(TEXT("SceneCapture"), PreviewRenderTarget);
	
	PreviewScene = MakeUnique<FPreviewScene>( FPreviewScene::ConstructionValues() );
	UWorld* previewWorld = PreviewScene->GetWorld();
	RendererActor = previewWorld->SpawnActor<AMounteaAdvancedInventoryItemPreviewRenderer>(RendererActorClass);
	if (!IsValid(RendererActor))
	{
		CleanUpPreviewScene();
		return false;
	}
	
	PreviewScene->SetLightBrightness(3.0f);
	PreviewScene->SetSkyBrightness(1.0f);
	PreviewScene->UpdateCaptureContents();
	
	RendererActor->SetRenderTarget( PreviewRenderTarget );
	PreviewImage->SetBrushFromMaterial(PreviewMaterialInstance);

	if (bAutoStartTick)
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
	const UWorld* contextWorld = GetWorld();
	if (!contextWorld) return;
	
	if (FMath::IsNearlyZero(LastInteractionTime))
		LastInteractionTime = GetWorld()->GetTimeSeconds();
		
	if (!contextWorld->GetTimerManager().IsTimerActive(TimerHandle_PreviewTick) && !FMath::IsNearlyEqual(PreviewTickFrequency, 0.f, KINDA_SMALL_NUMBER))
	{
		const float interval = 1.f / PreviewTickFrequency;
		contextWorld->GetTimerManager().SetTimer(
			TimerHandle_PreviewTick,
			this,
			&UMounteaAdvancedInventoryInteractableObjectWidget::TickPreview,
			interval,
			true
		);

		FVector4 currentValues;
		RendererActor->GetCurrentValues(currentValues);
		CurrentZoom = currentValues.X;
		CurrentCameraHeight = currentValues.Y;
		CurrentPitch = currentValues.Z;
		CurrentYaw = currentValues.W;
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
	if (RendererActor)
	{
		RendererActor->ResetToDefaults();
		RendererActor->CaptureScene();
	}

	FVector4 currentValues;
	RendererActor->GetCurrentValues(currentValues);
	CurrentZoom = currentValues.X;
	CurrentCameraHeight = currentValues.Y;
	CurrentPitch = currentValues.Z;
	CurrentYaw = currentValues.W;

	OnPreviewReset();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::UpdateCameraRotation(const FVector2D& MouseDelta)
{
	if (!RendererActor) return;
	
	const float rotationSensitivity = CameraRotationSensitivity; // * CurrentZoom;
	
	CurrentYaw += MouseDelta.X * rotationSensitivity;
	CurrentPitch = FMath::Clamp(CurrentPitch - MouseDelta.Y * rotationSensitivity, -YawLimits, YawLimits);
	
	RendererActor->SetCameraRotation(CurrentYaw, CurrentPitch);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::UpdateCameraHeight(const FVector2D& MouseDelta)
{
	if (!RendererActor) return;
	
	const float heightSensitivity = CameraHeightSensitivity; // * CurrentZoom;
	const float heightIncrement = (MouseDelta.Y > 0) ? heightSensitivity : -heightSensitivity;
	
	CurrentCameraHeight = FMath::Clamp(CurrentCameraHeight + heightIncrement, -HeightLimit, HeightLimit);
	RendererActor->SetCameraHeight(CurrentCameraHeight);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::UpdateCameraZoom(const float WheelDelta)
{
	if (!RendererActor) return;
	
	const float zoomSensitivity = 1.f / CameraHeightSensitivity;
	const float zoomFactor = 1.0f + WheelDelta * zoomSensitivity;
	CurrentZoom = FMath::Clamp(CurrentZoom * zoomFactor, ScaleLimits.X, ScaleLimits.Y);
	
	RendererActor->SetCameraDistance(CurrentZoom);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetCameraRotationAbsolute(const float YawNormalized, const float PitchNormalized)
{
	if (!RendererActor) return;
	
	CurrentYaw = FMath::Lerp(-YawLimits, YawLimits, YawNormalized);
	CurrentPitch = FMath::Lerp(-YawLimits, YawLimits, PitchNormalized);
	
	RendererActor->SetCameraRotation(CurrentYaw, CurrentPitch);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetCameraHeightAbsolute(const float HeightNormalized)
{
	if (!RendererActor) return;
	
	CurrentCameraHeight = FMath::Lerp(-HeightLimit, HeightLimit, HeightNormalized);
	RendererActor->SetCameraHeight(CurrentCameraHeight);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::SetCameraZoomAbsolute(const float ZoomNormalized)
{
	if (!RendererActor) return;
	
	CurrentZoom = FMath::Lerp(ScaleLimits.X, ScaleLimits.Y, ZoomNormalized);
	RendererActor->SetCameraDistance(CurrentZoom);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::UpdateCameraRotationAnalog(const FVector2D& AnalogInput, const float DeltaTime)
{
	if (!RendererActor) return;
	
	const float rotationSpeed = YawLimits;
	const float adjustedSpeed = rotationSpeed / FMath::Max(CurrentZoom, SMALL_NUMBER);
	
	CurrentYaw += AnalogInput.X * adjustedSpeed * DeltaTime;
	CurrentPitch = FMath::Clamp(CurrentPitch - AnalogInput.Y * adjustedSpeed * DeltaTime, -YawLimits, YawLimits);
	
	RendererActor->SetCameraRotation(CurrentYaw, CurrentPitch);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::UpdateCameraHeightAnalog(const float AnalogInput, const float DeltaTime)
{
	if (!RendererActor) return;
	
	const float heightSpeed = HeightLimit * (ScaleLimits.Y - ScaleLimits.X);
	const float adjustedSpeed = heightSpeed / FMath::Max(CurrentZoom, SMALL_NUMBER);
	
	CurrentCameraHeight = FMath::Clamp(CurrentCameraHeight + AnalogInput * adjustedSpeed * DeltaTime, -HeightLimit, HeightLimit);
	RendererActor->SetCameraHeight(CurrentCameraHeight);
	UpdateLastInteractionAndStartPreview();
}

void UMounteaAdvancedInventoryInteractableObjectWidget::UpdateCameraZoomAnalog(const float AnalogInput, const float DeltaTime)
{
	if (!RendererActor) return;
	
	const float zoomSpeed = ScaleLimits.Y - ScaleLimits.X;
	const float zoomMultiplier = 1.0f + (AnalogInput * zoomSpeed * CameraHeightSensitivity * DeltaTime);
	
	CurrentZoom = FMath::Clamp(CurrentZoom * zoomMultiplier, ScaleLimits.X, ScaleLimits.Y);
	RendererActor->SetCameraDistance(CurrentZoom);
	UpdateLastInteractionAndStartPreview();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (!IsValid(RendererActor)) return FReply::Unhandled();
	
	const FVector2D currentMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const FVector2D mouseDelta = currentMousePos - LastMousePosition;
	
	if (bIsMousePressed)
	{
		UpdateCameraRotation(mouseDelta);
		LastMousePosition = currentMousePos;
		return FReply::Handled();
	}
	else if (bIsMiddleMousePressed)
	{
		UpdateCameraHeight(mouseDelta);
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
	if (!IsValid(RendererActor)) return FReply::Unhandled();
	
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
	if (!IsValid(RendererActor)) return FReply::Unhandled();
	
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
	if (!IsValid(RendererActor)) return FReply::Unhandled();
	
	UpdateCameraZoom(InMouseEvent.GetWheelDelta());
	return FReply::Handled();
}

FReply UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnFocusReceived(const FGeometry& InGeometry,
	const FFocusEvent& InFocusEvent)
{
	StartPreview();
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UMounteaAdvancedInventoryInteractableObjectWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	PausePreview();
}