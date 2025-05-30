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

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryInteractableObjectWidget : public UMounteaAdvancedInventoryBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	bool InitializeInteractableWidget();
	void SetPreviewMesh(UStaticMesh* StaticMesh);
	void SetPreviewSkeletalMesh(USkeletalMesh* SkeletalMesh);
	void ClearPreview();

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PreviewImage;

	UPROPERTY(EditAnywhere, Category="Preview Settings")
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	UPROPERTY(EditAnywhere, Category="Preview Settings")
	TSubclassOf<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActorClass;

private:
	UPROPERTY()
	TObjectPtr<AMounteaAdvancedInventoryItemPreviewRenderer> RendererActor = nullptr;

	TUniquePtr<FPreviewScene> PreviewScene;
	
	bool bIsMousePressed = false;
	float CurrentYaw   = 45.0f;
	float CurrentPitch = -20.0f;
	float CurrentZoom  = 200.0f;
};
