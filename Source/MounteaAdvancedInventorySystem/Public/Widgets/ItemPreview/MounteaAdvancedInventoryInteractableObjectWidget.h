// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MounteaAdvancedInventoryBaseWidget.h"
#include "Components/Viewport.h"
#include "MounteaAdvancedInventoryInteractableObjectWidget.generated.h"

class UImage;
class UViewport;
class UTextureRenderTarget2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class ULevelStreamingDynamic;
class UWorld;
class UStaticMesh;
class USkeletalMesh;
class AMounteaAdvancedInventoryItemPreviewRenderer;

/**
 * A self-contained widget for interactable inventory objects that manages its own preview world.
 *
 * This widget creates and manages its own preview world and renderer actor, providing
 * functionality for previewing inventory items including both static and skeletal meshes.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryInteractableObjectWidget : public UMounteaAdvancedInventoryBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Sets the static mesh to preview. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Preview")
	void SetPreviewMesh(UStaticMesh* StaticMesh);

	/** Sets the skeletal mesh to preview. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Preview")
	void SetPreviewSkeletalMesh(USkeletalMesh* SkeletalMesh);

	/** Clears the current preview. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Preview")
	void ClearPreview();

	/** Rotates the preview camera. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Preview")
	void RotatePreview(float DeltaYaw, float DeltaPitch);

	/** Zooms the preview camera. */
	UFUNCTION(BlueprintCallable, Category="Mountea|Item Preview")
	void ZoomPreview(float ZoomDelta);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PreviewImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNamedSlot> PreviewSlot;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UFUNCTION()
	void OnPreviewLevelShown();

	void InitializePreviewWorld();
	void CleanupPreviewWorld();
	void SetupRendererActor() const;
	void SetupMaterial();

	UPROPERTY()
	TObjectPtr<UViewport> PreviewViewport = nullptr;

	/** Render target asset for capturing the preview image. */
	UPROPERTY(EditAnywhere, Category="Preview")
	TSoftObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	/** Base material for displaying the render target in the UImage. */
	UPROPERTY(EditAnywhere, Category="Preview")
	TSoftObjectPtr<UMaterialInterface> PreviewMaterial;

	UPROPERTY(Transient)
	TObjectPtr<ULevelStreamingDynamic> PreviewLevelInstance;

	UPROPERTY(Transient)
	TObjectPtr<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActor;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;

	bool bIsMousePressed = false;
	FVector2D LastMousePosition;
	float CurrentYaw = 45.0f;
	float CurrentPitch = -20.0f;
	float CurrentZoom = 200.0f;
};
