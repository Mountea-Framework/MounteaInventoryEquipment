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
#include "Components/VerticalBox.h"
#include "MounteaInventoryScrollBox.generated.h"

UCLASS()
class UMounteaInventoryScrollBox : public UVerticalBox
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	
	virtual const FText GetPaletteCategory() override
	{
		return NSLOCTEXT("MounteaAdvancedInventory", "WidgetPaletteCategory", "Mountea Advanced Inventory & Equipment");
	}

#endif

public:
	
	// How much time the Animation takes. Higher the value, slower it is.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Box",
		meta=(NoResetToDefault))
	float InterpolationSpeed = 10.0f;
	
	void SetActiveIndex(int32 NewIndex);
	int32 GetActiveIndex() const { return ActiveIndex; }

protected:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	void CalculateDesiredTransform();
	void InterpolateToTarget();
	void StartInterpolation();
	void StopInterpolation();

	int32 ActiveIndex = INDEX_NONE;
	FVector2D CurrentTranslation = FVector2D::ZeroVector;
	FVector2D TargetTranslation = FVector2D::ZeroVector;
	FTimerHandle InterpolationTimer;
};
