// All rights reserved Dominik Morse 2023


#include "MounteaItemAction_DetailsPanel.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyRestriction.h"
#include "Definitions/MounteaItemAction.h"

#include "DetailsPanel/Helpers/MounteaInventoryItemAction_CustomDetailsHelper.h"


#define LOCTEXT_NAMESPACE "FMounteaItemAction_DetailsPanel"

void FMounteaItemAction_DetailsPanel::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;
	PropertyUtils = StructCustomizationUtils.GetPropertyUtilities();
}

void FMounteaItemAction_DetailsPanel::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Make Custom elements
	{
		PropertyRow = &StructBuilder.AddProperty
		(
			StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaItemAction, ItemAction)).ToSharedRef()
		);
		PropertyRow->Visibility(EVisibility::Visible);

		// Add Custom buttons
		MounteaItemAction_CustomDetailsHelper = MakeShared<FMounteaInventoryItemAction_CustomDetailsHelper>(PropertyRow);
		MounteaItemAction_CustomDetailsHelper->SetPropertyUtils(PropertyUtils);
		MounteaItemAction_CustomDetailsHelper->Update();
		MounteaItemAction_CustomDetailsHelper->SetFunctionNameToOpen
		(
			EMounteaInventoryEquipmentBlueprintOpenType::Function,
			GET_FUNCTION_NAME_CHECKED(UMounteaInventoryItemAction, DisplayAction)
		);

		TSharedPtr<IPropertyHandle> ItemActionHandle = PropertyRow->GetPropertyHandle();
		uint32 NumChildren = 0;
		ItemActionHandle->GetNumChildren(NumChildren);

		if (NumChildren == 0) return;
		
		TSharedPtr<IPropertyHandle> ChildHandle = ItemActionHandle->GetChildHandle(0);

		uint32 ChildrenCount;
		ChildHandle->GetNumChildren(ChildrenCount);

		for (uint32 i = 0; i < ChildrenCount; i++)
		{
			TSharedPtr<IPropertyHandle> ActionHandle = ChildHandle->GetChildHandle(i);

			if (!ActionHandle.Get())
			{
				continue;
			}

			if (!ActionHandle->GetPropertyDisplayName().EqualTo(FText::FromString("1. Required")))
			{
				continue;
			}

			uint32 ActionChildrenCount;
			ActionHandle->GetNumChildren(ActionChildrenCount);

			for (uint32 j = 0; j < ActionChildrenCount; j++)
			{
				TSharedPtr<IPropertyHandle> ChildActionHandle = ActionHandle->GetChildHandle(j);

				if (!ChildActionHandle.Get())
				{
					continue;
				}
				ChildActionHandle->GetProperty()->SetPropertyFlags(CPF_EditConst);
			}
		}
	}
}

#undef  LOCTEXT_NAMESPACE