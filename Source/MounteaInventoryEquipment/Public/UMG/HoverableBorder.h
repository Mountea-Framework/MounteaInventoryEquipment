// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "Input/Reply.h"

#include "HoverableBorder.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBorderHoverEvent);

/**
 * Custom Border Extension which creates native Events similar to buttons.
 *
 * * Hoverable
 * * Drag and Drop support
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UHoverableBorder : public UBorder
{
	GENERATED_UCLASS_BODY()

public:

	/** Called when the border is hovered */
	UPROPERTY( BlueprintAssignable, Category = "Border|Event" )
	FOnBorderHoverEvent OnHovered;

	/** Called when the border is unhovered */
	UPROPERTY( BlueprintAssignable, Category = "Border|Event" )
	FOnBorderHoverEvent OnUnhovered;

protected:

	void MouseEntered(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void MouseLeft(const FPointerEvent& PointerEvent);

protected:

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	void ExecuteHoverStateChanged() const;
	
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

};
