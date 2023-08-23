// All rights reserved Dominik Pavlicek 2023


#include "MounteaInventoryItemConfig_CustomDetailsHelper.h"

#include "DetailWidgetRow.h"
#include "IPropertyUtilities.h"

#include "SourceCodeNavigation.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Helpers/MIEEditorStyle.h"

#include "Setup/MounteaInventoryItemConfig.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemConfig_CustomDetailsHelper"
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

FMounteaInventoryItemConfig_CustomDetailsHelper::FMounteaInventoryItemConfig_CustomDetailsHelper(IDetailPropertyRow* InPropertyRow)
: PropertyRow(InPropertyRow)
{}

void FMounteaInventoryItemConfig_CustomDetailsHelper::Update()
{
	TSharedPtr<SWidget> DefaultNameWidget;
	TSharedPtr<SWidget> DefaultValueWidget;
	PropertyRow->GetDefaultWidgets
	(
		DefaultNameWidget,
		DefaultValueWidget,
		false
	);
	
	PropertyRow->ShowPropertyButtons(true);

	FExecuteAction OnInsertClicked; // Insert is not allowed
	FExecuteAction OnDeleteClicked = FExecuteAction::CreateSP( this, &FMounteaInventoryItemConfig_CustomDetailsHelper::RequestDeleteItem );
	FExecuteAction OnDuplicateClicked; // Duplicates are not allowed
	
	FDetailWidgetRow& DetailWidgetRow = PropertyRow->CustomWidget(true);
	DetailWidgetRow.NameContent()
	[
		DefaultNameWidget.ToSharedRef()
	];

	TSharedPtr<SHorizontalBox> HorizontalBox;
	DetailWidgetRow.ValueContent()
	.MinDesiredWidth(GetRowMinimumDesiredWidth())
	[
		SAssignNew(HorizontalBox, SHorizontalBox)
	];

	// Default previous widget
	HorizontalBox->AddSlot()
   .Padding(0.f, 0.f, 2.f, 0.f)
   .FillWidth(1.f)
   [
	   DefaultValueWidget.ToSharedRef()
   ];

	/* We don't delete here good sir
	HorizontalBox->AddSlot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	.Padding(4.f)
	[
		PropertyCustomizationHelpers::MakeInsertDeleteDuplicateButton( OnInsertClicked, OnDeleteClicked, OnDuplicateClicked)
	];
	*/
	
	// GEditor doesn't support inserting Handle values for EditInline
	/*
	HorizontalBox->AddSlot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	.Padding(4.f)
	[
		PropertyCustomizationHelpers::MakeUseSelectedButton( OnUseSelectedClicked)
	];
	*/
	
	// Browse Asset
	HorizontalBox->AddSlot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	.Padding(4.f)
	[
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
		.ToolTipText(this, &Self::GetBrowseObjectText)
		.ContentPadding(4.f)
		.ForegroundColor(FSlateColor::UseForeground())
		.Visibility(this, &Self::GetBrowseButtonVisibility)
		.OnClicked(this, &Self::OnBrowseClicked)
		[
			SNew(SImage)
			.Image(FMIEEditorStyle::GetBrush("MIEStyleSet.Icon.Browse"))
			.ColorAndOpacity(FSlateColor::UseForeground())
		]
	];

	// Jump to Object
	HorizontalBox->AddSlot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	.Padding(4.f, 2.f)
	[
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
		.ToolTipText(this, &Self::GetJumpToObjectText)
		.ContentPadding(4.f)
		.ForegroundColor(FSlateColor::UseForeground())
		.Visibility(this, &Self::GetOpenButtonVisibility)
		.OnClicked(this, &Self::OnOpenClicked)
		[
			SNew(SImage)
			.Image(FMIEEditorStyle::GetBrush("MIEStyleSet.Icon.Edit"))
			 .ColorAndOpacity( FSlateColor::UseForeground() )
		]
	];

	// Open Documentation
	HorizontalBox->AddSlot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	.Padding(4.f, 2.f)
	[
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
		.ToolTipText(this, &Self::GetOpenDocumentationText)
		.ContentPadding(4.f)
		.ForegroundColor(FSlateColor::UseForeground())
		.Visibility(this, &Self::GetOpenDocumentationButtonVisibility)
		.OnClicked(this, &Self::OnDocumentationClicked)
		[
			SNew(SImage)
			.Image(FMIEEditorStyle::GetBrush("MIEStyleSet.Buttons.Documentation.small"))
			 .ColorAndOpacity( FSlateColor::UseForeground() )
		]
	];

	PropertyRow->ShouldAutoExpand(true);
}

FReply FMounteaInventoryItemConfig_CustomDetailsHelper::OnBrowseClicked()
{
	if (!GEditor)
	{
		return FReply::Handled();
	}

	static constexpr bool bFocusContentBrowser = true;
	TArray<UObject*> ObjectsToSyncTo;
	if (UBlueprint* Blueprint = GetBlueprint())
	{
		ObjectsToSyncTo.Add(Blueprint);
	}
	GEditor->SyncBrowserToObjects(ObjectsToSyncTo, bFocusContentBrowser);

	return FReply::Handled();
}

FReply FMounteaInventoryItemConfig_CustomDetailsHelper::OnOpenClicked()
{
	if (UBlueprint* Blueprint = GetBlueprint())
	{
		FMounteaInventoryEquipmentEditorUtilities::OpenBlueprintEditor(
			Blueprint,
			OpenType,
			FunctionNameToOpen,
			bForceFullEditor,
			bAddBlueprintFunctionIfItDoesNotExist
		);
	}
	else if (UObject* Object = GetObject())
	{
		// Native
		FSourceCodeNavigation::NavigateToClass(Object->GetClass());
	}
	return FReply::Handled();
}

FReply FMounteaInventoryItemConfig_CustomDetailsHelper::OnDocumentationClicked() const
{
	FPlatformProcess::LaunchURL(*ItemConfigWikiPage, nullptr, nullptr);
	return FReply::Handled();
}

UObject* FMounteaInventoryItemConfig_CustomDetailsHelper::GetObject() const
{
	if (!PropertyRow)
	{
		return nullptr;
	}

	TSharedPtr<IPropertyHandle> PropertyHandle = PropertyRow->GetPropertyHandle();
	if (!PropertyHandle.IsValid())
	{
		return nullptr;
	}
	UObject* Object = nullptr;
	PropertyHandle->GetValue(Object);
	return Object;
}

UBlueprint* FMounteaInventoryItemConfig_CustomDetailsHelper::GetBlueprint() const
{
	UObject* Object = GetObject();
	if (!Object) return nullptr;

	// Class
	UClass* Class = Object->GetClass();
	if (const UBlueprintGeneratedClass* BlueprintClass = Cast<UBlueprintGeneratedClass>(Class))
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintClass->ClassGeneratedBy))
		{
			return Blueprint;
		}
	}

	return Cast<UBlueprint>(Object);
}

bool FMounteaInventoryItemConfig_CustomDetailsHelper::IsObjectABlueprint() const
{
	return GetBlueprint() != nullptr;
}

FText FMounteaInventoryItemConfig_CustomDetailsHelper::GetBrowseObjectText() const
{
	return LOCTEXT("MounteaInventoryItemConfig_CustomDetailsHelper_BrowseButtonToolTipText", "Browse to Asset in Content Browser");
}

FText FMounteaInventoryItemConfig_CustomDetailsHelper::GetJumpToObjectText() const
{
	if (IsObjectABlueprint())
	{
		return LOCTEXT("MounteaInventoryItemConfig_CustomDetailsHelper_OpenObjectBlueprintTooltipKey", "Open Blueprint Editor");
	}

	// Native Class
	return FText::Format(
		LOCTEXT("MounteaInventoryItemConfig_CustomDetailsHelper_OpenObjectBlueprintTooltipKey", "Open Source File in {0}"),
		FSourceCodeNavigation::GetSelectedSourceCodeIDE()
	);
}

FText FMounteaInventoryItemConfig_CustomDetailsHelper::GetOpenDocumentationText() const
{
	return LOCTEXT("MounteaInventoryItemConfig_CustomDetailsHelper_OpenDocumentationText", "Open Documentation page for Inventory Item Configuration");
}

EVisibility FMounteaInventoryItemConfig_CustomDetailsHelper::GetOpenButtonVisibility() const
{
	if (!CanBeVisible())
	{
		return EVisibility::Collapsed;
	}

	if (UObject* Object = GetObject())
	{
		// Blueprint
		if (IsObjectABlueprint())
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FMounteaInventoryItemConfig_CustomDetailsHelper::GetBrowseButtonVisibility() const
{
	if (!CanBeVisible())
	{
		return EVisibility::Collapsed;
	}
	
	return IsObjectABlueprint() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FMounteaInventoryItemConfig_CustomDetailsHelper::GetOpenDocumentationButtonVisibility() const
{
	if (!CanBeVisible())
	{
		return EVisibility::Collapsed;
	}

	return EVisibility::Visible;
}

void FMounteaInventoryItemConfig_CustomDetailsHelper::RequestDeleteItem()
{
	PropertyUtils->EnqueueDeferredAction( FSimpleDelegate::CreateSP( this, &FMounteaInventoryItemConfig_CustomDetailsHelper::OnDeleteItem ) );
}

// TODO: Make this just better
// This one is not used at all
void FMounteaInventoryItemConfig_CustomDetailsHelper::OnDeleteItem()
{
	auto ArrayHandle = PropertyRow->GetPropertyHandle()->GetParentHandle()->GetParentHandle()->AsArray(); // NodeDecorators

	// PLEASE TELL ME THERE IS A BETTER WAY
	// I SUFFER IMAGINING THIS WILL GO LIVE :(
	FString IndexString =  PropertyRow->GetPropertyHandle()->GetParentHandle()->GetPropertyDisplayName().ToString();
	int32 Index = FCString::Atoi((TEXT("%s"), *IndexString));

	check(ArrayHandle.IsValid());

	if (ArrayHandle.IsValid())
	{
		ArrayHandle->DeleteItem(Index);
	}

}

#undef LOCTEXT_NAMESPACE
#undef DEFAULT_FONT