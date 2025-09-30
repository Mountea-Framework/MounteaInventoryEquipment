// Copyright Dominik Morse 2024

#include "Editor/SMounteaInventoryTemplateEditor.h"

#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Framework/Docking/TabManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Notifications/NotificationManager.h"

#include "AssetRegistry/AssetRegistryModule.h"

#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "IContentBrowserSingleton.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/MessageDialog.h"
#include "Decorations/MounteaInventoryItemAction.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#include "Subsystems/MounteaInventoryTemplateEditorSubsystem.h"

#include "AssetToolsModule.h"
#include "Algo/ForEach.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "UObject/SavePackage.h"

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
	
	if (AvailableTemplates.Num() == 0 || !SelectedTemplates.Num())
	{
		CreateTransientTemplate();
		RefreshTemplateList();

		OnTemplateSelectionChanged(nullptr, ESelectInfo::Direct);
	}
}

void SMounteaInventoryTemplateEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
		return;
	
	if (auto itemTemplate = const_cast<UMounteaInventoryItemTemplate*>(Cast<UMounteaInventoryItemTemplate>(PropertyChangedEvent.GetObjectBeingEdited(0))))
	{
		if (!itemTemplate->HasAnyFlags(RF_Transient))
		{
			itemTemplate->GetPackage()->MarkPackageDirty();
			TrackDirtyAsset(itemTemplate);
		}
		else
			TrackDirtyAsset(itemTemplate);

		if (TemplateListView.IsValid())
			TemplateListView->RequestListRefresh();
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
	
	TArray<UMounteaInventoryItemTemplate*> filteredTemplates;
	Algo::TransformIf(
		assetDataList,
		filteredTemplates,
		[](const FAssetData& AssetData)
		{
			const auto* Item = Cast<UMounteaInventoryItemTemplate>(AssetData.GetAsset());
			return Item && !Item->HasAnyFlags(RF_Transient);
		},
		[](const FAssetData& AssetData)
		{
			return Cast<UMounteaInventoryItemTemplate>(AssetData.GetAsset());
		}
	);

	allTemplates.Append(filteredTemplates);
	
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
	Algo::TransformIf(
		allTemplates,
		AvailableTemplates,
		[](UObject* Obj)
		{
			auto* ItemTemplate = Cast<UMounteaInventoryItemTemplate>(Obj);
			return ItemTemplate && !ItemTemplate->HasAnyFlags(RF_Transient);
		},
		[](UObject* Obj)
		{
			return TWeakObjectPtr<UMounteaInventoryItemTemplate>(Cast<UMounteaInventoryItemTemplate>(Obj));
		}
	);
	
	if (TemplateListView.IsValid())
		TemplateListView->RequestListRefresh();
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreatePropertyMatrix()
{
	TArray<UObject*> allTemplates = LoadAllTemplatesForMatrix();
	
	// Convert to TWeakObjectPtr for list view
	AvailableTemplates.Empty();
	Algo::TransformIf(
		allTemplates,
		AvailableTemplates,
		[](UObject* obj)
		{
			return Cast<UMounteaInventoryItemTemplate>(obj) != nullptr;
		},
		[](UObject* obj)
		{
			return TWeakObjectPtr<UMounteaInventoryItemTemplate>(Cast<UMounteaInventoryItemTemplate>(obj));
		}
	);
	
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
		.PhysicalSplitterHandleSize(1.f)
		
		+ SSplitter::Slot()
		.Value(0.3f)
		[
			SNew(SBorder)
			.Padding(4.0f)
			.BorderImage(FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.RoundedBorder"))
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

	const bool bHasTransient = bIsShowingTransient && CurrentTemplate.IsValid() && CurrentTemplate->HasAnyFlags(RF_Transient);
	const bool bIsDirty = DirtyTemplates.Contains(CurrentTemplate);

	if (bHasTransient && bIsDirty && !CheckForUnsavedChanges())
	{
		TemplateListView->SetSelection(CurrentTemplate, ESelectInfo::Direct);
		return;
	}

	SelectedTemplates = TemplateListView->GetSelectedItems();

	TArray<UObject*> selectedObjects;
	bIsShowingTransient = false;
	
	Algo::TransformIf(
		SelectedTemplates,
		selectedObjects,
		[this](const TWeakObjectPtr<UMounteaInventoryItemTemplate>& WeakTemplate)
		{
			auto* Template = WeakTemplate.Get();
			if (Template && Template->HasAnyFlags(RF_Transient))
				bIsShowingTransient = true;
			return Template != nullptr;
		},
		[](const TWeakObjectPtr<UMounteaInventoryItemTemplate>& WeakTemplate)
		{
			return static_cast<UObject*>(WeakTemplate.Get());
		}
	);

	if (SelectedTemplates.Num() == 1)
		CurrentTemplate = SelectedTemplates[0];
	else
		CurrentTemplate = nullptr;

	if (selectedObjects.IsEmpty())
	{
		UMounteaInventoryTemplateEditorSubsystem* subsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>();
		if (subsystem && subsystem->HasTempTemplate())
		{
			UObject* tempObject = subsystem->GetOrCreateTempTemplate();
			selectedObjects.Add(tempObject);
			CurrentTemplate = Cast<UMounteaInventoryItemTemplate>(tempObject);;
			bIsShowingTransient = true;
		}
	}
	
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
	//SelectTransientTemplateInList();

	TemplateListView->ClearSelection();
	return FReply::Handled();
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
	if (DirtyTemplates.IsEmpty())
		return FReply::Unhandled();

	int32 savedCount = 0;
	TArray<FString> failedAssets;

	Algo::ForEach(DirtyTemplates, [&](const TWeakObjectPtr<UMounteaInventoryItemTemplate>& weakTemplate)
	{
		UMounteaInventoryItemTemplate* itemTemplate = weakTemplate.Get();
		if (!itemTemplate || itemTemplate->HasAnyFlags(RF_Transient))
			return;

		UPackage* package = itemTemplate->GetPackage();
		if (!package)
			return;

		const FString packageName = package->GetName();
		const FString fileName = FPackageName::LongPackageNameToFilename(packageName, FPackageName::GetAssetPackageExtension());

		FSavePackageArgs saveArgs;
		saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		saveArgs.SaveFlags = SAVE_None;

		if (UPackage::SavePackage(package, itemTemplate, *fileName, saveArgs))
		{
			savedCount++;
			UntrackDirtyAsset(itemTemplate);
		}
		else
			failedAssets.Add(itemTemplate->DisplayName.ToString());
	});

	if (!failedAssets.IsEmpty())
	{
		const FString errorMsg = FString::Printf(TEXT("Failed to save %d assets: %s"),
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

	TemplateListView->ClearSelection();
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
		
		FSavePackageArgs saveArgs;
		saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		saveArgs.SaveFlags = SAVE_None;
		if (UPackage::SavePackage(package, itemTemplate, *fileName, saveArgs))
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
		newTemplate->ItemSpecialAffects = transientTemplate->ItemSpecialAffects;
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
			FSavePackageArgs saveArgs;
			saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			saveArgs.SaveFlags = SAVE_None;
			if (UPackage::SavePackage(package, newTemplate, *fileName, saveArgs))
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

FTemplateDisplayInfo SMounteaInventoryTemplateEditor::GenerateTemplateDisplayInfo(
	TWeakObjectPtr<UMounteaInventoryItemTemplate> Template,
	const TSet<TWeakObjectPtr<UMounteaInventoryItemTemplate>>& AllDirtyTemplates)
{
	FTemplateDisplayInfo returnInfo;

	if (!Template.IsValid())
		return returnInfo;

	if (UMounteaInventoryItemTemplate* TemplatePtr = Template.Get())
	{
		returnInfo.bIsTransient = TemplatePtr->HasAnyFlags(RF_Transient);
		returnInfo.bIsDirty = AllDirtyTemplates.Contains(TemplatePtr);

		const FText baseName = TemplatePtr->DisplayName.IsEmpty()
			? FText::FromString(TemplatePtr->GetName())
			: TemplatePtr->DisplayName;

		returnInfo.FullText = baseName;

		FText displayName = baseName;
		if (returnInfo.bIsTransient)
			displayName = FText::Format(LOCTEXT("TransientTemplate", "{0} *"), baseName);
		else if (returnInfo.bIsDirty)
			displayName = FText::Format(LOCTEXT("DirtyTemplate", "{0} *"), baseName);

		const FString truncatedName = displayName.ToString();
		returnInfo.DisplayText = FText::FromString(
			truncatedName.Len() > 32 ? truncatedName.Left(32) + TEXT("...") : truncatedName
		);

		if (UPackage* assetPackage = TemplatePtr->GetPackage())
			returnInfo.AssetPath = returnInfo.bIsTransient ? TEXT("Unsaved") : assetPackage->GetName();

		returnInfo.TooltipText = FText::Format(
			LOCTEXT("TemplateTooltip", "Name: {0}\nPath: {1}\nCategory: {2}\nRarity: {3}\nStatus: {4}"),
			baseName,
			FText::FromString(returnInfo.AssetPath),
			FText::FromString(TemplatePtr->ItemCategory),
			FText::FromString(TemplatePtr->ItemRarity),
			returnInfo.bIsTransient
				? LOCTEXT("UnsavedStatus", "Unsaved")
				: (returnInfo.bIsDirty
					? LOCTEXT("ModifiedStatus", "Modified")
					: LOCTEXT("SavedStatus", "Saved"))
		);
	}

	return returnInfo;
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
			.IsEnabled_Lambda([this]() { 
				if (bIsShowingTransient && CurrentTemplate.IsValid() && CurrentTemplate.Get()->HasAnyFlags(RF_Transient))
					return true;
				if (CurrentTemplate.IsValid() && DirtyTemplates.Contains(CurrentTemplate))
					return true;
				UMounteaInventoryTemplateEditorSubsystem* subsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>();
				return subsystem && subsystem->HasTempTemplate() && DirtyTemplates.Contains(subsystem->GetOrCreateTempTemplate());
			})
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
	const FTemplateDisplayInfo Info = GenerateTemplateDisplayInfo(Template, DirtyTemplates);

	FText displayText = Info.DisplayText;
	FText fullText = Info.FullText;
	FString assetPath = Info.AssetPath;
	FText tooltipText = Info.TooltipText;
	bool bIsTransient = Info.bIsTransient;
	bool bIsDirty = Info.bIsDirty;

	const auto foregroundColor = FSlateColor::UseForeground();
	
	return SNew(STableRow<TWeakObjectPtr<UMounteaInventoryItemTemplate>>, OwnerTable)
		.Padding(2.f)
	//.Style(&FMounteaAdvancedInventoryEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("MAISStyleSet.TemplateTableRow"))
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
				.Text(TAttribute<FText>::Create(
					TAttribute<FText>::FGetter::CreateLambda([this, Template]() -> FText
					{
						const FTemplateDisplayInfo displayInfo = GenerateTemplateDisplayInfo(Template, DirtyTemplates);
						return displayInfo.DisplayText;
					})
				))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
				.ToolTipText(TAttribute<FText>::Create(
					TAttribute<FText>::FGetter::CreateLambda([this, Template]() -> FText
					{
						const FTemplateDisplayInfo displayInfo = GenerateTemplateDisplayInfo(Template, DirtyTemplates);
						return displayInfo.TooltipText;
					})
				))
				.ColorAndOpacity(TAttribute<FSlateColor>::Create(TAttribute<FSlateColor>::FGetter::CreateLambda([this, Template, foregroundColor]() 
				{
					if (Template.IsValid())
					{
						UMounteaInventoryItemTemplate* templatePtr = Template.Get();
						if (templatePtr->HasAnyFlags(RF_Transient)) 
							return FSlateColor(FLinearColor(1.f, 1.f, 0.5f));
						if (DirtyTemplates.Contains(Template)) 
							return FSlateColor(FLinearColor(1.f, 0.7f, 0.7f));
					}
					return foregroundColor;
				})))
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
		CreateTransientTemplate();
		TemplateListView->ClearSelection();
		return true;
	}
	
	// Cancel
	return false;
}

#undef LOCTEXT_NAMESPACE