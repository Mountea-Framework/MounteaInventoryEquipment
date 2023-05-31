// All rights reserved Dominik Pavlicek 2022.


#include "WBP/MounteaBaseUserWidget.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"


UMounteaBaseUserWidget::UMounteaBaseUserWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	bIsFocusable = true;
}

void UMounteaBaseUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();

	LoadFromConfig();
}

void UMounteaBaseUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UnBindEvents();
}

void UMounteaBaseUserWidget::RenderPreview()
{
	OnWidgetRebuilt();
}

UMounteaInventoryThemeConfig* UMounteaBaseUserWidget::GetThemeConfig() const
{
	return ThemeConfigOverride != nullptr ? ThemeConfigOverride : UMounteaInventoryEquipmentBPF::GetThemeConfig();
}