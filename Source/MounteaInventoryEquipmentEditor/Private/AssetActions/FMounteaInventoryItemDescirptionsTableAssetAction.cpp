// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryItemDescriptionsTableAssetAction.h"
#include "Definitions/MounteaInventoryTableTypes.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemDescriptionsTableAssetAction"

FText FMounteaInventoryTableAssetAction_ItemDescriptions::GetName() const
{
	return LOCTEXT("MounteaInventoryTableAssetAction_ItemDescriptions_Name", "Items Descriptions Data Table");
}

UClass* FMounteaInventoryTableAssetAction_ItemDescriptions::GetSupportedClass() const
{
	return UMounteaInventoryItemsDescriptionsTable::StaticClass();
}

#undef LOCTEXT_NAMESPACE
