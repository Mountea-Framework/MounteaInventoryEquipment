// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MounteaAdvancedInventoryItemPreviewRenderer.generated.h"

class USpringArmComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USceneCaptureComponent2D;
class USceneComponent;
class UTextureRenderTarget2D;
class UStaticMesh;
class USkeletalMesh;

/**
 * Represents a preview renderer for advanced inventory items.
 *
 * This actor is designed to render both static and skeletal meshes for preview purposes,
 * with customization options such as dynamic mesh updates, camera distance, and rotation adjustments.
 * It provides functionality to handle mesh clearing and ensures the previewed items fit within the
 * defined view by automatically adjusting the camera if required.
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"),
	   HideCategories=("Cooking"), meta=(DisplayName="Mountea Item Renderer"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API AMounteaAdvancedInventoryItemPreviewRenderer : public AActor
{
	GENERATED_BODY()

public:
	AMounteaAdvancedInventoryItemPreviewRenderer();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * Assigns a render target to the preview renderer for capturing rendered output.
	 *
	 * @param NewRenderTarget A pointer to a UTextureRenderTarget2D to be used for rendering.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Renderer")
	void SetRenderTarget(UTextureRenderTarget2D* NewRenderTarget);

	/**
	 * Sets a new static mesh for the preview renderer.
	 *
	 * @param Mesh A pointer to the UStaticMesh to be set for rendering. Pass nullptr to clear.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Renderer")
	void SetStaticMesh(UStaticMesh* Mesh);

	/**
	 * Sets a new skeletal mesh for the preview renderer.
	 *
	 * @param Mesh A pointer to the USkeletalMesh to be set for rendering. Pass nullptr to clear.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Renderer")
	void SetSkeletalMesh(USkeletalMesh* Mesh);

	/**
	 * Updates the camera's rotation in the preview renderer.
	 *
	 * @param Yaw Rotation around the vertical axis, in degrees.
	 * @param Pitch Rotation around the horizontal axis, clamped between -80.0f and 80.0f.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Renderer")
	void SetCameraRotation(float Yaw, float Pitch) const;

	/**
	 * Adjusts the distance of the camera from the previewed item.
	 *
	 * @param Distance The desired distance, in units (clamped between 50.0f and 1000.0f).
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Renderer")
	void SetCameraDistance(float Distance) const;

	/**
	 * Clears the current mesh from the preview renderer.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Renderer")
	void ClearMesh() const;

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Preview")
	void SetPreviewMesh();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Preview")
	FORCEINLINE void ClearPreviewMesh() { ClearMesh(); };
#endif

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Components")
	USceneComponent* PreviewPivotSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Components")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Components")
	USceneCaptureComponent2D* SceneCaptureComponent;

	/** The render target where the scene capture will output the preview image. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Rendering")
	UTextureRenderTarget2D* RenderTarget;

private:
	void AutoFitMeshInView() const;
	UPrimitiveComponent* GetActiveMeshComponent() const;
};
