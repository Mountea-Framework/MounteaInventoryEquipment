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

#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"

AMounteaAdvancedInventoryItemPreviewRenderer::AMounteaAdvancedInventoryItemPreviewRenderer()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	PreviewPivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	PreviewPivotComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(PreviewPivotComponent);
	StaticMeshComponent->SetVisibility(false);
	StaticMeshComponent->bVisibleInRayTracing = false;
	StaticMeshComponent->bVisibleInRealTimeSkyCaptures = false;
	StaticMeshComponent->bVisibleInReflectionCaptures = false;
	StaticMeshComponent->bUseAsOccluder = false;
	StaticMeshComponent->bVisibleInSceneCaptureOnly = true;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(PreviewPivotComponent);
	SkeletalMeshComponent->SetVisibility(false);
	SkeletalMeshComponent->bVisibleInRayTracing = false;
	SkeletalMeshComponent->bVisibleInRealTimeSkyCaptures = false;
	SkeletalMeshComponent->bVisibleInReflectionCaptures = false;
	SkeletalMeshComponent->bVisibleInSceneCaptureOnly = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootSceneComponent);
	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->bEnableCameraLag = false;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, InitialCameraHeight));

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCaptureComponent->SetupAttachment(SpringArmComponent);
	SceneCaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComponent->CaptureSource = SCS_SceneColorHDR;
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Perspective;
	SceneCaptureComponent->FOVAngle = 45.0f;
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->bCaptureOnMovement = false;
	SceneCaptureComponent->bAlwaysPersistRenderingState = true;
	SceneCaptureComponent->ShowOnlyActors.Add(this);
	SceneCaptureComponent->ShowOnlyComponents.Add(StaticMeshComponent);
	SceneCaptureComponent->ShowOnlyComponents.Add(SkeletalMeshComponent);
}

void AMounteaAdvancedInventoryItemPreviewRenderer::BeginPlay()
{
	Super::BeginPlay();
	
	SetCameraRotation(0.f, 0.f);
	CurrentUserZoom = 1.0f;
	AutoFitMeshInView();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetRenderTarget(UTextureRenderTarget2D* NewRT) const
{
	if (SceneCaptureComponent)
	{
		SceneCaptureComponent->TextureTarget = NewRT;
		SceneCaptureComponent->CaptureScene();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetStaticMesh(UStaticMesh* Mesh)
{
	ClearMesh();
	if (Mesh)
	{
		StaticMeshComponent->SetStaticMesh(Mesh);
		StaticMeshComponent->SetVisibility(true);
		CurrentUserZoom = 1.0f;
		AutoFitMeshInView();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetSkeletalMesh(USkeletalMesh* Mesh)
{
	ClearMesh();
	if (Mesh)
	{
		SkeletalMeshComponent->SetSkeletalMesh(Mesh);
		SkeletalMeshComponent->SetVisibility(true);
		CurrentUserZoom = 1.0f;
		AutoFitMeshInView();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::ClearMesh()
{
	StaticMeshComponent->SetStaticMesh(nullptr);
	StaticMeshComponent->SetVisibility(false);
	SkeletalMeshComponent->SetSkeletalMesh(nullptr);
	SkeletalMeshComponent->SetVisibility(false);
	BaseFitScale = 1.0f;
	CurrentUserZoom = 1.0f;
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraRotation(const float Yaw, const float Pitch) const
{
	const float clampedPitch = FMath::Clamp(Pitch, -80.0f, 80.0f);
	PreviewPivotComponent->SetRelativeRotation(FRotator(clampedPitch, Yaw, 0.0f));
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraDistance(const float ZoomLevel)
{
	CurrentUserZoom = ZoomLevel;
	ApplyCombinedScale();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraHeight(const float ZOffset) const
{
	FVector location = SpringArmComponent->GetRelativeLocation();
	location.Z = InitialCameraHeight + ZOffset;
	SpringArmComponent->SetRelativeLocation(location);
}

void AMounteaAdvancedInventoryItemPreviewRenderer::ResetToDefaults()
{
	PreviewPivotComponent->SetRelativeRotation(FRotator::ZeroRotator);
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, InitialCameraHeight));
	CurrentUserZoom = 1.0f;
	AutoFitMeshInView();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::CaptureScene() const
{
	if (!IsValid(SceneCaptureComponent)) return;
	if (SceneCaptureComponent->bCaptureEveryFrame) return;
	
	SceneCaptureComponent->CaptureScene();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::GetCurrentValues(FVector4& ZoomHeightYawPitch) const
{
	ZoomHeightYawPitch.X = CurrentUserZoom;
	ZoomHeightYawPitch.Y = SpringArmComponent->GetRelativeLocation().Z;
	ZoomHeightYawPitch.Z = PreviewPivotComponent->GetRelativeRotation().Yaw;
	ZoomHeightYawPitch.W = PreviewPivotComponent->GetRelativeRotation().Pitch;
}

void AMounteaAdvancedInventoryItemPreviewRenderer::AutoFitMeshInView()
{
	UPrimitiveComponent* activeComp = GetActiveMeshComponent();
	if (!activeComp)
	{
		BaseFitScale = 1.0f;
		ApplyCombinedScale();
		return;
	}

	const FBoxSphereBounds bounds = activeComp->GetLocalBounds();
	const float meshRadius = bounds.SphereRadius;
	
	if (FMath::IsNearlyZero(meshRadius, 0.001f))
	{
		BaseFitScale = 1.0f;
		ApplyCombinedScale();
		return;
	}

	const float halfFOV = FMath::DegreesToRadians(SceneCaptureComponent->FOVAngle * 0.5f);
	const float cameraDistance = SpringArmComponent->TargetArmLength;
	const float viewRadius = cameraDistance * FMath::Tan(halfFOV);
	
	const float desiredScreenRatio = 0.8f;
	const float targetViewRadius = viewRadius * desiredScreenRatio;
	
	BaseFitScale = targetViewRadius / meshRadius;
	BaseFitScale = FMath::Clamp(BaseFitScale, 0.01f, 100.0f);
	
	ApplyCombinedScale();
	SetCameraHeight(bounds.Origin.Z * BaseFitScale);
}

void AMounteaAdvancedInventoryItemPreviewRenderer::ApplyCombinedScale() const
{
	const float finalScale = BaseFitScale * CurrentUserZoom;
	PreviewPivotComponent->SetRelativeScale3D(FVector(finalScale));
}

UPrimitiveComponent* AMounteaAdvancedInventoryItemPreviewRenderer::GetActiveMeshComponent() const
{
	if (StaticMeshComponent->IsVisible())
		return StaticMeshComponent;
	if (SkeletalMeshComponent->IsVisible())
		return SkeletalMeshComponent;
	return nullptr;
}
