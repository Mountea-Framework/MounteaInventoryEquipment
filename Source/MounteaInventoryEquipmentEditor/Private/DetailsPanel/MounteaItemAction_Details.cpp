// All rights reserved Dominik Pavlicek 2023


#include "MounteaItemAction_Details.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Definitions/MounteaItemAction.h"


#define LOCTEXT_NAMESPACE "FMounteaItemAction_Details"


void FMounteaItemAction_Details::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// Only support one object being customized
	if (ObjectsBeingCustomized.Num() != 1) return;

	const TWeakObjectPtr<UMounteaInventoryItemAction> WeakObject = Cast<UMounteaInventoryItemAction>(ObjectsBeingCustomized[0].Get());
	if (!WeakObject.IsValid()) return;

	EditingAction = WeakObject.Get();
	if (!EditingAction) return;

	IDetailCategoryBuilder& ItrCategoryBuild = DetailBuilder.EditCategory(TEXT("Required"));

	TArray<TSharedRef<IPropertyHandle> > RequiredProperties;
	ItrCategoryBuild.GetDefaultProperties(RequiredProperties);

	for (const auto& Itr : RequiredProperties)
	{
		if (Itr->GetProperty())
		{
			auto Flags = Itr->GetProperty()->GetPropertyFlags();

			Itr->GetProperty()->ClearPropertyFlags(CPF_EditConst);
		}
	}
}

#undef LOCTEXT_NAMESPACE