// All rights reserved Dominik Morse 2024


#include "Slate/MounteaAdvancedInventoryAnimatedSwitcher.h"


UMounteaAdvancedInventoryAnimatedSwitcher::UMounteaAdvancedInventoryAnimatedSwitcher(
	const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnActiveWidgetIndexChanged.AddUObject(this, &UMounteaAdvancedInventoryAnimatedSwitcher::ProcessIndexChanged);
}

void UMounteaAdvancedInventoryAnimatedSwitcher::ProcessIndexChanged(UWidget* Widget, int32 Index) const
{
	OnAnimationFinished.Broadcast(Widget, Index);
}

#if WITH_EDITOR

const FText UMounteaAdvancedInventoryAnimatedSwitcher::GetPaletteCategory()
{
	return NSLOCTEXT("MounteaAdvancedInventory", "WidgetPaletteCategory", "Mountea Advanced Inventory & Equipment");
}

#endif
