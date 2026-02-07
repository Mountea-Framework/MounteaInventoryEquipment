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

#include "ComponentVisualizer.h"

class UMounteaAttachmentContainerComponent;
class UMounteaAdvancedAttachmentSlot;
class USceneComponent;

class FMounteaEquipmentComponentVisualizer : public FComponentVisualizer
{
public:
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;

private:
	struct FSlotVisualData
	{
		FVector Location = FVector::ZeroVector;
		FString Label;
		FLinearColor Color = FLinearColor::White;
	};

	struct FWarningVisualData
	{
		FVector Location = FVector::ZeroVector;
		FString Message;
	};

	static bool IsComponentSelected(const UActorComponent* Component);
	void GatherSlotData(const UMounteaAttachmentContainerComponent* EquipmentComponent, TArray<FSlotVisualData>& OutSlots, TArray<FWarningVisualData>& OutWarnings) const;
	USceneComponent* ResolveAttachmentTarget(const UMounteaAttachmentContainerComponent* EquipmentComponent, const UMounteaAdvancedAttachmentSlot* Slot, TArray<FString>& OutWarnings) const;
	static FString GetSlotLabel(const UMounteaAdvancedAttachmentSlot* Slot);
	static FLinearColor GetSlotColor(int32 Index);
};
