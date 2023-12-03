// Copyright Epic Games, Inc. All Rights Reserved.

#include "MounteaEquipmentConfig_DetailsPanel.h"

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "IPropertyTypeCustomization.h"
#include "GameplayTagContainer.h"

#include "Editor.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "Helpers/MounteaEquipmentHelpers.h"
#include "Widgets/Input/SHyperlink.h"

#define LOCTEXT_NAMESPACE "GameplayTagCustomization"

void FMounteaEquipmentSlotIdentity_DetailsPanel::OnSearchForReferences()
{
	/*
	FName TagFName(*TagName, FNAME_Find);
	if (FEditorDelegates::OnOpenReferenceViewer.IsBound() && !TagFName.IsNone())
	{
		TArray<FAssetIdentifier> AssetIdentifiers;
		AssetIdentifiers.Add(FAssetIdentifier(FGameplayTag::StaticStruct(), TagFName));
		FEditorDelegates::OnOpenReferenceViewer.Broadcast(AssetIdentifiers, FReferenceViewerParams());
	}
	*/
}

FMounteaEquipmentSlotIdentity_CustomDetailsHelper::FMounteaEquipmentSlotIdentity_CustomDetailsHelper(IDetailPropertyRow* InPropertyRow) : PropertyRow(InPropertyRow)
{}

void FMounteaEquipmentSlotIdentity_CustomDetailsHelper::Update()
{
	TSharedPtr<SWidget> DefaultNameWidget;
	TSharedPtr<SWidget> DefaultValueWidget;
	PropertyRow->GetDefaultWidgets
	(
		DefaultNameWidget,
		DefaultValueWidget,
		false
	);

	PropertyRow->ShowPropertyButtons(true);

	PropertyRow->ShouldAutoExpand(true);
}

void FMounteaEquipmentSlotIdentity_DetailsPanel::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	CachedStructPropertyHandle = StructPropertyHandle;
	PropertyUtils = StructCustomizationUtils.GetPropertyUtilities();

	HeaderRow.Visibility(EVisibility::All);
}

void FMounteaEquipmentSlotIdentity_DetailsPanel::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	CachedStructPropertyHandle = InStructPropertyHandle;
	
	CachedStructPropertyHandle->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FMounteaEquipmentSlotIdentity_DetailsPanel::OnPropertyValueChanged));
		
	Group = &ChildBuilder.AddGroup(FName(TEXT("Slot Identity Row")),
											   LOCTEXT("FMounteaEquipmentSlotIdentity_DetailsPanel_SlotRowName",
													   "Slot Identity Row"));


	auto SlotNameRow = &Group->AddPropertyRow
	(
		CachedStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaEquipmentSlotIdentity, SlotName)).ToSharedRef()
	);
	SlotNameRow->Visibility(EVisibility::Visible);
	
	auto SlotTagRow = &Group->AddPropertyRow
		(
			CachedStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaEquipmentSlotIdentity, SlotTag)).ToSharedRef()
		);
	SlotTagRow->Visibility(EVisibility::Visible);
	
	auto SlotTagsRow = &Group->AddPropertyRow
		(
			CachedStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMounteaEquipmentSlotIdentity, SlotCompatibleTags)).ToSharedRef()
		);
	SlotTagsRow->Visibility(EVisibility::Visible);

	SlotTagRow->GetPropertyHandle()->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FMounteaEquipmentSlotIdentity_DetailsPanel::OnSlotTagChanged));
	
	PropertyRows.Add(SlotNameRow);
	PropertyRows.Add(SlotTagRow);
	PropertyRows.Add(SlotTagsRow);

	// Add Custom buttons
	MounteaEquipmentSlotIdentity_CustomDetailsHelper = MakeShared<FMounteaEquipmentSlotIdentity_CustomDetailsHelper>(SlotTagsRow);
	MounteaEquipmentSlotIdentity_CustomDetailsHelper->SetPropertyUtils(PropertyUtils);
	MounteaEquipmentSlotIdentity_CustomDetailsHelper->Update();
}

void FMounteaEquipmentSlotIdentity_DetailsPanel::OnPropertyValueChanged()
{
	/*
	TagName = TEXT("");
	if (CachedStructPropertyHandle.IsValid() && CachedStructPropertyHandle->GetProperty())
	{
		uint32 Children;
		CachedStructPropertyHandle->GetNumChildren(Children);
		auto NameHandle = CachedStructPropertyHandle->GetChildHandle("SlotTag");
		auto Handle = CachedStructPropertyHandle->GetChildHandle(2);

		UE_LOG(LogTemp, Warning, TEXT("%d Children | %s NamedHandle | %s Index 2 Handle"), Children, *(NameHandle.Get()->GetProperty() ? NameHandle->GetPropertyDisplayName().ToString() : TEXT("Invalid")), *(Handle.Get()->GetProperty() ? Handle->GetPropertyDisplayName().ToString() : TEXT("Invalid")))
	}
	*/
}

void FMounteaEquipmentSlotIdentity_DetailsPanel::OnSlotTagChanged()
{
	TagName = TEXT("");
	for (auto& Itr : PropertyRows)
	{
		auto TagPropertyName = GET_MEMBER_NAME_CHECKED(FMounteaEquipmentSlotIdentity, SlotTag);
		if (Itr->GetPropertyHandle() && Itr->GetPropertyHandle()->GetProperty()->GetFName().IsEqual(TagPropertyName))
		{
			Itr->GetPropertyHandle()->ResetToDefault();
		}
	}
}

FMounteaEquipmentSlotIdentity_DetailsPanel::~FMounteaEquipmentSlotIdentity_DetailsPanel()
{
	
}

#undef LOCTEXT_NAMESPACE
