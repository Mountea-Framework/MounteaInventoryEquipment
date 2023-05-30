// Copyright Epic Games, Inc. All Rights Reserved.

#include "UMG/HoverableBorder.h"

#define LOCTEXT_NAMESPACE "MounteaUMG"

/////////////////////////////////////////////////////
// UHoverableBorder

UHoverableBorder::UHoverableBorder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UHoverableBorder::MouseEntered(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsHovered())
	{
		return ExecuteHoverStateChanged();
	}
}

void UHoverableBorder::MouseLeft(const FPointerEvent& PointerEvent)
{
	if (!IsHovered())
	{
		ExecuteHoverStateChanged();
	}
}

void UHoverableBorder::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyBorder->SetOnMouseEnter(BIND_UOBJECT_DELEGATE(FNoReplyPointerEventHandler, MouseEntered));
	MyBorder->SetOnMouseLeave(BIND_UOBJECT_DELEGATE(FSimpleNoReplyPointerEventHandler, MouseLeft));
}

void UHoverableBorder::ExecuteHoverStateChanged() const
{
	if (IsHovered() && OnHovered.IsBound())
	{
		OnHovered.Broadcast();
		return;
	}

	if (!IsHovered() && OnUnhovered.IsBound())
	{
		OnUnhovered.Broadcast();
		return;
	}
}

const FText UHoverableBorder::GetPaletteCategory()
{
	return LOCTEXT("Mountea", "Mountea");
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
