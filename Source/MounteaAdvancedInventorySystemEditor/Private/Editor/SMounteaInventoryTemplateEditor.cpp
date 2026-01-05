// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

// SMounteaInventoryTemplateEditor.cpp

#include "Editor/SMounteaInventoryTemplateEditor.h"

#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Views/STreeView.h"
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
#include "Decorations/MounteaInventoryItemAction.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#include "Subsystems/MounteaInventoryTemplateEditorSubsystem.h"

#include "AssetToolsModule.h"
#include "DesktopPlatformModule.h"
#include "MounteaItemTemplatesEditorHelp.h"
#include "Algo/ForEach.h"
#include "Search/MounteaInventoryTemplateSearchFilter.h"
#include "Settings/EditorStyleSettings.h"
#include "Settings/MounteaAdvancedInventorySettingsEditor.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "UObject/SavePackage.h"

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateEditor"

SMounteaInventoryTemplateEditor::~SMounteaInventoryTemplateEditor()
{
	CleanupTransientTemplate();
	
	if (UMounteaInventoryTemplateEditorSubsystem* editorTemplateSubsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>())
		editorTemplateSubsystem->OnTemplatesChanged().RemoveAll(this);
}

void SMounteaInventoryTemplateEditor::Construct(const FArguments& InArgs)
{
	OnTemplateChanged = InArgs._OnTemplateChanged;
	
	ChildSlot
	[
		SAssignNew(RootOverlay, SOverlay)
		
		+ SOverlay::Slot()
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MinHeight(36.f)
			[
				CreateToolbar()
			]
			
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				CreatePropertyMatrix()
			]
		]		
	];
	
	CreateTransientTemplate();
	RefreshTemplateList();
	OnTreeSelectionChanged(nullptr, ESelectInfo::Direct);
	
	if (UMounteaInventoryTemplateEditorSubsystem* editorTemplateSubsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>())
		editorTemplateSubsystem->OnTemplatesChanged().AddRaw(this, &SMounteaInventoryTemplateEditor::RefreshTemplateList);
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

		if (TemplateTreeView.IsValid())
			TemplateTreeView->RequestTreeRefresh();
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
		[](const FAssetData& assetData)
		{
			const auto* item = Cast<UMounteaInventoryItemTemplate>(assetData.GetAsset());
			return item && !item->HasAnyFlags(RF_Transient);
		},
		[](const FAssetData& assetData)
		{
			return Cast<UMounteaInventoryItemTemplate>(assetData.GetAsset());
		}
	);

	allTemplates.Append(filteredTemplates);
	
	allTemplates.Sort([](const UObject& A, const UObject& B)
	{
		const UMounteaInventoryItemTemplate* templateA = Cast<UMounteaInventoryItemTemplate>(&A);
		const UMounteaInventoryItemTemplate* templateB = Cast<UMounteaInventoryItemTemplate>(&B);
		
		if (!templateA || !templateB)
			return templateA != nullptr;
		
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
	
	AvailableTemplates.Empty();
	Algo::TransformIf(
		allTemplates,
		AvailableTemplates,
		[](UObject* obj)
		{
			auto* itemTemplate = Cast<UMounteaInventoryItemTemplate>(obj);
			return itemTemplate && !itemTemplate->HasAnyFlags(RF_Transient);
		},
		[](UObject* obj)
		{
			return TWeakObjectPtr<UMounteaInventoryItemTemplate>(Cast<UMounteaInventoryItemTemplate>(obj));
		}
	);
	
	RebuildTreeStructure();
	ApplySearchFilter();
	
	if (TemplateTreeView.IsValid())
		TemplateTreeView->RequestTreeRefresh();
}

void SMounteaInventoryTemplateEditor::RebuildTreeStructure()
{
	TreeRootItems.Empty();
	
	TMap<FString, TSharedPtr<FTemplateTreeItem>> categoryMap;
	
	for (const TWeakObjectPtr<UMounteaInventoryItemTemplate>& templatePtr : AvailableTemplates)
	{
		if (!templatePtr.IsValid())
			continue;
		
		const FString category = templatePtr->ItemCategory;
		
		if (!categoryMap.Contains(category))
		{
			TSharedPtr<FTemplateTreeItem> categoryItem = FTemplateTreeItem::MakeCategory(category);
			categoryMap.Add(category, categoryItem);
			TreeRootItems.Add(categoryItem);
		}
		
		TSharedPtr<FTemplateTreeItem> templateItem = FTemplateTreeItem::MakeTemplate(templatePtr);
		categoryMap[category]->Children.Add(templateItem);
	}
	
	TreeRootItems.Sort([](const TSharedPtr<FTemplateTreeItem>& A, const TSharedPtr<FTemplateTreeItem>& B)
	{
		return A->CategoryName < B->CategoryName;
	});
	
	if (TemplateTreeView.IsValid())
	{
		for (const TSharedPtr<FTemplateTreeItem>& categoryItem : TreeRootItems)
		{
			if (ExpandedCategories.Contains(categoryItem->CategoryName))
				TemplateTreeView->SetItemExpansion(categoryItem, true);
		}
	}
}

void SMounteaInventoryTemplateEditor::GetChildrenForTree(TSharedPtr<FTemplateTreeItem> Item,
	TArray<TSharedPtr<FTemplateTreeItem>>& OutChildren)
{
	if (Item.IsValid() && Item->Type == ETemplateTreeItemType::Category)
		OutChildren = Item->Children;
}

TSharedPtr<FTemplateTreeItem> SMounteaInventoryTemplateEditor::FindTreeItemForTemplate(
	const TWeakObjectPtr<UMounteaInventoryItemTemplate>& Template) const
{
	if (!Template.IsValid())
		return nullptr;
	
	for (const TSharedPtr<FTemplateTreeItem>& rootItem : FilteredTreeRootItems)
	{
		if (!rootItem.IsValid() || rootItem->Type != ETemplateTreeItemType::Category)
			continue;
		
		for (const TSharedPtr<FTemplateTreeItem>& childItem : rootItem->Children)
		{
			if (childItem.IsValid() && childItem->Template == Template)
				return childItem;
		}
	}
	
	return nullptr;
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreatePropertyMatrix()
{
	TArray<UObject*> allTemplates = LoadAllTemplatesForMatrix();
	
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
	
	RebuildTreeStructure();
	
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
			.BorderImage(FAppStyle::GetBrush("Brushes.Background"))
			[
				SAssignNew(TemplateTreeView, STreeView<TSharedPtr<FTemplateTreeItem>>)
				.TreeItemsSource(&FilteredTreeRootItems)
				.OnGenerateRow(this, &SMounteaInventoryTemplateEditor::GenerateTemplateTreeRow)
				.OnGetChildren(this, &SMounteaInventoryTemplateEditor::GetChildrenForTree)
				.OnSelectionChanged(this, &SMounteaInventoryTemplateEditor::OnTreeSelectionChanged)
				.SelectionMode(ESelectionMode::Multi)
				.OnExpansionChanged_Lambda([this](TSharedPtr<FTemplateTreeItem> item, bool bExpanded)
				{
					if (item.IsValid() && item->Type == ETemplateTreeItemType::Category)
					{
						if (bExpanded)
							ExpandedCategories.Add(item->CategoryName);
						else
							ExpandedCategories.Remove(item->CategoryName);
					}
				})
			]
		]
		
		+ SSplitter::Slot()
		.Value(0.7f)
		[
			PropertyDetailsView.ToSharedRef()
		];
}

void SMounteaInventoryTemplateEditor::OnTreeSelectionChanged(TSharedPtr<FTemplateTreeItem> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (!PropertyDetailsView.IsValid())
		return;

	const bool bHasTransient = bIsShowingTransient && CurrentTemplate.IsValid() && CurrentTemplate->HasAnyFlags(RF_Transient);
	const bool bIsDirty = DirtyTemplates.Contains(CurrentTemplate);

	if (bHasTransient && bIsDirty && !CheckForUnsavedChanges())
	{
		if (SelectedItem.IsValid())
			TemplateTreeView->SetSelection(SelectedItem, ESelectInfo::Direct);
		return;
	}

	TArray<TSharedPtr<FTemplateTreeItem>> allSelectedItems = TemplateTreeView->GetSelectedItems();
	
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> selectedTemplates;
	for (const TSharedPtr<FTemplateTreeItem>& item : allSelectedItems)
	{
		if (item.IsValid() && item->Type == ETemplateTreeItemType::Template && item->Template.IsValid())
			selectedTemplates.Add(item->Template);
	}

	TArray<UObject*> selectedObjects;
	bIsShowingTransient = false;
	
	Algo::TransformIf(
		selectedTemplates,
		selectedObjects,
		[this](const TWeakObjectPtr<UMounteaInventoryItemTemplate>& weakTemplate)
		{
			auto* templatePtr = weakTemplate.Get();
			if (templatePtr && templatePtr->HasAnyFlags(RF_Transient))
				bIsShowingTransient = true;
			return templatePtr != nullptr;
		},
		[](const TWeakObjectPtr<UMounteaInventoryItemTemplate>& weakTemplate)
		{
			return static_cast<UObject*>(weakTemplate.Get());
		}
	);

	if (selectedTemplates.Num() == 1)
		CurrentTemplate = selectedTemplates[0];
	else
		CurrentTemplate = nullptr;

	if (selectedObjects.IsEmpty())
	{
		UMounteaInventoryTemplateEditorSubsystem* subsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>();
		if (subsystem && subsystem->HasTempTemplate())
		{
			UObject* tempObject = subsystem->GetOrCreateTempTemplate();
			selectedObjects.Add(tempObject);
			CurrentTemplate = Cast<UMounteaInventoryItemTemplate>(tempObject);
			bIsShowingTransient = true;
		}
	}
	
	PropertyDetailsView->SetObjects(selectedObjects);
}

FReply SMounteaInventoryTemplateEditor::OnCreateNewTemplate()
{
	if (bIsShowingTransient && CurrentTemplate.IsValid() && DirtyTemplates.Contains(CurrentTemplate))
	{
		if (!CheckForUnsavedChanges())
			return FReply::Handled();
	}
	
	CreateTransientTemplate();
	RefreshTemplateList();

	TemplateTreeView->ClearSelection();
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

	TemplateTreeView->ClearSelection();
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

FReply SMounteaInventoryTemplateEditor::ShowHelpModal()
{
	if (HelpWidget.IsValid())
		return FReply::Handled();
	
	RootOverlay->AddSlot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SAssignNew(HelpModalContent, SBox)
		[
			SNew(SOverlay)
			
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.75f))
			]
			
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride_Lambda([this]()
				{
					const FVector2D parentSize = GetCachedGeometry().GetLocalSize();
					return parentSize.X * 0.8f;
				})
				.HeightOverride_Lambda([this]()
				{
					const FVector2D parentSize = GetCachedGeometry().GetLocalSize();
					return parentSize.Y * 0.8f;
				})
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
					.BorderBackgroundColor(FAppStyle::GetSlateColor("DefaultForeground"))
					.Padding(0)
					[
						SNew(SOverlay)
						
						+ SOverlay::Slot()
						[
							SAssignNew(HelpWidget, SMounteaItemTemplatesEditorHelp)
						]
						
						+ SOverlay::Slot()
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Top)
						.Padding(0)
						[
							SNew(SButton)
							.ButtonStyle(FAppStyle::Get(), "SimpleButton")
							.ContentPadding(FMargin(2))
							.RenderTransformPivot(FVector2f(1.f, 1.f))
							.RenderTransform(FSlateRenderTransform(FVector2f(25.f, 0.f)))
							.OnClicked(this, &SMounteaInventoryTemplateEditor::OnCloseHelp)
							[
								SNew(SImage)
								.Image(FAppStyle::GetBrush("MAISStyleSet.Close"))
								.ColorAndOpacity(FSlateColor::UseForeground())
								.DesiredSizeOverride(FVector2D(16, 16))
							]
						]
					]
				]
			]
		]
	];
	
	HelpWidget->Start();
	return FReply::Handled();
}

FReply SMounteaInventoryTemplateEditor::OnCloseHelp()
{
	if (RootOverlay.IsValid() && HelpModalContent.IsValid())
	{
		HelpWidget->Reset();
		HelpWidget.Reset();
		RootOverlay->RemoveSlot(HelpModalContent.ToSharedRef());
	}	
	return FReply::Handled();
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
		
		if (UPackage* package = newTemplate->GetPackage())
		{
			FString fileName = FPackageName::LongPackageNameToFilename(package->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs saveArgs;
			saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			saveArgs.SaveFlags = SAVE_None;
			if (UPackage::SavePackage(package, newTemplate, *fileName, saveArgs))
			{
				UntrackDirtyAsset(transientTemplate);
				CleanupTransientTemplate();
				
				RefreshTemplateList();
				
				TSharedPtr<FTemplateTreeItem> foundItem = FindTreeItemForTemplate(newTemplate);
				if (foundItem.IsValid())
				{
					TSharedPtr<FTemplateTreeItem> parentCategory;
					for (const TSharedPtr<FTemplateTreeItem>& rootItem : FilteredTreeRootItems)
					{
						if (rootItem.IsValid() && rootItem->Children.Contains(foundItem))
						{
							parentCategory = rootItem;
							break;
						}
					}
					
					if (parentCategory.IsValid())
						TemplateTreeView->SetItemExpansion(parentCategory, true);
					
					TemplateTreeView->SetSelection(foundItem, ESelectInfo::Direct);
				}
				
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
	{
		config.DialogTitleOverride = LOCTEXT("SaveInventoryTemplateDialogTitle", "Save Inventory Template");
		config.DefaultPath = TEXT("/Game");
		config.DefaultAssetName = TEXT("NewInventoryTemplate");
		config.AssetClassNames.Add(UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName());
		config.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;
	}
    
	return contentBrowserModule.Get().CreateModalSaveAssetDialog(config);
}

FTemplateDisplayInfo SMounteaInventoryTemplateEditor::GenerateTemplateDisplayInfo(
	const TWeakObjectPtr<UMounteaInventoryItemTemplate> Template,
	const TSet<TWeakObjectPtr<UMounteaInventoryItemTemplate>>& AllDirtyTemplates)
{
	FTemplateDisplayInfo returnInfo;

	if (!Template.IsValid())
		return returnInfo;

	if (UMounteaInventoryItemTemplate* templatePtr = Template.Get())
	{
		returnInfo.bIsTransient = templatePtr->HasAnyFlags(RF_Transient);
		returnInfo.bIsDirty = AllDirtyTemplates.Contains(templatePtr);

		const FText baseName = templatePtr->DisplayName.IsEmpty()
			? FText::FromString(templatePtr->GetName())
			: templatePtr->DisplayName;

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

		if (UPackage* assetPackage = templatePtr->GetPackage())
			returnInfo.AssetPath = returnInfo.bIsTransient ? TEXT("Unsaved") : assetPackage->GetName();

		returnInfo.TooltipText = FText::Format(
			LOCTEXT("TemplateTooltip", "Name: {0}\nPath: {1}\nCategory: {2}\nRarity: {3}\nStatus: {4}"),
			baseName,
			FText::FromString(returnInfo.AssetPath),
			FText::FromString(templatePtr->ItemCategory),
			FText::FromString(templatePtr->ItemRarity),
			returnInfo.bIsTransient
				? LOCTEXT("UnsavedStatus", "Unsaved")
				: (returnInfo.bIsDirty
					? LOCTEXT("ModifiedStatus", "Modified")
					: LOCTEXT("SavedStatus", "Saved"))
		);
	}

	return returnInfo;
}

void SMounteaInventoryTemplateEditor::OnSearchTextChanged(const FText& InSearchText)
{
	ApplySearchFilter();
}

void SMounteaInventoryTemplateEditor::OnFiltersChanged()
{
	ApplySearchFilter();
}

bool SMounteaInventoryTemplateEditor::PassesFilters(const TWeakObjectPtr<UMounteaInventoryItemTemplate> Template) const
{
	if (!Template.IsValid() || !SearchFilterWidget.IsValid())
		return false;
	
	return SearchFilterWidget->GetActiveFilters().PassesFilter(
		DirtyTemplates.Contains(Template), Template->ItemCategory, Template->ItemRarity
	);
}

void SMounteaInventoryTemplateEditor::ApplySearchFilter()
{
	FilteredTreeRootItems.Empty();
	
	if (!SearchFilterWidget.IsValid())
	{
		FilteredTreeRootItems = TreeRootItems;
		if (TemplateTreeView.IsValid())
			TemplateTreeView->RequestTreeRefresh();
		return;
	}
	
	for (const TSharedPtr<FTemplateTreeItem>& categoryItem : TreeRootItems)
	{
		if (!categoryItem.IsValid() || categoryItem->Type != ETemplateTreeItemType::Category)
			continue;
		
		TSharedPtr<FTemplateTreeItem> filteredCategory = FTemplateTreeItem::MakeCategory(categoryItem->CategoryName);
		
		for (const TSharedPtr<FTemplateTreeItem>& templateItem : categoryItem->Children)
		{
			if (!templateItem.IsValid() || !templateItem->Template.IsValid())
				continue;
			
			if (!PassesFilters(templateItem->Template))
				continue;
			
			if (!SearchFilterWidget->DoesTemplateMatchSearch(templateItem->Template))
				continue;
			
			filteredCategory->Children.Add(templateItem);
		}
		
		if (filteredCategory->Children.Num() > 0)
			FilteredTreeRootItems.Add(filteredCategory);
	}
	
	if (TemplateTreeView.IsValid())
		TemplateTreeView->RequestTreeRefresh();
}

FReply SMounteaInventoryTemplateEditor::DeleteTemplate(const TWeakObjectPtr<UMounteaInventoryItemTemplate> Template)
{
	if (!Template.IsValid())
		return FReply::Unhandled();
	
	UMounteaInventoryItemTemplate* itemTemplate = Template.Get();
	
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
		UntrackDirtyAsset(itemTemplate);
		
		if (UEditorAssetLibrary::DeleteAsset(itemTemplate->GetPathName()))
		{
			RefreshTemplateList();
			ShowTemplateEditorNotification(TEXT("Template deleted successfully."), true);
		}
		else
			ShowTemplateEditorNotification(TEXT("Failed to delete template."), false);
	}
	
	return FReply::Handled();
}

FReply SMounteaInventoryTemplateEditor::DuplicateTemplate(const TWeakObjectPtr<UMounteaInventoryItemTemplate> Template)
{
	if (!Template.IsValid())
		return FReply::Unhandled();
	
	UMounteaInventoryItemTemplate* sourceTemplate = Template.Get();
	
	FString packagePath = FPackageName::GetLongPackagePath(sourceTemplate->GetPackage()->GetName());
	FString assetName = sourceTemplate->GetName() + TEXT("_Copy");
	
	FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	UObject* duplicatedAsset = assetToolsModule.Get().DuplicateAsset(assetName, packagePath, sourceTemplate);
	TWeakObjectPtr<UMounteaInventoryItemTemplate> duplicatedTemplate = Cast<UMounteaInventoryItemTemplate>(duplicatedAsset);
	
	if (duplicatedAsset)
	{
		RefreshTemplateList();
		
		TSharedPtr<FTemplateTreeItem> foundItem = FindTreeItemForTemplate(duplicatedTemplate);
		if (foundItem.IsValid())
		{
			TSharedPtr<FTemplateTreeItem> parentCategory;
			for (const TSharedPtr<FTemplateTreeItem>& rootItem : FilteredTreeRootItems)
			{
				if (rootItem.IsValid() && rootItem->Children.Contains(foundItem))
				{
					parentCategory = rootItem;
					break;
				}
			}
			
			if (parentCategory.IsValid())
				TemplateTreeView->SetItemExpansion(parentCategory, true);
			
			TemplateTreeView->SetSelection(foundItem, ESelectInfo::Direct);
		}
		
		ShowTemplateEditorNotification(TEXT("Template duplicated successfully."), true);
	}
	else
		ShowTemplateEditorNotification(TEXT("Failed to duplicate template."), false);
	
	return FReply::Handled();
}

void SMounteaInventoryTemplateEditor::ImportTemplate()
{
	TArray<UMounteaInventoryItemTemplate*> importedTemplates;
	FString errorMessage;
    
	if (UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFile(importedTemplates, errorMessage))
	{
		ShowTemplateEditorNotification(errorMessage, true);
		RefreshTemplateList();
	}
	else
	{
		ShowTemplateEditorNotification(errorMessage, false);
	}
}

void SMounteaInventoryTemplateEditor::ExportTemplate()
{
	TArray<TSharedPtr<FTemplateTreeItem>> selectedItems = TemplateTreeView->GetSelectedItems();
	
	TArray<UMounteaInventoryItemTemplate*> templatesToExport;
	for (const TSharedPtr<FTemplateTreeItem>& item : selectedItems)
	{
		if (item.IsValid() && item->Type == ETemplateTreeItemType::Template && item->Template.IsValid())
			templatesToExport.Add(item->Template.Get());
	}

	FString errorMessage;
	if (UMounteaAdvancedInventoryItemTemplateEditorStatics::ExportTemplatesToFile(templatesToExport, errorMessage))
	{
		const FString tempMessage = FString::Printf(
			TEXT("Successfully exported %d template%s"),
			templatesToExport.Num(),
			templatesToExport.Num() > 1 ? TEXT("s") : TEXT("")
		);
		ShowTemplateEditorNotification(tempMessage, true);
	}
	else
		ShowTemplateEditorNotification(errorMessage, false);
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateToolbar()
{
	return 
	SNew(SBorder)
	.VAlign(VAlign_Center)
	.BorderImage(FAppStyle::Get().GetBrush("AssetEditorToolbar.Background"))
	.Padding(4.f, 4.f, 0.f, 4.f)
	[
		SNew(SHorizontalBox)
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground())
			.ToolTipText(LOCTEXT("NewTemplateTooltip", "Create new empty template"))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::OnCreateNewTemplate)
			.IsEnabled_Lambda([this]() 
			{ 
				return true;
			})
			[
				SNew(SHorizontalBox)
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("Icons.Plus"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("NewTemplate", "New Template"))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([this]()
					{
						const uint8 bMounteaAllowed = GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText;
						if (!bMounteaAllowed)
							return EVisibility::Collapsed;
						return GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
			]
		]
			
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground())
			.ToolTipText(LOCTEXT("SaveTemplateTooltip", "Save the current template"))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::SaveTemplate)
			.IsEnabled_Lambda([this]() 
			{ 
				if (bIsShowingTransient && CurrentTemplate.IsValid() && CurrentTemplate.Get()->HasAnyFlags(RF_Transient))
					return true;
				if (CurrentTemplate.IsValid() && DirtyTemplates.Contains(CurrentTemplate))
					return true;
				UMounteaInventoryTemplateEditorSubsystem* subsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>();
				return subsystem && subsystem->HasTempTemplate() && DirtyTemplates.Contains(subsystem->GetOrCreateTempTemplate());
			})
			[
				SNew(SHorizontalBox)
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("Icons.Save"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SaveTemplate", "Save Template"))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([this]()
					{
						const uint8 bMounteaAllowed = GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText;
						if (!bMounteaAllowed)
							return EVisibility::Collapsed;
						return GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
			]
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground())
			.ToolTipText(LOCTEXT("SaveTemplateTooltip", "Save all unsaved templates."))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::SaveAllDirtyTemplates)
			.IsEnabled_Lambda([this]() { return DirtyTemplates.Num() > 0; })
			[
				SNew(SHorizontalBox)
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("MAISStyleSet.SaveMultiple"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SaveTemplates", "Save All Templates"))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([this]()
					{
						const uint8 bMounteaAllowed = GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText;
						if (!bMounteaAllowed)
							return EVisibility::Collapsed;
						return GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(8.0f, 4.0f)
		[
			SNew(SSeparator)
			.SeparatorImage(FAppStyle::Get().GetBrush("Separator"))
			.Thickness(1.0f)
			.Orientation(Orient_Vertical)
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground())
			.ToolTipText(LOCTEXT("ImportTemplateTooltip", "Import single or multiple item templates from source files."))
			.OnClicked_Lambda([this]() { ImportTemplate(); return FReply::Handled(); })
			.IsEnabled_Lambda([this]() 
			{ 
				return true;
			})
			[
				SNew(SHorizontalBox)
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("MAISStyleSet.Import"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ImportTemplate", "Import Template"))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([this]()
					{
						const uint8 bMounteaAllowed = GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText;
						if (!bMounteaAllowed)
							return EVisibility::Collapsed;
						return GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
			]
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground())
			.ToolTipText(LOCTEXT("ExportTemplateTooltip", "Export all selected item templates to new source files."))
			.OnClicked_Lambda([this]() { ExportTemplate(); return FReply::Handled(); })
			.IsEnabled_Lambda([this]() 
			{ 
				return true;
			})
			[
				SNew(SHorizontalBox)
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("MAISStyleSet.Export"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ExportTemplate", "Export Item"))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([this]()
					{
						const uint8 bMounteaAllowed = GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText;
						if (!bMounteaAllowed)
							return EVisibility::Collapsed;
						return GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(8.0f, 4.0f)
		[
			SNew(SSeparator)
			.SeparatorImage(FAppStyle::Get().GetBrush("Separator"))
			.Thickness(1.0f)
			.Orientation(Orient_Vertical)
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground())
			.ToolTipText(LOCTEXT("CloseTemplateTooltip", "Close current template and return to blank canvas."))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::CloseTemplate)
			.IsEnabled_Lambda([this]() 
			{ 
				return true;
			})
			[
				SNew(SHorizontalBox)
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("MAISStyleSet.Close"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("CloseTemplate", "Close Template"))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([this]()
					{
						const uint8 bMounteaAllowed = GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText;
						if (!bMounteaAllowed)
							return EVisibility::Collapsed;
						return GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(8.0f, 4.0f)
		[
			SNew(SSeparator)
			.SeparatorImage(FAppStyle::Get().GetBrush("Separator"))
			.Thickness(1.0f)
			.Orientation(Orient_Vertical)
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor::UseForeground())
			.ToolTipText(LOCTEXT("HelpTooltip", "Show a quick help window explaining how to work with the Item Template Editor."))
			.OnClicked(this, &SMounteaInventoryTemplateEditor::ShowHelpModal)
			.IsEnabled_Lambda([this]() 
			{ 
				return true;
			})
			[
				SNew(SHorizontalBox)
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("MAISStyleSet.Help"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Help", "Show Help"))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Visibility_Lambda([this]()
					{
						const uint8 bMounteaAllowed = GetDefault<UMounteaAdvancedInventorySettingsEditor>()->bDisplayEditorButtonText;
						if (!bMounteaAllowed)
							return EVisibility::Collapsed;
						return GetDefault<UEditorStyleSettings>()->bUseSmallToolBarIcons ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(8.0f, 4.0f)
		[
			SNew(SSeparator)
			.SeparatorImage(FAppStyle::Get().GetBrush("Separator"))
			.Thickness(1.0f)
			.Orientation(Orient_Vertical)
		]
		
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNullWidget::NullWidget
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SBox)
			.WidthOverride(300.0f)
			[
				SAssignNew(SearchFilterWidget, SMounteaInventoryTemplateSearchFilter)
				.OnSearchTextChanged(this, &SMounteaInventoryTemplateEditor::OnSearchTextChanged)
				.OnFiltersChanged(this, &SMounteaInventoryTemplateEditor::OnFiltersChanged)
			]
		]
	];
}

TSharedRef<ITableRow> SMounteaInventoryTemplateEditor::GenerateTemplateTreeRow(TSharedPtr<FTemplateTreeItem> Item,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Item.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FTemplateTreeItem>>, OwnerTable)
			[
				SNullWidget::NullWidget
			];
	}
	
	if (Item->Type == ETemplateTreeItemType::Category)
	{
		return SNew(STableRow<TSharedPtr<FTemplateTreeItem>>, OwnerTable)
			.Style(FAppStyle::Get(), "TableView.NoHoverTableRow")
			.Padding(FMargin(0, 4, 0, 2))
			.ShowSelection(false)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop"))
				.BorderBackgroundColor(FLinearColor::Transparent)
				.Padding(FMargin(6, 4))
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Item->CategoryName).ToUpper())
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
				]
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(6, 0, 0, 0)
				[
					SNew(STextBlock)
					.Text_Lambda([Item]()
					{
						return FText::AsNumber(Item->Children.Num());
					})
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				]
			]
		];
	}
	
	const TWeakObjectPtr<UMounteaInventoryItemTemplate> templatePtr = Item->Template;
	const FTemplateDisplayInfo templateInfo = GenerateTemplateDisplayInfo(templatePtr, DirtyTemplates);
	const auto foregroundColor = FSlateColor::UseForeground();
	
	struct FButtonHolder { TWeakPtr<SButton> Button; };
	TSharedRef<FButtonHolder> buttonHolder = MakeShared<FButtonHolder>();
	
	TSharedPtr<SButton> interactionButton;
	
	auto row = SNew(STableRow<TSharedPtr<FTemplateTreeItem>>, OwnerTable)
		.Style(FAppStyle::Get(), "TableView.NoHoverTableRow")
		.Padding(FMargin(4, 2))
		.ShowWires(true)
		.ShowSelection(false)
		[
			SAssignNew(interactionButton, SButton)
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.OnClicked_Lambda([this, Item]() -> FReply
			{
				if (TemplateTreeView.IsValid() && Item.IsValid() && Item->Type == ETemplateTreeItemType::Template)
				{
					const FModifierKeysState modifierKeys = FSlateApplication::Get().GetModifierKeys();
					
					if (modifierKeys.IsControlDown())
						TemplateTreeView->SetItemSelection(Item, !TemplateTreeView->IsItemSelected(Item), ESelectInfo::OnMouseClick);
					else if (modifierKeys.IsShiftDown())
					{
						TArray<TSharedPtr<FTemplateTreeItem>> selected = TemplateTreeView->GetSelectedItems();
						if (selected.Num() > 0)
						{
							TSharedPtr<FTemplateTreeItem> lastSelected = selected.Last();
							if (lastSelected.IsValid() && lastSelected->Type == ETemplateTreeItemType::Template)
							{
								TSharedPtr<FTemplateTreeItem> parentCategory;
								for (const TSharedPtr<FTemplateTreeItem>& rootItem : FilteredTreeRootItems)
								{
									if (rootItem->Children.Contains(lastSelected) && rootItem->Children.Contains(Item))
									{
										parentCategory = rootItem;
										break;
									}
								}
								
								if (parentCategory.IsValid())
								{
									int32 lastIndex = parentCategory->Children.Find(lastSelected);
									int32 currentIndex = parentCategory->Children.Find(Item);
									
									if (lastIndex != INDEX_NONE && currentIndex != INDEX_NONE)
									{
										int32 startIndex = FMath::Min(lastIndex, currentIndex);
										int32 endIndex = FMath::Max(lastIndex, currentIndex);
										
										TemplateTreeView->ClearSelection();
										for (int32 i = startIndex; i <= endIndex; ++i)
										{
											TemplateTreeView->SetItemSelection(parentCategory->Children[i], true, ESelectInfo::OnMouseClick);
										}
									}
								}
							}
						}
						else
							TemplateTreeView->SetSelection(Item, ESelectInfo::OnMouseClick);
					}
					else
						TemplateTreeView->SetSelection(Item, ESelectInfo::OnMouseClick);
				}
				return FReply::Handled();
			})
			.ContentPadding(0)
			.IsFocusable(false)
			[
				SNew(SBorder)
				.BorderImage(TAttribute<const FSlateBrush*>::Create([this, Item, buttonHolder]() -> const FSlateBrush*
				{
					bool bSelected = TemplateTreeView.IsValid() && TemplateTreeView->GetSelectedItems().Contains(Item);
					bool bHovered = false;
					
					if (TSharedPtr<SButton> button = buttonHolder->Button.Pin())
						bHovered = button->IsHovered();
					
					if (bSelected && bHovered)
						return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.SelectedHovered");
					if (bSelected)
						return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.Selected");
					if (bHovered)
						return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.Hovered");
					
					return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.Normal");
				}))
				.Padding(FMargin(8, 6))
				[
					SNew(SHorizontalBox)
					
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SVerticalBox)
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(TAttribute<FText>::Create(
								TAttribute<FText>::FGetter::CreateLambda([this, templatePtr]()
								{
									const FTemplateDisplayInfo displayInfo = GenerateTemplateDisplayInfo(templatePtr, DirtyTemplates);
									return displayInfo.DisplayText;
								})
							))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
							.ToolTipText(TAttribute<FText>::Create(
								TAttribute<FText>::FGetter::CreateLambda([this, templatePtr]()
								{
									const FTemplateDisplayInfo displayInfo = GenerateTemplateDisplayInfo(templatePtr, DirtyTemplates);
									return displayInfo.TooltipText;
								})
							))
							.ColorAndOpacity(TAttribute<FSlateColor>::Create(TAttribute<FSlateColor>::FGetter::CreateLambda([this, templatePtr, foregroundColor]() 
							{
								if (templatePtr.IsValid())
								{
									UMounteaInventoryItemTemplate* itemTemplate = templatePtr.Get();
									if (itemTemplate->HasAnyFlags(RF_Transient)) 
										return FSlateColor(FLinearColor(1.f, 1.f, 0.5f));
									if (DirtyTemplates.Contains(templatePtr)) 
										return FSlateColor(FLinearColor(1.f, 0.7f, 0.7f));
								}
								return foregroundColor;
							})))
						]
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 2, 0, 0)
						[
							SNew(STextBlock)
							.Text(FText::FromString(templateInfo.AssetPath))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
							.ColorAndOpacity(FSlateColor::UseSubduedForeground())
						]
					]
					
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(8, 0, 0, 0)
					[
						SNew(SHorizontalBox)
						
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2, 0)
						[
							SNew(SButton)
							.ButtonStyle(FAppStyle::Get(), "SimpleButton")
							.ContentPadding(FMargin(2))
							.ToolTipText(LOCTEXT("NavigateToFolderTooltip", "Navigate to folder in Content Browser"))
							.Visibility_Lambda([templateInfo]() { return templateInfo.bIsTransient ? EVisibility::Collapsed : EVisibility::Visible; })
							.OnClicked_Lambda([templatePtr]()
							{
								if (templatePtr.IsValid() && !templatePtr.Get()->HasAnyFlags(RF_Transient))
								{
									TArray<FAssetData> assetDataList;
									assetDataList.Add(FAssetData(templatePtr.Get()));
									
									FContentBrowserModule& contentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
									contentBrowserModule.Get().SyncBrowserToAssets(assetDataList);
								}
								return FReply::Handled();
							})
							[
								SNew(SImage)
								.Image(FAppStyle::GetBrush("Icons.FolderOpen"))
								.ColorAndOpacity(FSlateColor::UseForeground())
								.DesiredSizeOverride(FVector2D(16, 16))
							]
						]
						
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2, 0)
						[
							SNew(SButton)
							.ButtonStyle(FAppStyle::Get(), "SimpleButton")
							.ContentPadding(FMargin(2))
							.ToolTipText(LOCTEXT("DuplicateTemplateTooltip", "Duplicate this template"))
							.Visibility_Lambda([templateInfo]() { return templateInfo.bIsTransient ? EVisibility::Collapsed : EVisibility::Visible; })
							.OnClicked(this, &SMounteaInventoryTemplateEditor::DuplicateTemplate, templatePtr)
							[
								SNew(SImage)
								.Image(FAppStyle::GetBrush("Icons.Duplicate"))
								.ColorAndOpacity(FSlateColor::UseForeground())
								.DesiredSizeOverride(FVector2D(16, 16))
							]
						]
						
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2, 0)
						[
							SNew(SButton)
							.ButtonStyle(FAppStyle::Get(), "SimpleButton")
							.ContentPadding(FMargin(2))
							.ToolTipText(LOCTEXT("DeleteTemplateTooltip", "Delete this template"))
							.Visibility_Lambda([templateInfo]() { return templateInfo.bIsTransient ? EVisibility::Collapsed : EVisibility::Visible; })
							.OnClicked(this, &SMounteaInventoryTemplateEditor::DeleteTemplate, templatePtr)
							[
								SNew(SImage)
								.Image(FAppStyle::GetBrush("Icons.Delete"))
								.ColorAndOpacity(FSlateColor::UseForeground())
								.DesiredSizeOverride(FVector2D(16, 16))
							]
						]
					]
				]
			]
		];
	
	buttonHolder->Button = interactionButton;
	
	return row;
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

void SMounteaInventoryTemplateEditor::ShowTemplateEditorNotification(const FString& Message, const bool bSuccess)
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
		SaveAllDirtyTemplates();
		return true;
	}
	else if (result == EAppReturnType::No)
	{
		DirtyTemplates.Empty();
		CreateTransientTemplate();
		TemplateTreeView->ClearSelection();
		return true;
	}
	
	return false;
}

#undef LOCTEXT_NAMESPACE