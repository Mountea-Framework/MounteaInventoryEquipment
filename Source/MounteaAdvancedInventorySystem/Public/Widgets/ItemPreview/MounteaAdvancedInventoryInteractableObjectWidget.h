// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MounteaAdvancedInventoryInteractableObjectWidget.generated.h"

class UImage;
class UTextureRenderTarget2D;
class UWorld;
class ULevelStreamingDynamic;
class AMounteaAdvancedInventoryItemPreviewRenderer;
class UMaterialInstanceDynamic;
class AActor;

/**
 * Widget that streams in a preview level under a parent actor,
 * finds the renderer actor, and displays its render target in a UImage.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryInteractableObjectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Change the static mesh to preview. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void SetPreviewMesh(UStaticMesh* StaticMesh);

	/** Change the skeletal mesh to preview. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void SetPreviewSkeletalMesh(USkeletalMesh* SkeletalMesh);

	/** Clear the previewed mesh. */
	UFUNCTION(BlueprintCallable, Category="Item Preview")
	void ClearPreview();
	
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	/** Image widget to display the render target. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PreviewImage;

	/** The render target to display. */
	UPROPERTY(EditAnywhere, Category="Preview Settings")
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	/** Which actor class inside the level will render. */
	UPROPERTY(EditAnywhere, Category="Preview Settings")
	TSubclassOf<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActorClass;

	/** Parent actor under which to attach the streamed level. */
	UPROPERTY(EditAnywhere, Category="Preview Settings")
	TObjectPtr<AActor> PreviewParentActor;

private:
	void InitializePreviewLevel();
	void CleanupPreviewLevel();

	UFUNCTION()
	void OnLevelLoaded();

	UPROPERTY(Transient)
	TObjectPtr<ULevelStreamingDynamic> LevelInstance;

	UPROPERTY(Transient)
	TObjectPtr<UWorld> PreviewWorld;

	UPROPERTY(Transient)
	TObjectPtr<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActor;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	bool	  bIsMousePressed = false;
	FVector2D LastMousePosition;
	float	 CurrentYaw	  = 45.0f;
	float	 CurrentPitch	= -20.0f;
	float	 CurrentZoom	 = 200.0f;
};
