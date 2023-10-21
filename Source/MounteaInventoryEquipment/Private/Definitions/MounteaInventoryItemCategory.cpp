// All rights reserved Dominik Pavlicek 2023.


#include "Definitions/MounteaInventoryItemCategory.h"

#if WITH_EDITOR

void UMounteaInventoryItemCategory::EnsureValidConfig()
{
	if (CategoryThemeConfig.ContentThemeConfig == nullptr)
	{
		UContentThemeConfig* NewConfig = NewObject<UCategoryThemeConfig>(GetPackage());
		CategoryThemeConfig.ContentThemeConfig = NewConfig;
	}
}

void UMounteaInventoryItemCategory::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property->GetFName();

	if (PropertyName.IsEqual("ContentThemeConfig"))
	{
		EnsureValidConfig();
	}
}

#endif