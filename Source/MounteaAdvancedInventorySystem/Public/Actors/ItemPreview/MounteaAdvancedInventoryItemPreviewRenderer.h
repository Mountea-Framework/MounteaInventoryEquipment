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
 * @class AMounteaAdvancedInventoryItemPreviewRenderer
 *
 * @brief A class responsible for rendering advanced previews of inventory items
 * in the Mountea Inventory System.
 *
 * This class provides functionality to render detailed and customizable
 * previews for inventory items, potentially including 3D models, detailed
 * descriptions, animations, and other interactive elements.
 *
 * Key features include:
 * - Customizable preview templates for inventory items.
 * - Compatibility with advanced rendering systems and frameworks.
 * - Ability to dynamically update the preview based on inventory changes.
 *
 * Intended for use within the Mountea Framework, this renderer enhances the
 * user experience by providing visually engaging and detailed item previews.
 */
UCLASS(ClassGroup=(Mountea))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API AMounteaAdvancedInventoryItemPreviewRenderer : public AActor
{
	GENERATED_BODY()

public:
	AMounteaAdvancedInventoryItemPreviewRenderer();
	
	void SetRenderTarget(UTextureRenderTarget2D* NewRT) const;
	void SetStaticMesh(UStaticMesh* Mesh) const;
	void SetSkeletalMesh(USkeletalMesh* Mesh) const;
	void ClearMesh() const;
	void SetCameraRotation(const float Yaw, const float Pitch) const;
	void SetCameraDistance(const float Distance) const;
	void CaptureScene() const;

protected:
	virtual void BeginPlay() override;

public:
	/** Scene root. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	/** Pivot for mesh rotation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> PreviewPivotComponent;

	/** Holds a static mesh. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	/** Holds a skeletal mesh. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	/** Arm to position the camera. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	/** Scene capture that writes into the RT. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

	/** The render target to capture into. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

private:
	void AutoFitMeshInView() const;
	UPrimitiveComponent* GetActiveMeshComponent() const;
};
