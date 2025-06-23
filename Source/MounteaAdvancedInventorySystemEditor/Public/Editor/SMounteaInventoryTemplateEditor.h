// Copyright Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"

class UMounteaInventoryItemTemplate;

DECLARE_DELEGATE_OneParam(FOnTemplateChanged, UMounteaInventoryItemTemplate*);

class SMounteaInventoryTemplateEditor : public SCompoundWidget, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SMounteaInventoryTemplateEditor) {}
	SLATE_EVENT(FOnTemplateChanged, OnTemplateChanged)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

protected:
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:
	TSharedRef<SWidget> CreatePropertyMatrix();
	TSharedRef<SWidget> CreateToolbar();
	TSharedRef<ITableRow> GenerateTemplateListRow(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template, const TSharedRef<STableViewBase>& OwnerTable);
	
	TArray<UObject*> LoadAllTemplatesForMatrix();
	void OnTemplateSelectionChanged(TWeakObjectPtr<UMounteaInventoryItemTemplate> SelectedTemplate, ESelectInfo::Type SelectInfo);
	
	FReply OnCreateNewTemplate();
	FReply SaveTemplate();
	FReply CloseTemplate();
	FReply SaveNewTemplate();
	FReply SaveExistingTemplate();

	void ImportTemplate();
	void ExportTemplate();
	
	void CreateTransientTemplate();
	void CleanupTransientTemplate();
	bool ValidateTemplateData(FString& ErrorMessage) const;
	void ShowTemplateEditorNotification(const FString& Message, const bool bSuccess = true) const;

private:
	FOnTemplateChanged OnTemplateChanged;
	
	TSharedPtr<IDetailsView> PropertyDetailsView;
	TSharedPtr<SListView<TWeakObjectPtr<UMounteaInventoryItemTemplate>>> TemplateListView;
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> AvailableTemplates;
	
	TWeakObjectPtr<UMounteaInventoryItemTemplate> CurrentTemplate;
	TWeakObjectPtr<UMounteaInventoryItemTemplate> OriginalTemplate;
	bool bIsEditingExisting = false;
};