// Copyright Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
template<typename OptionType> class SComboBox;
class SCheckBox;
template<typename NumericType> class SSpinBox;
class SVerticalBox;
class SHorizontalBox;
class SScrollBox;
class SBorder;
class SSeparator;
template<typename ItemType> class SListView;
class STableViewBase;
template<typename ItemType> class STableRow;

DECLARE_DELEGATE_OneParam(FOnTemplateChanged, UMounteaInventoryItemTemplate*);

/**
 * Template Browser and Editor - manages all Mountea Inventory Item Templates in the project
 */
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API SMounteaInventoryTemplateEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaInventoryTemplateEditor) {}
        SLATE_EVENT(FOnTemplateChanged, OnTemplateChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Sets the template being edited */
	void SetTemplate(UMounteaInventoryItemTemplate* InTemplate)
	{ CurrentTemplate = InTemplate; };

	/** Gets the current template */
	UMounteaInventoryItemTemplate* GetTemplate() const { return CurrentTemplate; }

	/** Creates a new template */
	void CreateNewTemplate();

	/** Saves the current template */
	void SaveTemplate();

	/** Loads template from mnteaitem */
	void ImportTemplate();

	/** Exports template to mnteaitem */
	void ExportTemplate();

	/** Refreshes the template list from Asset Registry */
	void RefreshTemplateList();

private:
	/** Current template being edited */
	TObjectPtr<UMounteaInventoryItemTemplate> CurrentTemplate;

	/** Delegate for when template changes */
	FOnTemplateChanged OnTemplateChanged;

	/** List of all available templates */
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> AvailableTemplates;

	/** Template list widgets */
	TSharedPtr<SListView<TWeakObjectPtr<UMounteaInventoryItemTemplate>>> TemplateListView;

	/** Widget creation methods */
	TSharedRef<SWidget> CreateToolbar();
	TSharedRef<SWidget> CreateTemplateListPanel();
	TSharedRef<SWidget> CreateEditorPanel();
	TSharedRef<ITableRow> GenerateTemplateListRow(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template, const TSharedRef<STableViewBase>& OwnerTable);

	/** Section creation methods */
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

	/** Template management */
	void LoadAllTemplates();
};