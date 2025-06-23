// Copyright Dominik Morse 2024

#include "Editor/SMounteaInventoryTemplateEditor.h"

#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Application/SlateApplication.h"
#include "EditorStyleSet.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "PropertyCustomizationHelpers.h"
#include "SGameplayTagContainerCombo.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "SGameplayTagWidget.h"
#include "Decorations/MounteaInventoryItemAction.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Subsystems/UMounteaInventoryTemplateEditorSubsystem.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateEditor"

void SMounteaInventoryTemplateEditor::Construct(const FArguments& InArgs)
{
	OnTemplateChanged = InArgs._OnTemplateChanged;
	ThumbnailPool = MakeShareable(new FAssetThumbnailPool(16));
	LoadAllTemplates();
	CreateTransientTemplate();
	bIsEditingTransient = true;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Toolbar
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			CreateToolbar()
		]
		
		// Main content area
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SHorizontalBox)
			
			// Template List Panel (Left side)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f)
			[
				SNew(SBox)
				.WidthOverride(500.0f)
				[
					CreateTemplateListPanel()
				]
			]
			
			// Separator
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SSeparator)
				.Orientation(Orient_Vertical)
			]
			
			// Editor Panel (Right side)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(5.0f)
			[
				CreateEditorPanel()
			]
		]
	];
}

SMounteaInventoryTemplateEditor::~SMounteaInventoryTemplateEditor()
{
	CleanupTransientTemplate();
}

void SMounteaInventoryTemplateEditor::CreateNewTemplate()
{
	if (TemplateListView.IsValid())
		TemplateListView->ClearSelection();
	
	CleanupTransientTemplate();
	CreateTransientTemplate();
	DisplayNameTextBox.Get()->SetText(FText::FromString(TEXT("")));
	bIsEditingTransient = true;
	CurrentTemplate = TransientTemplate;
}

FReply SMounteaInventoryTemplateEditor::SaveTemplate()
{
	if (!TransientTemplate || !bIsEditingTransient)
	{
		ShowTemplateEditorNotification(TEXT("Cannot save: No template is being edited."), false);
		return FReply::Unhandled();
	}

	// TODO: run validation before saving!
	FString validationError;
	if (!ValidateTemplateData(validationError))
	{
		ShowTemplateEditorNotification(FString::Printf(TEXT("Cannot save: %s."), *validationError), false);
		return FReply::Unhandled();
	}
	
	// Determine default asset name
	FString defaultAssetName = DisplayNameTextBox.IsValid() ? DisplayNameTextBox->GetText().ToString() : TEXT("");
	if (defaultAssetName.IsEmpty())
	{
		ShowTemplateEditorNotification(TEXT("Failed to create a package with invalid name."), false);
		return FReply::Unhandled();
	}

	// Configure Save Asset Dialog
	FSaveAssetDialogConfig saveAssetDialogConfig;
	saveAssetDialogConfig.DefaultPath = TEXT("/Game/Inventory/Templates");
	saveAssetDialogConfig.DefaultAssetName = defaultAssetName;
	saveAssetDialogConfig.AssetClassNames.Add(UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName());
	saveAssetDialogConfig.DialogTitleOverride = FText::FromString(TEXT("Save Inventory Template As"));
	saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

	// Show the Save Asset Dialog
	FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	const FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog(saveAssetDialogConfig);

	// Handle user canceling the dialog
	if (saveObjectPath.IsEmpty())
	{
		ShowTemplateEditorNotification(TEXT("Save cancelled."), false);
		return FReply::Unhandled();
	}

	// Create or load the package from the selected path
	UPackage* newPackage = CreatePackage(*saveObjectPath);
	if (!newPackage)
	{
		ShowTemplateEditorNotification(TEXT("Failed to create a package at the selected location."), false);
		return FReply::Unhandled();
	}

	// Duplicate the transient template into the new package
	UMounteaInventoryItemTemplate* newTemplate = DuplicateObject<UMounteaInventoryItemTemplate>(TransientTemplate, newPackage);
	if (!newTemplate)
	{
		ShowTemplateEditorNotification(TEXT("Failed to create new Template at the selected location."), false);
		return FReply::Unhandled();
	}

	// Set asset flags
	newTemplate->SetFlags(RF_Public | RF_Standalone);
	newTemplate->ClearFlags(RF_Transient);

	// Register the asset
	FAssetRegistryModule::AssetCreated(newTemplate);
	newPackage->MarkPackageDirty();

	// Refresh UI and selection
	RefreshTemplateList();

	if (TemplateListView.IsValid())
	{
		for (const TWeakObjectPtr<UMounteaInventoryItemTemplate>& Template : AvailableTemplates)
		{
			if (Template.IsValid() && Template.Get() == newTemplate)
			{
				TemplateListView->SetSelection(Template);
				break;
			}
		}
	}

	// Clear editing state
	bIsEditingTransient = false;
	CurrentTemplate = newTemplate;
	DisplayNameTextBox.Get()->SetText(FText::FromString(TEXT("")));

	ShowTemplateEditorNotification(TEXT("Item Template created."), true);
	return FReply::Handled();
}

void SMounteaInventoryTemplateEditor::ImportTemplate()
{
}

void SMounteaInventoryTemplateEditor::ExportTemplate()
{
}

void SMounteaInventoryTemplateEditor::RefreshTemplateList()
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	if (assetRegistryModule.Get().IsLoadingAssets())
	{
		assetRegistryModule.Get().OnFilesLoaded().AddLambda([this]()
		{
			RefreshTemplateList();
		});
		return;
	}
	
	TWeakObjectPtr<UMounteaInventoryItemTemplate> currentlySelectedTemplate = nullptr;
	if (TemplateListView.IsValid())
	{
		TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> SelectedItems = TemplateListView->GetSelectedItems();
		if (SelectedItems.Num() > 0)
			currentlySelectedTemplate = SelectedItems[0];
	}
	
	LoadAllTemplates();
	
	if (currentlySelectedTemplate.IsValid() && TemplateListView.IsValid())
	{
		for (const TWeakObjectPtr<UMounteaInventoryItemTemplate>& itemTemplate : AvailableTemplates)
		{
			if (itemTemplate.IsValid() && itemTemplate.Get() == currentlySelectedTemplate.Get())
			{
				TemplateListView->SetSelection(itemTemplate);
				TemplateListView->RequestScrollIntoView(itemTemplate);
				break;
			}
		}
	}
	
	if (!currentlySelectedTemplate.IsValid() && TemplateListView.IsValid())
	{
		TemplateListView->ClearSelection();
		CurrentTemplate = nullptr;
	}
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateToolbar()
{
	return SNew(SHorizontalBox)
		
		// New Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("NewTemplate", "New Template"))
		]
		
		// Save Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("SaveTemplate", "Save Template"))
			.OnClicked(FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::SaveTemplate))
		]
		
		// Import Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("ImportTemplate", "Import Items"))
		]
		
		// Export Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("ExportTemplate", "Export Items"))
		]
		
		// Spacer
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNullWidget::NullWidget
		]
		
		// Search box
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SBox)
			.WidthOverride(200.0f)
			[
				SNew(SEditableTextBox)
				.HintText(LOCTEXT("SearchTemplates", "Search templates..."))
			]
		];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateTemplateListPanel()
{
	return SNew(SVerticalBox)
		
		// Header
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("TemplateListHeader", "Templates"))
		]
		
		// Template List
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SBorder)
			.Padding(2.0f)
			[
				SAssignNew(TemplateListView, SListView<TWeakObjectPtr<UMounteaInventoryItemTemplate>>)
				.ListItemsSource(&AvailableTemplates)
				.OnGenerateRow(this, &SMounteaInventoryTemplateEditor::GenerateTemplateListRow)
				.SelectionMode(ESelectionMode::Single)
				.OnSelectionChanged(this, &SMounteaInventoryTemplateEditor::OnTemplateSelectionChanged)
			]
		];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateEditorPanel()
{
	return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
			
			// Basic Information Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateBasicInfoSection()
			]
			
			// Item Properties Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateItemPropertiesSection()
			]
			
			// Weight System Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateWeightSystemSection()
			]
			
			// Price System Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreatePriceSystemSection()
			]
			
			// Durability System Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateDurabilitySystemSection()
			]
			
			// Item Flags Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateItemFlagsSection()
			]
			
			// Equipment Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateEquipmentSection()
			]
			
			// Visual Assets Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateVisualAssetsSection()
			]
			
			// Materials Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateMaterialsSection()
			]
			
			// Custom Properties Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				CreateCustomPropertiesSection()
			]
		];
}

TSharedRef<ITableRow> SMounteaInventoryTemplateEditor::GenerateTemplateListRow(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template, const TSharedRef<STableViewBase>& OwnerTable)
{
	FText displayText = LOCTEXT("InvalidTemplate", "Invalid Template");
	FText fullText = displayText;
	FString assetPath = TEXT("Unknown Path");
	FText tooltipText = LOCTEXT("InvalidTemplateTooltip", "Invalid Template");
	
	if (Template.IsValid())
	{
		if (UMounteaInventoryItemTemplate* templatePtr = Template.Get())
		{
			fullText = templatePtr->DisplayName.IsEmpty() ? FText::FromString(templatePtr->GetName()) : templatePtr->DisplayName;
			displayText = FText::FromString(fullText.ToString().Len() > 32 ? fullText.ToString().Left(32) + TEXT("...") : fullText.ToString());
			if (UPackage* package = templatePtr->GetPackage())
				assetPath = package->GetName();

			tooltipText = FText::Format(
				LOCTEXT("TemplateTooltip", "Name: {0}\nPath: {1}\nCategory: {2}\nRarity: {3}"),
				fullText,
				FText::FromString(assetPath),
				FText::FromString(templatePtr->ItemCategory),
				FText::FromString(templatePtr->ItemRarity)
			);
		}
	}
	
	return SNew(STableRow<TWeakObjectPtr<UMounteaInventoryItemTemplate>>, OwnerTable)
	[
		SNew(SVerticalBox)
		
		// Row 1: Title + Icons
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			// Title
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Text(displayText)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
				.ToolTipText(tooltipText)
			]
			// Navigation Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.ContentPadding(FMargin(2.0f))
				.ToolTipText(LOCTEXT("NavigateToFolderTooltip", "Navigate to folder in Content Browser"))
				.OnClicked_Lambda([Template]()
				{
					if (Template.IsValid())
					{
						TArray<FAssetData> AssetDataList;
						AssetDataList.Add(FAssetData(Template.Get()));
						
						FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
						ContentBrowserModule.Get().SyncBrowserToAssets(AssetDataList);
					}
					return FReply::Handled();
				})
				[
					SNew(SBox)
					.MaxAspectRatio(1.f)
					.WidthOverride(16)
					.HeightOverride(16)
					[
						SNew(SImage)
						.Image(FAppStyle::GetBrush("Icons.FolderOpen"))
						.ColorAndOpacity(FSlateColor::UseForeground())
					]
				]
			]
			// Duplicate Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.ContentPadding(FMargin(2.0f))
				.ToolTipText(LOCTEXT("DuplicateTemplateTooltip", "Duplicate this template"))
				.OnClicked_Lambda([this, Template]()
				{
					// TODO: Implement duplicate functionality
					ShowTemplateEditorNotification(TEXT("Duplicate functionality not yet implemented."), false);
					return FReply::Handled();
				})
				[
					SNew(SBox)
					.MaxAspectRatio(1.f)
					.WidthOverride(16)
					.HeightOverride(16)
					[
						SNew(SImage)
						.Image(FAppStyle::GetBrush("Icons.Duplicate"))
						.ColorAndOpacity(FSlateColor::UseForeground())
					]
				]
			]
			// Delete Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.ContentPadding(FMargin(2.0f))
				.ToolTipText(LOCTEXT("DeleteTemplateTooltip", "Delete this template"))
				.OnClicked_Lambda([this, Template]()
				{
					// TODO: Implement delete functionality
					ShowTemplateEditorNotification(TEXT("Delete functionality not yet implemented."), false);
					return FReply::Handled();
				})
				[
					SNew(SBox)
					.MaxAspectRatio(1.f)
					.WidthOverride(16)
					.HeightOverride(16)
					[
						SNew(SImage)
						.Image(FAppStyle::GetBrush("Icons.Delete"))
						.ColorAndOpacity(FSlateColor::UseForeground())
					]
				]
			]
		]
		
		// Row 2: Asset Path
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f, 0.0f, 5.0f, 2.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(assetPath))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			.IsEnabled(false)
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateBasicInfoSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("BasicInformation", "Basic Information"))
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemName", "Item Name"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SAssignNew(DisplayNameTextBox, SEditableTextBox)
				.Text_Lambda([this]() {
					return IsValid(CurrentTemplate) ? 
						FText::FromString(CurrentTemplate->GetName()) : 
						FText::GetEmpty();
				})
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("DisplayName", "Display Name"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SAssignNew(DisplayNameTextBox, SEditableTextBox)
				.Text_Lambda([this]() {
					return CurrentTemplate != nullptr ? 
						CurrentTemplate->DisplayName : 
						FText::GetEmpty();
				})
				.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type)
				{
					if (CurrentTemplate != nullptr)
						CurrentTemplate->DisplayName = NewText;
				})
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemID", "Item ID"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SAssignNew(ItemIDTextBox, SEditableTextBox)
					.Text_Lambda([this]() {
						return IsValid(CurrentTemplate) ? 
							FText::FromString(CurrentTemplate->Guid.ToString()) : 
							FText::GetEmpty();
					})
					.IsReadOnly(true)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("GenerateGUID", "Generate New GUID"))
					.OnClicked_Lambda([this]()
					{
						if (IsValid(CurrentTemplate))
						{
							CurrentTemplate->Guid = FGuid::NewGuid();
						}
						return FReply::Handled();
					})
				]
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ThumbnailDescription", "Thumbnail Description"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SAssignNew(ThumbnailDescriptionTextBox, SEditableTextBox)
				.Text_Lambda([this]() {
					return IsValid(CurrentTemplate) ? 
						CurrentTemplate->ItemShortInfo : 
						FText::GetEmpty();
				})
				.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type)
				{
					if (IsValid(CurrentTemplate))
						CurrentTemplate->ItemShortInfo = NewText;
				})
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Description", "Description"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SBox)
				.HeightOverride(100.0f)
				[
					SNew(SBorder)
					.Padding(0.0f)
					[
						SAssignNew(DescriptionTextBox, SMultiLineEditableTextBox)
						.AllowMultiLine(true)
						.WrappingPolicy(ETextWrappingPolicy::DefaultWrapping)
						.AutoWrapText(true)
						.Text_Lambda([this]() {
							return IsValid(CurrentTemplate)
								? CurrentTemplate->ItemLongInfo
								: FText::GetEmpty();
						})
						.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type)
						{
							if (IsValid(CurrentTemplate))
							{
								CurrentTemplate->ItemLongInfo = NewText;
							}
						})
					]
				]
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateItemPropertiesSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ItemProperties", "Item Properties"))
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemType", "Item Type"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SubCategory", "Sub Category"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Rarity", "Rarity"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MaxStackSize", "Max Stack Size"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<int32>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MaxQuantity", "Max Quantity"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<int32>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Tags", "Tags"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.HintText(LOCTEXT("TagsHint", "Type to add tags"))
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateWeightSystemSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("WeightSystem", "Weight System"))
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.0f, 10.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Weight", "Weight"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<float>)
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreatePriceSystemSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("PriceSystem", "Price System"))
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.0f, 10.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("BasePrice", "Base Price"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<float>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.0f, 5.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SellPriceCoefficient", "Sell Price Coefficient"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<float>)
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateDurabilitySystemSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("DurabilitySystem", "Durability System"))
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.0f, 10.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MaxDurability", "Max Durability"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<float>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.0f, 5.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("BaseDurability", "Base Durability"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<float>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.0f, 5.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("DurabilityPenalty", "Durability Penalty"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<float>)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(20.0f, 5.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("DurabilityToPriceCoefficient", "Durability to Price Coefficient"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpinBox<float>)
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateItemFlagsSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ItemFlags", "Item Flags"))
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			
			// Left column
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SVerticalBox)
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SCheckBox)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Stackable", "Stackable"))
					]
				]
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SCheckBox)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Usable", "Usable"))
					]
				]
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SCheckBox)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Tradeable", "Tradeable"))
					]
				]
			]
			
			// Right column
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SVerticalBox)
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SCheckBox)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Droppable", "Droppable"))
					]
				]
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SCheckBox)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Equippable", "Equippable"))
					]
				]
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SCheckBox)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("QuestItem", "Quest Item"))
					]
				]
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateEquipmentSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("EquipmentProperties", "Equipment Properties"))
		]
		
		// Affector Slots (TSet<FGameplayTag>)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AffectorSlots", "Affector Slots"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				// TODO: Replace with a GameplayTag widget
				SNew(SEditableTextBox)
				.Text_Lambda([this]() {
					return IsValid(CurrentTemplate) ? 
						FText::FromString(CurrentTemplate->Tags.ToString()) : 
						FText::GetEmpty();
				})
				.HintText(LOCTEXT("TagsHint", "GameplayTags"))
			]
		]
		
		// Spawn Actor (TSoftClassPtr<AActor>)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SpawnActor", "Spawn Actor"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SClassPropertyEntryBox)
				.MetaClass(AActor::StaticClass())
				.AllowNone(true)
				.SelectedClass_Lambda([this]() -> const UClass*
				{
					return nullptr;
				})
				.OnSetClass_Lambda([this](const UClass* NewClass)
				{
					// Handle class selection change
					if (IsValid(CurrentTemplate))
						CurrentTemplate->SpawnActor = NewClass;
				})
			]
		]
		
		// Item Special Affect (UObject)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemSpecialAffect", "Special Affect"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UMounteaInventoryItemAction::StaticClass())
				.AllowClear(true)
				.DisplayThumbnail(true)
				.ThumbnailPool(ThumbnailPool.ToSharedRef())
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateVisualAssetsSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("VisualAssets", "Visual Assets"))
		]
		
		// Item Thumbnail (UTexture2D)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemThumbnail", "Item Thumbnail"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UTexture2D::StaticClass())
				.AllowClear(true)
				.DisplayThumbnail(true)
				.ThumbnailPool(ThumbnailPool.ToSharedRef())
			]
		]
		
		// Item Cover (UTexture2D)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemCover", "Item Cover"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UTexture2D::StaticClass())
				.AllowClear(true)
				.DisplayThumbnail(true)
				.ThumbnailPool(ThumbnailPool.ToSharedRef())
			]
		]
		
		// Item Mesh (UStreamableRenderAsset - supports both Static and Skeletal meshes)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemMesh", "Item Mesh"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UStreamableRenderAsset::StaticClass())
				.AllowClear(true)
				.DisplayThumbnail(true)
				.ThumbnailPool(ThumbnailPool.ToSharedRef())
				.AllowCreate(false)
				.OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &SMounteaInventoryTemplateEditor::OnFilterMeshAssets))
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateMaterialsSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Materials", "Materials"))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNullWidget::NullWidget
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("AddMaterial", "Add Material"))
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(5.0f)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)
				+ SScrollBox::Slot()
				[
					SNew(SVerticalBox)
					
					// Example material entry - using native UMaterialInterface reference
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.0f, 0.0f, 10.0f, 0.0f)
						[
							SNew(SBox)
							.WidthOverride(100.0f)
							[
								SNew(STextBlock)
								.Text(LOCTEXT("MaterialSlot", "Slot 0"))
							]
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SObjectPropertyEntryBox)
							.AllowedClass(UMaterialInterface::StaticClass())
							.AllowClear(true)
							.DisplayThumbnail(true)
							.ThumbnailPool(ThumbnailPool.ToSharedRef())
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.0f, 0.0f, 0.0f, 0.0f)
						[
							SNew(SButton)
							.Text(LOCTEXT("Remove", "Remove"))
						]
					]
					
					// Placeholder when no materials
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.Padding(0.0f, 10.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoMaterials", "No materials added. Click 'Add Material' to add one."))
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					]
				]
			]
		]
	];
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateCustomPropertiesSection()
{
	return SNew(SBorder)
	.Padding(10.0f)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CustomProperties", "Custom Properties"))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNullWidget::NullWidget
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("AddProperty", "Add Property"))
			]
		]
		
		// Tags (FGameplayTagContainer)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(150.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Tags", "Tags"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SGameplayTagContainerCombo)
				.TagContainer_Lambda([this]() -> FGameplayTagContainer {
					return IsValid(CurrentTemplate) ? CurrentTemplate->Tags : FGameplayTagContainer();
				})
				.OnTagContainerChanged_Lambda([this](const FGameplayTagContainer& NewContainer) {
					if (IsValid(CurrentTemplate))
						CurrentTemplate->Tags = NewContainer;
				})
				.Filter(TEXT(""))
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(5.0f)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)
				+ SScrollBox::Slot()
				[
					SNew(SVerticalBox)
					
					// Placeholder for future custom properties system
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.Padding(0.0f, 10.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoCustomProperties", "Custom properties can be added via GameplayTags above."))
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					]
				]
			]
		]
	];
}

void SMounteaInventoryTemplateEditor::CreateTransientTemplate()
{
	UUMounteaInventoryTemplateEditorSubsystem* templateEditorSubsystem = GEditor->GetEditorSubsystem<UUMounteaInventoryTemplateEditorSubsystem>();
	TransientTemplate = templateEditorSubsystem->GetOrCreateTempTemplate();
	CurrentTemplate = TransientTemplate;
}

void SMounteaInventoryTemplateEditor::CleanupTransientTemplate()
{
	if (UUMounteaInventoryTemplateEditorSubsystem* templateEditorSubsystem = GEditor->GetEditorSubsystem<UUMounteaInventoryTemplateEditorSubsystem>())
		templateEditorSubsystem->ClearTempTemplate();
	TransientTemplate = nullptr;
}

void SMounteaInventoryTemplateEditor::OnTemplateSelectionChanged(
	TWeakObjectPtr<UMounteaInventoryItemTemplate> SelectedTemplate, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo == ESelectInfo::Direct)
		return;

	if (SelectedTemplate.IsValid())
	{
		LoadTemplateData(SelectedTemplate.Get());
		bIsEditingTransient = false;
	}
	else
		CreateNewTemplate();
}

void SMounteaInventoryTemplateEditor::LoadTemplateData(UMounteaInventoryItemTemplate* Template)
{
	if (!Template)
		return;

	CleanupTransientTemplate();
	CreateTransientTemplate();
	
	if (TransientTemplate)
	{
		TransientTemplate->Guid = Template->Guid;
		TransientTemplate->DisplayName = Template->DisplayName;
		TransientTemplate->ItemCategory = Template->ItemCategory;
		TransientTemplate->ItemSubCategory = Template->ItemSubCategory;
		TransientTemplate->ItemRarity = Template->ItemRarity;
		TransientTemplate->ItemFlags = Template->ItemFlags;
		TransientTemplate->MaxQuantity = Template->MaxQuantity;
		TransientTemplate->MaxStackSize = Template->MaxStackSize;
		TransientTemplate->Tags = Template->Tags;
		TransientTemplate->SpawnActor = Template->SpawnActor;
		TransientTemplate->ItemShortInfo = Template->ItemShortInfo;
		TransientTemplate->ItemLongInfo = Template->ItemLongInfo;
		TransientTemplate->ItemThumbnail = Template->ItemThumbnail;
		TransientTemplate->ItemCover = Template->ItemCover;
		TransientTemplate->ItemMesh = Template->ItemMesh;
		TransientTemplate->bHasDurability = Template->bHasDurability;
		TransientTemplate->MaxDurability = Template->MaxDurability;
		TransientTemplate->BaseDurability = Template->BaseDurability;
		TransientTemplate->DurabilityPenalization = Template->DurabilityPenalization;
		TransientTemplate->DurabilityToPriceCoefficient = Template->DurabilityToPriceCoefficient;
		TransientTemplate->bHasPrice = Template->bHasPrice;
		TransientTemplate->BasePrice = Template->BasePrice;
		TransientTemplate->SellPriceCoefficient = Template->SellPriceCoefficient;
		TransientTemplate->bHasWeight = Template->bHasWeight;
		TransientTemplate->Weight = Template->Weight;
		TransientTemplate->AffectorSlots = Template->AffectorSlots;
		TransientTemplate->ItemSpecialAffect = Template->ItemSpecialAffect;
	}
	
	CurrentTemplate = TransientTemplate;
	bIsEditingTransient = true;
	
	// Force UI refresh for all bound widgets
	if (DisplayNameTextBox.IsValid())
		DisplayNameTextBox->Invalidate(EInvalidateWidget::Layout);
	if (DisplayNameTextBox.IsValid())
		DisplayNameTextBox->Invalidate(EInvalidateWidget::Layout);
	if (ItemIDTextBox.IsValid())
		ItemIDTextBox->Invalidate(EInvalidateWidget::Layout);
	if (ThumbnailDescriptionTextBox.IsValid())
		ThumbnailDescriptionTextBox->Invalidate(EInvalidateWidget::Layout);
	if (DescriptionTextBox.IsValid())
		DescriptionTextBox->Invalidate(EInvalidateWidget::Layout);
}

bool SMounteaInventoryTemplateEditor::ValidateTemplateData(FString& ErrorMessage) const
{
	if (!DisplayNameTextBox.IsValid())
	{
		ErrorMessage = TEXT("Item name text box is not valid.");
		return false;
	}
	if (DisplayNameTextBox.Get()->GetText().IsEmpty())
	{
		ErrorMessage = TEXT("Item name is empty.");
		return false;
	}
	return true;
}

// TODO: provide link to click to navigate to the template in the content browser
void SMounteaInventoryTemplateEditor::ShowTemplateEditorNotification(const FString& Message, const bool bSuccess) const
{
	FNotificationInfo notifInfo(FText::FromString(Message));
	notifInfo.bFireAndForget = true;
	notifInfo.ExpireDuration = 4.0f;
	notifInfo.bUseThrobber = false;
	notifInfo.bUseSuccessFailIcons = true;
	notifInfo.bUseLargeFont = false;
	notifInfo.WidthOverride = FOptionalSize();

	TSharedPtr<SNotificationItem> notificationItem = FSlateNotificationManager::Get().AddNotification(notifInfo);
	if (notificationItem.IsValid())
		notificationItem->SetCompletionState(bSuccess ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);
}

bool SMounteaInventoryTemplateEditor::OnFilterMeshAssets(const FAssetData& AssetData) const
{
	const auto potentialAsset = AssetData.GetAsset();

	if (!IsValid(potentialAsset))
		return true;

	if (potentialAsset->IsA(UStaticMesh::StaticClass()))
		return false;

	if (potentialAsset->IsA(USkeletalMesh::StaticClass()))
		return false;
	
	return true;
}

void SMounteaInventoryTemplateEditor::LoadAllTemplates()
{
	AvailableTemplates.Empty();
	
	// Use Asset Registry to find all UMounteaInventoryItemTemplate assets
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	TArray<FAssetData> assetDataList;
	assetRegistryModule.Get().GetAssetsByClass(UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName(), assetDataList, true);
	
	for (const FAssetData& assetData : assetDataList)
	{
		if (UMounteaInventoryItemTemplate* itemTemplate = Cast<UMounteaInventoryItemTemplate>(assetData.GetAsset()))
			AvailableTemplates.Add(itemTemplate);
	}
	
	// Sort templates by display name for better organization
	AvailableTemplates.Sort([](const TWeakObjectPtr<UMounteaInventoryItemTemplate>& A, const TWeakObjectPtr<UMounteaInventoryItemTemplate>& B)
	{
		if (!A.IsValid() || !B.IsValid())
			return A.IsValid();
		
		FString nameA = A->DisplayName.IsEmpty() ? A->GetName() : A->DisplayName.ToString();
		FString nameB = B->DisplayName.IsEmpty() ? B->GetName() : B->DisplayName.ToString();
		
		return nameA < nameB;
	});
	
	// Refresh the list view if it exists
	if (TemplateListView.IsValid())
		TemplateListView->RequestListRefresh();
}

#undef LOCTEXT_NAMESPACE