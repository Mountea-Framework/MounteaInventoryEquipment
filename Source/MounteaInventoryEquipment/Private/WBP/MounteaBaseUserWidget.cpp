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

UMounteaInventoryThemeConfig* UMounteaBaseUserWidget::GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult) const
{
	if (ClassFilter == nullptr)
	{
		bResult = false;
		return nullptr;
	}

	bResult = true;
	const auto FoundTheme = ThemeConfigOverride != nullptr ? ThemeConfigOverride : UMounteaInventoryEquipmentBPF::GetThemeConfig(ClassFilter, bResult);

	if (!bResult)
	{
		return NewObject<UMounteaInventoryThemeConfig>(nullptr, ClassFilter);
	}

	return FoundTheme->IsA(ClassFilter) ? FoundTheme : NewObject<UMounteaInventoryThemeConfig>(nullptr, ClassFilter);;
}