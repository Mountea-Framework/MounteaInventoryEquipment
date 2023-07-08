// All rights reserved Dominik Pavlicek 2023


#include "MounteaItemConfig_DetailsPanel.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"

#include "DetailsPanel/Helpers/MounteaInventoryItemConfig_CustomDetailsHelper.h"

#include "Setup/MounteaInventoryItemConfig.h"

#define LOCTEXT_NAMESPACE "FMounteaItemConfig_DetailsPanel"

void FMounteaItemConfig_DetailsPanel::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;
	PropertyUtils = StructCustomizationUtils.GetPropertyUtilities();
}

void FMounteaItemConfig_DetailsPanel::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Make Custom elements
	{
		DecoratorPropertyRow = &StructBuilder.AddProperty
		(
			StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaItemConfig, ItemConfig)).ToSharedRef()
		);
		DecoratorPropertyRow->Visibility(EVisibility::Visible);

		// Add Custom buttons
		MounteaItemConfig_CustomDetailsHelper = MakeShared<FMounteaInventoryItemConfig_CustomDetailsHelper>(DecoratorPropertyRow);
		MounteaItemConfig_CustomDetailsHelper->SetPropertyUtils(PropertyUtils);
		MounteaItemConfig_CustomDetailsHelper->Update();
		/*
		MounteaItemConfig_CustomDetailsHelper->SetFunctionNameToOpen
		(
			EMounteaInventoryEquipmentBlueprintOpenType::Function,
			GET_FUNCTION_NAME_CHECKED(UMounteaInventoryItemConfig, ProcessConfig)
		);
		*/
	}
}

#undef  LOCTEXT_NAMESPACE