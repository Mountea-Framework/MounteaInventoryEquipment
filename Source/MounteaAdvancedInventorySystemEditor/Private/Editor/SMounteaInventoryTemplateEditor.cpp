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
#include "Editor/MounteaInventoryEditorLayout.h"

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
#include "IContentBrowserSingleton.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#include "Subsystems/MounteaInventoryTemplateEditorSubsystem.h"

#include "AssetToolsModule.h"
#include "DesktopPlatformModule.h"
#include "MounteaItemTemplatesEditorHelp.h"
#include "ObjectTools.h"
#include "Algo/ForEach.h"
#include "Editor/STemplateTreeItemRow.h"
#include "Helpers/MounteaEditorNotificationHelpers.h"
#include "Search/MounteaInventoryTemplateSearchFilter.h"
#include "Settings/EditorStyleSettings.h"
#include "Settings/MounteaAdvancedInventorySettingsEditor.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "UObject/SavePackage.h"

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateEditor"

SMounteaInventoryTemplateEditor::~SMounteaInventoryTemplateEditor()
{
	UnbindAssetRegistry();
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
			MounteaInventoryEditorLayout::MakeLayout(CreateToolbar(), CreatePropertyMatrix())
		]		
	];
	
	CreateTransientTemplate();
	RefreshTemplateList();
	OnTreeSelectionChanged(nullptr, ESelectInfo::Direct);
	BindAssetRegistry();
	
	if (UMounteaInventoryTemplateEditorSubsystem* editorTemplateSubsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>())
		editorTemplateSubsystem->OnTemplatesChanged().AddRaw(this, &SMounteaInventoryTemplateEditor::RefreshTemplateList);
}

void SMounteaInventoryTemplateEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
		return;

	const int32 editedObjectsNumber = PropertyChangedEvent.GetNumObjectsBeingEdited();
	bool bAnyChanged = false;

	for (int32 i = 0; i < editedObjectsNumber; ++i)
	{
		const UObject* editedObjConst = PropertyChangedEvent.GetObjectBeingEdited(i);
		if (!editedObjConst)
			continue;

		UMounteaInventoryItemTemplate* itemTemplate = const_cast<UMounteaInventoryItemTemplate*>(Cast<UMounteaInventoryItemTemplate>(editedObjConst));
		if (!itemTemplate)
			continue;

		if (!itemTemplate->HasAnyFlags(RF_Transient))
			itemTemplate->GetPackage()->MarkPackageDirty();

		TrackDirtyAsset(itemTemplate);

		bAnyChanged = true;
	}

	if (bAnyChanged && TemplateTreeView.IsValid())
		TemplateTreeView->RequestTreeRefresh();
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
	SaveExpansionState();
	
	TArray<UObject*> allTemplates = LoadAllTemplatesForMatrix();
	
	AvailableTemplates.Empty();
	UnbindItemChanged();
	
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
	BindItemChanged();
	
	RebuildTreeStructure();
	ApplySearchFilter();
	
	if (TemplateTreeView.IsValid())
	{
		TemplateTreeView->RequestTreeRefresh();
		RestoreExpansionState();
	}
}

void SMounteaInventoryTemplateEditor::RebuildTreeStructure()
{
	SaveExpansionState();
	
	const bool bFirstLoad = ExpandedCategories.IsEmpty();
	
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
			
			if (bFirstLoad)
				ExpandedCategories.Add(category);
		}
		
		TSharedPtr<FTemplateTreeItem> templateItem = FTemplateTreeItem::MakeTemplate(templatePtr);
		categoryMap[category]->Children.Add(templateItem);
	}
	
	TreeRootItems.Sort([](const TSharedPtr<FTemplateTreeItem>& A, const TSharedPtr<FTemplateTreeItem>& B)
	{
		return A->CategoryName < B->CategoryName;
	});
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
	
	return MounteaInventoryEditorLayout::MakeBrowserDetails(
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
				}), PropertyDetailsView.ToSharedRef(),
		TAttribute<float>::CreateLambda([this]() { return GetCachedGeometry().GetLocalSize().X * 0.20f; }),
		TAttribute<float>::CreateLambda([this]() { return GetCachedGeometry().GetLocalSize().X * 0.50f; }));

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

void SMounteaInventoryTemplateEditor::SaveExpansionState()
{
	if (!TemplateTreeView.IsValid())
		return;
	
	ExpandedCategories.Empty();
	for (const TSharedPtr<FTemplateTreeItem>& rootItem : FilteredTreeRootItems)
	{
		if (rootItem.IsValid() && TemplateTreeView->IsItemExpanded(rootItem))
			ExpandedCategories.Add(rootItem->CategoryName);
	}
}

void SMounteaInventoryTemplateEditor::RestoreExpansionState()
{
	if (!TemplateTreeView.IsValid())
		return;
	
	for (const TSharedPtr<FTemplateTreeItem>& rootItem : FilteredTreeRootItems)
	{
		if (rootItem.IsValid() && ExpandedCategories.Contains(rootItem->CategoryName))
			TemplateTreeView->SetItemExpansion(rootItem, true);
	}
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
		FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Cannot save: No template is being edited."), EMounteaEditorNotificationType::Error);
		return FReply::Unhandled();
	}

	FString validationError;
	if (!ValidateTemplateData(validationError))
	{
		FMounteaEditorNotificationHelpers::ShowNotification(FString::Printf(TEXT("Cannot save: %s."), *validationError), EMounteaEditorNotificationType::Error);
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
	TArray<UMounteaInventoryItemTemplate*> templatesToUntrack;

	Algo::ForEach(DirtyTemplates, [&](const TWeakObjectPtr<UMounteaInventoryItemTemplate>& weakTemplate)
	{
		UMounteaInventoryItemTemplate* itemTemplate = weakTemplate.Get();
		if (!itemTemplate || itemTemplate->HasAnyFlags(RF_Transient))
			return;

		itemTemplate->CalculateJson();
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
			templatesToUntrack.Add(itemTemplate);
		}
		else
			failedAssets.Add(itemTemplate->DisplayName.ToString());
	});

	for (UMounteaInventoryItemTemplate* templateToUntrack : templatesToUntrack)
	{
		UntrackDirtyAsset(templateToUntrack);
	}

	if (!failedAssets.IsEmpty())
	{
		const FString errorMsg = FString::Printf(TEXT("Failed to save %d assets: %s"),
			failedAssets.Num(), *FString::Join(failedAssets, TEXT(", ")));
		FMounteaEditorNotificationHelpers::ShowNotification(errorMsg, EMounteaEditorNotificationType::Error);
	}
	else if (savedCount > 0)
		FMounteaEditorNotificationHelpers::ShowNotification(FString::Printf(TEXT("Successfully saved %d templates."), savedCount), EMounteaEditorNotificationType::Success);
	else
		FMounteaEditorNotificationHelpers::ShowNotification(TEXT("No templates to save."), EMounteaEditorNotificationType::Success);

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
			FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Template saved successfully!"), EMounteaEditorNotificationType::Success);
			return FReply::Handled();
		}
	}
	
	FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Failed to save template."), EMounteaEditorNotificationType::Error);
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
								.Image(FMounteaAdvancedInventoryEditorStyle::GetBrush("MAISStyleSet.Close"))
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
				
				FMounteaEditorNotificationHelpers::ShowNotification(TEXT("New template created successfully!"), EMounteaEditorNotificationType::Success);
				return FReply::Handled();
			}
		}
	}
	
	FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Failed to create new template."), EMounteaEditorNotificationType::Error);
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

		returnInfo.DisplayText = MounteaInventoryTreeRows::DisplayName(baseName, returnInfo.bIsTransient || returnInfo.bIsDirty);

		returnInfo.AssetPath = MounteaInventoryTreeRows::AssetPath(templatePtr);

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

bool SMounteaInventoryTemplateEditor::IsInventoryTemplateAsset(const FAssetData& AssetData)
{
	return AssetData.AssetClassPath == UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName();
}

void SMounteaInventoryTemplateEditor::OnAssetRegistryRemoved(const FAssetData& AssetData)
{
	if (AssetData.AssetClassPath != UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName())
		return;

	PendingRemovedObjectPaths.Add(*AssetData.GetObjectPathString());
	bPendingRefresh = true;

	if (!PendingRefreshTimer.IsValid())
		PendingRefreshTimer = RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateRaw(
			this, &SMounteaInventoryTemplateEditor::HandleDeferredRefresh));
}

void SMounteaInventoryTemplateEditor::OnAssetRegistryRenamed(const FAssetData& AssetData, const FString& String)
{
	if (!IsInventoryTemplateAsset(AssetData))
		return;

	RefreshTemplateList();
}

void SMounteaInventoryTemplateEditor::OnAssetRegistryAdded(const FAssetData& AssetData)
{
	if (!IsInventoryTemplateAsset(AssetData))
		return;

	RefreshTemplateList();
}

void SMounteaInventoryTemplateEditor::OnAssetRegistryUpdated(const FAssetData& AssetData)
{
	if (!IsInventoryTemplateAsset(AssetData))
		return;

	RefreshTemplateList();
}

EActiveTimerReturnType SMounteaInventoryTemplateEditor::HandleDeferredRefresh(double, float)
{
	if (!bPendingRefresh || PendingRemovedObjectPaths.Num() == 0)
	{
		bPendingRefresh = false;
		PendingRefreshTimer.Reset();
		return EActiveTimerReturnType::Stop;
	}

	bool bAnyStillExists = false;

	if (!GEditor)
		return EActiveTimerReturnType::Stop;
	
	UEditorAssetSubsystem* editorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
	if (!editorAssetSubsystem)
		return EActiveTimerReturnType::Stop;
	
	for (const FName& objPathName : PendingRemovedObjectPaths)
	{
		const FString objPath = objPathName.ToString();
		if (editorAssetSubsystem->DoesAssetExist(objPath))
		{
			bAnyStillExists = true;
			break;
		}
	}

	if (bAnyStillExists)
		return EActiveTimerReturnType::Continue;

	PendingRemovedObjectPaths.Reset();
	bPendingRefresh = false;

	RefreshTemplateList();

	PendingRefreshTimer.Reset();
	return EActiveTimerReturnType::Stop;
}

void SMounteaInventoryTemplateEditor::BindAssetRegistry()
{
	const FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& assetRegistry = assetRegistryModule.Get();

	AssetRemovedHandle	= assetRegistry.OnAssetRemoved().AddRaw(this, &SMounteaInventoryTemplateEditor::OnAssetRegistryRemoved);
	AssetRenamedHandle	= assetRegistry.OnAssetRenamed().AddRaw(this, &SMounteaInventoryTemplateEditor::OnAssetRegistryRenamed);
	AssetAddedHandle	= assetRegistry.OnAssetAdded().AddRaw(this, &SMounteaInventoryTemplateEditor::OnAssetRegistryAdded);
	AssetUpdatedHandle	= assetRegistry.OnAssetUpdated().AddRaw(this, &SMounteaInventoryTemplateEditor::OnAssetRegistryUpdated);
}

void SMounteaInventoryTemplateEditor::UnbindAssetRegistry()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
		return;

	FAssetRegistryModule& assetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& assetRegistry = assetRegistryModule.Get();

	if (AssetRemovedHandle.IsValid()) 
		assetRegistry.OnAssetRemoved().Remove(AssetRemovedHandle);
	if (AssetRenamedHandle.IsValid()) 
		assetRegistry.OnAssetRenamed().Remove(AssetRenamedHandle);
	if (AssetAddedHandle.IsValid()) 
		assetRegistry.OnAssetAdded().Remove(AssetAddedHandle);
	if (AssetUpdatedHandle.IsValid()) 
		assetRegistry.OnAssetUpdated().Remove(AssetUpdatedHandle);

	AssetRemovedHandle.Reset();
	AssetRenamedHandle.Reset();
	AssetAddedHandle.Reset();
	AssetUpdatedHandle.Reset();
}

void SMounteaInventoryTemplateEditor::BindItemChanged()
{
	for (auto itemTemplate : AvailableTemplates)
	{
		if (itemTemplate.IsValid())
			itemTemplate->TemplateChangedDelegate.AddRaw(this, &SMounteaInventoryTemplateEditor::OnItemChanged);
	}
}

void SMounteaInventoryTemplateEditor::UnbindItemChanged()
{
	for (auto itemTemplate : AvailableTemplates)
	{
		if (itemTemplate.IsValid())
			itemTemplate->TemplateChangedDelegate.RemoveAll(this);
	}
}

void SMounteaInventoryTemplateEditor::OnItemChanged(UMounteaInventoryItemTemplate* Template)
{
	if (!Template)
		return;

	UPackage* itemPackage = Template->GetOutermost();
	if (itemPackage && itemPackage->IsDirty() && !PendingChanges.Contains(Template))
	{
		PendingChanges.Add(Template);

		if (ProcessChangesTimer.IsValid())
			GEditor->GetTimerManager()->ClearTimer(ProcessChangesTimer);

		GEditor->GetTimerManager()->SetTimer(
			ProcessChangesTimer,
			FTimerDelegate::CreateSP(this, &SMounteaInventoryTemplateEditor::ProcessPendingChanges),
			ChangeDebounceDelay,
			false
		);
	}
}

void SMounteaInventoryTemplateEditor::ProcessPendingChanges()
{
	if (PendingChanges.Num() == 0)
		return;

	for (UMounteaInventoryItemTemplate* itemTemplate : PendingChanges)
	{
		if (itemTemplate)
			TrackDirtyAsset(itemTemplate);
	}

	PendingChanges.Empty();
	RefreshTemplateList();
}

void SMounteaInventoryTemplateEditor::ApplySearchFilter()
{
	FilteredTreeRootItems.Empty();
	
	if (!SearchFilterWidget.IsValid())
	{
		FilteredTreeRootItems = TreeRootItems;
		if (TemplateTreeView.IsValid())
		{
			TemplateTreeView->RequestTreeRefresh();
			RestoreExpansionState();
		}
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
	{
		TemplateTreeView->RequestTreeRefresh();
		RestoreExpansionState();
	}
}

FReply SMounteaInventoryTemplateEditor::DeleteTemplate(const TWeakObjectPtr<UMounteaInventoryItemTemplate> Template)
{
	if (!Template.IsValid())
		return FReply::Unhandled();

	UMounteaInventoryItemTemplate* itemTemplate = Template.Get();

	if (itemTemplate->HasAnyFlags(RF_Transient))
	{
		FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Cannot delete unsaved template."), EMounteaEditorNotificationType::Error);
		return FReply::Handled();
	}

	const FText title = LOCTEXT("DeleteTemplateTitle", "Delete Template");
	const FText message = FText::Format(
		LOCTEXT("DeleteTemplateMessage", "Are you sure you want to delete '{0}'?"),
		itemTemplate->DisplayName.IsEmpty()
			? FText::FromString(itemTemplate->GetName())
			: itemTemplate->DisplayName
	);

	if (FMessageDialog::Open(EAppMsgType::YesNo, message, title) != EAppReturnType::Yes)
		return FReply::Handled();

	UntrackDirtyAsset(itemTemplate);

	TArray<UObject*> objectsToDelete;
	objectsToDelete.Add(itemTemplate);

	const int32 numDeleted = ObjectTools::DeleteObjects(objectsToDelete, false);

	if (numDeleted > 0)
	{
		RefreshTemplateList();
		FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Template deleted successfully."), EMounteaEditorNotificationType::Success);
	}
	else
		FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Failed to delete template."), EMounteaEditorNotificationType::Error);

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
		
		FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Template duplicated successfully."), EMounteaEditorNotificationType::Success);
	}
	else
		FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Failed to duplicate template."), EMounteaEditorNotificationType::Error);
	
	return FReply::Handled();
}

void SMounteaInventoryTemplateEditor::ImportTemplate()
{
	TArray<UMounteaInventoryItemTemplate*> importedTemplates;
	FString errorMessage;

	if (UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFile(importedTemplates, errorMessage))
	{
		FMounteaEditorNotificationHelpers::ShowNotification(errorMessage, EMounteaEditorNotificationType::Success);
		RefreshTemplateList();
	}
	else
	{
		FMounteaEditorNotificationHelpers::ShowNotification(errorMessage, EMounteaEditorNotificationType::Error);
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
		FMounteaEditorNotificationHelpers::ShowNotification(tempMessage, EMounteaEditorNotificationType::Success);
	}
	else
		FMounteaEditorNotificationHelpers::ShowNotification(errorMessage, EMounteaEditorNotificationType::Error);
}

TSharedRef<SWidget> SMounteaInventoryTemplateEditor::CreateToolbar()
{
	FMounteaInventoryEditorToolbarActions Actions;
	Actions.Create = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::OnCreateNewTemplate);
	Actions.Save = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::SaveTemplate);
	Actions.SaveAll = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::SaveAllDirtyTemplates);
	Actions.Close = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::CloseTemplate);
	Actions.Help = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::ShowHelpModal);
	Actions.Import = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::ImportTemplateAction);
	Actions.Export = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::ExportTemplateAction);
	Actions.CanSave = TAttribute<bool>::CreateLambda([this]()
	{
		if (bIsShowingTransient && CurrentTemplate.IsValid() && CurrentTemplate->HasAnyFlags(RF_Transient))
			return true;
		if (CurrentTemplate.IsValid() && DirtyTemplates.Contains(CurrentTemplate))
			return true;
		auto* Subsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>();
		return Subsystem && Subsystem->HasTempTemplate() && DirtyTemplates.Contains(Subsystem->GetOrCreateTempTemplate());
	});
	Actions.CanSaveAll = TAttribute<bool>::CreateLambda([this]() { return DirtyTemplates.Num() > 0; });
	return MounteaInventoryEditorLayout::MakeToolbar(Actions,
		SAssignNew(SearchFilterWidget, SMounteaInventoryTemplateSearchFilter)
			.OnSearchTextChanged(this, &SMounteaInventoryTemplateEditor::OnSearchTextChanged)
			.OnFiltersChanged(this, &SMounteaInventoryTemplateEditor::OnFiltersChanged));
}

FReply SMounteaInventoryTemplateEditor::ImportTemplateAction() { ImportTemplate(); return FReply::Handled(); }
FReply SMounteaInventoryTemplateEditor::ExportTemplateAction() { ExportTemplate(); return FReply::Handled(); }

TSharedRef<ITableRow> SMounteaInventoryTemplateEditor::GenerateTemplateTreeRow(
	TSharedPtr<FTemplateTreeItem> Item,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Item.IsValid()) return SNew(STemplateTreeItemRow, OwnerTable)[SNullWidget::NullWidget];
	if (Item->Type == ETemplateTreeItemType::Category)
		return SNew(STemplateTreeItemRow, OwnerTable).Padding(FMargin(0, 4, 0, 2))
		[MounteaInventoryTreeRows::MakeCategoryContent(Item->CategoryName,
			TAttribute<int32>::CreateLambda([Item]() { return Item->Children.Num(); }))];

	const TWeakObjectPtr<UMounteaInventoryItemTemplate> Template = Item->Template;
	const FTemplateDisplayInfo Info = GenerateTemplateDisplayInfo(Template, DirtyTemplates);
	FMounteaInventoryAssetRowPresentation Presentation;
	Presentation.Name = TAttribute<FText>::CreateLambda([this, Template]()
	{ return GenerateTemplateDisplayInfo(Template, DirtyTemplates).DisplayText; });
	Presentation.Tooltip = TAttribute<FText>::CreateLambda([this, Template]()
	{ return GenerateTemplateDisplayInfo(Template, DirtyTemplates).TooltipText; });
	Presentation.AssetPath = Info.AssetPath;
	Presentation.NameColor = TAttribute<FSlateColor>::CreateLambda([this, Template]()
	{
		if (Template.IsValid())
		{
			if (Template->HasAnyFlags(RF_Transient)) return FSlateColor(FLinearColor(1.f, 1.f, 0.5f));
			if (DirtyTemplates.Contains(Template)) return FSlateColor(FLinearColor(1.f, 0.7f, 0.7f));
		}
		return FSlateColor::UseForeground();
	});
	Presentation.IsSelected = TAttribute<bool>::CreateLambda([this, Item]()
	{ return TemplateTreeView.IsValid() && TemplateTreeView->IsItemSelected(Item); });
	Presentation.ActionsVisibility = Info.bIsTransient ? EVisibility::Collapsed : EVisibility::Visible;
	Presentation.Select = FOnClicked::CreateLambda([this, Item]()
	{ return MounteaInventoryTreeRows::SelectItem(TemplateTreeView, Item, FilteredTreeRootItems); });
	Presentation.Locate = FOnClicked::CreateLambda([Template]()
	{ return MounteaInventoryTreeRows::LocateAsset(Template.Get()); });
	Presentation.Duplicate = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::DuplicateTemplate, Template);
	Presentation.Delete = FOnClicked::CreateSP(this, &SMounteaInventoryTemplateEditor::DeleteTemplate, Template);
	return SNew(STemplateTreeItemRow, OwnerTable).Padding(FMargin(4, 2))
		[MounteaInventoryTreeRows::MakeAssetContent(Presentation)];
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
