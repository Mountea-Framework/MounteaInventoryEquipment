﻿// All rights reserved Dominik Morse 2024

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
	SpringArmComponent->SetRelativeLocation(FVector(0, 0, 90));

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

void AMounteaAdvancedInventoryItemPreviewRenderer::SetRenderTarget(UTextureRenderTarget2D* NewRT)
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
	const float clampedDistance = FMath::Clamp(Distance, 50.0f, 1000.0f);
	SpringArmComponent->TargetArmLength = clampedDistance;
}

void AMounteaAdvancedInventoryItemPreviewRenderer::CaptureScene() const
{
	if (!IsValid(SceneCaptureComponent)) return;

	if (SceneCaptureComponent->bCaptureEveryFrame) return;
	
 	SceneCaptureComponent->CaptureScene();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::AutoFitMeshInView() const
{
	UPrimitiveComponent* activeComp = GetActiveMeshComponent();
	if (!activeComp)
		return;
	const auto localBounds = activeComp->GetLocalBounds();

	const float maxExtent = FMath::Max3(localBounds.BoxExtent.X, localBounds.BoxExtent.Y, localBounds.BoxExtent.Z);
	const float desiredDistance = maxExtent * 2.5f;
	SpringArmComponent->TargetArmLength = FMath::Clamp(desiredDistance, 100.0f, 800.0f);
}

UPrimitiveComponent* AMounteaAdvancedInventoryItemPreviewRenderer::GetActiveMeshComponent() const
{
	if (StaticMeshComponent->IsVisible())
		return StaticMeshComponent;
	if (SkeletalMeshComponent->IsVisible())
		return SkeletalMeshComponent;
	return nullptr;
}
