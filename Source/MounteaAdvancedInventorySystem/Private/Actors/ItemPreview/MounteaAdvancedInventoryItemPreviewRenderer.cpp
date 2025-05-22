// All rights reserved Dominik Morse 2024


#include "Actors/ItemPreview/MounteaAdvancedInventoryItemPreviewRenderer.h"

#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"

AMounteaAdvancedInventoryItemPreviewRenderer::AMounteaAdvancedInventoryItemPreviewRenderer()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	// Create mesh components
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetVisibility(false);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->SetVisibility(false);

	// Create spring arm for camera positioning
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 200.0f;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetRelativeRotation(FRotator(-20.0f, 45.0f, 0.0f));

	// Create scene capture component
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCaptureComponent->SetupAttachment(SpringArmComponent);
	SceneCaptureComponent->CaptureSource = SCS_FinalColorLDR;
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Perspective;
	SceneCaptureComponent->FOVAngle = 45.0f;
}

void AMounteaAdvancedInventoryItemPreviewRenderer::BeginPlay()
{
	Super::BeginPlay();
	
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

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraRotation(float Yaw, float Pitch)
{
	Pitch = FMath::Clamp(Pitch, -80.0f, 80.0f);
	SpringArmComponent->SetRelativeRotation(FRotator(Pitch, Yaw, 0.0f));
}

void AMounteaAdvancedInventoryItemPreviewRenderer::SetCameraDistance(float Distance)
{
	Distance = FMath::Clamp(Distance, 50.0f, 1000.0f);
	SpringArmComponent->TargetArmLength = Distance;
}

void AMounteaAdvancedInventoryItemPreviewRenderer::ClearMesh()
{
	StaticMeshComponent->SetStaticMesh(nullptr);
	StaticMeshComponent->SetVisibility(false);
    
	SkeletalMeshComponent->SetSkeletalMesh(nullptr);
	SkeletalMeshComponent->SetVisibility(false);
}

void AMounteaAdvancedInventoryItemPreviewRenderer::AutoFitMeshInView() const
{
	UPrimitiveComponent* activeMesh = GetActiveMeshComponent();
	if (!activeMesh)
		return;

	const auto meshBounds = activeMesh->GetLocalBounds();
    
	float maxExtend = FMath::Max3(meshBounds.BoxExtent.X, meshBounds.BoxExtent.Y, meshBounds.BoxExtent.Z);
	float optimalDistance = maxExtend * 2.5f;
    
	SpringArmComponent->TargetArmLength = FMath::Clamp(optimalDistance, 100.0f, 800.0f);
}

UPrimitiveComponent* AMounteaAdvancedInventoryItemPreviewRenderer::GetActiveMeshComponent() const
{
	if (StaticMeshComponent->IsVisible())
		return StaticMeshComponent;
	else if (SkeletalMeshComponent->IsVisible())
		return SkeletalMeshComponent;
    
	return nullptr;
}

#if WITH_EDITOR

void AMounteaAdvancedInventoryItemPreviewRenderer::SetPreviewMesh()
{
	auto activeMesh = GetActiveMeshComponent();
	if (!activeMesh)
		return;
	
	activeMesh->SetVisibility(true);
}

#endif
