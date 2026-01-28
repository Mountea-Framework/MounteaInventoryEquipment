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
#include "Widgets/MounteaAdvancedInventoryBaseWidget.h"
#include "PreviewScene.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "MounteaAdvancedInventoryInteractableObjectWidget.generated.h"

class UImage;
class UTextureRenderTarget2D;
class UStaticMesh;
class USkeletalMesh;
class AMounteaAdvancedInventoryPreviewEnvironment;
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
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void ProcessRotationInput(const FVector2f& Delta);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void ProcessHeightInput(const FVector2f& Delta);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void ProcessZoomInput(const float Delta);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void ProcessAnalogRotation(const FVector2f& AnalogInput, const float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void ProcessAnalogHeight(const float AnalogInput, const float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void ProcessAnalogZoom(const float AnalogInput, const float DeltaTime);
    
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void SetCameraRotationAbsolute(const float YawNormalized, const float PitchNormalized);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void SetCameraHeightAbsolute(const float HeightNormalized);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Input Processing")
	void SetCameraZoomAbsolute(const float ZoomNormalized);

protected:
	bool CanConsumeNativeInput() const;
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
	
	UPROPERTY()
	FMounteaPreviewCameraControlSettings ControlSettings;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PreviewMaterialInstance;
	
	UPROPERTY()
	TSubclassOf<AMounteaAdvancedInventoryPreviewEnvironment> EnvironmentActorClass;

	UPROPERTY()
	TSubclassOf<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActorClass;

private:
	UPROPERTY()
	TObjectPtr<AMounteaAdvancedInventoryPreviewEnvironment> EnvironmentActor = nullptr;
	UPROPERTY()
	TObjectPtr<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActor = nullptr;

	TUniquePtr<FPreviewScene> PreviewScene;

	FTimerHandle TimerHandle_PreviewTick;

	float LastInteractionTime = 0.0f;
	FVector2f LastMousePosition = FVector2f::ZeroVector;
	bool bIsMiddleMousePressed = false;
	bool bIsMousePressed = false;

	UPROPERTY(BlueprintReadWrite, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	FGuid ActivePreviewItemGuid = FGuid();
	UPROPERTY(BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentYaw   = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentPitch = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentZoom  = 1.0f;
	UPROPERTY(BlueprintReadOnly, Category="Mountea|Preview Settings", meta=(AllowPrivateAccess))
	float CurrentCameraHeight = 0.0f;
};
