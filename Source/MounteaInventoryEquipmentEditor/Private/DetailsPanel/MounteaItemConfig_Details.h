// All rights reserved Dominik Pavlicek 2023

#pragma once

#include "IDetailCustomization.h"

class UMounteaInventoryItemConfig;

class FMounteaItemConfig_Details : public IDetailCustomization
{
	typedef FMounteaItemConfig_Details Self;

public:
	// Makes a new instance of this detail layout class for a specific detail view requesting it
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShared<Self>(); }
	
	// IDetailCustomization interface
	/** Called when details should be customized */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	IDetailLayoutBuilder* SavedLayoutBuilder = nullptr;

	UMounteaInventoryItemConfig* EditingConfig = nullptr;
};
