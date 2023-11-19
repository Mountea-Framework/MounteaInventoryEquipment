// All rights reserved Dominik Morse 2023


#include "MounteaItemConfig_Details.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Setup/MounteaInventoryItemConfig.h"


#define LOCTEXT_NAMESPACE "FMounteaItemConfig_Details"


void FMounteaItemConfig_Details::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// Only support one object being customized
	if (ObjectsBeingCustomized.Num() != 1) return;

	const TWeakObjectPtr<UMounteaInventoryItemConfig> WeakObject = Cast<UMounteaInventoryItemConfig>(ObjectsBeingCustomized[0].Get());
	if (!WeakObject.IsValid()) return;

	EditingConfig = WeakObject.Get();
	if (!EditingConfig) return;
	
	SavedLayoutBuilder = &DetailBuilder;
}

#undef LOCTEXT_NAMESPACE