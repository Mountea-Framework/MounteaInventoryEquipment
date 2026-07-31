// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
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

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * Replaces the default row for the "SourceFilePath" property with a read-only path
 * plus a folder-reveal button. Shared by UMounteaInventoryItemTemplate and UMounteaRecipeTemplate,
 * both of which expose a property of that exact name.
 */
class FMounteaSourceFilePathDetailsCustomization : public IDetailCustomization
{
	typedef FMounteaSourceFilePathDetailsCustomization Self;
public:

	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShared<Self>(); }
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
