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
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Framework/Docking/TabManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Notifications/NotificationManager.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "IContentBrowserSingleton.h"
#include "PropertyCustomizationHelpers.h"
#include "SGameplayTagContainerCombo.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Decorations/MounteaInventoryItemAction.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#include "Subsystems/MounteaInventoryTemplateEditorSubsystem.h"

#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dialogs/Dialogs.h"
#include "Dialogs/DlgPickAssetPath.h"

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateEditor"

SMounteaInventoryTemplateEditor::~SMounteaInventoryTemplateEditor()
{
	// Clean up transient template on destruction
	CleanupTransientTemplate();
}

void SMounteaInventoryTemplateEditor::Construct(const FArguments& InArgs)
{
	OnTemplateChanged = InArgs._OnTemplateChanged;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			CreateToolbar()
		]
		
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			CreatePropertyMatrix()
		]
	];
	
	// Show transient template by default if no templates exist
	if (AvailableTemplates.Num() == 0 || !SelectedTemplates.Num())
	{
		CreateTransientTemplate();
		RefreshTemplateList();
		SelectTransientTemplateInList();
	}
}

void SMounteaInventoryTemplateEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		if (auto itemTemplate = const_cast<UMounteaInventoryItemTemplate*>(Cast<UMounteaInventoryItemTemplate>(PropertyChangedEvent.GetObjectBeingEdited(0))))
		{
			if (!itemTemplate->HasAnyFlags(RF_Transient))
			{
				itemTemplate->GetPackage()->MarkPackageDirty();
				TrackDirtyAsset(itemTemplate);
			}
			else
				TrackDirtyAsset(itemTemplate);
		}
	}
}

TArray<UObject*> SMounteaInventoryTemplateEditor::LoadAllTemplatesForMatrix()
{
	TArray<UObject*> allTemplates;

	UMounteaInventoryTemplateEditorSubsystem* subsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>();
	if (subsystem && subsystem->HasTempTemplate())
		allTemplates.Add(subsystem->GetOrCreateTempTemplate());
	
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> assetDataList;
	assetRegistryModule.Get().GetAssetsByClass(UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName(), assetDataList, true);
	
	for (const FAssetData& assetData : assetDataList)
	{
		if (UMounteaInventoryItemTemplate* itemTemplate = Cast<UMounteaInventoryItemTemplate>(assetData.GetAsset()))
			allTemplates.Add(itemTemplate);
	}
	
	allTemplates.Sort([](const UObject& A, const UObject& B)
	{
		const UMounteaInventoryItemTemplate* templateA = Cast<UMounteaInventoryItemTemplate>(&A);
		const UMounteaInventoryItemTemplate* templateB = Cast<UMounteaInventoryItemTemplate>(&B);
		
		if (!templateA || !templateB)
			return templateA != nullptr;
		
		// Transient templates always come first
		if (templateA->HasAnyFlags(RF_Transient))
			return true;
		if (templateB->HasAnyFlags(RF_Transient))
			return false;
		
		FString nameA = templateA->DisplayName.IsEmpty() ? templateA->GetName() : templateA->DisplayName.ToString();
		FString nameB = templateB->DisplayName.IsEmpty() ? templateB->GetName() : templateB->DisplayName.ToString();
		
		return nameA < nameB;
	});
	
	return allTemplates;
}

void SMounteaInventoryTemplateEditor::RefreshTemplateList()
{
	TArray<UObject*> allTemplates = LoadAllTemplatesForMatrix();
	
	// Convert to TWeakObjectPtr for list view
	AvailableTemplates.Empty();
	for (UObject* obj : allTemplates)
	{
		if (UMounteaInventoryItemTemplate* itemTemplate = Cast<UMounteaInventoryItemTemplate>(obj))
			AvailableTemplates.Add(itemTemplate);
	}
	
	if (TemplateListView.IsValid())
		TemplateListView->RequestListRefresh();
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreatePropertyMatrix()
{
	TArray<UObject*> allTemplates = LoadAllTemplatesForMatrix();
	
	// Convert to TWeakObjectPtr for list view
	AvailableTemplates.Empty();
	for (UObject* obj : allTemplates)
	{
		if (UMounteaInventoryItemTemplate* itemTemplate = Cast<UMounteaInventoryItemTemplate>(obj))
			AvailableTemplates.Add(itemTemplate);
	}
	
	FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	FDetailsViewArgs detailsViewArgs;
	detailsViewArgs.bUpdatesFromSelection = false;
	detailsViewArgs.bLockable = false;
	detailsViewArgs.bAllowSearch = true;
	detailsViewArgs.bShowOptions = false;
	detailsViewArgs.NotifyHook = this;
	detailsViewArgs.bShowPropertyMatrixButton = false;
	detailsViewArgs.bHideSelectionTip = true;
	
	PropertyDetailsView = propertyEditorModule.CreateDetailView(detailsViewArgs);
	
	return SNew(SSplitter)
		.Orientation(Orient_Horizontal)
		
		+ SSplitter::Slot()
		.Value(0.3f)
		[
			SNew(SBorder)
			.Padding(4.0f)
			[
				SAssignNew(TemplateListView, SListView<TWeakObjectPtr<UMounteaInventoryItemTemplate>>)
				.ListItemsSource(&AvailableTemplates)
				.OnGenerateRow(this, &SMounteaInventoryTemplateEditor::GenerateTemplateListRow)
				.OnSelectionChanged(this, &SMounteaInventoryTemplateEditor::OnTemplateSelectionChanged)
				.SelectionMode(ESelectionMode::Multi)
			]
		]
		
		+ SSplitter::Slot()
		.Value(0.7f)
		[
			PropertyDetailsView.ToSharedRef()
		];
}

void SMounteaInventoryTemplateEditor::OnTemplateSelectionChanged(TWeakObjectPtr<UMounteaInventoryItemTemplate> SelectedTemplate, ESelectInfo::Type SelectInfo)
{
	if (!PropertyDetailsView.IsValid())
		return;
	
	// Check for unsaved changes in transient template
	if (bIsShowingTransient && CurrentTemplate.IsValid() && CurrentTemplate.Get()->HasAnyFlags(RF_Transient))
	{
		if (DirtyTemplates.Contains(CurrentTemplate))
		{
			if (!CheckForUnsavedChanges())
			{
				// User cancelled - reselect the transient template
				TemplateListView->SetSelection(CurrentTemplate, ESelectInfo::Direct);
				return;
			}
		}
	}
		
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> selectedTemplates = TemplateListView->GetSelectedItems();
	SelectedTemplates = selectedTemplates;
	
	TArray<UObject*> selectedObjects;
	bIsShowingTransient = false;
	
	for (const TWeakObjectPtr<UMounteaInventoryItemTemplate>& weakTemplate : selectedTemplates)
	{
		if (UMounteaInventoryItemTemplate* itemTemplate = weakTemplate.Get())
		{
			selectedObjects.Add(itemTemplate);
			if (itemTemplate->HasAnyFlags(RF_Transient))
				bIsShowingTransient = true;
		}
	}

	if (selectedTemplates.Num() == 1)
		CurrentTemplate = selectedTemplates[0];
	else
		CurrentTemplate = nullptr;
	
	PropertyDetailsView->SetObjects(selectedObjects);
}

FReply SMounteaInventoryTemplateEditor::OnCreateNewTemplate()
{
	// Check for unsaved changes in current transient template
	if (bIsShowingTransient && CurrentTemplate.IsValid() && DirtyTemplates.Contains(CurrentTemplate))
	{
		if (!CheckForUnsavedChanges())
			return FReply::Handled();
	}
	
	CreateTransientTemplate();
	RefreshTemplateList();
	SelectTransientTemplateInList();
	
	return FReply::Handled();
}

void SMounteaInventoryTemplateEditor::SelectTransientTemplateInList()
{
	if (TemplateListView.IsValid() && CurrentTemplate.IsValid())
	{
		TemplateListView->SetSelection(CurrentTemplate, ESelectInfo::Direct);
		TemplateListView->RequestScrollIntoView(CurrentTemplate);
	}
}

FReply SMounteaInventoryTemplateEditor::SaveTemplate()
{
	if (!CurrentTemplate.IsValid())
	{
		ShowTemplateEditorNotification(TEXT("Cannot save: No template is being edited."), false);
		return FReply::Unhandled();
	}

	FString validationError;
	if (!ValidateTemplateData(validationError))
	{
		ShowTemplateEditorNotification(FString::Printf(TEXT("Cannot save: %s."), *validationError), false);
		return FReply::Unhandled();
	}

	if (CurrentTemplate.Get()->HasAnyFlags(RF_Transient))
		return SaveNewTemplate();
	else
		return SaveExistingTemplate();
}

FReply SMounteaInventoryTemplateEditor::SaveAllDirtyTemplates()
{
	int32 savedCount = 0;
	TArray<FString> failedAssets;
	if (DirtyTemplates.IsEmpty()) return FReply::Unhandled();
	
	for (auto& dirtyTemplate : DirtyTemplates)
	{
		if (UMounteaInventoryItemTemplate* itemTemplate = dirtyTemplate.Get())
		{
			// Skip transient templates in bulk save
			if (itemTemplate->HasAnyFlags(RF_Transient))
				continue;
			
			if (UPackage* package = itemTemplate->GetPackage())
			{
				FString packageName = package->GetName();
				FString fileName = FPackageName::LongPackageNameToFilename(packageName, FPackageName::GetAssetPackageExtension());
				
				if (UPackage::SavePackage(package, itemTemplate, RF_Public | RF_Standalone, *fileName))
				{
					savedCount++;
					UntrackDirtyAsset(itemTemplate);
				}
				else
					failedAssets.Add(itemTemplate->DisplayName.ToString());
			}
		}
	}
	
	if (failedAssets.Num() > 0)
	{
		FString errorMsg = FString::Printf(TEXT("Failed to save %d assets: %s"), 
			failedAssets.Num(), *FString::Join(failedAssets, TEXT(", ")));
		ShowTemplateEditorNotification(errorMsg, false);
	}
	else if (savedCount > 0)
		ShowTemplateEditorNotification(FString::Printf(TEXT("Successfully saved %d templates."), savedCount), true);
	else
		ShowTemplateEditorNotification(TEXT("No templates to save."), true);
	
	return FReply::Handled();
}

FReply SMounteaInventoryTemplateEditor::CloseTemplate()
{
	if (HasUnsavedChanges())
	{
		if (!CheckForUnsavedChanges())
			return FReply::Unhandled();
	}
	
	return FReply::Handled();
}

FReply SMounteaInventoryTemplateEditor::SaveExistingTemplate()
{
	if (!CurrentTemplate.IsValid())
		return FReply::Unhandled();
	
	UMounteaInventoryItemTemplate* itemTemplate = CurrentTemplate.Get();
	if (UPackage* package = itemTemplate->GetPackage())
	{
		FString packageName = package->GetName();
		FString fileName = FPackageName::LongPackageNameToFilename(packageName, FPackageName::GetAssetPackageExtension());
		
		if (UPackage::SavePackage(package, itemTemplate, RF_Public | RF_Standalone, *fileName))
		{
			UntrackDirtyAsset(itemTemplate);
			ShowTemplateEditorNotification(TEXT("Template saved successfully!"), true);
			return FReply::Handled();
		}
	}
	
	ShowTemplateEditorNotification(TEXT("Failed to save template."), false);
	return FReply::Unhandled();
}

FReply SMounteaInventoryTemplateEditor::SaveNewTemplate()
{
	FString selectedPath = ShowSaveAssetDialog();
	if (selectedPath.IsEmpty())
		return FReply::Unhandled();

	if (!CurrentTemplate.IsValid())
		return FReply::Unhandled();

	UMounteaInventoryItemTemplate* transientTemplate = CurrentTemplate.Get();

	FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	FString packageName;
	FString assetName;
	FString className;
	FString subObjectName;
	FPackageName::SplitFullObjectPath(selectedPath, className, packageName, assetName, subObjectName);

	FString packagePath = FPackageName::GetLongPackagePath(packageName);

	UObject* newAsset = assetToolsModule.Get().CreateAsset(
		assetName,
		packagePath,
		UMounteaInventoryItemTemplate::StaticClass(),
		nullptr
	);
	
	if (UMounteaInventoryItemTemplate* newTemplate = Cast<UMounteaInventoryItemTemplate>(newAsset))
	{
		// TODO: Copy properties from transient to new template
		newTemplate->Guid = transientTemplate->Guid;
		newTemplate->DisplayName = transientTemplate->DisplayName;
		newTemplate->ItemCategory = transientTemplate->ItemCategory;
		newTemplate->ItemRarity = transientTemplate->ItemRarity;
		newTemplate->MaxQuantity = transientTemplate->MaxQuantity;
		newTemplate->MaxStackSize = transientTemplate->MaxStackSize;
		newTemplate->ItemSubCategory = transientTemplate->ItemSubCategory;
		newTemplate->ItemFlags = transientTemplate->ItemFlags;
		newTemplate->AttachmentSlots = transientTemplate->AttachmentSlots;
		newTemplate->ItemSpecialAffect = transientTemplate->ItemSpecialAffect;
		newTemplate->bHasDurability = transientTemplate->bHasDurability;
		newTemplate->MaxDurability = transientTemplate->MaxDurability;
		newTemplate->BaseDurability = transientTemplate->BaseDurability;
		newTemplate->DurabilityPenalization = transientTemplate->DurabilityPenalization;
		newTemplate->DurabilityToPriceCoefficient = transientTemplate->DurabilityToPriceCoefficient;
		newTemplate->bHasPrice = transientTemplate->bHasPrice;
		newTemplate->BasePrice = transientTemplate->BasePrice;
		newTemplate->SellPriceCoefficient = transientTemplate->SellPriceCoefficient;
		newTemplate->bHasWeight = transientTemplate->bHasWeight;
		newTemplate->Weight = transientTemplate->Weight;
		
		// Save the new asset
		if (UPackage* package = newTemplate->GetPackage())
		{
			FString fileName = FPackageName::LongPackageNameToFilename(package->GetName(), FPackageName::GetAssetPackageExtension());
			if (UPackage::SavePackage(package, newTemplate, RF_Public | RF_Standalone, *fileName))
			{
				// Clean up transient template
				UntrackDirtyAsset(transientTemplate);
				CleanupTransientTemplate();
				
				// Refresh list and select the new asset
				RefreshTemplateList();
				TemplateListView->SetSelection(newTemplate, ESelectInfo::Direct);
				
				ShowTemplateEditorNotification(TEXT("New template created successfully!"), true);
				return FReply::Handled();
			}
		}
	}
	
	ShowTemplateEditorNotification(TEXT("Failed to create new template."), false);
	return FReply::Unhandled();
}

FString SMounteaInventoryTemplateEditor::ShowSaveAssetDialog()
{
	FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	FSaveAssetDialogConfig config;
	config.DialogTitleOverride = LOCTEXT("SaveInventoryTemplateDialogTitle", "Save Inventory Template");
	config.DefaultPath = TEXT("/Game");
	config.DefaultAssetName = TEXT("NewInventoryTemplate");
	config.AssetClassNames.Add(UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName());
	config.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;
    
	return contentBrowserModule.Get().CreateModalSaveAssetDialog(config);
}

FReply SMounteaInventoryTemplateEditor::DeleteTemplate(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template)
{
	if (!Template.IsValid())
		return FReply::Unhandled();
	
	UMounteaInventoryItemTemplate* itemTemplate = Template.Get();
	
	// Don't delete transient templates
	if (itemTemplate->HasAnyFlags(RF_Transient))
	{
		ShowTemplateEditorNotification(TEXT("Cannot delete unsaved template."), false);
		return FReply::Handled();
	}
	
	FText title = LOCTEXT("DeleteTemplateTitle", "Delete Template");
	FText message = FText::Format(LOCTEXT("DeleteTemplateMessage", "Are you sure you want to delete '{0}'?"), 
		itemTemplate->DisplayName.IsEmpty() ? FText::FromString(itemTemplate->GetName()) : itemTemplate->DisplayName);
	
	if (FMessageDialog::Open(EAppMsgType::YesNo, message, title) == EAppReturnType::Yes)
	{
		if (UEditorAssetLibrary::DeleteAsset(itemTemplate->GetPathName()))
		{
			UntrackDirtyAsset(itemTemplate);
			RefreshTemplateList();
			ShowTemplateEditorNotification(TEXT("Template deleted successfully."), true);
		}
		else
			ShowTemplateEditorNotification(TEXT("Failed to delete template."), false);
	}
	
	return FReply::Handled();
}

FReply SMounteaInventoryTemplateEditor::DuplicateTemplate(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template)
{
	if (!Template.IsValid())
		return FReply::Unhandled();
	
	UMounteaInventoryItemTemplate* sourceTemplate = Template.Get();
	
	// Get the package path
	FString packagePath = FPackageName::GetLongPackagePath(sourceTemplate->GetPackage()->GetName());
	FString assetName = sourceTemplate->GetName() + TEXT("_Copy");
	
	// Create duplicate
	FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	UObject* duplicatedAsset = assetToolsModule.Get().DuplicateAsset(assetName, packagePath, sourceTemplate);
	TWeakObjectPtr<UMounteaInventoryItemTemplate> duplicatedTemplate = Cast<UMounteaInventoryItemTemplate>(duplicatedAsset);
	
	if (duplicatedAsset)
	{
		RefreshTemplateList();
		TemplateListView->SetSelection(duplicatedTemplate, ESelectInfo::Direct);
		ShowTemplateEditorNotification(TEXT("Template duplicated successfully."), true);
	}
	else
		ShowTemplateEditorNotification(TEXT("Failed to duplicate template."), false);
	
	return FReply::Handled();
}

void SMounteaInventoryTemplateEditor::ImportTemplate()
{
	// TODO: Implement import functionality
	ShowTemplateEditorNotification(TEXT("Import functionality not yet implemented."), false);
}

void SMounteaInventoryTemplateEditor::ExportTemplate()
{
	// TODO: Implement export functionality
	ShowTemplateEditorNotification(TEXT("Export functionality not yet implemented."), false);
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
			.OnClicked(this, &SMounteaInventoryTemplateEditor::OnCreateNewTemplate)
		]
		
		// Save Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("SaveTemplate", "Save Template"))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::SaveTemplate)
			.IsEnabled_Lambda([this]() { return CurrentTemplate.IsValid() && DirtyTemplates.Contains(CurrentTemplate); })
		]
		
		// Save All
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("SaveAllTemplates", "Save All"))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::SaveAllDirtyTemplates)
			.IsEnabled_Lambda([this]() { return DirtyTemplates.Num() > 0; })
		]
		
		// Import Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("ImportTemplate", "Import Items"))
			.OnClicked_Lambda([this]() { ImportTemplate(); return FReply::Handled(); })
		]
		
		// Export Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("ExportTemplate", "Export Items"))
			.OnClicked_Lambda([this]() { ExportTemplate(); return FReply::Handled(); })
		]

		// Close Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("CloseTemplate", "Close Template"))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::CloseTemplate)
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

TSharedRef<ITableRow> SMounteaInventoryTemplateEditor::GenerateTemplateListRow(TWeakObjectPtr<UMounteaInventoryItemTemplate> Template, const TSharedRef<STableViewBase>& OwnerTable)
{
	FText displayText = LOCTEXT("InvalidTemplate", "Invalid Template");
	FText fullText = displayText;
	FString assetPath = TEXT("Unknown Path");
	FText tooltipText = LOCTEXT("InvalidTemplateTooltip", "Invalid Template");
	bool bIsTransient = false;
	bool bIsDirty = false;
	
	if (Template.IsValid())
	{
		if (UMounteaInventoryItemTemplate* templatePtr = Template.Get())
		{
			bIsTransient = templatePtr->HasAnyFlags(RF_Transient);
			bIsDirty = DirtyTemplates.Contains(Template);
			
			fullText = templatePtr->DisplayName.IsEmpty() ? FText::FromString(templatePtr->GetName()) : templatePtr->DisplayName;
			
			if (bIsTransient)
				fullText = FText::Format(LOCTEXT("TransientTemplate", "{0} *"), fullText);
			else if (bIsDirty)
				fullText = FText::Format(LOCTEXT("DirtyTemplate", "{0} *"), fullText);
			
			displayText = FText::FromString(fullText.ToString().Len() > 32 ? fullText.ToString().Left(32) + TEXT("...") : fullText.ToString());
			
			if (UPackage* package = templatePtr->GetPackage())
				assetPath = bIsTransient ? TEXT("Unsaved") : package->GetName();

			tooltipText = FText::Format(
				LOCTEXT("TemplateTooltip", "Name: {0}\nPath: {1}\nCategory: {2}\nRarity: {3}\nStatus: {4}"),
				fullText,
				FText::FromString(assetPath),
				FText::FromString(templatePtr->ItemCategory),
				FText::FromString(templatePtr->ItemRarity),
				bIsTransient ? LOCTEXT("UnsavedStatus", "Unsaved") : (bIsDirty ? LOCTEXT("ModifiedStatus", "Modified") : LOCTEXT("SavedStatus", "Saved"))
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
				.ColorAndOpacity_Lambda([bIsTransient, bIsDirty]() 
				{
					if (bIsTransient) return FSlateColor(FLinearColor(0.8f, 0.8f, 0.2f)); // Yellow for transient
					if (bIsDirty) return FSlateColor(FLinearColor(0.8f, 0.3f, 0.3f)); // Red for dirty
					return FSlateColor::UseForeground();
				})
			]
			// Navigation Icon (hidden for transient)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.ContentPadding(FMargin(2.0f))
				.ToolTipText(LOCTEXT("NavigateToFolderTooltip", "Navigate to folder in Content Browser"))
				.Visibility_Lambda([bIsTransient]() { return bIsTransient ? EVisibility::Collapsed : EVisibility::Visible; })
				.OnClicked_Lambda([Template]()
				{
					if (Template.IsValid() && !Template.Get()->HasAnyFlags(RF_Transient))
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
			// Duplicate Icon (hidden for transient)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.ContentPadding(FMargin(2.0f))
				.ToolTipText(LOCTEXT("DuplicateTemplateTooltip", "Duplicate this template"))
				.Visibility_Lambda([bIsTransient]() { return bIsTransient ? EVisibility::Collapsed : EVisibility::Visible; })
				.OnClicked(this, &SMounteaInventoryTemplateEditor::DuplicateTemplate, Template)
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
			// Delete Icon (hidden for transient)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.ContentPadding(FMargin(2.0f))
				.ToolTipText(LOCTEXT("DeleteTemplateTooltip", "Delete this template"))
				.Visibility_Lambda([bIsTransient]() { return bIsTransient ? EVisibility::Collapsed : EVisibility::Visible; })
				.OnClicked(this, &SMounteaInventoryTemplateEditor::DeleteTemplate, Template)
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

void SMounteaInventoryTemplateEditor::CreateTransientTemplate()
{
	UMounteaInventoryTemplateEditorSubsystem* templateEditorSubsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>();
	CurrentTemplate = templateEditorSubsystem->GetOrCreateTempTemplate();
	bIsShowingTransient = true;
	bIsEditingExisting = false;
}

void SMounteaInventoryTemplateEditor::CleanupTransientTemplate()
{
	if (UMounteaInventoryTemplateEditorSubsystem* templateEditorSubsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>())
	{
		UntrackDirtyAsset(templateEditorSubsystem->GetOrCreateTempTemplate());
		templateEditorSubsystem->ClearTempTemplate();
	}
	CurrentTemplate = nullptr;
	bIsShowingTransient = false;
}

bool SMounteaInventoryTemplateEditor::ValidateTemplateData(FString& ErrorMessage) const
{
	if (!CurrentTemplate.IsValid())
	{
		ErrorMessage = TEXT("No template selected");
		return false;
	}
	
	UMounteaInventoryItemTemplate* itemTemplate = CurrentTemplate.Get();
	
	if (itemTemplate->DisplayName.IsEmpty())
	{
		ErrorMessage = TEXT("Display name cannot be empty");
		return false;
	}
	
	if (itemTemplate->ItemCategory.IsEmpty())
	{
		ErrorMessage = TEXT("Item category cannot be empty");
		return false;
	}
	
	return true;
}

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

void SMounteaInventoryTemplateEditor::TrackDirtyAsset(UMounteaInventoryItemTemplate* Template)
{
	if (Template)
		DirtyTemplates.Add(Template);
}

void SMounteaInventoryTemplateEditor::UntrackDirtyAsset(UMounteaInventoryItemTemplate* Template)
{
	if (Template)
		DirtyTemplates.Remove(Template);
}

bool SMounteaInventoryTemplateEditor::HasUnsavedChanges() const
{
	return DirtyTemplates.Num() > 0;
}

bool SMounteaInventoryTemplateEditor::CheckForUnsavedChanges()
{
	if (!HasUnsavedChanges())
		return true;
	
	FText title = LOCTEXT("UnsavedChangesTitle", "Unsaved Changes");
	FText message = LOCTEXT("UnsavedChangesMessage", "You have unsaved changes. Do you want to save them?");
	
	EAppReturnType::Type result = FMessageDialog::Open(EAppMsgType::YesNoCancel, message, title);
	
	if (result == EAppReturnType::Yes)
	{
		// Save all dirty templates
		SaveAllDirtyTemplates();
		return true;
	}
	else if (result == EAppReturnType::No)
	{
		// Discard changes
		DirtyTemplates.Empty();
		CleanupTransientTemplate();
		return true;
	}
	
	// Cancel
	return false;
}

#undef LOCTEXT_NAMESPACE