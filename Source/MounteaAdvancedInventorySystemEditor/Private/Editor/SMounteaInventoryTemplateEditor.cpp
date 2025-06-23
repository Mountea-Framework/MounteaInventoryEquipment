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

#include "Subsystems/UMounteaInventoryTemplateEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "SMounteaInventoryTemplateEditor"

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
}

void SMounteaInventoryTemplateEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		if (const auto itemTemplate = Cast<UMounteaInventoryItemTemplate>(PropertyChangedEvent.GetObjectBeingEdited(0)))
		{
			if (!itemTemplate->HasAnyFlags(RF_Transient))
				itemTemplate->GetPackage()->MarkPackageDirty();
		}
	}
}

TArray<UObject*> SMounteaInventoryTemplateEditor::LoadAllTemplatesForMatrix()
{
	TArray<UObject*> allTemplates;
	
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
		
		FString nameA = templateA->DisplayName.IsEmpty() ? templateA->GetName() : templateA->DisplayName.ToString();
		FString nameB = templateB->DisplayName.IsEmpty() ? templateB->GetName() : templateB->DisplayName.ToString();
		
		return nameA < nameB;
	});
	
	return allTemplates;
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
		
	TArray<TWeakObjectPtr<UMounteaInventoryItemTemplate>> selectedTemplates = TemplateListView->GetSelectedItems();
	TArray<UObject*> selectedObjects;
	
	for (const TWeakObjectPtr<UMounteaInventoryItemTemplate>& weakTemplate : selectedTemplates)
	{
		if (UMounteaInventoryItemTemplate* itemTemplate = weakTemplate.Get())
			selectedObjects.Add(itemTemplate);
	}
	
	PropertyDetailsView->SetObjects(selectedObjects);
}

FReply SMounteaInventoryTemplateEditor::OnCreateNewTemplate()
{
	CreateTransientTemplate();
	
	if (PropertyDetailsView.IsValid())
	{
		TArray<UObject*> currentObjects = LoadAllTemplatesForMatrix();
		currentObjects.Add(CurrentTemplate.Get());
		PropertyDetailsView->SetObjects(currentObjects);
	}
	
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

	if (bIsEditingExisting && OriginalTemplate.IsValid())
		return SaveExistingTemplate();
	else
		return SaveNewTemplate();
}

FReply SMounteaInventoryTemplateEditor::CloseTemplate()
{
	return FReply::Handled();
}

FReply SMounteaInventoryTemplateEditor::SaveExistingTemplate()
{
	return FReply::Unhandled();
}

FReply SMounteaInventoryTemplateEditor::SaveNewTemplate()
{
	return FReply::Unhandled();
}

void SMounteaInventoryTemplateEditor::ImportTemplate()
{
}

void SMounteaInventoryTemplateEditor::ExportTemplate()
{
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

		// Close Template
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("CloseTemplate", "Close Template"))
			.OnClicked_Lambda([this]() {
				return CloseTemplate();
			})
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

void SMounteaInventoryTemplateEditor::CreateTransientTemplate()
{
	UUMounteaInventoryTemplateEditorSubsystem* templateEditorSubsystem = GEditor->GetEditorSubsystem<UUMounteaInventoryTemplateEditorSubsystem>();
	CurrentTemplate = templateEditorSubsystem->GetOrCreateTempTemplate();
}

void SMounteaInventoryTemplateEditor::CleanupTransientTemplate()
{
	if (UUMounteaInventoryTemplateEditorSubsystem* templateEditorSubsystem = GEditor->GetEditorSubsystem<UUMounteaInventoryTemplateEditorSubsystem>())
		templateEditorSubsystem->ClearTempTemplate();
	CurrentTemplate = nullptr;
}

bool SMounteaInventoryTemplateEditor::ValidateTemplateData(FString& ErrorMessage) const
{
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


#undef LOCTEXT_NAMESPACE
