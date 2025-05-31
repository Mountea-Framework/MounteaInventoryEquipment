// MounteaAdvancedInventoryInteractableObjectWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MounteaAdvancedInventoryBaseWidget.h"
#include "PreviewScene.h"
#include "MounteaAdvancedInventoryInteractableObjectWidget.generated.h"

class UImage;
class UTextureRenderTarget2D;
class UStaticMesh;
class USkeletalMesh;
class AMounteaAdvancedInventoryItemPreviewRenderer;

/**
 * @class UMounteaAdvancedInventoryInteractableObjectWidget
 *
 * @brief Represents an advanced interactable widget specifically designed for inventory objects in the Mountea framework.
 *
 * This class is intended to handle and display information related to inventory interactable objects.
 * It serves as an integral part of the user interface, providing functionality and visualization
 * to interact with inventory elements in virtual 3D space.
 *
 * The class integrates seamlessly with the Mountea Inventory system, and may include features such
 * as dynamic updates, custom interactions, and extended customization for specific inventory objects.
 *
 * Key features include:
 * - 3D representation of inventory objects through widgets.
 * - Customizable behavior for interaction with inventory items.
 */
UCLASS(ClassGroup=(Mountea), meta=(DisplayName="Interactable Object Widget"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryInteractableObjectWidget : public UMounteaAdvancedInventoryBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	bool InitializeInteractableWidget();
	void CleanUpPreviewScene();
	void SetPreviewMesh(UStaticMesh* StaticMesh) const;
	void SetPreviewSkeletalMesh(USkeletalMesh* SkeletalMesh) const;
	void ClearPreview() const;

	void StartPreview();
	void PausePreview();

	UFUNCTION()
	void TickPreview();
	void UpdateLastInteractionAndStartPreview();

	void ResetCameraToDefaults();

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PreviewImage;

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings")
	bool bAutoStartTick = true;

	/**
	 * How many times per second to update the preview. The higher the value,
	 * the more responsive the preview will be, yet the higher the performance cost.
	 */
	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings", meta=(UIMin = "0.0", ClampMin = "0.0", UIMax = "60.0", ClampMax = "60.0"))
	float PreviewTickFrequency = 30.f;

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings", meta=(UIMin = "0.0", ClampMin = "0.0", UIMax = "60.0", ClampMax = "60.0"))
	float IdleThreshold = 3.f;

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings", meta=(UIMin = "0.01", ClampMin = "0.01", UIMax = "100.0", ClampMax = "100.0"))
	FVector2D ScaleLimits = FVector2D(0.1f, 10.f);

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings", meta=(UIMin = "0.01", ClampMin = "0.01", UIMax = "10.0", ClampMax = "10.0"))
	float CameraHeightSensitivity = 0.2f;

	UPROPERTY(VisibleAnywhere, Category="Mountea|Preview Settings")
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;
	
	UPROPERTY(VisibleAnywhere, Category="Mountea|Preview Settings")
	TObjectPtr<UMaterialInstanceDynamic> PreviewMaterialInstance;

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings")
	TSubclassOf<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActorClass;

private:
	UPROPERTY()
	TObjectPtr<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActor = nullptr;

	TUniquePtr<FPreviewScene> PreviewScene;

	FTimerHandle TimerHandle_PreviewTick;

	float LastInteractionTime = 0.0f;
	FVector2D LastMousePosition = FVector2D::ZeroVector;
	bool bIsMiddleMousePressed = false;
	bool bIsMousePressed = false;
	float CurrentYaw   = 0.0f;
	float CurrentPitch = 0.0f;
	float CurrentZoom  = 1.0f;
	float CurrentCameraHeight = 0.0f;
};
