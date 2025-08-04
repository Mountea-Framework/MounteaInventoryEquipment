// Copyright Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Misc/NotifyHook.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"

class UMounteaInventoryItemTemplate;

DECLARE_DELEGATE_OneParam(FOnTemplateChanged, UMounteaInventoryItemTemplate*);

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateEditor"

struct FTemplateDisplayInfo
{
	FText DisplayText;
	FText TooltipText;
	FText FullText;
	FString AssetPath;
	bool bIsTransient = false;
	bool bIsDirty = false;

	FTemplateDisplayInfo()
		: DisplayText(LOCTEXT("InvalidTemplate", "Invalid Template"))
		, TooltipText(LOCTEXT("InvalidTemplateTooltip", "Invalid Template"))
		, FullText(DisplayText)
		, AssetPath(TEXT("Unknown Path"))
	{}
};

class SMounteaInventoryTemplateEditor : public SCompoundWidget, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SMounteaInventoryTemplateEditor) {}
	SLATE_EVENT(FOnTemplateChanged, OnTemplateChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);	
	virtual ~SMounteaInventoryTemplateEditor();

protected:
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:
	TSharedRef<SWidget> CreatePropertyMatrix();
	TSharedRef<SWidget> CreateToolbar();
	TSharedRef<ITableRow> GenerateTemplateListRow(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template, const TSharedRef<STableViewBase>& OwnerTable);
	
	TArray<UObject*> LoadAllTemplatesForMatrix();
	void RefreshTemplateList();
	void OnTemplateSelectionChanged(TWeakObjectPtr<UMounteaInventoryItemTemplate> SelectedTemplate, ESelectInfo::Type SelectInfo);
	
	FReply OnCreateNewTemplate();
	FReply SaveTemplate();
	FReply SaveAllDirtyTemplates();
	FReply CloseTemplate();
	FReply SaveNewTemplate();
	FReply SaveExistingTemplate();
	FReply DeleteTemplate(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template);
	FReply DuplicateTemplate(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template);

	void ImportTemplate();
	void ExportTemplate();
	
	void CreateTransientTemplate();
	void CleanupTransientTemplate();
	bool ValidateTemplateData(FString& ErrorMessage) const;
	void ShowTemplateEditorNotification(const FString& Message, const bool bSuccess = true) const;
	
	// New functions for tracking dirty assets
	void TrackDirtyAsset(UMounteaInventoryItemTemplate* Template);
	void UntrackDirtyAsset(UMounteaInventoryItemTemplate* Template);
	bool HasUnsavedChanges() const;
	bool CheckForUnsavedChanges();
	
	// Asset creation dialog
	FString ShowSaveAssetDialog();

	FTemplateDisplayInfo  GenerateTemplateDisplayInfo(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template,
	const TSet<TWeakObjectPtr<UMounteaInventoryItemTemplate>>& AllDirtyTemplates);

private:
	FOnTemplateChanged OnTemplateChanged;
	
	TSharedPtr<IDetailsView> PropertyDetailsView;
	TSharedPtr<SListView<TWeakObjectPtr<UMounteaInventoryItemTemplate>>> TemplateListView;
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> AvailableTemplates;
	
	// Track selected and dirty templates
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> SelectedTemplates;
	TSet<TWeakObjectPtr<UMounteaInventoryItemTemplate>> DirtyTemplates;
	
	TWeakObjectPtr<UMounteaInventoryItemTemplate> CurrentTemplate;
	TWeakObjectPtr<UMounteaInventoryItemTemplate> OriginalTemplate;
	bool bIsEditingExisting = false;
	bool bIsShowingTransient = false;
};

#undef LOCTEXT_NAMESPACE