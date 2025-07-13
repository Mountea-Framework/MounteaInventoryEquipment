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
 * UMounteaAdvancedInventoryInteractableObjectWidget provides 3D preview and interaction for inventory items.
 * Interactable widgets render inventory items in 3D space with camera controls, mesh preview capabilities,
 * and interactive manipulation for detailed item inspection within the inventory interface.
 *
 * @see [3D Item Preview](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemPreview)
 * @see UMounteaAdvancedInventoryBaseWidget
 * @see AMounteaAdvancedInventoryItemPreviewRenderer
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
	
public:
	// Mouse delta-based functions
	void UpdateCameraRotation(const FVector2D& MouseDelta);
	void UpdateCameraHeight(const FVector2D& MouseDelta);
	void UpdateCameraZoom(const float WheelDelta);
    
	// Absolute value functions for sliders (0.0 to 1.0 range)
	void SetCameraRotationAbsolute(const float YawNormalized, const float PitchNormalized);
	void SetCameraHeightAbsolute(const float HeightNormalized);
	void SetCameraZoomAbsolute(const float ZoomNormalized);
    
	// Analog input functions for gamepad (per-frame updates with -1.0 to 1.0 range)
	void UpdateCameraRotationAnalog(const FVector2D& AnalogInput, const float DeltaTime);
	void UpdateCameraHeightAnalog(const float AnalogInput, const float DeltaTime);
	void UpdateCameraZoomAnalog(const float AnalogInput, const float DeltaTime);

protected:
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	/**
	 * @brief Resets the preview to its initial state.
	 *
	 * This method is called to restore the preview scene or object to its default parameters,
	 * clearing any modifications or changes that may have occurred during interaction.
	 *
	 * Intended for use in scenarios where the preview needs to be refreshed or reverted
	 * to its original configuration.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Event")
	void OnPreviewReset();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PreviewImage;

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings")
	bool bAutoStartTick = false;

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

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings", meta=(UIMin = "0.0", ClampMin = "0.0", UIMax = "180.0", ClampMax = "180.0"))
	float YawLimits = 180.f;

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings", meta=(UIMin = "0.01", ClampMin = "0.01", UIMax = "100.0", ClampMax = "100.0"))
	float HeightLimit = 100.f;

	UPROPERTY(EditAnywhere, Category="Mountea|Preview Settings", meta=(UIMin = "0.01", ClampMin = "0.01", UIMax = "10.0", ClampMax = "10.0"))
	float CameraRotationSensitivity = 0.2f;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	FGuid ActivePreviewItemGuid = FGuid();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentYaw   = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentPitch = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentZoom  = 1.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentCameraHeight = 0.0f;
};
