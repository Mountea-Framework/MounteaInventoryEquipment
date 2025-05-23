// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MounteaAdvancedInventoryItemPreviewRenderer.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UStaticMesh;
class USkeletalMesh;

/**
 * Renders a single mesh into a render target for UI preview.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API AMounteaAdvancedInventoryItemPreviewRenderer : public AActor
{
	GENERATED_BODY()

public:
	AMounteaAdvancedInventoryItemPreviewRenderer();

	/** Assign the render target for capturing. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void SetRenderTarget(UTextureRenderTarget2D* NewRT);

	/** Display a static mesh. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void SetStaticMesh(UStaticMesh* Mesh);

	/** Display a skeletal mesh. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void SetSkeletalMesh(USkeletalMesh* Mesh);

	/** Clear any displayed mesh. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void ClearMesh();

	/** Rotate the preview camera. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void SetCameraRotation(float Yaw, float Pitch);

	/** Zoom the preview camera. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void SetCameraDistance(float Distance);

protected:
	virtual void BeginPlay() override;

	/** Scene root. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	/** Pivot for mesh rotation. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> PreviewPivotComponent;

	/** Component that should be used to generate lightning for this actor. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> EmissionDomeComponent;

	/** Holds a static mesh. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	/** Holds a skeletal mesh. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	/** Arm to position the camera. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	/** Scene capture that writes into the RT. */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

	/** The render target to capture into. */
	UPROPERTY(EditAnywhere, Category="Rendering")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

private:
	void AutoFitMeshInView();
	UPrimitiveComponent* GetActiveMeshComponent() const;
};
