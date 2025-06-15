// All rights reserved Dominik Morse 2024

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
