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
	void SetStaticMesh(UStaticMesh* Mesh);
	void SetSkeletalMesh(USkeletalMesh* Mesh);
	void ClearMesh();
	void SetCameraRotation(const float Yaw, const float Pitch) const;
	void SetCameraDistance(const float ZoomLevel);
	void SetCameraHeight(const float ZOffset) const;
	void ResetToDefaults();
	void CaptureScene() const;

	void GetCurrentValues(FVector4& ZoomHeightYawPitch) const;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> PreviewPivotComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

private:
	float InitialCameraHeight = 0.f;
	float BaseFitScale = 1.0f;
	float CurrentUserZoom = 1.0f;

private:
	void AutoFitMeshInView();
	void ApplyCombinedScale() const;
	UPrimitiveComponent* GetActiveMeshComponent() const;
};
