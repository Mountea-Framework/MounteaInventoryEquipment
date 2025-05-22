// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MounteaAdvancedInventoryItemPreviewRenderer.generated.h"

class USpringArmComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UTextureRenderTarget2D;

/**
 * Represents a preview renderer for advanced inventory items.
 *
 * This actor is designed to render both static and skeletal meshes for preview purposes,
 * with customization options such as dynamic mesh updates, camera distance, and rotation adjustments.
 * It provides functionality to handle mesh clearing and ensures the previewed items fit within the
 * defined view by automatically adjusting the camera if required.
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,  AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"),
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
	 * Sets a new static mesh for the preview renderer.
	 *
	 * Clears the existing mesh in the preview renderer and assigns a new static mesh to the
	 * StaticMeshComponent. If the provided mesh is not null, it becomes visible and automatically
	 * fits within the view of the renderer.
	 *
	 * @param Mesh A pointer to the UStaticMesh to be set for rendering. Pass nullptr to clear the current mesh.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Renderer")
	void SetStaticMesh(UStaticMesh* Mesh);

	/**
	 * Sets a new skeletal mesh for the preview renderer.
	 *
	 * This method clears any existing mesh in the preview and assigns a new skeletal mesh
	 * to the SkeletalMeshComponent. If the provided mesh is not null, it becomes visible
	 * and automatically adjusts the view to fit the mesh.
	 *
	 * @param Mesh A pointer to the USkeletalMesh to be set for rendering. Pass nullptr to clear the current mesh.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Renderer")
	void SetSkeletalMesh(USkeletalMesh* Mesh);

	/**
	 * Updates the camera's rotation in the preview renderer.
	 *
	 * Adjusts the spring arm's pitch and yaw angles to orient the camera.
	 * The pitch angle is clamped between -80.0f and 80.0f to prevent undesirable camera positions.
	 *
	 * @param Yaw The rotation around the vertical axis, in degrees.
	 * @param Pitch The rotation around the horizontal axis, clamped between -80.0f and 80.0f, in degrees.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Renderer")
	void SetCameraRotation(float Yaw, float Pitch);

	/**
	 * Adjusts the distance of the camera from the previewed item.
	 *
	 * This function sets the target arm length of the SpringArmComponent, effectively changing
	 * how far the camera is positioned from the rendered item. The distance is clamped
	 * between a minimum of 50.0f and a maximum of 1000.0f to ensure the camera stays within
	 * a reasonable range.
	 *
	 * @param Distance The desired distance of the camera from the previewed item, in units.
	 *                 Values outside the clamped range will be automatically adjusted.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Renderer")
	void SetCameraDistance(float Distance);

	/**
	 * Clears the current mesh from the preview renderer.
	 *
	 * This method resets both the static and skeletal mesh components of the actor by
	 * setting their respective meshes to null and hiding them from view. It ensures that
	 * the preview renderer is in a clean state, ready for a new mesh to be assigned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Renderer")
	void ClearMesh();

#if WITH_EDITOR

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Item Renderer")
	void SetPreviewMesh();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Item Renderer")
	FORCEINLINE void ClearPreviewMesh()
	{ ClearMesh(); };
	
#endif
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

private:
	void AutoFitMeshInView() const;
	UPrimitiveComponent* GetActiveMeshComponent() const;
};
