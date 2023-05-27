// All rights reserved Dominik Pavlicek 2022.


#include "WBP/MounteaBaseUserWidget.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"

UMounteaInventoryThemeConfig* UMounteaBaseUserWidget::GetThemeConfig() const
{
	return ThemeConfigOverride != nullptr ? ThemeConfigOverride : UMounteaInventoryEquipmentBPF::GetThemeConfig();
}
