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
	RootComponent	  = RootSceneComponent;

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
	SpringArmComponent->TargetArmLength  = 300;
	SpringArmComponent->bEnableCameraLag = false;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.0f));
	SpringArmComponent->SetRelativeLocation(FVector(0, 0, InitialCameraHeight));

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCaptureComponent->SetupAttachment(SpringArmComponent);
	SceneCaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComponent->CaptureSource	  = SCS_SceneColorHDR;
	SceneCaptureComponent->ProjectionType	 = ECameraProjectionMode::Perspective;
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

	AutoFitMeshInView();
	SetCameraRotation(0, 0);
	SetCameraDistance(300.f);
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetRenderTarget(UTextureRenderTarget2D* NewRT) const
{
	if (SceneCaptureComponent)
	{
		SceneCaptureComponent->TextureTarget = NewRT;
		SceneCaptureComponent->CaptureScene();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetStaticMesh(UStaticMesh* Mesh) const
{
	ClearMesh();
	if (Mesh)
	{
		StaticMeshComponent->SetStaticMesh(Mesh);
		StaticMeshComponent->SetVisibility(true);
		AutoFitMeshInView();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetSkeletalMesh(USkeletalMesh* Mesh) const
{
	ClearMesh();
	if (Mesh)
	{
		SkeletalMeshComponent->SetSkeletalMesh(Mesh);
		SkeletalMeshComponent->SetVisibility(true);
		AutoFitMeshInView();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::ClearMesh() const
{
	StaticMeshComponent->SetStaticMesh(nullptr);
	StaticMeshComponent->SetVisibility(false);
	SkeletalMeshComponent->SetSkeletalMesh(nullptr);
	SkeletalMeshComponent->SetVisibility(false);
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraRotation(const float Yaw, const float Pitch) const
{
	const float clampedPitch = FMath::Clamp(Pitch, -80.0f, 80.0f);
	SpringArmComponent->SetRelativeRotation(FRotator(clampedPitch, Yaw, 0.0f));
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraDistance(const float Distance) const
{
	SpringArmComponent->SetRelativeScale3D(FVector(Distance));
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraHeight(const float ZOffset) const
{
	FVector baseLocation = SpringArmComponent->GetRelativeLocation();
	baseLocation.Z = InitialCameraHeight + ZOffset;
	SpringArmComponent->SetRelativeLocation(baseLocation);
}

void AMounteaAdvancedInventoryItemPreviewRenderer::ResetToDefaults() const
{
	SpringArmComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.0f));
	SpringArmComponent->SetRelativeLocation(FVector(0, 0, InitialCameraHeight));
	SpringArmComponent->SetRelativeScale3D(FVector(1));
	AutoFitMeshInView();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::CaptureScene() const
{
	if (!IsValid(SceneCaptureComponent)) return;

	if (SceneCaptureComponent->bCaptureEveryFrame) return;
	
 	SceneCaptureComponent->CaptureScene();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::GetCurrentValues(FVector4& ScaleHeightPitchYaw) const
{
	ScaleHeightPitchYaw.X = SpringArmComponent->GetRelativeScale3D().Z;
	ScaleHeightPitchYaw.Y = SpringArmComponent->GetRelativeLocation().Z;
	ScaleHeightPitchYaw.Z = SpringArmComponent->GetRelativeRotation().Yaw;
	ScaleHeightPitchYaw.W = SpringArmComponent->GetRelativeRotation().Pitch;
}

void AMounteaAdvancedInventoryItemPreviewRenderer::AutoFitMeshInView() const
{
	UPrimitiveComponent* activeComp = GetActiveMeshComponent();
	if (!activeComp)
		return;

	const float meshRadius = activeComp->GetLocalBounds().SphereRadius;
	PreviewPivotComponent->SetRelativeScale3D(FVector::OneVector);
	const float halfFOV = FMath::DegreesToRadians(SceneCaptureComponent->FOVAngle * 0.5f);
	const float baseDistance = SpringArmComponent->TargetArmLength;
	const float viewRadius = baseDistance * FMath::Tan(halfFOV);
    
	const float desiredScreenRatio = 0.8f;
	const float targetViewRadius = viewRadius * desiredScreenRatio;
	const float requiredScale = meshRadius / targetViewRadius;
	const float clampedScale = FMath::Clamp(requiredScale, 0.1f, 10.0f);

	SetCameraDistance(clampedScale);
	const FBoxSphereBounds meshBounds = activeComp->GetLocalBounds();
	SetCameraHeight(meshBounds.Origin.Z);
}

UPrimitiveComponent* AMounteaAdvancedInventoryItemPreviewRenderer::GetActiveMeshComponent() const
{
	if (StaticMeshComponent->IsVisible())
		return StaticMeshComponent;
	if (SkeletalMeshComponent->IsVisible())
		return SkeletalMeshComponent;
	return nullptr;
}
