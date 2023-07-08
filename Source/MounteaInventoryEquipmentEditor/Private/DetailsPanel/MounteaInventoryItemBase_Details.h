// All rights reserved Dominik Pavlicek 2023

#pragma once

#include "IDetailCustomization.h"

class UMounteaInventoryItemBase;
class SScrollBox;

class FMounteaInventoryItem_Details : public IDetailCustomization
{
	typedef FMounteaInventoryItem_Details Self;

public:
	// Makes a new instance of this detail layout class for a specific detail view requesting it
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShared<Self>(); }
	
	FReply OnDocumentationClicked() const;

	const FSlateBrush* GetDocumentationBrush() const;
	FSlateColor GetDocumentationColorOpacity() const;

	void OnDocumentationHovered();

	bool IsSetValidDataEnabled() const;

	FReply OnSetValidDataClicked();
	const FSlateBrush* GetSetValidDataBrush() const;
	void OnSetValidDataHovered();
	
	// IDetailCustomization interface
	/** Called when details should be customized */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	TSharedPtr<SButton> DocumentationButton;
	TSharedPtr<SImage> DocumentationImage;

	TSharedPtr<SButton> ValidDataButton;
	TSharedPtr<SImage> ValidDataImage;
	
	FButtonStyle DocumentationButtonStyle;
	FButtonStyle ValidDataButtonStyle;
	
	IDetailLayoutBuilder* SavedLayoutBuilder = nullptr;

	UMounteaInventoryItemBase* EditingItem = nullptr;
};

