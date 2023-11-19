// All rights reserved Dominik Morse 2023

#pragma once

#include "IDetailCustomization.h"


class IHttpResponse;
class IHttpRequest;
class FHttpModule;
class UMounteaInventoryEquipmentSettings;

class FMounteaSettings_Details : public IDetailCustomization
{
	typedef FMounteaSettings_Details Self;

public:
	
	virtual ~FMounteaSettings_Details() override;

public:
	
	// Makes a new instance of this detail layout class for a specific detail view requesting it
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShared<Self>(); }
	
	// IDetailCustomization interface
	/** Called when details should be customized */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:
	
	void OnGetResponse_Tags(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);
	FReply OnRefreshTagsClicked();

	const FSlateBrush* GetRefreshTagsBrush() const;
	FSlateColor GetRefreshTagsColorOpacity() const;
	int32 GetWidgetSwitcherIndex() const;

	void OnRefreshTagsHovered();

private:
	
	void RefreshGameplayTags() const;
	void CreateTagsConfig(const FString& Context) const;
	void UpdateTagsConfig(const FString& Context) const;
	bool DoesHaveValidTags();

private:

	int32 WidgetSwitcherIndex = 0;

	TSharedPtr<SButton> RefreshTagsButton;
	TSharedPtr<SWidgetSwitcher> RefreshTagsWidgetSwitcher;
	TSharedPtr<SImage> RefreshTagsImage;
	
	FButtonStyle RefreshTagsButtonStyle;
	
	IDetailLayoutBuilder* SavedLayoutBuilder = nullptr;

	UMounteaInventoryEquipmentSettings* Editing = nullptr;

	FHttpModule* Http = nullptr;
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request;
};
