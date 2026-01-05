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
#include "Blueprint/UserWidget.h"
#include "MounteaInventoryScrollBox.generated.h"

class UVerticalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewIndexCalculated, int32, NewIndex);

/**
 * UMounteaInventoryScrollBox is a User Widget which behaves as scrollbox
 * with specific visual behaviour which mimics some of the most famous RPG
 * Inventory components.
 *
 * @see [Inventory Scroll Box](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface/MounteaInventoryScrollBox/)
 * @see FInventoryItem
 * @see UMounteaInventoryManagerComponent
 */
UCLASS(ClassGroup=(Mountea), NotBlueprintable, BlueprintType)
class UMounteaInventoryScrollBox : public UUserWidget
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
	
	// When you scroll/move in the list, this event will provide you next active index without setting it automatically.
	UPROPERTY(BlueprintAssignable, Category = "Scroll Box")
	FOnNewIndexCalculated OnNewIndexCalculated;
	
	// How much time the Animation takes. Higher the value, slower it is.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Box",
		meta=(NoResetToDefault))
	float InterpolationSpeed = 10.0f;
	
	// If false, scroll/move will stop by the end of the child widgets, otherwise it will go back to infinity.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Box",
		meta=(NoResetToDefault))
	bool bWrapAround = false;
	
	// Vertical Box which contains all child elements.
	UPROPERTY(BlueprintReadWrite, Category = "Scroll Box")
	TObjectPtr<UVerticalBox> VerticalBox;
	
public:	
	void SetActiveIndex(int32 NewIndex);
	int32 GetActiveIndex() const { return ActiveIndex; }
	int32 GetChildrenCount() const;
	
	void AddChild(UWidget* Content);
	void ResetChildren();

protected:
	virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;


protected:
	
	// If set to false scrolling will be disabled.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scroll Box")
	bool bCaptureInput = true;

private:
	void BroadcastIndexChange(int32 Delta) const;
	void CalculateDesiredTransform();
	void InterpolateToTarget(const float InDeltaTime);

private:	
	int32 ActiveIndex = INDEX_NONE;
	FVector2D CurrentTranslation = FVector2D::ZeroVector;
	FVector2D TargetTranslation = FVector2D::ZeroVector;
};
