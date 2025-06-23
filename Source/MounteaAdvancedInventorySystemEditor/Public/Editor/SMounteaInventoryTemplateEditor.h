// Copyright Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "AssetThumbnail.h"
#include "Widgets/Input/SSpinBox.h"

class SGameplayTagContainerCombo;
class SObjectPropertyEntryBox;
class UMounteaInventoryItemTemplate;
class SNotificationItem;
class SMultiLineEditableTextBox;


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

	bool ValidateTemplateData(FString& ErrorMessage) const;

	void ShowTemplateEditorNotification(const FString& Message, const bool bSuccess = true) const;

	void RefreshOptions();
	TArray<TSharedPtr<FString>> GetCategoryOptions() const;
	TArray<TSharedPtr<FString>> GetSubCategoryOptions() const;
	TArray<TSharedPtr<FString>> GetRarityOptions() const;
	TArray<TSharedPtr<FString>> GetFlagOptions() const;

private:
	FOnTemplateChanged OnTemplateChanged;
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool;
	TSharedPtr<SListView<TWeakObjectPtr<UMounteaInventoryItemTemplate>>> TemplateListView;
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> AvailableTemplates;
	
	TObjectPtr<UMounteaInventoryItemTemplate> CurrentTemplate;
	TObjectPtr<UMounteaInventoryItemTemplate> TransientTemplate;

	TSharedPtr<SEditableTextBox> DisplayNameTextBox;
	TSharedPtr<SEditableTextBox> ItemIDTextBox;
	TSharedPtr<SEditableTextBox> ThumbnailDescriptionTextBox;
	
	TSharedPtr<SMultiLineEditableTextBox> DescriptionTextBox;
	
	TSharedPtr<SGameplayTagContainerCombo> TagsCombo;
	TSharedPtr<SGameplayTagContainerCombo> AffectorsTagsCombo;
	
	TSharedPtr<STextComboBox> CategoryCombo;
	TSharedPtr<STextComboBox> SubCategoryCombo;
	TSharedPtr<STextComboBox> RarityCombo;
	TSharedPtr<STextComboBox> FlagsCombo;

	// Quantity
	TSharedPtr<SSpinBox<int32>> MaxQuantitySpinBox;
	TSharedPtr<SSpinBox<int32>> MaxStackSizeSpinBox;

	// Durability
	TSharedPtr<SCheckBox> DurabilityCheckBox;
	TSharedPtr<SSpinBox<float>> MaxDurabilitySpinBox;
	TSharedPtr<SSpinBox<float>> BaseDurabilitySpinBox;
	TSharedPtr<SSpinBox<float>> DurabilityPenaltySpinBox;
	TSharedPtr<SSpinBox<float>> DurabilityToPriceSpinBox;

	// Weight
	TSharedPtr<SCheckBox> WeightCheckBox;
	
	TSharedPtr<SObjectPropertyEntryBox> SpawnActorPicker;
	TSharedPtr<SObjectPropertyEntryBox> ThumbnailPicker;
	TSharedPtr<SObjectPropertyEntryBox> CoverPicker;
	TSharedPtr<SObjectPropertyEntryBox> MeshPicker;

	TArray<TSharedPtr<FString>> CachedCategoryOptions;
	TArray<TSharedPtr<FString>> CachedSubCategoryOptions;
	TArray<TSharedPtr<FString>> CachedRarityOptions;
	TArray<TSharedPtr<FString>> CachedFlagOptions;
	
	bool bIsEditingTransient = true;
};