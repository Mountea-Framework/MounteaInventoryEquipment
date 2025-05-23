// All rights reserved Dominik Morse 2024

#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"

AMounteaAdvancedInventoryItemPreviewRenderer::AMounteaAdvancedInventoryItemPreviewRenderer()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	PreviewPivotSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	PreviewPivotSceneComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(PreviewPivotSceneComponent);
	StaticMeshComponent->SetVisibility(false);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(PreviewPivotSceneComponent);
	SkeletalMeshComponent->SetVisibility(false);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 200.0f;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetRelativeRotation(FRotator(-20.0f, 45.0f, 0.0f));

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCaptureComponent->SetupAttachment(SpringArmComponent);
	SceneCaptureComponent->CaptureSource = SCS_FinalColorLDR;
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Perspective;
	SceneCaptureComponent->FOVAngle = 45.0f;
	SceneCaptureComponent->bCaptureEveryFrame = true;
	SceneCaptureComponent->bCaptureOnMovement = true;
	SceneCaptureComponent->ShowOnlyActors.Add(this);

	RenderTarget = nullptr;
}

void AMounteaAdvancedInventoryItemPreviewRenderer::BeginPlay()
{
	Super::BeginPlay();
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetRenderTarget(UTextureRenderTarget2D* NewRenderTarget)
{
	RenderTarget = NewRenderTarget;
	if (SceneCaptureComponent)
	{
		SceneCaptureComponent->TextureTarget = NewRenderTarget;
		SceneCaptureComponent->ShowOnlyActors.Empty();
		SceneCaptureComponent->ShowOnlyActors.Add(this);
		SceneCaptureComponent->CaptureScene();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetStaticMesh(UStaticMesh* Mesh)
{
	ClearMesh();
	if (StaticMeshComponent && Mesh)
	{
		StaticMeshComponent->SetStaticMesh(Mesh);
		StaticMeshComponent->SetVisibility(true);
		AutoFitMeshInView();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetSkeletalMesh(USkeletalMesh* Mesh)
{
	ClearMesh();
	if (SkeletalMeshComponent && Mesh)
	{
		SkeletalMeshComponent->SetSkeletalMesh(Mesh);
		SkeletalMeshComponent->SetVisibility(true);
		AutoFitMeshInView();
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraRotation(const float Yaw, float Pitch) const
{
	Pitch = FMath::Clamp(Pitch, -80.0f, 80.0f);
	PreviewPivotSceneComponent->SetRelativeRotation(FRotator(Pitch, Yaw, 0.0f));
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraDistance(float Distance) const
{
	Distance = FMath::Clamp(Distance, 50.0f, 1000.0f);
	if (SpringArmComponent)
	{
		SpringArmComponent->TargetArmLength = Distance;
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::ClearMesh() const
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetStaticMesh(nullptr);
		StaticMeshComponent->SetVisibility(false);
	}
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetSkeletalMesh(nullptr);
		SkeletalMeshComponent->SetVisibility(false);
	}
}

void AMounteaAdvancedInventoryItemPreviewRenderer::AutoFitMeshInView() const
{
	UPrimitiveComponent* activeMesh = GetActiveMeshComponent();
	if (!activeMesh) return;

	auto localBounds = activeMesh->GetLocalBounds();

	float maxExtend = FMath::Max3(localBounds.BoxExtent.X, localBounds.BoxExtent.Y, localBounds.BoxExtent.Z);
	float optimalDistance = maxExtend * 2.5f;
	SpringArmComponent->TargetArmLength = FMath::Clamp(optimalDistance, 100.0f, 800.0f);
}

UPrimitiveComponent* AMounteaAdvancedInventoryItemPreviewRenderer::GetActiveMeshComponent() const
{
	if (StaticMeshComponent && StaticMeshComponent->IsVisible())
		return StaticMeshComponent;
	if (SkeletalMeshComponent && SkeletalMeshComponent->IsVisible())
		return SkeletalMeshComponent;
	return nullptr;
}

#if WITH_EDITOR
void AMounteaAdvancedInventoryItemPreviewRenderer::SetPreviewMesh()
{
	if (UPrimitiveComponent* activeMesh = GetActiveMeshComponent())
		activeMesh->SetVisibility(true);
}
#endif
