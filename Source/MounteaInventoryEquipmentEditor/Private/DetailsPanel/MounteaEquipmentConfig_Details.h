// All rights reserved Dominik Morse 2023

#pragma once

#include "GameplayTagContainer.h"
#include "IDetailCustomization.h"

class UMounteaEquipmentConfigData;

class FMounteaEquipmentConfig_Details : public IDetailCustomization
{
	typedef FMounteaEquipmentConfig_Details Self;

public:
	
	virtual ~FMounteaEquipmentConfig_Details() override;

public:
	
	// Makes a new instance of this detail layout class for a specific detail view requesting it
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShared<Self>(); }
	
	// IDetailCustomization interface
	/** Called when details should be customized */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:

private:

	IDetailLayoutBuilder* SavedLayoutBuilder = nullptr;
	UMounteaEquipmentConfigData* Editing = nullptr;

	void OnTagModified(FGameplayTag& ModifiedTag);

	FDelegateHandle OnPropertyModifiedHandle;
};
