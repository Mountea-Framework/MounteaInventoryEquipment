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
	void SetCameraHeight(const float ZOffset) const;
	void ResetToDefaults() const;
	void CaptureScene() const;

	void GetCurrentValues(FVector4& ScaleHeightPitchYaw) const;

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
	float InitialCameraHeight = 0.f; //90.f;
	float InitialCameraHeightSkeletalMesh = 90.f;

private:
	void AutoFitMeshInView() const;
	UPrimitiveComponent* GetActiveMeshComponent() const;
};
