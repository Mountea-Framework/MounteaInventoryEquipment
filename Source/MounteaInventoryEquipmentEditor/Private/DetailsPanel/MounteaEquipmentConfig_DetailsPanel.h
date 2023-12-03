// All rights reserved Dominik Morse 2023

#pragma once

#include "IPropertyTypeCustomization.h"
#include "CoreMinimal.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "EditorUndoClient.h"
#include "IDetailPropertyRow.h"

class IDetailGroup;
class IPropertyHandle;

class FMounteaEquipmentSlotIdentity_CustomDetailsHelper;

/** Customization for the gameplay tag struct */
class FMounteaEquipmentSlotIdentity_DetailsPanel : public IPropertyTypeCustomization
{
	typedef FMounteaEquipmentSlotIdentity_DetailsPanel Self;
	
public:

	virtual ~FMounteaEquipmentSlotIdentity_DetailsPanel() override;

	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShared<Self>(); }
	
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
	/** Overridden to do nothing */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, class IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
private:

	FSimpleDelegate OnTagChanged;

	/** Updates the selected tag*/
	void OnPropertyValueChanged();

	void OnSlotTagChanged();

	/** Cached property handle */
	TSharedPtr<IPropertyHandle> CachedStructPropertyHandle;

	TSharedPtr<IPropertyUtilities> PropertyUtils;

	TArray<IDetailPropertyRow*> PropertyRows;
	IDetailGroup* Group = nullptr;

	TSharedPtr<FMounteaEquipmentSlotIdentity_CustomDetailsHelper> MounteaEquipmentSlotIdentity_CustomDetailsHelper;
	
	/** Tag name selected*/
	FString TagName;
	
	void OnSearchForReferences();
};

class FMounteaEquipmentSlotIdentity_CustomDetailsHelper : public TSharedFromThis<FMounteaEquipmentSlotIdentity_CustomDetailsHelper>
{
	typedef FMounteaEquipmentSlotIdentity_CustomDetailsHelper Self;
	
public:
	
	FMounteaEquipmentSlotIdentity_CustomDetailsHelper(IDetailPropertyRow* InPropertyRow);
	virtual ~FMounteaEquipmentSlotIdentity_CustomDetailsHelper() {}
	// Update the full property row.
	void Update();


	Self& SetPropertyUtils(const TSharedPtr<IPropertyUtilities>& Utils)
	{
		PropertyUtils = Utils;
		return *this;
	}

protected:

	TSharedPtr<IPropertyUtilities> PropertyUtils;
	
	// The Property handle of what this row represents
	IDetailPropertyRow* PropertyRow = nullptr;
};