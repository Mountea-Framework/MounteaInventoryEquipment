// Copyright Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "AssetThumbnail.h"

class UMounteaInventoryItemTemplate;

DECLARE_DELEGATE_OneParam(FOnTemplateChanged, UMounteaInventoryItemTemplate*);

class SMounteaInventoryTemplateEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaInventoryTemplateEditor) {}
		SLATE_EVENT(FOnTemplateChanged, OnTemplateChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SMounteaInventoryTemplateEditor();

private:
	void CreateNewTemplate();
	FReply SaveTemplate();
	void ImportTemplate();
	void ExportTemplate();
	void RefreshTemplateList();
	void LoadAllTemplates();
	bool OnFilterMeshAssets(const FAssetData& AssetData) const;

	TSharedRef<SWidget> CreateToolbar();
	TSharedRef<SWidget> CreateTemplateListPanel();
	TSharedRef<SWidget> CreateEditorPanel();
	TSharedRef<ITableRow> GenerateTemplateListRow(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template, const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<SWidget> CreateBasicInfoSection();
	TSharedRef<SWidget> CreateItemPropertiesSection();
	TSharedRef<SWidget> CreateWeightSystemSection();
	TSharedRef<SWidget> CreatePriceSystemSection();
	TSharedRef<SWidget> CreateDurabilitySystemSection();
	TSharedRef<SWidget> CreateItemFlagsSection();
	TSharedRef<SWidget> CreateEquipmentSection();
	TSharedRef<SWidget> CreateVisualAssetsSection();
	TSharedRef<SWidget> CreateMaterialsSection();
	TSharedRef<SWidget> CreateCustomPropertiesSection();

	void CreateTransientTemplate();
	void CleanupTransientTemplate();
	void OnTemplateSelectionChanged(TWeakObjectPtr<UMounteaInventoryItemTemplate> SelectedTemplate, ESelectInfo::Type SelectInfo);
	void LoadTemplateData(UMounteaInventoryItemTemplate* Template);

	bool ValidateTemplateData() const;

private:
	FOnTemplateChanged OnTemplateChanged;
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool;
	TSharedPtr<SListView<TWeakObjectPtr<UMounteaInventoryItemTemplate>>> TemplateListView;
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> AvailableTemplates;
	
	TObjectPtr<UMounteaInventoryItemTemplate> CurrentTemplate;
	TObjectPtr<UMounteaInventoryItemTemplate> TransientTemplate;

	TSharedPtr<SEditableTextBox> ItemNameTextBox;
	bool bIsEditingTransient = true;
};