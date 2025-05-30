﻿// MounteaAdvancedInventoryInteractableObjectWidget.h

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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	bool InitializeInteractableWidget();
	void CleanUpPreviewScene();
	void SetPreviewMesh(UStaticMesh* StaticMesh) const;
	void SetPreviewSkeletalMesh(USkeletalMesh* SkeletalMesh) const;
	void ClearPreview() const;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PreviewImage;

	UPROPERTY(VisibleAnywhere, Category="Preview Settings")
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;
	
	UPROPERTY(VisibleAnywhere, Category="Preview Settings")
	TObjectPtr<UMaterialInstanceDynamic> PreviewMaterialInstance;

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
