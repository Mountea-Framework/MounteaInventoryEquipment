// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "IDetailCustomization.h"

/** Fixed texture selectors for the active inventory taxonomy. */
class FMounteaThemeCategoryIconsCustomization : public IDetailCustomization
{
public:
	virtual ~FMounteaThemeCategoryIconsCustomization() override;

	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	FDelegateHandle SettingsChangedHandle;
};
