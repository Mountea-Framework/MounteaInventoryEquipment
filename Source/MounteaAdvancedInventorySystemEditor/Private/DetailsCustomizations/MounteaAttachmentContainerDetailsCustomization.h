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

class UMounteaAttachmentContainerComponent;

class FMounteaAttachmentContainerDetailsCustomization  : public IDetailCustomization
{
	typedef FMounteaAttachmentContainerDetailsCustomization Self;
public:

	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShared<Self>(); }
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
private:

	UMounteaAttachmentContainerComponent* SourceContainerComponent = nullptr;
};
